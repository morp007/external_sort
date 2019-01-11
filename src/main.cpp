#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <sstream>

#include <algorithm>

#include "Helper.h"


namespace
{
std::string getHelp()
{
    std::stringstream str;

    str << "1 аргумент - файл, содержимое которого требуется отсортировать;\n"
           "2 аргумент - тип сортировки (по возрастанию - 1, по убыванию - 0);\n"
           "3 аргумент - объем выделяемой памяти, мб;\n"
           "4 аргумент - максимальное количество потоков.\n"
           "             (максимальный объем памяти будет поделен между потоками)\n\n"
           "Пример использования:\n"
           "    sort in.txt 1 200 4";

    return str.str();
}
}  // end of unnamed namespace


int main(int argc, char **argv)
{
    // std::vector<int> a = {-2, -3, 4};
    // auto min = std::min_element(a.begin(), a.end());
    // // *min = 55;
    // for (const auto &i : a)
    // {
    //     std::cout << i << ' ';
    // }
    // return 1;

    // if (argc != 5)
    // {
    //     std::cout << "Программа была запущена с некорректными аргументами.\n\n"
    //               << getHelp() << std::endl;
    //     return 1;
    // }

    using CharType = Helper::CharType;
    const std::string filename = "out.bin";

    //
    // создаем файл
    //
    std::fstream file;
    {
        file.open(filename,
                  std::ios_base::out | std::ios_base::trunc | std::ios::binary);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<Helper::CharType> r(std::numeric_limits<Helper::CharType>::min());

        while (file.tellg() < (7 * 1024 * 1024))
        {
            const Helper::CharType num = r(gen);
            Helper::File::writeOneItem(file, num);
        }

        // for (CharType i = 0; i < 10; i++)
        // {
        //     const auto num = std::numeric_limits<CharType>::max() - i;
        //     Helper::File::writeOneItem(file, num);
        //     std::cout << num << std::endl;
        // }
        //
        // std::vector<CharType> vec(10);
        //
        // for (CharType i = 0; i < 10; i++)
        // {
        //     vec[i] = i;
        //     std::cout << i << std::endl;
        // }
        //
        // Helper::File::write<CharType>(file, vec);

        std::cout << "end of generating file\n" << std::endl;
        std::flush(std::cout);

        file.close();
    }

    //
    // сортируем
    //
    {
        Helper::sort(filename, Helper::SortType::ASC, 4, 2);
        // return 1;
    }

    //
    // проверяем
    //
    {
        file.open(filename + ".sorted", std::ios_base::in | std::ios_base::binary);

        file.seekg(0, std::ios_base::end);
        const auto length = file.tellg();
        std::cout << "length: " << length << std::endl;
        file.seekg(0);

        unsigned i = 0;
        CharType prevValue = std::numeric_limits<CharType>::min(); // если сортировка по возрастанию
        // CharType prevValue = std::numeric_limits<CharType>::max(); // если сортировка по убыванию

        while (file.tellg() < length)
        {

            const auto value = Helper::File::read<CharType>(file, 1)[0];

            // std::cout << std::setw(4) << i++ << " | " << "tellg() = " << file.tellg() << " | ";
            // std::cout << value << std::endl;

            if (prevValue > value)
            {
                std::cout << std::hex << prevValue << " > " << value
                          << "\nERROR: not sorted!" << std::endl;

                std::cout << "last pos " << std::dec << file.tellp() << std::endl;
                return 1;
            }

            prevValue = value;
        }

        std::cout << "SUCCESS: sorted" << std::endl;
        return 0;
    }

    return 0;
}
