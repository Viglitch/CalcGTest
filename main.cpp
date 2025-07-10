#include "SimpleCalculator.h"
#include "InMemoryHistory.h"
#include <iostream>

int main()
{
    calc::InMemoryHistory history;
    calc::SimpleCalculator calculator(history);

    int result = calculator.Add(5, 3);
    std::cout << "5 + 3 = " << result << std::endl;

    auto lastOps = history.GetLastOperations(1);
    for (const auto& op : lastOps)
    {
        std::cout << "Last operation: " << op << std::endl;
    }

    return 0;
}