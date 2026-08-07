/*
 * =====================================================================================
 * CONCEPT        : Requires Expressions under Templates in C++20 (with Portable Fallback)
 * DESCRIPTION    : Production-grade executable code demonstrating all four requirement 
 *                  categories inside C++20 `requires` expressions:
 *
 *                  1. Simple Requirements :
 *                     - Checks that an expression is syntactically valid (e.g., `a + b`).
 *
 *                  2. Type Requirements :
 *                     - Checks that a type name or nested type alias exists (e.g., `typename T::value_type`).
 *
 *                  3. Compound Requirements :
 *                     - Checks expression validity, optional `noexcept` specifications, 
 *                       and constraints on return type using concepts.
 *                       (e.g., `{ constContainer.size() } noexcept -> std::convertible_to<std::size_t>`).
 *
 *                  4. Nested Requirements :
 *                     - Enforces local compile-time boolean predicate checks.
 *                       (e.g., `requires sizeof(T) > 0`).
 *
 * RESOLUTION FOR :
 *                  - "'T' does not refer to a value" & "Expected expression":
 *                    Occurs when C++20 `concept` keywords are compiled under C++17 or earlier standards.
 *                  - "No type named 'inspectContainer' in 'RequiresExpressionProcessor'":
 *                    Caused by compiler parse failures when concepts are used without C++20 mode enabled.
 *                  - Fixed by wrapping C++20 native concepts with feature macro check (`__cpp_concepts`) 
 *                    and supplying clean portable SFINAE fallbacks for pre-C++20 environments.
 *
 * COMPILER REQ   : Compile with `-std=c++20` for native C++20 concepts support.
 *
 * HEADER AUDIT   : Strictly audited headers used in this translation unit:
 *                  - <iostream>    : Console I/O streams (cin, cout, flush, endl)
 *                  - <type_traits> : std::is_same_v, std::enable_if_t, std::void_t, std::false_type, std::true_type
 *                  - <utility>     : std::declval for expression inspection
 *                  - <string>      : Type std::string
 *                  - <typeinfo>    : RTTI typeid operator for type inspection
 *                  - <limits>      : Stream clearing via std::numeric_limits
 *                  - <cstddef>     : Type std::size_t
 *                  - <concepts>    : Standard C++20 concepts (conditionally included under C++20)
 *
 * TIME COMPLEXITY  : Compile-time Expression Evaluation : O(1) predicate evaluation per template call.
 *                    Runtime Execution                 : O(1) Zero-cost abstraction.
 * SPACE COMPLEXITY : Binary Footprint                   : Zero runtime memory or vtable overhead.
 * =====================================================================================
 */

#include <iostream>
#include <type_traits>
#include <utility>
#include <string>
#include <typeinfo>
#include <limits>
#include <cstddef>

// Feature test macro detection for C++20 Concepts and Requires Expressions
#if defined(__cpp_concepts) && __cpp_concepts >= 201907L
    #include <concepts>
    #define HAS_CPP20_REQUIRES 1
#else
    #define HAS_CPP20_REQUIRES 0
#endif

using namespace std;

// =====================================================================================
// 1. CONCEPT DEFINITIONS DEMONSTRATING ALL 4 REQUIRES EXPRESSION REQUIREMENTS
// =====================================================================================

#if HAS_CPP20_REQUIRES

// Simple requirement concept checking arithmetic addition syntax
template <typename T>
concept Addable = requires(T a, T b) {
    // Simple Requirement: Validates + operator existence
    a + b;
};

// Custom Concept demonstrating all 4 requirement types inside a requires expression
template <typename T>
concept CustomContainer = requires(T container, const T constContainer, typename T::value_type val) {
    // ---------------------------------------------------------------------------------
    // A. SIMPLE REQUIREMENTS
    // Asserts that the statements are syntactically valid expressions.
    // ---------------------------------------------------------------------------------
    container.clear();
    container.push_back(val);

    // ---------------------------------------------------------------------------------
    // B. TYPE REQUIREMENTS
    // Asserts that specific type aliases or nested types exist within T.
    // ---------------------------------------------------------------------------------
    typename T::value_type;
    typename T::iterator;

    // ---------------------------------------------------------------------------------
    // C. COMPOUND REQUIREMENTS
    // { expression } noexcept(optional) -> concept_constraint;
    // Validates expression validity, exception guarantee, and return type compatibility.
    // ---------------------------------------------------------------------------------
    { constContainer.size() } noexcept -> std::convertible_to<std::size_t>;
    { constContainer.empty() } noexcept -> std::same_as<bool>;

    // ---------------------------------------------------------------------------------
    // D. NESTED REQUIREMENTS
    // requires boolean_predicate;
    // Evaluates a local compile-time boolean expression.
    // ---------------------------------------------------------------------------------
    requires sizeof(T) > 0;
    requires !std::same_as<T, std::string>; // Excludes std::string explicitly
};

#else

// --- PORTABLE C++17 SFINAE FALLBACK DETECTORS (For pre-C++20 compilers) ---

template <typename T, typename = void>
struct HasAddOperator : std::false_type {};

template <typename T>
struct HasAddOperator<T, std::void_t<decltype(std::declval<T>() + std::declval<T>())>> : std::true_type {};

