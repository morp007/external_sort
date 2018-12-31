#include <iostream>
#include <sstream>


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
    if (argc != 5)
    {
        std::cout << "Программа была запущена с некорректными аргументами.\n\n"
                  << getHelp() << std::endl;
        return 1;
    }

    return 0;
}
