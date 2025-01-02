#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <unordered_map>
#include <set>
#include <sstream>
#include <utility>
#include <vector>
#include <random>
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

    int GetMiddleNumber()
    {
        if (pagesToUpdate.size() %2 == 0)
        {
            throw std::runtime_error("Cannot get middle number of this update: there are " + std::to_string(pagesToUpdate.size()) + " elements, not an odd number.");
            return -1;
        }
        if (pagesToUpdate.size() < 1)
        {
            throw std::runtime_error("Cannot get middle number of this update: there are " + std::to_string(pagesToUpdate.size()) + " elements, not enough.");
            return -2;
        }
        size_t middleIndex = (pagesToUpdate.size() - 1) / 2;
        return pagesToUpdate[middleIndex];
    }

    bool IsUpdatePageValid(const OrderingRequirements &requirements, size_t positionToCheck) const
    {
        // TODO: Check that we don't fall off the end here.
        if (positionToCheck >= pagesToUpdate.size())
        {
            throw std::runtime_error("Error: tried to check validity of page that is out of bounds.");
        }
        int page = pagesToUpdate[positionToCheck];
        std::vector<int>::const_iterator pageIt = pagesToUpdate.cbegin() + positionToCheck;

        // Check before requirements
        for (int pageThatNeedsToComeBefore : requirements.pagesNeededBeforeThis)
        {
            if (std::find(pageIt, pagesToUpdate.end(), pageThatNeedsToComeBefore) != pagesToUpdate.end())
            {
                //std::cerr << "Page " << page << " is invalid here. It requires that page " << *pageIt << " comes before it, and it comes after it.\n";
                return false;
            }
        }

        // Check after requirements
        for (int pageThatNeedsToComeAfter : requirements.pagesNeededAfterThis)
        {
            // If it's found before this page, the update is not valid.
            if (std::find(pagesToUpdate.begin(), pageIt, pageThatNeedsToComeAfter) != pageIt)
            {
                //std::cerr << "Page " << page << " is invalid here. It requires that page " << *pageIt << " comes after it, and it comes before it.\n";
                return false;
            }
        }
        return true;
    }

    bool IsUpdatePageValid(const OrderingRequirements &requirements, std::vector<int>::const_iterator pageIterator) const
    {
        return this->IsUpdatePageValid(requirements, std::distance(pagesToUpdate.begin(), pageIterator));
    }
};

