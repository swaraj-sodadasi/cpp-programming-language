/*
 * =====================================================================================
 * CONCEPT        : Function Template Overloading under Templates in Modern C++
 * DESCRIPTION    : Production-grade executable code demonstrating the rules, mechanics,
 *                  partial ordering, and overload resolution precedence for function
 *                  templates:
 *
 *                  1. Non-Template vs. Template Overload Resolution :
 *                     - Non-template functions are preferred over function templates 
 *                       when both offer an equally good exact match.
 *                     - Explicit template arguments `<T>` force the compiler to select 
 *                       a template instance instead of a non-template function.
 *
 *                  2. Partial Ordering of Function Templates :
 *                     - When multiple template signatures match, the compiler chooses the 
 *                       MOST SPECIALIZED template (e.g., `T*` is more specialized than `T`).
 *
 *                  3. Multi-Parameter & Arity Specialization :
 *                     - Overloads taking homogeneous types `(T, T)` vs. heterogeneous 
 *                       types `(T, U)`. `(T, T)` is more specialized for identical arguments.
 *
 *                  4. Container Specialization :
 *                     - Overloading primary templates `(T)` with container wrappers `(std::vector<T>&)`.
 *
 * HEADER AUDIT   : Strictly audited. Only headers whose symbols are actively used in the code:
 *                  - <iostream>  : Console I/O streams (cin, cout, flush)
 *                  - <string>    : Type std::string and std::to_string
 *                  - <vector>    : Type std::vector
 *                  - <typeinfo>  : Type inspection via typeid operator
 *                  - <limits>    : Stream clearing via std::numeric_limits
 *
 * TIME COMPLEXITY  : Compile-time Overload Resolution : O(N) candidate signature evaluation.
 *                    Runtime Execution               : O(1) Zero-cost abstraction dispatch.
 * SPACE COMPLEXITY : Binary footprint proportional to generated specialized functions.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <typeinfo>
#include <limits>

using namespace std;

// =====================================================================================
// 1. NON-TEMPLATE VS. TEMPLATE OVERLOAD RESOLUTION
// Demonstrates priority rules when a non-template function shares a name with a template.
// =====================================================================================
class NonTemplateVsTemplateDemo {
public:
    // Non-template function exact match for int
    static void display(int value) {
        cout << "    [SELECTED]: Non-Template function display(int): " << value << "\n";
    }

    // Primary function template
    template <typename T>
    static void display(T value) {
        cout << "    [SELECTED]: Primary Template display(T) [Type: " << typeid(T).name() 
             << "]: " << value << "\n";
    }
};

// =====================================================================================
// 2. PARTIAL ORDERING RULES (DEGREE OF SPECIALIZATION)
// Compiler selects the most specialized template signature for the given arguments.
// =====================================================================================
class PartialOrderingDemo {
public:
    // Primary Template (Least Specialized)
    template <typename T>
    static void inspect(T value) {
        cout << "    [SELECTED]: Primary Template inspect(T) -> Value: " << value << "\n";
    }

    // Pointer Overload (More Specialized than Primary T)
    template <typename T>
    static void inspect(T* ptr) {
        cout << "    [SELECTED]: Pointer Overload inspect(T*) -> Pointed Value: " 
             << (ptr ? to_string(*ptr) : "nullptr") << "\n";
    }

    // Container Overload (More Specialized for std::vector)
    template <typename T>
    static void inspect(const vector<T>& vec) {
        cout << "    [SELECTED]: Vector Overload inspect(const vector<T>&) -> Size: " 
             << vec.size() << " | Elements: { ";
        for (size_t i = 0; i < vec.size(); ++i) {
            cout << vec[i] << (i + 1 < vec.size() ? ", " : " ");
        }
        cout << "}\n";
    }
};

// =====================================================================================
// 3. HOMOGENEOUS VS. HETEROGENEOUS PARAMETER OVERLOADING
// Demonstrates `(T, T)` vs `(T, U)` candidate resolution.
// =====================================================================================
class MultiParamOverloadDemo {
public:
    // Heterogeneous Overload: Accepts two distinct template types
    template <typename T, typename U>
    static void combine(T a, U b) {
        cout << "    [SELECTED]: Heterogeneous Template combine(T, U) -> Types: (" 
             << typeid(T).name() << ", " << typeid(U).name() << ") | Values: (" 
             << a << ", " << b << ")\n";
    }

    // Homogeneous Overload: Constrained to identical types (More specialized when types match)
    template <typename T>
    static void combine(T a, T b) {
        cout << "    [SELECTED]: Homogeneous Template combine(T, T) -> Type: " 
             << typeid(T).name() << " | Values: (" << a << ", " << b << ")\n";
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Function Template Overloading analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. NON-TEMPLATE VS TEMPLATE PRECEDENCE
    // =====================================================================================
    cout << "\n================ 1. NON-TEMPLATE VS. TEMPLATE PRECEDENCE ================\n";

    cout << "  - Calling display(" << userInputValue << ") [Exact match available for non-template]:\n";
    NonTemplateVsTemplateDemo::display(userInputValue); 
    // Chooses non-template display(int)

    cout << "\n  - Calling display(" << userInputValue << ".55) [No non-template match]:\n";
    NonTemplateVsTemplateDemo::display(static_cast<double>(userInputValue) + 0.55); 
    // Chooses primary template display<double>(double)

    cout << "\n  - Calling display<int>(" << userInputValue << ") [Explicit <int> template qualification]:\n";
    NonTemplateVsTemplateDemo::display<int>(userInputValue); 
    // Bypasses non-template and forces primary template display<int>(int)

    // =====================================================================================
    // 2. PARTIAL ORDERING OF TEMPLATES
    // =====================================================================================
    cout << "\n================ 2. PARTIAL ORDERING OF FUNCTION TEMPLATES ================\n";

    int scalarVal = userInputValue;
    int* ptrVal = &scalarVal;
    vector<int> vectorVal = {userInputValue, userInputValue + 10, userInputValue + 20};

    cout << "  - Calling inspect(scalarVal) [Matches primary template]:\n";
    PartialOrderingDemo::inspect(scalarVal);

    cout << "\n  - Calling inspect(ptrVal) [Matches primary T and pointer T* -> T* selected]:\n";
    PartialOrderingDemo::inspect(ptrVal);

    cout << "\n  - Calling inspect(vectorVal) [Matches primary T and vector<T> -> vector<T> selected]:\n";
    PartialOrderingDemo::inspect(vectorVal);

    // =====================================================================================
    // 3. HOMOGENEOUS VS HETEROGENEOUS OVERLOAD RESOLUTION
    // =====================================================================================
    cout << "\n================ 3. HOMOGENEOUS VS HETEROGENEOUS OVERLOADS ================\n";

    cout << "  - Calling combine(" << userInputValue << ", " << userInputValue + 50 << ") [Identical types (int, int)]:\n";
    MultiParamOverloadDemo::combine(userInputValue, userInputValue + 50); 
    // Matches both combine(T, U) and combine(T, T); combine(T, T) is selected (more specialized)

    cout << "\n  - Calling combine(" << userInputValue << ", 99.99) [Mixed types (int, double)]:\n";
    MultiParamOverloadDemo::combine(userInputValue, 99.99); 
    // Only matches combine(T, U)

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ FUNCTION TEMPLATE OVERLOADING SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Overload Candidate    | Call Argument Example             | Winning Selection & Priority Rule |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Non-Template vs T     | `display(10)`                     | Non-template wins exact matches   |\n"
         << "| Forced Template Call  | `display<int>(10)`                | Explicit `<int>` bypasses non-T   |\n"
         << "| Pointer vs Primary    | `inspect(intPtr)`                 | `T*` wins over `T` (More specialized)|\n"
         << "| Container vs Primary  | `inspect(vector<int>)`            | `vector<T>` wins over generic `T` |\n"
         << "| `(T, T)` vs `(T, U)`  | `combine(10, 20)`                 | `(T, T)` wins when types match    |\n"
         << "| `(T, T)` vs `(T, U)`  | `combine(10, 20.5)`               | `(T, U)` selected for mixed types |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}