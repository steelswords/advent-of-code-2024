#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <boost/log/trivial.hpp>

enum class MatchDirection
{
    HorizontalToRight,
    HorizontalToLeft,
    VerticalToDown,
    VerticalToUp,
    DiagonalStartLeftGoDown,
    DiagonalStartLeftGoUp,
    DiagonalStartRightGoDown,
    DiagonalStartRightGoUp,
};
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
        Coordinate(size_t xCoord, size_t yCoord)
            : x(xCoord), y(yCoord)
        { }
    };

    struct WordSearchMatch
    {
        Coordinate startPoint;
        MatchDirection direction;

        WordSearchMatch(size_t xCoord, size_t yCoord, MatchDirection direction)
            : startPoint(xCoord, yCoord),
              direction(direction)
        { }
    };

    /** Checks in `direction` to see if the next few characters match `searchTerm`
     * starting at `x`, `y`. Returns true if it is a match, false if not a match
     * or not enough room. */
    bool IsThereAMatch(size_t x, size_t y, const std::string &searchTerm, MatchDirection direction) const;

    using WordSearchMatches = std::vector<WordSearchMatch>;
    std::vector<WordSearchMatch> FindMatches(const std::string &searchTerm) const;

    std::vector<WordSearch::WordSearchMatch> FindMatchesInAssembledString(const std::string &searchArea, const std::string &searchTerm, MatchDirection directionIfFound, size_t startingX, size_t startingY);
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

// Returns a coordinate of a match along an arbitrary searchArea string given the searchArea string's begining
// x,y coordinates, the direction, and how far along the searchArea string the match was found.
static WordSearch::Coordinate GetCoordinate(size_t startX, size_t startY, MatchDirection direction, size_t length)
{
    size_t resultX = startX;
    size_t resultY = startY;
    switch (direction)
    {
        case MatchDirection::HorizontalToRight:
            resultX += length;
            break;
        case MatchDirection::HorizontalToLeft:
            resultX -= length;
            break;
        case MatchDirection::VerticalToUp:
            resultY -= length;
            break;
        case MatchDirection::VerticalToDown:
            resultY += length;
            break;
        case MatchDirection::DiagonalStartLeftGoUp:
            resultX += length;
            resultY -= length;
            break;
        case MatchDirection::DiagonalStartRightGoUp:
            resultX -= length;
            resultY -= length;
            break;
        case MatchDirection::DiagonalStartLeftGoDown:
            resultX += length;
            resultY += length;
            break;
        case MatchDirection::DiagonalStartRightGoDown:
            resultX -= length;
            resultY += length;
            break;
        default:
            std::cerr << "ERROR: Unexpected MatchDirection!" << std::endl;
            exit(EXIT_FAILURE);
            break;
    }
    return WordSearch::Coordinate(resultX, resultY);
}

std::vector<WordSearch::WordSearchMatch> WordSearch::FindMatchesInAssembledString(const std::string &searchArea, const std::string &searchTerm, MatchDirection directionIfFound, size_t startingX, size_t startingY)
{
    std::vector<WordSearch::WordSearchMatch> matches {};
    size_t matchPosition = std::string::npos;
    size_t searchPosition = 0;
    do
    {
        matchPosition = searchArea.find(searchTerm, searchPosition);
        if (matchPosition != std::string::npos)
        {
            Coordinate coordinate = GetCoordinate(startingX, startingY, directionIfFound, matchPosition);
            matches.emplace_back(WordSearchMatch(coordinate.x, coordinate.y, directionIfFound));
            searchPosition = matchPosition + searchTerm.size();
            // TODO: Optimize
        }
    } while (matchPosition != std::string::npos);

#if 0
    // Backwards search
    matchPosition = std::string::npos;
    searchPosition = 0;
    do
    {
        matchPosition = line.find(searchTerm, searchPosition);
        if (matchPosition != std::string::npos)
        {
            matches.emplace_back(WordSearchMatch(matchPosition, lineNumber, MatchDirection::HorizontalToLeft));
            searchPosition = matchPosition + searchTerm.size();
            // TODO: Optimize
        }
    } while (matchPosition != std::string::npos);


    lineNumber++;
#endif

    return matches;
}



