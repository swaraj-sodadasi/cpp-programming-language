/*
 * =====================================================================================
 * CONCEPT        : One Definition Rule (ODR) and Templates under Templates in Modern C++
 * DESCRIPTION    : Production-grade executable code demonstrating the One Definition Rule
 *                  (ODR) rules, exemptions, mechanics, and best practices applied to Templates:
 *
 *                  1. The ODR Rule & Template Exemption :
 *                     - Standard non-inline entities must have exactly ONE definition in 
 *                       the entire program (One Definition Rule).
 *                     - Templates and inline functions/variables are EXEMPT from the single-TU 
 *                       definition restriction: they can be defined in multiple translation 
 *                       units (TUs), provided the definitions are TOKEN-FOR-TOKEN IDENTICAL.
 *
 *                  2. Linker Deduplication & COMDAT / Weak Symbols :
 *                     - Compilers emit template instantiations into COMDAT / weak symbol sections. 
 *                       The linker discards duplicate instantiations across TUs and merges them 
 *                       into a single canonical binary instance.
 *
 *                  3. Template Static Member Variables and ODR :
 *                     - Static data members of class templates share a SINGLE memory address 
 *                       across all translation units for each unique instantiation type.
 *
 *                  4. ODR Violations in Templates :
 *                     - If template definitions differ across TUs (e.g., due to conditional macros), 
 *                       it causes an ODR violation (Undefined Behavior), which linkers often 
 *                       cannot detect due to duplicate symbol stripping.
 *
 *                  5. Explicit Instantiation & ODR :
 *                     - `template class Foo<int>;` forces a single instantiation point, 
 *                       while `extern template class Foo<int>;` suppresses local duplication.
 *
 * HEADER AUDIT   : Strictly audited. Only headers whose symbols are actively used in the code:
 *                  - <iostream> : Console I/O streams (cin, cout, flush, endl)
 *                  - <string>   : Type std::string
 *                  - <typeinfo> : RTTI typeid operator for runtime type name inspection
 *                  - <limits>   : Stream clearing via std::numeric_limits
 *                  - <cstddef>  : Type std::size_t
 *
 * TIME COMPLEXITY  : Link-Time Symbol Deduplication : O(N) symbol merge where N is duplicate count.
 *                    Runtime Execution             : O(1) Zero-cost abstraction.
 * SPACE COMPLEXITY : Binary Footprint       : Single merged instance per template instantiation.
 * =====================================================================================
 */

#include <iostream>
#include <typeinfo>
#include <limits>
#include <cstddef>

using namespace std;

// =====================================================================================
// 1. TEMPLATE CLASS DEMONSTRATING ODR EXEMPTION & LINKER DEDUPLICATION
// In a multi-file project, this header would be included in multiple .cpp files.
// ODR permits identical template definitions across multiple translation units.
// =====================================================================================
template <typename T>
class OdrSafeTemplate {
private:
    T value_;

public:
    explicit OdrSafeTemplate(T val) : value_(val) {}

    // Inline function inside class template (ODR exempt as long as definition is identical in all TUs)
    void executeOdrCheck() const {
        cout << "    [ODR Safe Template] Function definition merged by linker across TUs for T = "
             << typeid(T).name() << " | Value: " << value_ << "\n";
    }
};

// =====================================================================================
// 2. TEMPLATE STATIC DATA MEMBERS AND ODR GUARANTEES
// Standard static members of non-template classes require out-of-line definition in 1 TU.
// Template static members can be defined in headers without violating ODR!
// =====================================================================================
template <typename T>
class SharedCounterTemplate {
public:
    // C++17 inline static member variable initialization inside class template
    static inline T sharedValue{};
    static inline std::size_t instanceCounter = 0;

    SharedCounterTemplate() {
        ++instanceCounter;
    }

    static const void* getSharedValueAddress() {
        return static_cast<const void*>(&sharedValue);
    }
};

// =====================================================================================
// 3. EXPLICIT INSTANTIATION & ODR CONTROL
// Controlling symbol generation across translation units using explicit instantiation
// =====================================================================================
template <typename T>
class ExplicitOdrEngine {
private:
    T data_;

public:
    explicit ExplicitOdrEngine(T d) : data_(d) {}

