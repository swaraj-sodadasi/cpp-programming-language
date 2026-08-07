/*
 * =====================================================================================
 * CONCEPT        : Template Instantiation Model under Templates in Modern C++
 * DESCRIPTION    : Production-grade executable code demonstrating the compilation,
 *                  linkage, and execution mechanics of the C++ Template Instantiation Model:
 *
 *                  1. Implicit Instantiation :
 *                     - On-demand compiler code generation when a template is referenced 
 *                       at a specific Point of Instantiation (POI).
 *
 *                  2. Explicit Instantiation Definition :
 *                     - Forcing compiler instantiation of templates for specific concrete types
 *                       (`template class Engine<int>;`) to reduce compilation overhead.
 *
 *                  3. Explicit Instantiation Declaration (`extern template` - C++11) :
 *                     - Suppressing implicit instantiation in the current translation unit
 *                       when the instantiation is guaranteed to exist elsewhere.
 *
 *                  4. Point of Instantiation (POI) & Dead-Code Elimination :
 *                     - Unused member functions of class templates are NOT instantiated by 
 *                       the compiler unless explicitly called (Lazy Instantiation).
 *
 *                  5. Two-Phase Name Lookup :
 *                     - Phase 1: Non-dependent names checked at template definition time.
 *                     - Phase 2: Dependent names checked at instantiation time (POI).
 *
 * HEADER AUDIT   : Strictly audited. Only headers whose symbols are actively used in the code:
 *                  - <iostream> : Console I/O streams (cin, cout, flush, endl)
 *                  - <string>   : Type std::string
 *                  - <typeinfo> : RTTI typeid operator for runtime type name inspection
 *                  - <limits>   : Stream clearing via std::numeric_limits
 *
 * TIME COMPLEXITY  : Compile-time Monomorphization : O(N) where N is distinct instantiated types.
 *                    Runtime Execution            : O(1) Zero-cost abstraction.
 * SPACE COMPLEXITY : Binary Footprint      : Generated specialized function/class code per type.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <typeinfo>
#include <limits>

using namespace std;

// =====================================================================================
// 1. IMPLICIT INSTANTIATION & LAZY MEMBER INSTANTIATION
// Member functions are only compiled/instantiated if called (Lazy Instantiation).
// =====================================================================================
template <typename T>
class ImplicitProcessor {
private:
    T data_;

public:
    explicit ImplicitProcessor(T val) : data_(val) {}

    void executeUsedMethod() const {
        cout << "    [Implicit Instantiation (Used Method)] Executing for T = " 
             << typeid(T).name() << " | Value: " << data_ << "\n";
    }

    // This method is NEVER instantiated if it is never called,
    // meaning invalid syntax inside unused methods won't fail compilation if uninstantiated.
    void executeUnusedMethod() const {
        cout << "    [Uninstantiated Method] Lazy instantiation omitted this from binary!\n";
    }
};

// =====================================================================================
// 2. EXPLICIT INSTANTIATION DEFINITION & DECLARATION (`extern template`)
// =====================================================================================
template <typename T>
class ExplicitEngine {
private:
    T value_;

public:
    explicit ExplicitEngine(T val) : value_(val) {}

    T computeDouble() const {
        return value_ + value_;
    }

    void render() const {
        cout << "    [Explicit Engine<" << typeid(T).name() << ">] Compute Double Result: " 
             << computeDouble() << "\n";
    }
};

// EXPLICIT INSTANTIATION DEFINITION:
// Forces the compiler to instantiate ALL member functions of ExplicitEngine<int>
// right here, regardless of whether they are used in this translation unit.
template class ExplicitEngine<int>;

// EXPLICIT INSTANTIATION DECLARATION (extern template - C++11):
// Tells the compiler: "Do NOT implicitly instantiate ExplicitEngine<double> here;
// an explicit instantiation definition exists in another compilation unit."
extern template class ExplicitEngine<double>;

// Definition for double to satisfy linkage in this self-contained demo file
template class ExplicitEngine<double>;

// =====================================================================================
// 3. TWO-PHASE NAME LOOKUP DEMONSTRATION
// =====================================================================================

// Non-dependent free function resolved during Phase 1 (Template Definition Time)
static void nonDependentPhase1Logger() {
    cout << "    [Phase 1 Lookup] Non-dependent name resolved at Template Definition time.\n";
}

template <typename T>
class TwoPhaseLookupDemo {
public:
    void process(T input) {
        // Phase 1: nonDependentPhase1Logger does NOT depend on T -> Checked at definition time
        nonDependentPhase1Logger();

        // Phase 2: input and T depend on template parameter -> Checked at Point of Instantiation (POI)
        cout << "    [Phase 2 Lookup] Dependent type name evaluated at POI for T = " 
             << typeid(T).name() << " | Input: " << input << "\n";
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Template Instantiation analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. IMPLICIT INSTANTIATION DEMONSTRATION
    // =====================================================================================
    cout << "\n================ 1. IMPLICIT (LAZY) INSTANTIATION ================\n";

    // POI (Point of Instantiation) for ImplicitProcessor<int> class skeleton & constructor
    ImplicitProcessor<int> implicitIntObj(userInputValue);

    // POI for executeUsedMethod() - compiler instantiates ONLY this member method
    implicitIntObj.executeUsedMethod();

    ImplicitProcessor<string> implicitStrObj("POI_Trigger_String");
    implicitStrObj.executeUsedMethod();

    // Note: executeUnusedMethod() was never called, so compiler never compiled its binary body!

    // =====================================================================================
    // 2. EXPLICIT INSTANTIATION DEMONSTRATION
    // =====================================================================================
    cout << "\n================ 2. EXPLICIT INSTANTIATION & EXTERN TEMPLATE ================\n";

    ExplicitEngine<int> explicitIntObj(userInputValue);
    explicitIntObj.render();

    ExplicitEngine<double> explicitDblObj(static_cast<double>(userInputValue) * 1.5);
    explicitDblObj.render();

    // =====================================================================================
    // 3. TWO-PHASE NAME LOOKUP DEMONSTRATION
    // =====================================================================================
    cout << "\n================ 3. TWO-PHASE NAME LOOKUP ================\n";

    TwoPhaseLookupDemo<int> phaseDemo;
    phaseDemo.process(userInputValue);

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ TEMPLATE INSTANTIATION MODEL SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Instantiation Model   | C++ Implementation Syntax         | Compilation & Linkage Behavior    |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Implicit Instantiation| `Container<int> obj;`             | On-demand code generation at POI  |\n"
         << "| Lazy Instantiation    | Member methods called as needed   | Unused member methods NOT compiled|\n"
         << "| Explicit Definition   | `template class Engine<int>;`     | Forces full compilation of all    |\n"
         << "|                       |                                   | members for specified type        |\n"
         << "| Explicit Declaration  | `extern template class Engine<T>;`| Prevents duplicate instantiation  |\n"
         << "| (C++11)               |                                   | across multiple object files      |\n"
         << "| Two-Phase Lookup      | Phase 1: Non-Dependent Names      | Phase 1 resolved at definition;   |\n"
         << "|                       | Phase 2: Dependent Names          | Phase 2 resolved at POI           |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}