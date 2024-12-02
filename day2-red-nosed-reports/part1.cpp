#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>


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

    bool IsSafe() const;
};

struct SafetyReport
{
    std::vector<ReactorReport> unsafeReports;
    std::vector<ReactorReport> safeReports;
};

bool ReactorReport::IsSafe() const
{
    return AreAllLevelChangesGradual() && (AreAllLevelsDecreasing() || AreAllLevelsIncreasing());
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
        if (report.IsSafe())
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
