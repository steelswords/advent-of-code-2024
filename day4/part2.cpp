#include <iostream>
#include <fstream>
#include <vector>

/* I need code that checks for these configurations:
 * M.S
 * .A.
 * M.S
 *
 * M.M
 * .A.
 * S.S
 *
 * S.S
 * .A.
 * M.M
 */

// Should I look for 'A' characters in the grid, and then check the corners?
// Or should I build out a generic pattern matching machine? I want to do the
// second one because it'd be fun and cool, but I have a lot of ground to cover.

/** Reads from a file, then searches for 'MAS' in an X formation */
class XmasSearch
{
public:
    XmasSearch(const std::string &inputFileName);
    using LineT = std::string;

    std::vector<LineT> lines {};

    friend std::ostream &operator<<(std::ostream &out, const XmasSearch &obj);

    struct Coordinate
    {
        size_t x {0};
        size_t y {0};
        Coordinate(size_t xCoord, size_t yCoord)
            : x(xCoord), y(yCoord)
        { }
        friend std::ostream &operator<<(std::ostream &out, const Coordinate &coord);
    };

    bool CheckACorners(size_t x, size_t y) const;

    bool IsCoordinateValid(Coordinate coord) const
    {
        return lines.size() > coord.y && lines[coord.y].size() > coord.x;
    }

    char GetChar(const size_t x, const size_t y) const
    {
        if (y >= lines.size())
        {
            throw std::invalid_argument("y-coordinate " + std::to_string(y) + " is out of bounds");
        }
        if (x >= lines[y].size())
        {
            throw std::invalid_argument("x-coordinate " + std::to_string(x) + " is out of bounds");
        }
        return lines[y][x];
    }

    size_t GetNumberOfXMases() const;
private:
};

XmasSearch::XmasSearch(const std::string &inputFileName)
{
    std::ifstream file(inputFileName);
    std::string line = "";
    while (std::getline(file, line))
    {
        lines.push_back(line);
    }
    std::cout << "Constructed XmasSearch object. Here it is:\n\n"
        << *this << "=============" << std::endl;

}
std::ostream &operator<<(std::ostream &out, const XmasSearch::Coordinate &coord)
{
    out << "(" << std::to_string(coord.x) << ", " << std::to_string(coord.y) << ")";
    return out;
}
std::ostream &operator<<(std::ostream &out, const XmasSearch &obj)
{
    for (const XmasSearch::LineT &line : obj.lines)
    {
        for (char c : line)
        {
            out << c << " ";
        }
        out << "\n";
    }
    return out;
}

/** Given the coordinates (`x`, `y`) of an 'A' character, checks if the corners
 * constitute an "X MAS". If they do, returns true. */
bool XmasSearch::CheckACorners(size_t x, size_t y) const
{
    try
    {
        char upperLeft = GetChar(x-1, y-1);
        char upperRight = GetChar(x+1, y-1);
        char lowerLeft = GetChar(x-1, y+1);
        char lowerRight = GetChar(x+1, y+1);

        // Big ol' long if statement. The positioning is supposed to make it easier
        // to see what's going on.
        // clang-format off
        if (
                (upperLeft == 'M' &&       upperRight == 'S' &&
                                  // 'A'
                lowerLeft == 'M' &&      lowerRight == 'S')
           ||
           (
                (upperLeft == 'M' &&       upperRight == 'M' &&
                                  // 'A'
                lowerLeft == 'S' &&      lowerRight == 'S')
           ) ||
           (
                (upperLeft == 'S' &&       upperRight == 'M' &&
                                  // 'A'
                lowerLeft == 'S' &&      lowerRight == 'M')
           ) ||
           (
                (upperLeft == 'S' &&       upperRight == 'S' &&
                                  // 'A'
                lowerLeft == 'M' &&      lowerRight == 'M')
           )
        )
        {
            return true;
        }
        else
        {
            return false;
        }
        // clang-format on
    }
    catch (std::invalid_argument &e)
    {
        std::cerr << "Could not access coordinate " << XmasSearch::Coordinate(x,y) << std::endl;
    }
}

size_t XmasSearch::GetNumberOfXMases() const
{
    size_t numberOfXmases = 0;
    // Find all the 'A' characters
    size_t x = 1,
           y = 1;

    // TODO: Think about these -1s.
    size_t maxX = lines[0].size() - 1;
    size_t maxY = lines.size() - 1;
    for (; y < maxY; y++)
    {
        for (x = 1; x < maxX; x++)
        {
            Coordinate coord = Coordinate(x,y);
            // Check corners around A. If it's a valid X-MAS, add to total
            try
            {
                if ('A' == GetChar(x, y))
                {
                    std::cout << "Found 'A' at " << coord << "\n";
                    if (CheckACorners(x,y))
                    {
                        std::cout << "-> Found XMAS @ " << coord << "\n";
                        numberOfXmases++;
                    }
                    else
                    {
                        std::cout << "< No XMAS @ " << coord << "\n";
                    }
                }
            }
            catch (std::invalid_argument &e)
            {
                std::cerr << "Could not access coordinate " << Coordinate(x,y) << std::endl;
            }
            std::cout << "Done checking " << coord << std::endl;
        }
        std::cout << "Done with line " << std::to_string(y) << '\n';
    }
    return numberOfXmases;
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <input.txt>" << std::endl;
        return 1;
    }

    XmasSearch search(argv[1]);

    size_t numberOfXmases = search.GetNumberOfXMases();
    std::cout << "There are " << numberOfXmases << "X MASes" << std::endl;



}
