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
               const std::fstream::off_type blockBegin,
               const size_t blockSize_char /* количество символов */,
               const uint64_t blockOffset,
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
    }
}
