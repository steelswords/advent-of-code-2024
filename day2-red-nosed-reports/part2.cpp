#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <functional>


class ReactorReport
{
public:
    ReactorReport(const std::string &line);
    ~ReactorReport() = default;
    std::vector<int> levels {};

    // Returns true if all levels are increasing
    bool AreAllLevelsIncreasing() const;

    // Returns true if all levels are decreasing
    bool AreAllLevelsDecreasing() const;

    // Returns true if the differences between levels are at least one and at most three
    bool AreAllLevelChangesGradual() const;

    // Returns true if IsSafe() with at most 1 level removed.
    bool IsSafeWithTolerance() const;

    // Checks if the isValidPredicate is valid for all of the pairs of levels or
    // all pairs, but with one level removed.
    bool CheckConditionWithTolerance(std::function<bool(int,int)> isValidPredicate, std::string debugMessage = "?") const;

    bool IsSafe() const;
    friend std::ostream &operator<<(std::ostream &out, const ReactorReport &report);
};

std::ostream &operator<<(std::ostream &out, const ReactorReport &report)
{
    bool firsttime = true;
    for (int i : report.levels)
    {
        if (firsttime)
        {
            firsttime = !firsttime;
        }
        else
        {
            out << " ";
        }
        out << i;
    }
    return out;
}

struct SafetyReport
{
    std::vector<ReactorReport> unsafeReports;
    std::vector<ReactorReport> safeReports;
};

bool ReactorReport::IsSafe() const
{
    return AreAllLevelChangesGradual() && (AreAllLevelsDecreasing() || AreAllLevelsIncreasing());
}

bool ReactorReport::CheckConditionWithTolerance(std::function<bool(int,int)> isValidPredicate, std::string debugMessage) const
{
    std::cout << "Checking condition " << debugMessage << " on report " << *this << std::endl;
    bool doesConditionHold = true;
    bool doesConditionHoldWithTolerance = true;
    bool neededARemoval = false;
    std::vector<int>::const_iterator it = levels.cbegin();
    std::vector<int>::const_iterator prevIt = levels.cbegin();
    for (; it < levels.cend() && prevIt < levels.cend(); it++, prevIt++)
    {
        if (isValidPredicate(*prevIt, *it))
        {
            std::cout << "Valid for " << *prevIt << " to " << *it << std::endl;
            continue;
        }
        else
        {
            std::cout << "INvalid for " << *prevIt << " to " << *it << "; checking if we skip " << *it << std::endl;
            doesConditionHold = false;
            std::vector<int>::const_iterator nextIt = it + 1;
            if (nextIt < levels.cend())
            {
                std::cout << "     Checking " << *prevIt << " to " << *nextIt << "... ";
                // In this if statement we can check if we skip "this" and move
                // to the "next" if the condition holds.
                if (!isValidPredicate(*prevIt, *nextIt))
                {
                    std::cout << " NOT valid!" << std::endl;
                    doesConditionHoldWithTolerance = false;
                    continue;
                }
                else if (!neededARemoval) // Not valid predicate && haven't needed a removal yet.
                {
                    std::cout << " valid!" << std::endl;
                    doesConditionHoldWithTolerance = true;
                    neededARemoval = true;
                    continue;
                }
            }
            else // Can't get a next iterator.
            {
                std::cout << "At end of line, can't check if we skip." << std::endl;
                doesConditionHoldWithTolerance = false;
                return false;
            }
#if 0
            std::vector<int>::const_iterator prevPrevIt = prevIt - 1;
            if (prevPrevIt >= levels.cbegin() && prevPrevIt < levels.cend())
            {

            }
#endif
        }
    }

    return doesConditionHold || doesConditionHoldWithTolerance;
}

bool ReactorReport::IsSafeWithTolerance() const
{
    auto isChangeGradual = [](int first, int second) {
        int difference = std::abs(second - first);
        bool valid = difference >= 1 && difference <= 3;
        return valid;
    };

    return CheckConditionWithTolerance(isChangeGradual) &&
        ( CheckConditionWithTolerance([](int a, int b) { return a < b; }) ||
          CheckConditionWithTolerance([](int a, int b) { return a > b; })
        );
}

ReactorReport::ReactorReport(const std::string &line)
{
    int level = 0;
    std::stringstream ss;
    ss << line;
    while (ss >> level)
    {
        levels.push_back(level);
    }

    std::reverse(levels.begin(), levels.end());
}

bool ReactorReport::AreAllLevelsDecreasing() const
{
    auto prevIt = levels.cbegin();
    for (auto it = levels.cbegin(); it < levels.cend(); prevIt = it, it++)
    {
        if (it == levels.cbegin())
        {
            continue;
        }
        // Now we are guaranteed that we have a prevIt
        if (*prevIt >= *it)
        {
            return false;
        }
    }
    return true;
}

bool ReactorReport::AreAllLevelsIncreasing() const
{
    auto prevIt = levels.cbegin();
    for (auto it = levels.cbegin(); it < levels.cend(); prevIt = it, it++)
    {
        if (it == levels.cbegin())
        {
            continue;
        }
        // Now we are guaranteed that we have a prevIt
        if (*prevIt <= *it)
        {
            return false;
        }
    }
    return true;
}

bool ReactorReport::AreAllLevelChangesGradual() const
{
    auto prevIt = levels.cbegin();
    for (auto it = levels.cbegin(); it < levels.cend(); prevIt = it, it++)
    {
        // TODO: Edge case in all of these: What about a 1 length? Or a 2 length?
        if (it == levels.cbegin())
        {
            continue;
        }
        // Now we are guaranteed that we have a prevIt
        int difference = std::abs(*it - *prevIt);
        if ( difference < 1 || difference > 3)
        {
            return false;
        }
    }
    return true;
}

SafetyReport CheckSafety(const std::vector<ReactorReport> reports)
{
    SafetyReport safetyReport;
    for (const auto &report : reports)
    {
        if (report.IsSafeWithTolerance())
        {
            safetyReport.safeReports.emplace_back(report);
        }
        else
        {
            safetyReport.unsafeReports.emplace_back(report);
        }
    }
    return safetyReport;
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <input.txt>" << std::endl;
        return 1;
    }

    std::vector<ReactorReport> reactorData{};

    std::ifstream file(argv[1]);
    for (std::string line; std::getline(file, line); )
    {
        reactorData.emplace_back(ReactorReport(line));
    }

    std::cout << "Reports: " << std::endl;
    for (auto report : reactorData)
    {
        for (auto level : report.levels)
        {
            std::cout << level << " ";
        }
        std::cout << "\n";
    }
    std::cout << std::endl;


    SafetyReport safetyReport = CheckSafety(reactorData);

    std::cout << "There are " << safetyReport.safeReports.size() << " safe reports in this data.\n"
        << "There are " << safetyReport.unsafeReports.size() << " unsafe reports in this data, for a total of \n"
        << reactorData.size() << " reports in total." << std::endl;

    return 0;
}
