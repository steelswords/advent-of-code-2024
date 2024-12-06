#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <regex>
#include <sstream>
#include <iterator>

class MultiplyInstruction
{
public:
    MultiplyInstruction(const std::string &representation);
    MultiplyInstruction(long _first, long _second)
        : first(_first),
          second(_second)
    {
    }

    MultiplyInstruction(const std::string &firstStr, const std::string &secondStr)
    {
#if 0
        first = std::stoi(firstStr);
        second = std::stoi(secondStr);
#else
        std::stringstream ss;
        ss << firstStr;
        ss >> this->first;
        ss.clear();
        ss.str("");

        ss << secondStr;
        ss >> second;
#endif
    }

    long first {0};
    long second {0};

    friend std::ostream &operator<<(std::ostream &out, const MultiplyInstruction op)
    {
        out << "<mul " << op.first << " by " << op.second << ">";
        return out;
    }

    long long GetResult() { return first * second; }
};

/** Takes a string in the form 'mul(x,y)' where x and y are 1-3 digit numbers */
MultiplyInstruction::MultiplyInstruction(const std::string &representation)
{
    std::stringstream ss;
    // Could use regex to get these directly, but I know how to do this off the
    // top of my head
    int tmp;
    ss >> tmp;
    this->first = tmp;
    ss >> second;
}

int main(int argc, char* argv[])
{

    auto multiplyInstructionRegex = std::regex("mul\\(([0-9]{1,3}),([0-9]{1,3})\\)");

    std::ifstream file(argv[1]);
#if 0
    std::string fileContents;
    while (file >> fileContents)
    {
        ;
    }
#endif

    std::string fileContents(std::istreambuf_iterator<char>{file}, {});

    std::sregex_iterator it(fileContents.begin(), fileContents.end(), multiplyInstructionRegex);
    std::sregex_iterator endOfMatches; // Default ctor makes an end-of-sequence iterator.
    int count = 0;

    long long totalResult = 0;

    for (; it != endOfMatches; it++)
    {
        const std::smatch &match = *it;

        std::cout << "Match size: " << match.size() << ", "
            << "[0] = " << match[0] << ", "
            << "[1] = " << match[1] << ", "
            << "[2] = " << match[2] << "    ";

        if (match.size() != 3)
        {
            std::cerr << "-=----------------------> That was weird." << std::endl;
            continue;
        }


        MultiplyInstruction instruction(match[1], match[2]);

        std::cout << "Match [" << count++ << "]: "
            << std::quoted(match.str())  << " aka " << instruction << std::endl;

        totalResult += instruction.GetResult();
    }

    if (count == 0)
    {
        std::cout << "no matches found" << std::endl;
    }

    std::cout << "=> Total = " << totalResult << std::endl;

}
