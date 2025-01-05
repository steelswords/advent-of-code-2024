#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <fstream>

struct Coordinates
{
    size_t x, y;
    Coordinates(size_t xCoord, size_t yCoord)
        : x(xCoord),
          y(yCoord)
    {
    }
    Coordinates(const Coordinates &other)
        : x(other.x),
          y(other.y) { }
};

struct MapSize
{
    size_t height, width;
    MapSize(size_t mapHeight, size_t mapWidth)
        : height(mapHeight), width(mapWidth) { }
};
std::ostream &operator<<(std::ostream &out, const MapSize &mapSize)
{
    out << "<width=" << mapSize.width << ", height=" << mapSize.height << ">";
    return out;
}

std::ostream &operator<<(std::ostream &out, Coordinates &coords)
{
    out << "(" << coords.x << ", " << coords.y << ")";
    return out;
}

enum GuardDirection : char
{
    Up = '^',
    Down = 'v',
    Right = '>',
    Left = '<',
};

constexpr char INVALID_SQUARE_RESULT = 'F';

class Floorplan
{
public:
    Coordinates guardLocation;

    MapSize GetMapSize() const
    {
        size_t mapWidth = 0;
        if (map.size() >= 1)
        {
            // Assuming a consistant width.
            mapWidth = map[0].size();
        }
        return MapSize(map.size(), mapWidth);
    }

    char GetGuard() const
    {
        char result = GetSquare(guardLocation);
        if (result == INVALID_SQUARE_RESULT)
        {
            std::stringstream ss;
            ss << "Out of bounds: Tried to access ("<< guardLocation.x << ", " << guardLocation.y << ") in a map of size " << GetMapSize();
            throw std::runtime_error(ss.str());
        }
       return result;
    }

    char GetSquare(Coordinates coords) const
    {
        if (map.size() <= coords.y || map[0].size() <= coords.x)
        {
            return INVALID_SQUARE_RESULT;
        }
        return map[coords.y][coords.x];
    }

    void SetSquare(Coordinates coords, char value)
    {
        if (INVALID_SQUARE_RESULT != GetSquare(coords))
        {
            map[coords.y][coords.x] = value;
        }
        else
        {
            std::cerr << "ERROR: Got request to set invalid square " << coords <<  " to '" << value << "'\n";
            return;
        }
    }

    // Returns true if outCoordinates is valid.
    // Stores the coordinates of the square in `direction` direction from the `startingPoint` in `outCoordinates`.
    bool CalculateSquareCoordinates(Coordinates startingPoint, GuardDirection direction, Coordinates &outCoordinates) const
    {
        std::cout << __func__ << " called with startingPoint " << startingPoint << ", direction = " << (char)direction << "\n";
        outCoordinates = startingPoint;
        // Input validation
        MapSize mapSize = GetMapSize();
        if (startingPoint.x >= mapSize.width || startingPoint.y >= mapSize.height)
        {
            std::cerr << __func__ << ": invalid parameters: startingPoint = " << startingPoint << ", direction = " << (char)direction << ", but size of map is " << GetMapSize() << "\n";
            return false;
        }

        switch(direction)
        {
            case GuardDirection::Up:
                if (startingPoint.y == 0)
                {
                    return false;
                }
                else
                {
                    outCoordinates.y--;
                    return true;
                }
                break;
            case GuardDirection::Down:
                if (startingPoint.y + 1 == mapSize.height)
                {
                    return false;
                }
                else
                {
                    outCoordinates.y++;
                    return true;
                }
                break;
            case GuardDirection::Left:
                if (startingPoint.x == 0)
                {
                    return false;
                }
                else
                {
                    outCoordinates.x--;
                    return true;
                }
                break;
            case GuardDirection::Right:
                if (startingPoint.x + 1 == mapSize.width)
                {
                    return false;
                }
                outCoordinates.x++;
                return true;
                break;
            default:
                std::cerr << "Something has gone horribly, horribly wrong. Guard direction is " << (char)direction << "\n";
                exit(EXIT_FAILURE);
                return false;
                break;
        }
    }

    // Each row is a string. The 0th string in this vector is the top row.
    std::vector<std::string> map;

