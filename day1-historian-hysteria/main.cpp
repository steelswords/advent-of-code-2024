#include <iostream>
#include <fstream>
#include <set>

struct TwoListsOfInts
{
public:
    using SetType = std::set<int>;

    SetType first;
    SetType second;

    TwoListsOfInts() = default;
    ~TwoListsOfInts() = default;
    TwoListsOfInts(const TwoListsOfInts &other)
    {
        this->first = other.first;
        this->second = other.second;
    }

    void InitFromFile(const std::string &fileName);


    friend std::ostream& operator<<(std::ostream& out, const TwoListsOfInts &lists);
};

void TwoListsOfInts::InitFromFile(const std::string &fileName)
{
    std::ifstream file(fileName);
    int firstItem, secondItem;
    while (file >> firstItem >> secondItem)
    {
        first.insert(firstItem);
        second.insert(secondItem);
    }
}

std::ostream& operator<<(std::ostream& out, const TwoListsOfInts &lists)
{
    out << "TwoListsOfInts @ " << std::hex << &lists << std::dec << "\n"
        << "=============================\n";
    TwoListsOfInts::SetType::const_iterator firstIt = lists.first.begin();
    TwoListsOfInts::SetType::const_iterator secondIt = lists.second.cbegin();
    while (firstIt != lists.first.cend() || secondIt != lists.second.cend())
    {
        if (firstIt != lists.first.cend())
        {
            out << *firstIt;
            firstIt++;
        }
        else
        {
            out << "     ";
        }

        // Divider
        out << "\t|\t";

        if (secondIt != lists.second.cend())
        {
            out << *secondIt << "\n";
            secondIt++;
        }
        else
        {
            out << "\n";
        }
    }
    return out;
}


int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <input.txt>" << std::endl;
        return 1;
    }

    TwoListsOfInts lists;
    lists.InitFromFile(argv[1]);

    std::cout << "The list is as follows:\n" << lists << std::endl;
    return 0;
}