std::ostream &operator<<(std::ostream& out, std::vector<int> v)
{
    out << "[";
    size_t elementsLeft = v.size(); for (int i : v) {
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

    Update FixOrdering(const Update& invalidUpdate) const;

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
    bool result = true;
    for (auto it = update.pagesToUpdate.begin(); it != update.pagesToUpdate.end(); ++it)
    {
        // If there are no requirements that involve this page, don't return false. This page is fine.
        try
        {
            const OrderingRequirements& orderingReqsForThisPage = orderingRequirements.at(*it);
            if (false == update.IsUpdatePageValid(orderingReqsForThisPage, it))
            {
                return false;
            }
        }
        catch (std::out_of_range &e)
        {
            continue;
        }
    }
    return true;
}

static void shuffleVector(std::vector<int> &vec)
{
    std::random_device device;
    std::mt19937 rng(device());

    // any kind of probability distribution you'd like.
    std::uniform_int_distribution<> dist(0, vec.size() - 2);
    int random_number = dist(rng);

    auto swapIt = vec.begin() + random_number;
    // Swap the last and the random elements
    std::iter_swap(swapIt, vec.end() - 1);
}

Update PrintQueue::FixOrdering(const Update &invalidUpdate) const
{
    // For each page to update, search through  OrderingRequirements. If there are any that don't fit, rearrange.
    Update fixedUpdate = invalidUpdate;

    std::chrono::time_point<std::chrono::steady_clock> startBruteForceTime = std::chrono::steady_clock::now();
    auto lastCheckinTime = startBruteForceTime;
    size_t numberIterations = 0, lastCheckinNumberIterations = 0;
    while (!IsUpdateValid(fixedUpdate))
    {
        // This is really stupid, but I bet it will work.
        // Lolololol this is so bad, so stupid, but I bet it won't take all that long.
        shuffleVector(fixedUpdate.pagesToUpdate);
#if 0
        for (auto pageIt = invalidUpdate.pagesToUpdate.begin(); pageIt != invalidUpdate.pagesToUpdate.end(); pageIt++)
        {
            try 
            {
                // Check before requirements
                for (const auto &beforeRequirement : orderingRequirements)
                {
                    // See if we can find a 
                    if (std::find(fixedUpdate.begin(), fixedUpdate.end(), beforeRequirement) != fixedUpdate.end())
                    {
                    }
                }
            }
            catch (std::out_of_range &e)
            {
            }
        }
#endif
        numberIterations++;
        auto now = std::chrono::steady_clock::now();
        if (now - lastCheckinTime > std::chrono::seconds(2))
        {
            double iterationsPerSecond = (numberIterations - lastCheckinNumberIterations) / (double)(std::chrono::duration_cast<std::chrono::milliseconds>(now - lastCheckinTime).count() / 1000.0f);

            // Let s = pagesToUpdate.size()
            // The number of possible permutations is sPs, or s!.
            // The gamma function gives G(n) = (n-1)! for positive integers,
            // thus the number of permutations is tgamma(s+1), and about how many iterations I would expect.
            double predictedNumberOfIterationsNeeded = tgamma(fixedUpdate.pagesToUpdate.size() + 1);
            double secsTillSuccess = (predictedNumberOfIterationsNeeded - numberIterations) / (iterationsPerSecond);
            std::cout << "-> Still trying to get this update fixed.... Currently looking at " << fixedUpdate.pagesToUpdate
                << ". I've been working on this for " << std::chrono::duration_cast<std::chrono::milliseconds>(now - startBruteForceTime).count()
                << " ms. I've done " << numberIterations - lastCheckinNumberIterations << " iterations since last time, for a total of "
                << numberIterations << " iterations total. That's " << iterationsPerSecond << " iterations per second. I expect to have to do "
                << predictedNumberOfIterationsNeeded << " iterations total, so that means I'll be done with this around.... say.... "
                << secsTillSuccess << "s from now."
                << std::endl;

            lastCheckinTime = now;
            lastCheckinNumberIterations = numberIterations;
        }

    }

    auto endBruteForceTime = std::chrono::steady_clock::now();
    auto bruteForceDuration = endBruteForceTime - startBruteForceTime;
    std::cout << "-> Found corrected update order: " << fixedUpdate.pagesToUpdate
        << ". Brute forcing took " << std::chrono::duration_cast<std::chrono::milliseconds>(bruteForceDuration).count() << " ms" << std::endl;

    return fixedUpdate;

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

    std::vector<Update> validUpdates = {};
    std::vector<Update> invalidUpdates = {};
    for (const auto& update : printQueue.requestedUpdates)
    {
        if (printQueue.IsUpdateValid(update))
        {
            validUpdates.push_back(update);
        }
        else
        {
            std::cout << "Update " << update.pagesToUpdate << " is not valid!" << std::endl;
            invalidUpdates.push_back(update);
        }
    }

    std::cout << "Of the " << printQueue.requestedUpdates.size() << " updates requested, "
        << validUpdates.size() << " are valid and " << invalidUpdates.size() << " are invalid." << std::endl;

    // Find the middle numbers and add them up
    uint64_t sumOfMiddlePageNumbers = 0;
    for (const Update& update : validUpdates)
    {
        size_t middleIndex = (update.pagesToUpdate.size() - 1) / 2;
        sumOfMiddlePageNumbers += update.pagesToUpdate[middleIndex];
    }

    std::cout << "The sum of the middle pages of the valid updates is " << sumOfMiddlePageNumbers << std::endl;

    int64_t sumOfFixedMiddleNumbers = 0;
    std::cout << "Checking the invalid ones now..." << std::endl;
    for (Update& invalidUpdate : invalidUpdates)
    {
        Update fixedUpdate = printQueue.FixOrdering(invalidUpdate);
        int middleNumber = fixedUpdate.GetMiddleNumber();
        sumOfFixedMiddleNumbers += middleNumber;
    }

    std::cout << "Sum of fixed middle numbers: " << sumOfFixedMiddleNumbers << std::endl;

    return 0;
}
