#include <cassert>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <set>

struct TwoListsOfInts
{
public:
    using SetType = std::multiset<int>;

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

    long long TotalDistances() const;

    /* From the problem:
     * Calculate a total similarity score by adding up each number in the left
     * list after multiplying it by the number of times that number appears in
     * the right list.
     */
    long long GetSimilarityScore() const;


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

long long TwoListsOfInts::TotalDistances() const
{
    // This simplifies things a bit.
    assert(first.size() == second.size());

    long long total = 0;
    using ItType = TwoListsOfInts::SetType::const_iterator;
    for ( ItType firstIt = first.cbegin(), secondIt = second.cbegin();
            firstIt != first.cend() && secondIt != second.cend();
            firstIt++, secondIt++)
    {
        total += std::llabs(*firstIt - *secondIt);
    }
    return total;
}

long long TwoListsOfInts::GetSimilarityScore() const
{
    long long similarityScore = 0;

    for (int firstItem : this->first)
    {
        long long thisItemsSimilarityScore = firstItem * this->second.count(firstItem);
        similarityScore += thisItemsSimilarityScore;
    }

    return similarityScore;
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

    std::cout << "The total distance is: " << lists.TotalDistances() << std::endl;

    std::cout << "\n\nThe similarity score between the two lists is: " << lists.GetSimilarityScore() << std::endl;
    return 0;
}
