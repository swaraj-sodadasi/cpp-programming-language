/*
 * =====================================================================================
 * CONCEPT        : Fold Expressions under Templates in Modern C++ (C++17)
 * DESCRIPTION    : Production-grade executable code demonstrating the 4 syntax variants,
 *                  mechanics, operator combinations, and practical applications of C++17
 *                  Fold Expressions over Variadic Template Parameter Packs:
 *
 *                  1. Unary Left Fold  : `(... op pack)`
 *                  2. Unary Right Fold : `(pack op ...)`
 *                  3. Binary Left Fold : `(init op ... op pack)`
 *                  4. Binary Right Fold: `(pack op ... op init)`
 *
 *                  5. Practical Applications :
 *                     - Arithmetic reductions (`+`, `*`, `-`)
 *                     - Logical predicate evaluations (`&&`, `||`)
 *                     - Comma operator fold for sequence execution (printing, vector population)
 *
 * HEADER AUDIT   : Strictly audited. Only headers whose symbols are actively used in the code:
 *                  - <iostream> : Console I/O streams (cin, cout, flush)
 *                  - <string>   : Type std::string
 *                  - <vector>   : Container std::vector for pack insertion demo
 *                  - <typeinfo> : RTTI typeid operator for type inspection
 *                  - <limits>   : Stream clearing via std::numeric_limits
 *                  - <cstddef>  : Type std::size_t
 *                  - <utility>  : std::forward for perfect forwarding
 *
 * TIME COMPLEXITY  : Compile-time Expansion : O(N) where N is the parameter pack size.
 *                    Runtime Execution     : O(N) or O(1) inlined evaluation.
 * SPACE COMPLEXITY : Binary Footprint      : Minimal inlined code generated per parameter pack.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <typeinfo>
#include <limits>
#include <cstddef>
#include <utility>

using namespace std;

// =====================================================================================
// 1. THE FOUR SYNTAX VARIANTS OF C++17 FOLD EXPRESSIONS
// =====================================================================================
class FoldVariantsDemo {
public:
    // A. Unary Left Fold: (... - args) -> (((arg1 - arg2) - arg3) - ...)
    template <typename... Args>
    [[nodiscard]] static auto unaryLeftSubtract(Args... args) {
        return (... - args);
    }

    // B. Unary Right Fold: (args - ...) -> (arg1 - (arg2 - (arg3 - ...)))
    template <typename... Args>
    [[nodiscard]] static auto unaryRightSubtract(Args... args) {
        return (args - ...);
    }

    // C. Binary Left Fold: (init + ... + args) -> (((init + arg1) + arg2) + ...)
    template <typename Init, typename... Args>
    [[nodiscard]] static auto binaryLeftAdd(Init init, Args... args) {
        return (init + ... + args);
    }

    // D. Binary Right Fold: (args + ... + init) -> (arg1 + (arg2 + (... + init)))
    template <typename Init, typename... Args>
    [[nodiscard]] static auto binaryRightAdd(Init init, Args... args) {
        return (args + ... + init);
    }
};

// =====================================================================================
// 2. LOGICAL & COMMA OPERATOR FOLD EXPRESSIONS
// =====================================================================================
class LogicalAndCommaFoldDemo {
public:
    // Logical AND fold: Checks if all arguments satisfy a condition (all positive)
    template <typename... Args>
    [[nodiscard]] static bool allPositive(Args... args) {
        return (... && (args > 0));
    }

    // Logical OR fold: Checks if at least one argument satisfies a condition (any even)
    template <typename... Args>
    [[nodiscard]] static bool anyEven(Args... args) {
        return (... || (args % 2 == 0));
    }

    // Comma fold expression for formatted sequence output
    template <typename... Args>
    static void printSequence(const string& label, const Args&... args) {
        cout << "    [" << label << "] Sequence (" << sizeof...(Args) << " items): ";
        // Using comma operator fold: ((expression), ...)
        ((cout << "{" << typeid(Args).name() << ": " << args << "} "), ...);
        cout << "\n";
    }

    // Comma fold expression for container population
    template <typename T, typename... Args>
    static void populateVector(vector<T>& vec, Args&&... args) {
        // Pushes each forwarded argument into the vector sequentially
        (vec.push_back(std::forward<Args>(args)), ...);
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Fold Expressions analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. DEMONSTRATING THE 4 FOLD EXPRESSION VARIANTS
    // =====================================================================================
    cout << "\n================ 1. FOUR VARIANTS OF C++17 FOLD EXPRESSIONS ================\n";

    // Testing Subtraction Order (Left vs Right fold reveals associativity differences)
    // 100 - 20 - 5
    // Unary Left:  ((100 - 20) - 5) = 80 - 5 = 75
    // Unary Right: (100 - (20 - 5)) = 100 - 15 = 85
    int leftSub = FoldVariantsDemo::unaryLeftSubtract(userInputValue, 20, 5);
    int rightSub = FoldVariantsDemo::unaryRightSubtract(userInputValue, 20, 5);

    cout << "  - Unary Left Subtract  (((" << userInputValue << " - 20) - 5)) = " << leftSub << "\n";
    cout << "  - Unary Right Subtract (" << userInputValue << " - (20 - 5)) = " << rightSub << "\n";

    // Testing Binary Folds with initial value = 500
    int binaryLeftSum = FoldVariantsDemo::binaryLeftAdd(500, userInputValue, 10, 20);
    int binaryRightSum = FoldVariantsDemo::binaryRightAdd(500, userInputValue, 10, 20);

    cout << "  - Binary Left Add  (500 + " << userInputValue << " + 10 + 20) = " << binaryLeftSum << "\n";
    cout << "  - Binary Right Add (" << userInputValue << " + 10 + 20 + 500) = " << binaryRightSum << "\n";

    // =====================================================================================
    // 2. LOGICAL OPERATOR FOLDS (&&, ||)
    // =====================================================================================
    cout << "\n================ 2. LOGICAL OPERATOR FOLDS (&&, ||) ================\n";

    bool isAllPos = LogicalAndCommaFoldDemo::allPositive(userInputValue, 15, 42, 8);
    bool isAllPosFail = LogicalAndCommaFoldDemo::allPositive(userInputValue, -5, 42);

    cout << "  - All Positive (" << userInputValue << ", 15, 42, 8)  : " << (isAllPos ? "TRUE" : "FALSE") << "\n";
    cout << "  - All Positive (" << userInputValue << ", -5, 42)    : " << (isAllPosFail ? "TRUE" : "FALSE") << "\n";

    bool hasEven = LogicalAndCommaFoldDemo::anyEven(userInputValue, 11, 13, 15);
    cout << "  - Any Even     (" << userInputValue << ", 11, 13, 15) : " << (hasEven ? "TRUE" : "FALSE") << "\n";

    // =====================================================================================
    // 3. COMMA OPERATOR FOLD (SEQUENCING & CONTAINER POPULATION)
    // =====================================================================================
    cout << "\n================ 3. COMMA OPERATOR FOLDS (SEQUENCING) ================\n";

    LogicalAndCommaFoldDemo::printSequence("MultiType Sequence", userInputValue, 3.14159, string("FoldExpr"), 'F');

    vector<int> container;
    LogicalAndCommaFoldDemo::populateVector(container, userInputValue, userInputValue + 10, userInputValue + 20);

    cout << "  - Populated Vector via Comma Fold (Size: " << container.size() << ") -> { ";
    for (std::size_t i = 0; i < container.size(); ++i) {
        cout << container[i] << (i + 1 < container.size() ? ", " : " ");
    }
    cout << "}\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ FOLD EXPRESSIONS SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Fold Syntax Variant   | General Expression Template       | Associativity & Expansion Order   |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Unary Left Fold       | `(... op pack)`                   | `(((E1 op E2) op E3) ... op EN)`  |\n"
         << "| Unary Right Fold      | `(pack op ...)`                   | `(E1 op (E2 op ... (EN-1 op EN)))`|\n"
         << "| Binary Left Fold      | `(init op ... op pack)`           | `((((I op E1) op E2) ...) op EN)` |\n"
         << "| Binary Right Fold     | `(pack op ... op init)`           | `(E1 op (E2 op ... (EN op I)))`   |\n"
         << "| Comma Fold            | `((cout << pack), ...)`           | Sequential execution left-to-right|\n"
         << "| Empty Pack Defaults   | `&&` -> true, `||` -> false, `,`  | Only 3 operators allow empty packs|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}