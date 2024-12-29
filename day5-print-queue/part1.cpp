#include <algorithm>
#include <iostream>
#include <cstdio>
#include <fstream>
#include <stdexcept>
#include <unordered_map>
#include <set>
#include <sstream>
#include <vector>
#include <boost/log/trivial.hpp>

class OrderingRequirements
{
public:
    OrderingRequirements() { }
    std::set<int> pagesNeededBeforeThis {};
    std::set<int> pagesNeededAfterThis {};
};

class Update
{
public:
    std::vector<int> pagesToUpdate {};
};

std::ostream &operator<<(std::ostream& out, std::vector<int> v)
{
    out << "[";
    size_t elementsLeft = v.size();
    for (int i : v)
    {
        out << std::to_string(i);
        if (--elementsLeft > 0)
        {
            out << ", ";
        }
    }
    out << "]";
    return out;
}
std::ostream &operator<<(std::ostream& out, std::set<int> v)
{
    out << "[";
    size_t elementsLeft = v.size();
    for (int i : v)
    {
        out << std::to_string(i);
        if (--elementsLeft > 0)
        {
            out << ", ";
        }
    }
    out << "]";
    return out;
}
class PrintQueue
{
public:
    PrintQueue(const std::string &inputFileName);
    std::vector<Update> requestedUpdates {};
    friend std::ostream &operator<<(std::ostream& out, const PrintQueue &printQueue);

    void AddOrderingRequirement(int beforePage, int afterPage);

    bool IsUpdateValid(const Update& update) const;

    OrderingRequirements& GetRequirementsForPage(int page);

private:
    std::unordered_map<int, OrderingRequirements> orderingRequirements;
};


OrderingRequirements& PrintQueue::GetRequirementsForPage(int page)
{
    if (orderingRequirements.find(page) == orderingRequirements.end())
    {
        orderingRequirements[page] = OrderingRequirements();
    }
    return orderingRequirements[page];
}

bool PrintQueue::IsUpdateValid(const Update &update) const
{
    // This is a crappy algorithm, but I'm just trying to get through these challenges.
    // For each element in the update
    for (auto it = update.pagesToUpdate.begin(); it != update.pagesToUpdate.end(); ++it)
    {
        // If there are no requirements that involve this page, don't return false. This page is fine.
        try
        {
            const OrderingRequirements& orderingReqsForThisPage = orderingRequirements.at(*it);
            for (const auto& beforeRequirement : orderingReqsForThisPage.pagesNeededBeforeThis)
            {
                // If the before requirement exists in the update AFTER the position, return false
                if (std::find(it, update.pagesToUpdate.end(), beforeRequirement) != update.pagesToUpdate.end())
                {
                    return false;
                }
            }
            // For each after requirement
            for (int afterRequirement : orderingReqsForThisPage.pagesNeededAfterThis)
            {
                // If the after requirement exists in the update AFTER the position, return false
                if (std::find(update.pagesToUpdate.begin(), it, afterRequirement) != it)
                {
                    return false;
                }
            }
        }
        catch (std::out_of_range &e)
        {
            continue;
        }
        
    }
    return true;
}

std::ostream &operator<<(std::ostream& out, const PrintQueue &printQueue)
{
    out << "************************************************\n";
    out << "Print Queue @ " << std::hex << &printQueue << std::dec << "\n";
    out << "========================================\n";
    out << " Ordering Requirements\n";
    out << "----------------------------------------\n";
    for (const auto& pair : printQueue.orderingRequirements)
    {
        out << "Page " << pair.first << " must have pages "
            << pair.second.pagesNeededBeforeThis << " come BEFORE it, and pages "
            << pair.second.pagesNeededAfterThis << " to come AFTER it.\n";
    }
    out << "========================================\n";
    out << " Updates requested\n"
        << "----------------------------------------\n";
    for (const Update& update : printQueue.requestedUpdates)
    {
        out << "* " << update.pagesToUpdate << "\n";
    }
    out << "************************************************\n";
    return out;
}

PrintQueue::PrintQueue(const std::string &inputFileName)
{
    std::ifstream file(inputFileName);
    std::string line = "";

    // Loop to parse the ordering requirements
    while (std::getline(file, line))
    {

        int beforePage = 0, afterPage = 0;
        int sscanfResult = sscanf(line.c_str(), "%d|%d", &beforePage, &afterPage);
        if (sscanfResult != 2)
        {
            std::cout << "-> Reached end of ordering requirements section. Proceeding to page updates section." << std::endl;
            break;
        }

        AddOrderingRequirement(beforePage, afterPage);
        std::cout << "Requirement loaded: page " << beforePage << " must come before " << afterPage << "\n";
    }

    // Loop to parse the update requests
    while (std::getline(file, line))
    {
        std::stringstream ss;
        ss << line;
        int pageNumber = -1;
        Update update;
        while (ss >> pageNumber)
        {
            update.pagesToUpdate.push_back(pageNumber);
            char comma;
            ss >> comma;
        }
        requestedUpdates.push_back(update);
    }

    std::cout << "Constructed PrintQueue object." << std::endl;
}

void PrintQueue::AddOrderingRequirement(int beforePage, int afterPage)
{
    // We have to add these in both indices
    GetRequirementsForPage(beforePage).pagesNeededAfterThis.emplace(afterPage);
    GetRequirementsForPage(afterPage).pagesNeededBeforeThis.emplace(beforePage);
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <input.txt>" << std::endl;
        return 1;
    }

    PrintQueue printQueue(argv[1]);

    std::cout << printQueue << std::endl;

    size_t validUpdates = 0;
    for (const auto& update : printQueue.requestedUpdates)
    {
        if (printQueue.IsUpdateValid(update))
        {
            validUpdates++;
        }
        else
        {
            std::cout << "Update " << update.pagesToUpdate << " is not valid!" << std::endl;
        }
    }

    std::cout << "Of the " << printQueue.requestedUpdates.size() << " updates requested, "
        << validUpdates << " are valid." << std::endl;

    return 0;
}
