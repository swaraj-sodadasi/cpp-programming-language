/*
 * =====================================================================================
 * CONCEPT        : Template Design Guidelines under Templates in Modern C++
 * DESCRIPTION    : Production-grade executable code demonstrating the 5 essential
 *                  architectural design guidelines for authoring clean, efficient,
 *                  and robust C++ Templates:
 *
 *                  1. Thin Template Idiom (Code Bloat Reduction) :
 *                     - Pulling non-dependent logic and raw memory handling into a non-templated
 *                       base class (`RawBufferBase`) to avoid code binary duplication across 
 *                       multiple type instantiations.
 *
 *                  2. Constrain Parameters & Early Diagnostics (`static_assert` / Traits) :
 *                     - Validating type invariants at the top of class template blueprints to fail
 *                       fast with human-readable compiler messages instead of deep template error dumps.
 *
 *                  3. Hidden Friend Idiom for Operator Overloading :
 *                     - Defining binary operators (e.g., `operator<<`) as non-template inline 
 *                       friends inside the class template to eliminate overload set pollution 
 *                       and leverage Argument-Dependent Lookup (ADL).
 *
 *                  4. Standard STL Type Traits Exposure (Interface Uniformity) :
 *                     - Exposing standardized type aliases (`value_type`, `reference`, `pointer`) 
 *                       so custom template abstractions interoperate seamlessly with STL algorithms.
 *
 *                  5. Universal Forwarding Constructors & Perfect Forwarding :
 *                     - Forwarding constructor parameters (`Args&&... args`) using `std::forward` 
 *                       while guarding against copy/move constructor hijacking via SFINAE.
 *
 * HEADER AUDIT   : Strictly audited. Only headers whose symbols are actively used in the code:
 *                  - <iostream>    : Console I/O streams (cin, cout, flush, endl, ostream)
 *                  - <type_traits> : std::is_arithmetic_v, std::is_same_v, std::enable_if_t, std::decay_t
 *                  - <utility>     : std::forward, std::move
 *                  - <string>      : Type std::string
 *                  - <typeinfo>    : RTTI typeid operator for runtime type name inspection
 *                  - <limits>      : Stream clearing via std::numeric_limits
 *                  - <cstddef>     : Type std::size_t
 *
 * TIME COMPLEXITY  : Compile-time Monomorphization : O(1) type deduction and constraint checking.
 *                    Runtime Execution            : O(1) Zero-cost abstraction.
 * SPACE COMPLEXITY : Binary Footprint             : Minimized via Thin Template Base hoisting.
 * =====================================================================================
 */

#include <iostream>
#include <type_traits>
#include <utility>
#include <string>
#include <typeinfo>
#include <limits>
#include <cstddef>

using namespace std;

// =====================================================================================
// GUIDELINE 1: THIN TEMPLATE IDIOM (HOISTING NON-DEPENDENT CODE)
// Non-dependent base class manages capacity and shared state to reduce binary bloat.
// =====================================================================================
class RawBufferBase {
protected:
    std::size_t capacity_{0};

    explicit RawBufferBase(std::size_t cap) : capacity_(cap) {}

    // Non-dependent method compiled ONCE in binary regardless of how many types use TypedBuffer<T>
    void logCapacityInfo() const {
        cout << "    [Thin Base Class] Shared non-dependent capacity log: " << capacity_ << " units.\n";
    }
};

template <typename T>
class TypedBuffer : public RawBufferBase {
private:
    T* storage_{nullptr};
    std::size_t size_{0};

public:
    explicit TypedBuffer(std::size_t cap)
        : RawBufferBase(cap), storage_(new T[cap]) {}

    ~TypedBuffer() {
        delete[] storage_;
    }

    // Disable copy for raw resource management clarity
    TypedBuffer(const TypedBuffer&) = delete;
    TypedBuffer& operator=(const TypedBuffer&) = delete;

    void push(const T& item) {
        if (size_ < capacity_) {
            storage_[size_++] = item;
        }
    }

    void render() const {
        logCapacityInfo(); // Invokes non-dependent base implementation
        cout << "    [TypedBuffer<" << typeid(T).name() << ">] Type-specific active count: " << size_ << "\n";
    }
};

// =====================================================================================
// GUIDELINE 2: CONSTRAIN PARAMETERS & EARLY DIAGNOSTICS (static_assert)
// Fail early at top of template definition with actionable error messages.
// =====================================================================================
template <typename T>
class NumericEngine {
    // Early diagnostic assertion preventing deep template error stack traces
    static_assert(std::is_arithmetic_v<T>,
                  "Template Design Constraint Error: NumericEngine requires an arithmetic type!");

private:
    T value_;

public:
    explicit NumericEngine(T val) : value_(val) {}

    [[nodiscard]] T computeScaledValue(T factor) const {
        return value_ * factor;
    }
};

// =====================================================================================
// GUIDELINE 3: HIDDEN FRIEND IDIOM FOR OPERATOR OVERLOADING
// Keeps global namespace clean and relies on Argument-Dependent Lookup (ADL).
// =====================================================================================
template <typename T>
class DomainPoint {
private:
    T x_;
    T y_;

public:
    DomainPoint(T x, T y) : x_(x), y_(y) {}

