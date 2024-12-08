#include <iostream>
#include <fstream>
#include <vector>
#include <boost/log/trivial.hpp>

class WordSearch
{
public:
    WordSearch(const std::string &inputFileName);
    using LineT = std::string;

    std::vector<LineT> lines {};

    friend std::ostream &operator<<(std::ostream &out, const WordSearch &obj);

    struct Coordinate
    {
        size_t x {0};
        size_t y {0};
    };

    struct WordSearchMatch
    {
        std::vector<Coordinate> coordinates;
    };

    using WordSearchMatches = std::vector<WordSearchMatch>;
    std::vector<WordSearchMatch> FindMatches(const std::string &searchTerm) const;
};

std::ostream &operator<<(std::ostream &out, const WordSearch &obj)
{
    for (const WordSearch::LineT &line : obj.lines)
    {
        for (char c : line)
        {
            out << c << " ";
        }
        out << "\n";
    }
    return out;
}

WordSearch::WordSearchMatches WordSearch::FindMatches(const std::string &searchTerm) const
{
    WordSearchMatches matches {};
    // TODO
    return matches;
}

WordSearch::WordSearch(const std::string &inputFileName)
{
    std::ifstream file(inputFileName);
    std::string line = "";
    while (std::getline(file, line))
    {
        lines.push_back(line);
    }
    std::cout << "Constructed WordSearch object. Here it is:\n\n"
        << *this << "=============" << std::endl;

}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <input.txt>" << std::endl;
        return 1;
    }

    WordSearch puzzle(argv[1]);
}
