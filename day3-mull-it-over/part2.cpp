#include <cassert>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <regex>
#include <sstream>
#include <iterator>
#include <algorithm>

// Man, this would be much easier with an awk script. But that's the point of this,
// to sharpen my C++ skills.

#if 0
class Instruction
{
public:
    Instruction(std::string::iterator begining);
};

class EnableInstruction : public Instruction
{
public:
    EnableInstruction(std::string::iterator beginning);
};

class DisableInstruction : public Instruction
{
public:
    DisableInstruction(std::string::iterator beginning);
}
#endif

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

std::string IgnoreTextInFileContents(std::ifstream &inFile)
{
    std::string wholeContents(std::istreambuf_iterator<char>{inFile}, std::istreambuf_iterator<char>());

    // Clean up spaces and newlines
    wholeContents.erase(std::remove_if(
                wholeContents.begin(), wholeContents.end(),
                [](unsigned char c) { return std::isspace(c); })
            );

    std::cout << "=======================================================\n"
        << wholeContents << "\n"
        "=======================================================" << std::endl;

    std::string startIgnoreRegex = "don't\\(\\)";
    std::string resumeReadingRegex  = "do\\(\\)";


    std::string ignoredSectionRegex = startIgnoreRegex + ".*?" + resumeReadingRegex;

    std::cout << "---> Removing all text matching this: " << ignoredSectionRegex << std::endl;

    std::string result;

    // We have to use ECMAScript regex engine here because it supports non-greedy .*? matches,
    // while regular POSIC regex does not.
    std::basic_regex re(ignoredSectionRegex, std::regex_constants::ECMAScript);
    result = std::regex_replace(wholeContents, re, "");

    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ The input with ignores removed ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
        << result
        << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n" << std::endl;

    return result;

}

#if 0
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

    std::string fileContents = IgnoreTextInFileContents(file);

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
#endif


int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage " << argv[0] << "<inputfile>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);

    const std::string disableCommandStart = "don't()";
    const std::string enableCommandStart = "do()";
    const std::string multiplyCommandStart = "mul(";
    auto multiplyInstructionRegex = std::regex("mul\\(([0-9]{1,3}),([0-9]{1,3})\\)");
    bool isMultiplyEnabled = true;

    bool looksLikeMul = false;
    char c = '\0';
    std::string chunk = "";
    long long result = 0;
    long numChars = 0;
    while (file.get(c))
    {
        numChars++;
        chunk.push_back(c);
        //std::cout << "Got '" << c << "'. Chunk = " << chunk << std::endl;
        if (chunk.find(disableCommandStart) != std::string::npos)
        {
            std::cout << "-> Disabling instructions chunk = " << chunk << std::endl;
            isMultiplyEnabled = false;
            looksLikeMul = false;
            chunk.clear();
        }
        else if (chunk.find(enableCommandStart) != std::string::npos)
        {
            std::cout << "-> Enabling instructions. chunk = " << chunk << std::endl;
            isMultiplyEnabled = true;
            looksLikeMul = false;
            chunk.clear();
        }
#if 0
        else if (isMultiplyEnabled && chunk.find(multiplyCommandStart) != std::string::npos)
        {
            std::cout << "Looks like mul. chunk = " << chunk << std::endl;
            looksLikeMul = true;
            chunk.clear();
        }
#endif
        else if (isMultiplyEnabled)
        {
            std::sregex_iterator it(chunk.begin(), chunk.end(), multiplyInstructionRegex);
            std::sregex_iterator endOfMatches; // Default ctor makes an end-of-sequence iterator.
            const size_t numOfMatches = std::distance(it, endOfMatches);
            if (numOfMatches != 1 && numOfMatches != 0)
            {
                std::cerr << "Something weird happenend on line " << __LINE__ << ". Found " << numOfMatches << "matches where I really only expected one. " <<  std::endl;
                exit(EXIT_FAILURE);
            }
            for (; it != endOfMatches; it++)
            {
                const std::smatch match = *it;

                if (match.size() !=3)
                {
                    // Haven't found anything yet.
                    continue;
                }

                long a = std::stol(match[1]);
                long b = std::stol(match[2]);
                std::cout << "Result was " << result << ". Multiplying " << a << " and " << b  <<" = " << a * b << ". Now result = ";

                long long oldResult = result;

                result += a * b;
                std::cout << result << std::endl;

                if (result < oldResult)
                {
                    std::cout << "Overflow!!! Need a bigger int." << std::endl;
                    exit(EXIT_FAILURE);
                }

                chunk.clear();
            }
        }

#if 0


            // We're trying to parse it right now.
            std::basic_regex re("([0-9]{1,3}),([0-9]{1,3})\\)");
            std::sregex_iterator it(chunk.begin(), chunk.end(), re), endOfMatchesIt;
            auto numOfMatches = std::distance(it, endOfMatchesIt);
            if (numOfMatches == 1)
            {
                std::smatch match = *it;
                std::cout << "Match size = " << match.size() << ". match = " << match.str() << std::endl;
                assert(match.size() == 3);

                long a = std::stol(match[1]);
                long b = std::stol(match[2]);
                std::cout << "Result was " << result << ". Multiplying " << a << " and " << b  <<" = " << a * b << ". Now result = ";

                long long oldResult = result;

                result += a * b;
                std::cout << result << std::endl;

                if (result < oldResult)
                {
                    std::cout << "Overflow!!! Need a bigger int." << std::endl;
                    exit(EXIT_FAILURE);
                }

                chunk.clear();
                looksLikeMul = false;

            }
            else if (numOfMatches == 0)
            {
                // See if we're waiting for the rest of the mul(xxx,yyy) to come in, or if we're
                // looking at a malformed mul() command.
                if ((chunk.size() > 8))
                {
                    chunk.clear();
                }
                else
                {
                    bool containsIllegalChar = false
                }
            }
            else
            {
                std::cout << "==========> Something W E I R D  happened!" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
#endif
        else
        {
            //std::cout << "* No matching chunk yet." << std::endl;
        }
    }

    std::cout << "Processed " << numChars << " bytes. " << std::endl;
    std::cout << "Result = " << result << std::endl;
}
