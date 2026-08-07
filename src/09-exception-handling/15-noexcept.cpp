/*
 * =====================================================================================
 * CONCEPT        : The `noexcept` Specifier and Operator in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the specifier, compile-time operator, conditional specifiers, and 
 *                  STL container performance optimizations enabled by `noexcept`:
 *
 *                  1. The `noexcept` Specifier (`void f() noexcept;`) :
 *                     - Promises to the compiler that a function will not allow exceptions
 *                       to escape its scope.
 *                     - Enables compiler optimizations (e.g., omitting unwind table code,
 *                       allowing registers to be kept across calls).
 *                     - Violating the contract by throwing an exception inside a `noexcept`
 *                       function immediately invokes `std::terminate()`.
 *
 *                  2. The `noexcept` Operator (`noexcept(expression)`) :
 *                     - Compile-time Boolean operator that evaluates to `true` if an 
 *                       expression is guaranteed not to throw, and `false` otherwise.
 *
 *                  3. Conditional `noexcept` Specifier (`noexcept(condition)`) :
 *                     - Enables template functions and member functions to be `noexcept`
 *                       if and only if their underlying operations/type traits are `noexcept`.
 *                       Example: `noexcept(std::is_nothrow_move_constructible_v<T>)`.
 *
 *                  4. STL Container Reallocation Optimization (`std::move_if_noexcept`) :
 *                     - Containers like `std::vector` require the Strong Exception Guarantee.
 *                     - During vector reallocation, if an element's move constructor is 
 *                       marked `noexcept`, `std::vector` uses O(1) move operations.
 *                     - If the move constructor is NOT `noexcept`, `std::vector` falls back 
 *                       to expensive O(N) deep copies to prevent data corruption on throw.
 *
 * TIME COMPLEXITY  : Compile-time Operator Evaluation : O(1) constant time (evaluated at build time).
 *                    Vector Reallocation with `noexcept` Move : O(N) pointer exchanges (0 heap allocs).
 *                    Vector Reallocation without `noexcept`  : O(N * M) element deep copies & allocations.
 * SPACE COMPLEXITY : Memory footprint with `noexcept` Move : 0 bytes extra heap allocations.
 * =====================================================================================
 */

#include <iostream>
#include <utility>
#include <vector>
#include <type_traits>
#include <stdexcept>
#include <limits>

using namespace std;

// =====================================================================================
// 1. CLASS WITHOUT `noexcept` MOVE CONSTRUCTOR
// Forces std::vector to fall back to deep copying during reallocation.
// =====================================================================================
class ThrowingMoveElement {
private:
    int elementId_;
    int* heapBuffer_;

public:
    explicit ThrowingMoveElement(int id)
        : elementId_(id), heapBuffer_(new int[100]()) {
        cout << "      [ThrowingElement Ctor] Created ID: " << elementId_ << "\n";
    }

    ~ThrowingMoveElement() noexcept {
        if (heapBuffer_ != nullptr) {
            delete[] heapBuffer_;
            heapBuffer_ = nullptr;
        }
    }

    // Copy Constructor (Deep Copy)
    ThrowingMoveElement(const ThrowingMoveElement& other)
        : elementId_(other.elementId_ + 1000), heapBuffer_(new int[100]()) {
        cout << "      [ThrowingElement COPY CTOR] Deep copied element ID " << other.elementId_ 
             << " into new ID " << elementId_ << " (Heavy allocation!)\n";
    }

    // Move Constructor WITHOUT `noexcept`
    ThrowingMoveElement(ThrowingMoveElement&& other) { // NOT marked noexcept!
        elementId_ = other.elementId_;
        heapBuffer_ = other.heapBuffer_;
        other.heapBuffer_ = nullptr;
        other.elementId_ = -1;
        cout << "      [ThrowingElement MOVE CTOR] Moved element ID " << elementId_ 
             << " (Potentially throwing)\n";
    }

    [[nodiscard]] int getId() const noexcept { return elementId_; }
};

// =====================================================================================
// 2. CLASS WITH `noexcept` MOVE CONSTRUCTOR
// Enables std::vector to perform fast zero-cost pointer transfers during reallocation.
// =====================================================================================
class NothrowMoveElement {
private:
    int elementId_;
    int* heapBuffer_;

public:
    explicit NothrowMoveElement(int id)
        : elementId_(id), heapBuffer_(new int[100]()) {
        cout << "      [NothrowElement Ctor] Created ID: " << elementId_ << "\n";
    }

