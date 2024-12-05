#include <iomanip>
#include <iostream>
#include <fstream>
#include <regex>
#include <vector>

class MultiplyInstruction
{
    MultiplyInstruction(const std::string &representation);
};

MultiplyInstruction::MultiplyInstruction(const std::string &representation)
{

}

int main(int argc, char* argv[])
{

    auto multiplyInstructionRegex = std::regex("mul\\(([0-9]{1,3}),([0-9]{1,3})\\)");

    std::string input(argv[1]);
    std::sregex_iterator it(input.begin(), input.end(), multiplyInstructionRegex);
    std::sregex_iterator endOfMatches; // Default ctor makes an end-of-sequence iterator.
    int count = 0;
    for (; it != endOfMatches; it++)
    {
        const std::smatch &match = *it;
        std::cout << "Match [" << count++ << "]: "
            << std::quoted(match.str()) << std::endl;
    }

    if (count == 0)
    {
        std::cout << "no matches found" << std::endl;
    }


}