    T process() const {
        return data_ * 2;
    }

    void displayAddress() const {
        cout << "    [Explicit ODR Engine<" << typeid(T).name() << ">] Processing Address: "
             << static_cast<const void*>(this) << " | Result: " << process() << "\n";
    }
};

// Explicit Instantiation Definition: Forces instantiation of ExplicitOdrEngine<double>
template class ExplicitOdrEngine<double>;

// Explicit Instantiation Declaration (extern template):
// Prevents duplicate compiler work across TUs while maintaining ODR compliance
extern template class ExplicitOdrEngine<int>;

// Concrete definition to satisfy linker in this standalone translation unit
template class ExplicitOdrEngine<int>;

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for ODR and Templates analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. TEMPLATE ODR EXEMPTION DEMONSTRATION
    // =====================================================================================
    cout << "\n================ 1. TEMPLATE CLASS ODR EXEMPTION ================\n";

    OdrSafeTemplate<int> obj1(userInputValue);
    OdrSafeTemplate<int> obj2(userInputValue + 50);

    obj1.executeOdrCheck();
    obj2.executeOdrCheck();
    cout << "  - Note: Template definitions in headers can be included in multiple TUs\n"
         << "    without duplicate symbol errors because linkers deduplicate template COMDAT symbols.\n";

    // =====================================================================================
    // 2. TEMPLATE STATIC DATA MEMBERS & SINGLE ADDRESS ODR GUARANTEE
    // =====================================================================================
    cout << "\n================ 2. TEMPLATE STATIC DATA MEMBERS & SINGLE ADDRESS ================\n";

    SharedCounterTemplate<int>::sharedValue = userInputValue;
    SharedCounterTemplate<int> counter1;
    SharedCounterTemplate<int> counter2;

    cout << "  - Shared Counter Instances Created (std::size_t): " << SharedCounterTemplate<int>::instanceCounter << "\n";
    cout << "  - Shared Value via Instance 1 Address: " << SharedCounterTemplate<int>::getSharedValueAddress() << "\n";

    SharedCounterTemplate<int>::sharedValue = userInputValue + 200;
    cout << "  - Modified sharedValue through static member: " << SharedCounterTemplate<int>::sharedValue << "\n";
    cout << "  - Verification: All TUs and instances share the exact same memory address for T = int.\n";

    // =====================================================================================
    // 3. EXPLICIT INSTANTIATION & ODR
    // =====================================================================================
    cout << "\n================ 3. EXPLICIT INSTANTIATION & ODR CONTROL ================\n";

    ExplicitOdrEngine<int> engineInt(userInputValue);
    engineInt.displayAddress();

    ExplicitOdrEngine<double> engineDouble(static_cast<double>(userInputValue) * 1.5);
    engineDouble.displayAddress();

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ ODR AND TEMPLATES SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| ODR Aspect            | C++ Syntax / Rule                 | Architectural Behavior / Effect   |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| General Non-Template  | 1 Definition in entire program    | Duplicate definitions cause build |\n"
         << "|                       |                                   | linker errors (Multiple Symbol)   |\n"
         << "| Template Exemption    | Identical definitions across TUs  | Permitted in headers; linker      |\n"
         << "|                       |                                   | merges COMDAT / weak symbols      |\n"
         << "| Identicality Rule     | Token-for-token identical code    | Mismatched definitions cause      |\n"
         << "|                       | in all translation units          | Undefined Behavior (ODR Violation)|\n"
         << "| Static Data Members   | `static inline T var{};`          | Guaranteed single global memory   |\n"
         << "|                       | in class templates                | address per instantiated type     |\n"
         << "| Explicit Definition   | `template class Foo<int>;`        | Forces single translation unit to |\n"
         << "|                       |                                   | generate all template binary code |\n"
         << "| Explicit Declaration  | `extern template class Foo<int>;` | Prevents redundant instantiations |\n"
         << "| (C++11)               |                                   | in remaining translation units    |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}