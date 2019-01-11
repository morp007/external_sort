/*
 * This is an open source non-commercial project. Dear PVS-Studio, please check it.
 * PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
 */

#include "Helper.h"

#include <algorithm>
#include <thread>


namespace
{
std::fstream::pos_type fstreamSize(const std::string &in)
{
    std::fstream stream;
    stream.open(in, std::ios_base::in | std::ios_base::binary);

    stream.seekg(0, std::ios_base::end);
    const auto res = stream.tellg();

    stream.close();

    return res;
}


void sortBlock(const std::string &filename,
               const std::fstream::off_type blockBegin /* байт */,
               const size_t blockSize_char /* количество символов */,
               const uint64_t blockOffset /* байт */,
               const Helper::SortType sortType)
{
    std::fstream stream;
    stream.open(filename,
                std::ios_base::binary
                | std::ios_base::in
                | std::ios_base::out);

    auto currentBlockBegin = blockBegin;
    const auto fileSize = fstreamSize(filename);

    while (currentBlockBegin < fileSize)
    {
        stream.seekg(currentBlockBegin);
        const size_t blockSize =
            ((currentBlockBegin + blockSize_char * sizeof(Helper::CharType)) > fileSize)
                ? static_cast<size_t>((fileSize - currentBlockBegin) / sizeof(Helper::CharType))
                : blockSize_char;

        // read block
        auto vec = Helper::File::read<Helper::CharType>(stream, blockSize);

        // sort block
        if (sortType == Helper::SortType::ASC)
        {
            std::sort(vec.begin(), vec.end(), std::less<Helper::CharType>());
        }
        else
        {
            std::sort(vec.begin(), vec.end(), std::greater<Helper::CharType>());
        }

        // write block
        stream.seekp(currentBlockBegin);
        Helper::File::write<Helper::CharType>(stream, vec);

        currentBlockBegin += blockOffset;
    }

    stream.close();
}


void merge(const std::string &filename,
           const size_t blockSize_char /* количество символов */,
           const uint64_t blockOffset /* байт */,
           const Helper::SortType sortType)
{
    size_t blocks = 0;
    const auto fileSize = fstreamSize(filename);
    const auto blockSize = blockSize_char * sizeof(Helper::CharType);

    // init blocks
    {
        // если блок больше файла
        if (blockOffset > fileSize)
        {
            // значит уже отсортировано все, выходим
            return;
        }

        blocks = static_cast<size_t>(fileSize / blockSize);

        if (fileSize % blockSize != 0)
        {
            blocks++;
        }
    }

    // вектор из потока и позиции его "конца"
    std::vector<std::pair<std::ifstream, std::fstream::pos_type>> streams(blocks);

    std::vector<Helper::CharType> streamValue(blocks);

    // init vectors
    {
        std::ifstream::off_type begin = 0;

        for (size_t i = 0; i < blocks; i++)
        {
            streams[i].first = std::ifstream();
            streams[i].first.open(filename, std::ios_base::in | std::ios_base::binary);
            streams[i].first.seekg(begin);

            begin += blockSize;
            if (begin > fileSize)
            {
                streams[i].second = fileSize;
            }
            else
            {
                streams[i].second = begin;
            }

            streamValue[i] = Helper::File::read<Helper::CharType>(streams[i].first, 1)[0];
        }
    }

    std::ofstream out;
    out.open(filename + ".sorted", std::ios_base::out | std::ios_base::binary);

    while (!streamValue.empty())
    {
        decltype(streamValue)::iterator itemToWrite;

        // sort block
        if (sortType == Helper::SortType::ASC)
        {
            itemToWrite = std::min_element(streamValue.begin(), streamValue.end());
        }
        else
        {
            itemToWrite = std::max_element(streamValue.begin(), streamValue.end());
        }

        Helper::File::writeOneItem(out, *itemToWrite);

        // обновляем вектор значений
        {
            const auto itemPos = std::distance(streamValue.begin(),
                                               itemToWrite);
            auto &stream = streams[itemPos].first;
            const auto &end = streams[itemPos].second;
            const auto currentPos = stream.tellg();

            if (currentPos != end)
            {
                *itemToWrite = Helper::File::read<Helper::CharType>(stream, 1)[0];
            }
            else
            {
                streamValue.erase(itemToWrite);

                decltype(streams) vecTmp(streams.size() - 1);

                for (size_t i = 0; i < streams.size(); i++)
                {
                    if (i == itemPos)
                    {
                        continue;
                    }

                    vecTmp[i] = std::move(streams[i]);
                }

                streams.swap(vecTmp);
            }
        }
    }
}
}   // end of unnamed namespace


void Helper::sort(const std::string &filename,
                  const Helper::SortType sortType,
                  const unsigned memSize_mb,
                  const unsigned threadCount)
{
    // проверки входных аргументов
    {

    }

    const auto charSize = sizeof(Helper::CharType);
    const uint64_t memSize_byte = memSize_mb * 1024 * 1024;
    const auto memSizePerThread_byte = memSize_byte / threadCount;
    const auto blockSize_char = memSizePerThread_byte / charSize;
    const auto blockOffset = memSizePerThread_byte * threadCount;

    std::cout << std::hex
              << "\ncharSize                  0x" << charSize
              << "\nmemSize_byte              0x" << memSize_byte
              << "\nmemSizePerThread_byte     0x" << memSizePerThread_byte
              << "\nblockSize_char            0x" << blockSize_char
              << "\nblockOffset               0x" << blockOffset
              << std::endl;

    std::vector<std::thread> threads(threadCount);
    std::fstream::off_type blockBegin = 0;

    // сортируем числа в блоках
    {
        for (int i = 0; i < threadCount; i++)
        {
            std::thread thread(&sortBlock,
                               filename,
                               blockBegin,
                               blockSize_char,
                               blockOffset,
                               sortType);

            blockBegin += memSizePerThread_byte;
            threads[i] = std::move(thread);
        }

        for (auto &thread : threads)
        {
            thread.join();
        }
    }

    // сливаем (merge) блоки
    {
        merge(filename, blockSize_char, blockOffset, sortType);
    }
}