    Floorplan(const std::string &filePath)
        : guardLocation(0,0)
    {
        std::ifstream file(filePath);
        if (!file)
        {
            throw std::runtime_error("Cannot open " + filePath);
        }

        std::string line;
        bool didFindGuard = false;
        while (std::getline(file, line))
        {
            map.push_back(line);
            // See if we have found the guard in this line.
            for (char guardChar : {GuardDirection::Up, GuardDirection::Down, GuardDirection::Right, GuardDirection::Left})
            {
                size_t guardXPosition = line.find(guardChar);
                if (guardXPosition != std::string::npos)
                {
                    if (didFindGuard)
                    {
                        std::cerr << "Something fishy happened here. This should not be possible. Malformed input?" << std::endl;
                        throw std::runtime_error("Found two guards when one was expected!");
                    }
                    didFindGuard = true;
                    size_t yPosition = map.size() - 1; // If we have one line, we want this to be 0, and so on.
                    guardLocation = Coordinates(guardXPosition, yPosition);
                }
            }
        }
        if (!didFindGuard)
        {
            std::cerr << "ERROR: Could not find guard!" << std::endl;
            throw std::runtime_error("Could not find guard");
        }

        std::cout << "Constructed Floorplan" << std::endl;
    }

    void RotateGuard90Degrees(GuardDirection &direction)
    {
        switch (direction)
        {
            case GuardDirection::Up:
                direction = GuardDirection::Right;
                break;
            case GuardDirection::Right:
                direction = GuardDirection::Down;
                break;
            case GuardDirection::Down:
                direction = GuardDirection::Left;
                break;
            case GuardDirection::Left:
                direction = GuardDirection::Up;
                break;
            default:
                std::cerr << "ERROR: Invalid GuardDirection: " << static_cast<char>(direction) << std::endl;
                break;
        }
    }

    void StepGuardThrough()
    {
        // Check the terrain in front of the guard
        char terrainInFrontOfGuard = '\0';
        GuardDirection facingDirection = static_cast<GuardDirection>(GetGuard());
        Coordinates nextSquareCoords(0,0);
        while(CalculateSquareCoordinates(guardLocation, facingDirection, nextSquareCoords))
        {
            char nextSquare = GetSquare(nextSquareCoords);
            Coordinates oldGuardLocation = guardLocation;
            switch (nextSquare)
            {
                // Clear => progress guard. Set last square to 'X'
                case '.':
                case 'X':
                    SetSquare(nextSquareCoords, static_cast<char>(facingDirection));
                    SetSquare(oldGuardLocation, 'X');
                    guardLocation = nextSquareCoords;
                    break;
                // Obstacle => Turn right 90 degrees. Return true
                case '#':
                    RotateGuard90Degrees(facingDirection);
                    SetSquare(guardLocation, static_cast<char>(facingDirection));
                    break;
                // Off map => remove guard, set last square to 'X'. Return.
                case INVALID_SQUARE_RESULT:
                    SetSquare(guardLocation, 'X');
                    break;
                default:
                    std::cerr << "Unexpected square value: " << nextSquare << "@ " << nextSquareCoords <<  "\n";
                    std::cout << "Floorplan looks like this:\n\n" << *this << std::endl;

                    std::cout << "Press Enter key to continue. ";
                    std::string s;
                    std::getline(std::cin, s, '\n');
                    break;
            }
        }
        // Assume we went off the map. So we need to set the last location to an 'X'
        SetSquare(guardLocation, 'X');
    }

    uint64_t GetNumberOfDistinctPositionsGuardVisited() const
    {
        uint64_t result = 0;
        for (const std::string &line : map)
        {
            for (const char c : line)
            {
                if (c == 'X')
                    result++;
            }
        }
        return result;
    }

    friend std::ostream &operator<<(std::ostream &out, Floorplan &floorplan);
};

std::ostream &operator<<(std::ostream &out, Floorplan &floorplan)
{
    out << "Floorplan\n"
        << "Guard @ " << floorplan.guardLocation << "\n"
        << "Map: \n"
        << "=============================================\n";
    for (const std::string &line : floorplan.map)
    {
        out << line << "\n";
    }
    out << "=============================================\n";
    return out;
}


int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <inputfile>" << std::endl;
        exit(EXIT_FAILURE);
    }

    Floorplan floorplan(argv[1]);

    std::cout << floorplan << std::endl;

    floorplan.StepGuardThrough();

    std::cout << "Now the floorplan looks like this:\n\n" << floorplan << std::endl;

    std::cout << "The guard visited " << floorplan.GetNumberOfDistinctPositionsGuardVisited() << " distinct positions.\n";

}