#if 0
WordSearch::WordSearchMatches WordSearch::FindMatches(const std::string &searchTerm) const
{
    WordSearchMatches matches {};
    // TODO

    // Go in each direction first...
    // Check each direction for forwards and backwards string.
    // Horizontally
    // Vertically
    // Diagonally, L -> R
    // Diagonally, R -> L        <-- We only have to do two kinds of diagonal since we're checking for the inverse of the searchTerm too.

    std::string invertedSearchTerm = searchTerm;
    std::reverse(invertedSearchTerm.begin(), invertedSearchTerm.end());

    // Horizontally
    size_t lineNumber = 0;
    for( const std::string &line : lines)
    {
        size_t matchPosition = std::string::npos;
        size_t searchPosition = 0;
        do
        {
            matchPosition = line.find(searchTerm, searchPosition);
            if (matchPosition != std::string::npos)
            {
                matches.emplace_back(WordSearchMatch(matchPosition, lineNumber, MatchDirection::HorizontalToRight));
                searchPosition = matchPosition + searchTerm.size();
                // TODO: Optimize
            }
        } while (matchPosition != std::string::npos);

        // Backwards Horizontal search
        matchPosition = std::string::npos;
        searchPosition = 0;
        do
        {
            matchPosition = line.find(searchTerm, searchPosition);
            if (matchPosition != std::string::npos)
            {
                matches.emplace_back(WordSearchMatch(matchPosition, lineNumber, MatchDirection::HorizontalToLeft));
                searchPosition = matchPosition + searchTerm.size();
                // TODO: Optimize
            }
        } while (matchPosition != std::string::npos);

        lineNumber++;
    }

    // TODO: Brittle
    const size_t lineWidth = lines[0].size();
    for (size_t x = 0; x < lineWidth; ++x)
    {
        for (size_t y = 0; y < lines.size(); ++y)
        {
            // x m a s t e
            // y s a m x s
            // x m a y e f
            // x e o i y s
            // e m a e r r
            // s s a s s s
            // s s s s . .
            //
            // lines.size() = 7
            // lineWidth = 6
            // searchWidth = 4
            //
            //
            //
            const size_t searchWidth = searchTerm.size();
            bool canCheckHorizontalToRight = x + searchWidth <= lineWidth;
            bool canCheckHorizontalToLeft = x - searchWidth >= 0;
            bool canCheckVerticalDownwards = y + searchWidth <= lines.size();
            bool canCheckVerticalUpwards = y - searchWidth >= 0;
            bool canCheckDiagonalLeftToDownRight = (canCheckHorizontalToRight && canCheckVerticalDownwards);
            bool canCheckDiagonalLeftToUpRight = canCheckHorizontalToRight && canCheckVerticalUpwards;

            if (canCheckVerticalDownwards)
            {
                std::string searchArea;
                for (size_t suby = y, subx = x;
                        0 <= suby && suby <= lines.size() && 0 <= subx && subx <= lineWidth;
                        ++suby)
                {
                    searchArea.push_back(lines[suby][subx]);
                }
            }
        }
    }

    return matches;
}
#endif

/** If there is room in the `lines` of `ws`, steps `x` and `y` in `direction`. */
bool StepCoordinate(const WordSearch &ws, size_t &x, size_t &y, MatchDirection direction)
{
    size_t maxWidth = ws.lines[0].size();
    size_t maxHeight = ws.lines.size();
    if (y > maxHeight)
        return false;
    if (x > maxWidth)
        return false;

    switch(direction)
    {
        case MatchDirection::HorizontalToRight:
            if (x + 1 > maxWidth)
            {
                return false;
            }
            x++;
            break;
        case MatchDirection::HorizontalToLeft:
            if (x == 0)
            {
                return false;
            }
            x--;
            break;
        case MatchDirection::VerticalToDown:
            if (y + 1 > maxHeight)
            {
                return false;
            }
            y++;
            break;
        case MatchDirection::VerticalToUp:
            if (y == 0)
            {
                return false;
            }
            y--;
            break;
        case MatchDirection::DiagonalStartLeftGoDown:
            if (x + 1 > maxWidth || y + 1 > maxHeight)
                return false;
            x++;
            y++;
            break;
        case MatchDirection::DiagonalStartLeftGoUp:
            if (y == 0 || x +1 > maxWidth)
                return false;
            y--;
            x++;
            break;
        case MatchDirection::DiagonalStartRightGoDown:
            // TODO
            break;
        case MatchDirection::DiagonalStartRightGoUp:
            // TODO
            break;
        default:
            std::cerr << "Can't figure it out! " << __func__ << ":" << __LINE__ << std::endl;
            break;
    }
}


WordSearch::WordSearchMatches WordSearch::FindMatches(const std::string &searchTerm) const
{
    WordSearch::WordSearchMatches matches;
    // TODO: Brittle
    const size_t lineWidth = lines[0].size();
    for (size_t x = 0; x < lineWidth; ++x)
    {
        for (size_t y = 0; y < lines.size(); ++y)
        {
            // x m a s t e
            // y s a m x s
            // x m a y e f
            // x e o i y s
            // e m a e r r
            // s s a s s s
            // s s s s . .
            //
            // lines.size() = 7
            // lineWidth = 6
            // searchWidth = 4
            //
            //
            //
            const size_t searchWidth = searchTerm.size();
            bool canCheckHorizontalToRight = x + searchWidth <= lineWidth;
            bool canCheckHorizontalToLeft = x - searchWidth >= 0;
            bool canCheckVerticalDownwards = y + searchWidth <= lines.size();
            bool canCheckVerticalUpwards = y - searchWidth >= 0;
            bool canCheckDiagonalLeftToDownRight = (canCheckHorizontalToRight && canCheckVerticalDownwards);
            bool canCheckDiagonalLeftToUpRight = canCheckHorizontalToRight && canCheckVerticalUpwards;

            std::vector<MatchDirection> directions = {
                MatchDirection::HorizontalToRight,
                MatchDirection::HorizontalToLeft,
                MatchDirection::VerticalToDown,
                MatchDirection::VerticalToUp,
                MatchDirection::DiagonalStartLeftGoDown,
                MatchDirection::DiagonalStartLeftGoUp,
                MatchDirection::DiagonalStartRightGoDown,
                MatchDirection::DiagonalStartRightGoUp,
            };

            for (auto direction : directions)
            {
                if (IsThereAMatch(x, y, searchTerm, direction))
                {
                    matches.emplace_back(WordSearch::WordSearchMatch(x, y, direction));
                }
            }
        }
    }

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
