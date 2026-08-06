/*
 * =====================================================================================
 * CONCEPT        : Basic Standard Template Library (STL) in C++
 * DESCRIPTION    : Comprehensive implementation explaining the core pillars of C++ STL:
 *                  1. Containers           : Sequence container (std::vector) and Associative 
 *                                            container (std::map) for dynamic memory management.
 *                  2. Iterators            : Forward (begin/end), Const (cbegin/cend), and 
 *                                            Reverse (rbegin/rend) iterators for traversing.
 *                  3. Algorithms           : Non-modifying (std::find), modifying (std::sort), 
 *                                            and numeric algorithms (std::accumulate).
 *                  4. Functors & Lambdas   : Inline lambda expressions used with algorithm 
 *                                            callbacks (std::for_each, std::count_if).
 *
 * TIME COMPLEXITY  : Best Case: O(N log N) — Dominated by sorting operations on N elements.
 * SPACE COMPLEXITY : Best Case: O(N)       — Dynamic linear memory storage for N container elements.
 * =====================================================================================
 */

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <numeric>
#include <iomanip>

using namespace std;

/**
 * @brief Demonstrates STL Sequence Containers (std::vector) and Iterator Traversal Mechanics.
 * @param count Number of dynamic elements to generate.
 * @param multiplier Step factor to populate initial values.
 * @return Populated vector instance.
 */
vector<int> demonstrateContainersAndIterators(size_t count, int multiplier) {
    cout << "\n================ 1. CONTAINERS & ITERATORS (std::vector) ================" << endl;
    
    // Dynamic sequence container allocation
    vector<int> vec;
    vec.reserve(count); // Optimizes vector capacity allocation

    for (size_t i = 1; i <= count; ++i) {
        vec.push_back(static_cast<int>(i) * multiplier);
    }

    // 1. Forward Iterator Traversal (cbegin / cend)
    cout << "Forward Iterator Traversal  : ";
    for (vector<int>::const_iterator it = vec.cbegin(); it != vec.cend(); ++it) {
        cout << *it << " ";
    }
    cout << endl;

    // 2. Reverse Iterator Traversal (crbegin / crend)
    cout << "Reverse Iterator Traversal  : ";
    for (vector<int>::const_reverse_iterator rit = vec.crbegin(); rit != vec.crend(); ++rit) {
        cout << *rit << " ";
    }
    cout << endl;

    cout << "Container Size / Capacity   : " << vec.size() << " / " << vec.capacity() << " elements" << endl;
    return vec;
}

/**
 * @brief Demonstrates STL Algorithms (sort, find, accumulate, for_each, count_if) with Lambdas.
 * @param numbers Reference to dynamic vector container.
 * @param target Value to search within the container.
 */
void demonstrateAlgorithmsAndLambdas(vector<int>& numbers, int target) {
    cout << "\n================ 2. STL ALGORITHMS & LAMBDAS ================" << endl;

    // 1. std::find Algorithm
    auto findIt = find(numbers.begin(), numbers.end(), target);
    if (findIt != numbers.end()) {
        cout << "std::find                   : Target " << target << " FOUND at index " 
             << distance(numbers.begin(), findIt) << endl;
    } else {
        cout << "std::find                   : Target " << target << " NOT FOUND" << endl;
    }

    // 2. std::accumulate Numeric Algorithm
    int totalSum = accumulate(numbers.begin(), numbers.end(), 0);
    cout << "std::accumulate (Sum)       : Total Sum = " << totalSum << endl;

    // 3. std::count_if Algorithm with Lambda Expression
    int evenCount = static_cast<int>(count_if(numbers.begin(), numbers.end(), [](int val) noexcept {
        return val % 2 == 0;
    }));
    cout << "std::count_if (Even count)  : " << evenCount << " even element(s)" << endl;

    // 4. std::for_each Algorithm with Lambda Mutation
    cout << "std::for_each (In-place *2) : ";
    for_each(numbers.begin(), numbers.end(), [](int& val) noexcept {
        val *= 2;
    });

    for (int v : numbers) {
        cout << v << " ";
    }
    cout << endl;

    // 5. std::sort Algorithm (Descending Order with Lambda Comparator)
    sort(numbers.begin(), numbers.end(), [](int a, int b) noexcept {
        return a > b; // Descending order predicate
    });

    cout << "std::sort (Descending)      : ";
    for (int v : numbers) {
        cout << v << " ";
    }
    cout << endl;
}

/**
 * @brief Demonstrates STL Associative Containers (std::map) for key-value pair mapping.
 * @param numbers Source vector data to map frequency.
 */
void demonstrateAssociativeContainers(const vector<int>& numbers) {
    cout << "\n================ 3. ASSOCIATIVE CONTAINERS (std::map) ================" << endl;
    
    // Map stores elements as ordered key-value pairs (Red-Black Tree backend)
    map<int, string> elementMap;

    for (size_t i = 0; i < numbers.size(); ++i) {
        elementMap[numbers[i]] = "Rank_" + to_string(i + 1);
    }

    cout << left << setw(18) << "Key (Sorted Value)" << "Value (Mapped Rank Label)" << endl;
    cout << string(45, '-') << endl;

    for (const auto& [key, label] : elementMap) {
        cout << left << setw(18) << key << label << endl;
    }
}

int main() {
    size_t elementCount = 0;
    int multiplier = 0;
    int searchTarget = 0;

    // Dynamic input collection with explicit stream flushing
    cout << "Enter number of elements for STL vector container: " << flush;
    if (!(cin >> elementCount) || elementCount == 0) {
        cout << "Invalid element count provided. Program terminated." << endl;
        return 0;
    }

    cout << "Enter integer step multiplier: " << flush;
    if (!(cin >> multiplier)) {
        cout << "Invalid multiplier input. Program terminated." << endl;
        return 0;
    }

    cout << "Enter a target value to search using std::find: " << flush;
    if (!(cin >> searchTarget)) {
        cout << "Invalid search target input. Program terminated." << endl;
        return 0;
    }

    // Execute STL Pillars Demonstration
    vector<int> numbers = demonstrateContainersAndIterators(elementCount, multiplier);
    demonstrateAlgorithmsAndLambdas(numbers, searchTarget);
    demonstrateAssociativeContainers(numbers);

    return 0;
}