/*
 * This is an open source non-commercial project. Dear PVS-Studio, please check it.
 * PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
 */

#include "Helper.h"

#include <algorithm>


namespace
{
std::fstream::pos_type fstreamSize(std::fstream &in)
{
    // запоминаем текущую позицию
    const auto currentPos = in.tellg();

    // смотрим размер
    in.seekg(0, std::ios_base::end);
    const auto res = in.tellg();

    // возращаемся обратно
    in.seekg(currentPos);

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
    const auto fileSize = fstreamSize(stream);

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