    ~NothrowMoveElement() noexcept {
        if (heapBuffer_ != nullptr) {
            delete[] heapBuffer_;
            heapBuffer_ = nullptr;
        }
    }

    // Copy Constructor (Deep Copy)
    NothrowMoveElement(const NothrowMoveElement& other)
        : elementId_(other.elementId_ + 1000), heapBuffer_(new int[100]()) {
        cout << "      [NothrowElement COPY CTOR] Deep copied element ID " << other.elementId_ 
             << " into new ID " << elementId_ << "\n";
    }

    // Move Constructor PROPERLY MARKED `noexcept`
    NothrowMoveElement(NothrowMoveElement&& other) noexcept { // Marked noexcept!
        elementId_ = other.elementId_;
        heapBuffer_ = other.heapBuffer_;
        other.heapBuffer_ = nullptr;
        other.elementId_ = -1;
        cout << "      [NothrowElement MOVE CTOR] STOLE pointer for ID " << elementId_ 
             << " in O(1) time! (Zero new allocations)\n";
    }

    [[nodiscard]] int getId() const noexcept { return elementId_; }
};

// =====================================================================================
// 3. CONDITIONAL `noexcept` TEMPLATE WRAPPER
// Dynamically deduces `noexcept` status based on inner type T's traits.
// =====================================================================================
template <typename T>
class ConditionalNoexceptContainer {
private:
    T storedData_;

public:
    explicit ConditionalNoexceptContainer(T data)
        : storedData_(std::move(data)) {}

    // Conditional noexcept specifier based on T's move constructor
    ConditionalNoexceptContainer(ConditionalNoexceptContainer&& other) 
        noexcept(std::is_nothrow_move_constructible_v<T>)
        : storedData_(std::move(other.storedData_)) {}

    // Conditional swap function
    void swap(ConditionalNoexceptContainer& other) 
        noexcept(noexcept(std::swap(storedData_, other.storedData_))) {
        using std::swap;
        swap(storedData_, other.storedData_);
    }

    [[nodiscard]] const T& getData() const noexcept { return storedData_; }
};

// =====================================================================================
// 4. HELPER FUNCTIONS FOR OPERATOR AND SPECIFIER EVALUATION
// =====================================================================================
void guaranteedNothrowFunction() noexcept {
    // Guaranteed not to throw
}

void potentiallyThrowingFunction() {
    throw std::runtime_error("Operational failure");
}