template <typename T>
inline constexpr bool Addable_v = HasAddOperator<T>::value;

template <typename T, typename = void>
struct HasContainerTraits : std::false_type {};

template <typename T>
struct HasContainerTraits<T, std::void_t<
    typename T::value_type,
    typename T::iterator,
    decltype(std::declval<T>().clear()),
    decltype(std::declval<T>().push_back(std::declval<typename T::value_type>())),
    decltype(std::declval<const T>().size()),
    decltype(std::declval<const T>().empty())
>> : std::true_type {};

template <typename T>
inline constexpr bool CustomContainer_v = HasContainerTraits<T>::value && !std::is_same_v<T, std::string>;

#endif

// =====================================================================================
// 2. CONSTRAINED FUNCTION TEMPLATES USING REQUIRES EXPRESSIONS & CLAUSES
// =====================================================================================
class RequiresExpressionProcessor {
public:
#if HAS_CPP20_REQUIRES

    // Using a trailing requires clause with Addable concept
    template <typename T>
    static void executeAddition(T a, T b) requires Addable<T> {
        cout << "    [C++20 Requires Expression] Result (" << a << " + " << b << ") = " << (a + b) << "\n";
    }

    // Function template constrained by CustomContainer concept
    template <CustomContainer C>
    static void inspectContainer(const C& container) {
        cout << "    [C++20 CustomContainer Match] Type: " << typeid(C).name()
             << " | Current Size: " << container.size() 
             << " | Is Empty: " << (container.empty() ? "TRUE" : "FALSE") << "\n";
    }

#else

    template <typename T, std::enable_if_t<Addable_v<T>, int> = 0>
    static void executeAddition(T a, T b) {
        cout << "    [C++17 Fallback Detector] Result (" << a << " + " << b << ") = " << (a + b) << "\n";
    }

    template <typename C, std::enable_if_t<CustomContainer_v<C>, int> = 0>
    static void inspectContainer(const C& container) {
        cout << "    [C++17 Fallback Container] Type: " << typeid(C).name()
             << " | Current Size: " << container.size() 
             << " | Is Empty: " << (container.empty() ? "TRUE" : "FALSE") << "\n";
    }

#endif
};

// =====================================================================================
// 3. MOCK CONTAINER IMPLEMENTATION SATISFYING ALL REQUIREMENTS
// =====================================================================================
template <typename T>
class MockVector {
private:
    T elements_[10]{};
    std::size_t currentSize_{0};

public:
    using value_type = T;
    using iterator = T*;

    MockVector() = default;

    void clear() noexcept { currentSize_ = 0; }
    void push_back(const T& val) { 
        if (currentSize_ < 10) elements_[currentSize_++] = val; 
    }

    [[nodiscard]] std::size_t size() const noexcept { return currentSize_; }
    [[nodiscard]] bool empty() const noexcept { return currentSize_ == 0; }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Requires Expressions analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // Display active compilation mode
    cout << "\n================ COMPILATION ENVIRONMENT MODE ================\n";
#if HAS_CPP20_REQUIRES
    cout << "  - Status: C++20 Requires Expressions ENABLED (__cpp_concepts = " << __cpp_concepts << ")\n";
#else
    cout << "  - Status: Pre-C++20 Mode ACTIVE (Using portable C++17 <type_traits> fallbacks)\n";
    cout << "  - Note  : To enable C++20 concepts natively, compile with `-std=c++20` flag.\n";
#endif

    // =====================================================================================
    // 1. SIMPLE REQUIREMENT EVALUATION
    // =====================================================================================
    cout << "\n================ 1. SIMPLE REQUIREMENT EVALUATION ================\n";
    RequiresExpressionProcessor::executeAddition(userInputValue, 50);
    RequiresExpressionProcessor::executeAddition(static_cast<double>(userInputValue) * 1.5, 2.5);

    // =====================================================================================
    // 2. COMPREHENSIVE REQUIRES EXPRESSION (SIMPLE, TYPE, COMPOUND, NESTED)
    // =====================================================================================
    cout << "\n================ 2. COMPREHENSIVE REQUIRES EXPRESSION ================\n";

    MockVector<int> validContainer;
    validContainer.push_back(userInputValue);
    validContainer.push_back(userInputValue * 2);

    RequiresExpressionProcessor::inspectContainer(validContainer);

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ REQUIRES EXPRESSIONS SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Requirement Category  | Syntax Example inside requires()  | Evaluated Compile-Time Rule       |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Simple Requirement    | `expr;`                           | Validates expression syntax       |\n"
         << "| Type Requirement      | `typename T::nested_type;`        | Validates nested type existence   |\n"
         << "| Compound Requirement  | `{ expr } noexcept -> Concept;`   | Validates syntax, exception spec, |\n"
         << "|                       |                                   | and return type constraint        |\n"
         << "| Nested Requirement    | `requires boolean_predicate;`     | Enforces local constraint check   |\n"
         << "| Return Type Matching  | `{ expr } -> std::same_as<Type>;` | Ensures exact return type match   |\n"
         << "| Build Flag Notice     | Pass `-std=c++20` to compiler     | Enables native concept syntax     |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}