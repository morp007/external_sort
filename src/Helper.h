#ifndef HELPER_H_
#define HELPER_H_

#include <fstream>
#include <limits>
#include <sstream>
#include <vector>


namespace Helper
{
namespace File
{
template<typename CharType>
std::vector<CharType> read(std::fstream &_stream, const size_t n)
{
    std::vector<CharType> res(n);
    _stream.read(reinterpret_cast<char *>(res.data()), n * sizeof(CharType));
    return res;
}

template<typename CharType, typename Containter>
void write(std::fstream &_stream, Containter &in)
{
    _stream.write(reinterpret_cast<char *>(in.data()),
                  in.size() * sizeof(CharType));
}

template<typename CharType>
void writeOneItem(std::fstream &_stream, const CharType &in)
{
    CharType _{in};
    _stream.write(reinterpret_cast<char *>(&_), sizeof(CharType));
}
}   // end of namespace File

}   // end of namespace Helper


#endif // HELPER_H_