template <typename T>
void processValue(T&& val) noexcept(noexcept(std::forward<T>(val))) {
    // Forwarding function marked conditionally noexcept
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for `noexcept` analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. COMPILE-TIME `noexcept` OPERATOR EVALUATION
    // =====================================================================================
    cout << "\n================ 1. COMPILE-TIME `noexcept` OPERATOR EVALUATION ================\n";

    cout << "  - Inspecting functions and expressions with `noexcept(expr)`:\n";
    cout << "    * noexcept(guaranteedNothrowFunction()) : " 
         << (noexcept(guaranteedNothrowFunction()) ? "TRUE  (Non-throwing)" : "FALSE (May throw)") << "\n";
    cout << "    * noexcept(potentiallyThrowingFunction()) : " 
         << (noexcept(potentiallyThrowingFunction()) ? "TRUE  (Non-throwing)" : "FALSE (May throw)") << "\n";

    int a = 10;
    int b = 20;
    cout << "    * noexcept(a + b)                        : " 
         << (noexcept(a + b) ? "TRUE  (Non-throwing primitive math)" : "FALSE") << "\n";

    // =====================================================================================
    // 2. STL VECTOR REALLOCATION & `std::move_if_noexcept` IMPACT
    // =====================================================================================
    cout << "\n================ 2. STL VECTOR REALLOCATION & PERFORMANCE IMPACT ================\n";

    // Scenario A: Element WITHOUT `noexcept` move constructor
    cout << "  - Scenario A: Vector reallocation WITH ThrowingMoveElement (NO `noexcept`):\n";
    {
        std::vector<ThrowingMoveElement> vecUnsafe;
        vecUnsafe.reserve(1); // Force reallocation on 2nd element insertion

        cout << "    * Pushing 1st element into vector (capacity 1)...\n";
        vecUnsafe.push_back(ThrowingMoveElement(userInputValue));

        cout << "\n    * Pushing 2nd element (Triggers vector capacity reallocation):\n";
        vecUnsafe.push_back(ThrowingMoveElement(userInputValue + 1));
        
        cout << "    [RESULT]: Vector fell back to DEEP COPIES to protect Strong Exception Guarantee!\n";
    }

    // Scenario B: Element WITH `noexcept` move constructor
    cout << "\n  - Scenario B: Vector reallocation WITH NothrowMoveElement (`noexcept` marked):\n";
    {
        std::vector<NothrowMoveElement> vecSafe;
        vecSafe.reserve(1); // Force reallocation on 2nd element insertion

        cout << "    * Pushing 1st element into vector (capacity 1)...\n";
        vecSafe.push_back(NothrowMoveElement(userInputValue + 10));

        cout << "\n    * Pushing 2nd element (Triggers vector capacity reallocation):\n";
        vecSafe.push_back(NothrowMoveElement(userInputValue + 11));

        cout << "    [RESULT]: Vector safely executed FAST MOVE POINTER TRANSFERS in O(1) time!\n";
    }

    // =====================================================================================
    // 3. CONDITIONAL `noexcept` SPECIFIERS IN TEMPLATES
    // =====================================================================================
    cout << "\n================ 3. CONDITIONAL `noexcept` SPECIFIERS IN TEMPLATES ================\n";

    using UnsafeContainer = ConditionalNoexceptContainer<ThrowingMoveElement>;
    using SafeContainer   = ConditionalNoexceptContainer<NothrowMoveElement>;

    cout << "  - Evaluating type traits of `ConditionalNoexceptContainer`:\n";
    cout << "    * Is UnsafeContainer nothrow move constructible? : " 
         << (std::is_nothrow_move_constructible_v<UnsafeContainer> ? "YES" : "NO") << "\n";
    cout << "    * Is SafeContainer nothrow move constructible?   : " 
         << (std::is_nothrow_move_constructible_v<SafeContainer> ? "YES" : "NO") << "\n";

    // =====================================================================================
    // 4. `std::move_if_noexcept` DEMONSTRATION
    // =====================================================================================
    cout << "\n================ 4. `std::move_if_noexcept` UTILITY MECHANICS ================\n";

    ThrowingMoveElement unsafeObj(userInputValue + 20);
    NothrowMoveElement  safeObj(userInputValue + 30);

    cout << "  - Testing `std::move_if_noexcept` behavior on lvalues:\n";
    
    // std::move_if_noexcept returns const& for unsafe types (forces copy)
    [[maybe_unused]] auto&& resUnsafe = std::move_if_noexcept(unsafeObj);
    cout << "    * `std::move_if_noexcept(unsafeObj)` produces: " 
         << (std::is_const_v<std::remove_reference_t<decltype(resUnsafe)>> ? "const Lvalue Ref (Forces COPY)" : "Rvalue Ref (Allows MOVE)") << "\n";

    // std::move_if_noexcept returns Rvalue ref for safe types (allows move)
    [[maybe_unused]] auto&& resSafe = std::move_if_noexcept(safeObj);
    cout << "    * `std::move_if_noexcept(safeObj)`   produces: " 
         << (std::is_rvalue_reference_v<decltype(resSafe)> ? "Rvalue Ref (Allows MOVE)" : "const Lvalue Ref (Forces COPY)") << "\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ `noexcept` SPECIFIER & OPERATOR SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Component / Aspect    | C++ Syntax / Usage                | Architectural Effect & Benefit    |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| `noexcept` Specifier  | `void func() noexcept;`           | Guarantees non-throwing boundary  |\n"
         << "| `noexcept` Operator   | `bool b = noexcept(expr);`        | Evaluates if expr throws at build |\n"
         << "| Conditional Specifier | `noexcept(condition_expr)`        | Dynamic template exception safety |\n"
         << "| STL Reallocation      | `std::move_if_noexcept(obj)`      | Moves if noexcept; copies if not  |\n"
         << "| Move Constructor Rule | `Type(Type&&) noexcept;`          | MANDATORY for fast vector reallocs|\n"
         << "| Destructor Default    | Destructors implicitly `noexcept` | Prevents terminate during unwinds |\n"
         << "| Contract Violation    | Throwing inside `noexcept` func   | Immediately invokes std::terminate|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}