    // HIDDEN FRIEND: Non-template function injected directly into class scope.
    // Avoids template overload pollution across the global namespace.
    friend ostream& operator<<(ostream& os, const DomainPoint& pt) {
        os << "DomainPoint(" << pt.x_ << ", " << pt.y_ << ")";
        return os;
    }
};

// =====================================================================================
// GUIDELINE 4 & 5: STL TYPE ALIAS EXPOSURE & PERFECT FORWARDING CONSTRUCTORS
// =====================================================================================
template <typename T>
class SmartContainer {
public:
    // GUIDELINE 4: Standard type traits exposure for STL interoperability
    using value_type = T;
    using reference  = T&;
    using pointer    = T*;

private:
    T data_;

public:
    // GUIDELINE 5: Perfect Forwarding Constructor guarded against Copy Ctor hijacking
    template <typename... Args,
              typename = std::enable_if_t<!(sizeof...(Args) == 1 &&
                          (std::is_same_v<std::decay_t<Args>, SmartContainer> || ...))>>
    explicit SmartContainer(Args&&... args)
        : data_(std::forward<Args>(args)...) {
        cout << "    [Perfect Forwarding Ctor] Constructing inner element in-place.\n";
    }

    reference get() noexcept { return data_; }
    [[nodiscard]] const T& get() const noexcept { return data_; }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Template Design Guidelines analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. THIN TEMPLATE IDIOM
    // =====================================================================================
    cout << "\n================ 1. THIN TEMPLATE IDIOM (BLOAT REDUCTION) ================\n";

    TypedBuffer<int> intBuf(5);
    intBuf.push(userInputValue);
    intBuf.push(userInputValue + 10);
    intBuf.render();

    TypedBuffer<double> dblBuf(5);
    dblBuf.push(static_cast<double>(userInputValue) * 1.5);
    dblBuf.render();

    // =====================================================================================
    // 2. CONSTRAINED TEMPLATES & EARLY DIAGNOSTICS
    // =====================================================================================
    cout << "\n================ 2. CONSTRAINED TEMPLATES & EARLY DIAGNOSTICS ================\n";

    NumericEngine<int> intEngine(userInputValue);
    cout << "  - Scaled Integer Result (" << userInputValue << " * 3): " 
         << intEngine.computeScaledValue(3) << "\n";

    NumericEngine<double> dblEngine(static_cast<double>(userInputValue) * 0.5);
    cout << "  - Scaled Double Result  : " << dblEngine.computeScaledValue(2.5) << "\n";

    // =====================================================================================
    // 3. HIDDEN FRIEND IDIOM
    // =====================================================================================
    cout << "\n================ 3. HIDDEN FRIEND IDIOM FOR OPERATOR OVERLOADING ================\n";

    DomainPoint<int> ptInt(userInputValue, userInputValue * 2);
    DomainPoint<double> ptDbl(static_cast<double>(userInputValue) * 0.1, 99.9);

    // Stream operators resolved via ADL without global template overload set pollution
    cout << "  - Integer Point : " << ptInt << "\n";
    cout << "  - Double Point  : " << ptDbl << "\n";

    // =====================================================================================
    // 4 & 5. STL ALIAS EXPOSURE & PERFECT FORWARDING
    // =====================================================================================
    cout << "\n================ 4 & 5. STL TYPE ALIASES & PERFECT FORWARDING ================\n";

    SmartContainer<string> strContainer("Template_Design_Guidelines");
    cout << "  - Inner Stored String: " << strContainer.get() << "\n";

    // Verifying standard type alias compliance
    SmartContainer<string>::value_type rawCopy = "Alias_Exposed_Value";
    cout << "  - Value using exposed value_type alias: " << rawCopy << "\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ TEMPLATE DESIGN GUIDELINES SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Design Guideline      | C++ Implementation Pattern        | Key Architectural Benefit         |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| 1. Thin Template      | Inherit non-dependent logic from  | Eliminates binary code bloat      |\n"
         << "|    Idiom              | non-templated base class          | across multiple instantiations    |\n"
         << "| 2. Early Diagnostics  | `static_assert(trait_v<T>, msg)`  | Fails fast with clear human       |\n"
         << "|                       | at top of class definition        | readable compiler error messages  |\n"
         << "| 3. Hidden Friend      | `friend os& op<<(os&, const C&)`  | Prevents global overload set      |\n"
         << "|    Idiom              | defined inside class template body| pollution; relies on clean ADL    |\n"
         << "| 4. STL Interop Aliases| `using value_type = T;`           | Allows custom templates to work   |\n"
         << "|                       | `using reference = T&;`           | seamlessly with standard algorithms|\n"
         << "| 5. Perfect Forwarding | `template<Args...> Ctor(Args&&..)`| In-place constructor forwarding   |\n"
         << "|    Constructors       | guarded with enable_if            | without hijacking copy/move ctors |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}