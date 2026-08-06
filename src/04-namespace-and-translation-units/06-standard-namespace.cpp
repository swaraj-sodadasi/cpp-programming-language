/*
 * =====================================================================================
 * CONCEPT        : Standard Namespace (`std`) in C++
 * DESCRIPTION    : Comprehensive implementation showcasing the C++ Standard Library namespace (`std`):
 *                  1. Nature of `std`             : Scope containing all Standard Library classes, 
 *                                                   functions, algorithms, and objects.
 *                  2. Standard I/O & Strings      : `std::cout`, `std::cin`, `std::string`, `std::flush`.
 *                  3. Standard Containers         : `std::vector`, `std::map`, `std::unordered_map`.
 *                  4. Standard Algorithms         : `std::sort`, `std::transform`, `std::find_if`.
 *                  5. Utilities & Smart Pointers  : `std::unique_ptr`, `std::tuple`, `std::optional`.
 *                  6. Scope Management Safety     : Why avoiding global `using namespace std;` 
 *                                                   prevents name collisions with standard symbols.
 *
 * TIME COMPLEXITY  : Best Case: O(1) scalar ops; O(N log N) for std::sort.
 * SPACE COMPLEXITY : Best Case: O(N) container dynamic memory allocation.
 * =====================================================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <memory>
#include <optional>
#include <tuple>
#include <iomanip>
#include <utility>
#include <cmath>

// Best practice: Avoid 'using namespace std;' at global scope.
// Explicitly qualify symbols with 'std::' or use localized declarations.

// =====================================================================================
// 1. STANDARD UTILITIES & OPTIONAL (C++17 std::optional)
// =====================================================================================

/**
 * @brief Demonstrates std::optional from standard namespace to represent nullable returns.
 */
std::optional<double> calculateSquareRoot(double value) {
    if (value < 0.0) {
        return std::nullopt; // Represents absence of a valid result
    }
    return std::sqrt(value);
}

// =====================================================================================
// 2. STANDARD CONTAINERS & ALGORITHMS DEMONSTRATION
// =====================================================================================

void demonstrateContainersAndAlgorithms(int userCount) {
    std::cout << "\n================ 1. STANDARD CONTAINERS & ALGORITHMS (`std::`) ================\n";

    // Standard Container: std::vector
    std::vector<int> numbers;
    for (int i = userCount; i > 0; --i) {
        numbers.push_back(i * 3);
    }

    std::cout << "  - Unsorted std::vector : ";
    for (int n : numbers) {
        std::cout << n << " ";
    }
    std::cout << "\n";

    // Standard Algorithm: std::sort from <algorithm>
    std::sort(numbers.begin(), numbers.end());

    std::cout << "  - Sorted std::vector   : ";
    for (int n : numbers) {
        std::cout << n << " ";
    }
    std::cout << "\n";

    // Standard Associative Container: std::map
    std::map<std::string, int> itemScores;
    itemScores["Alpha"] = numbers.front();
    itemScores["Beta"] = numbers.back();

    std::cout << "  - std::map Contents    :\n";
    for (const auto& [key, val] : itemScores) { // Structured bindings (C++17)
        std::cout << "    |- Key: " << std::left << std::setw(8) << key 
                  << " | Value: " << val << "\n";
    }
}

// =====================================================================================
// 3. SMART POINTERS & MEMORY MANAGEMENT (`std::unique_ptr`)
// =====================================================================================

class StandardResource {
private:
    std::string resourceName_;

public:
    explicit StandardResource(std::string name) : resourceName_(std::move(name)) {
        std::cout << "  - [std::unique_ptr] StandardResource '" << resourceName_ << "' allocated.\n";
    }

    ~StandardResource() {
        std::cout << "  - [std::unique_ptr] StandardResource '" << resourceName_ << "' automatically freed.\n";
    }

    void performAction() const {
        std::cout << "  - Resource '" << resourceName_ << "' executing work.\n";
    }
};

void demonstrateSmartPointers(const std::string& resName) {
    std::cout << "\n================ 2. SMART POINTERS & RAII (`std::unique_ptr`) ================\n";
    
    // std::make_unique from <memory> (C++14)
    std::unique_ptr<StandardResource> resPtr = std::make_unique<StandardResource>(resName);
    resPtr->performAction();
    // Managed memory is safely freed when resPtr goes out of scope (RAII)
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userNum = 0;
    std::string userString;

    // Dynamic input collection with stream flushing
    std::cout << "Enter a positive integer count (e.g., 5): " << std::flush;
    if (!(std::cin >> userNum) || userNum <= 0) {
        std::cout << "Invalid count input. Defaulting to 5." << std::endl;
        userNum = 5;
    }

    std::cout << "Enter a resource name string (e.g., DatabaseHandle): " << std::flush;
    std::cin.ignore();
    std::getline(std::cin, userString);
    if (userString.empty()) {
        userString = "DatabaseHandle";
    }

    // 1. CONTAINERS & ALGORITHMS
    demonstrateContainersAndAlgorithms(userNum);

    // 2. SMART POINTERS & MEMORY
    demonstrateSmartPointers(userString);

    // 3. STANDARD UTILITIES (std::optional & std::tuple)
    std::cout << "\n================ 3. STANDARD UTILITIES (`std::optional` & `std::tuple`) ================\n";
    
    double testVal = static_cast<double>(userNum) - 10.0;
    std::optional<double> sqrResult = calculateSquareRoot(testVal);

    if (sqrResult.has_value()) {
        std::cout << "  - std::optional result for sqrt(" << testVal << ") = " << sqrResult.value() << "\n";
    } else {
        std::cout << "  - std::optional result for sqrt(" << testVal << ") = nullopt (Negative Input)\n";
    }

    // std::tuple packaging multiple types from <tuple>
    std::tuple<int, std::string, double> record{101, userString, 99.9};
    std::cout << "  - std::tuple element 0 (ID): " << std::get<0>(record) 
              << " | element 1 (Name): " << std::get<1>(record) << "\n";

    std::cout << "\n================ STANDARD NAMESPACE SUMMARY ================\n";
    std::cout << "1. Core Scope     : `std` houses all classes, objects, and functions provided by standard headers.\n";
    std::cout << "2. Avoid Globals  : Do NOT write `using namespace std;` in global headers or production files.\n";
    std::cout << "3. Avoid Collisions: Explicit `std::` prefixing prevents name collisions (e.g., std::count vs local count).\n";
    std::cout << "4. Key Utilities  : Standard library provides container templates, algorithms, smart pointers, & I/O.\n";

    return 0;
}