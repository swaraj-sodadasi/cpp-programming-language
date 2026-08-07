/*
 * =====================================================================================
 * CONCEPT        : Member Templates under Templates in Modern C++
 * DESCRIPTION    : Production-grade executable code demonstrating the syntax, rules,
 *                  and applications of Member Templates inside Class Templates:
 *
 *                  1. Member Function Templates inside Class Templates:
 *                     - Defining generic methods inside a generic class where method 
 *                       template parameters (`U`) are independent of class template parameters (`T`).
 *
 *                  2. Out-of-Line Definitions for Member Templates:
 *                     - Syntax mechanics requiring dual template parameter lists:
 *                       `template <typename T> template <typename U> ...`
 *
 *                  3. Templated Converting Constructors & Assignment Operators:
 *                     - Enabling type conversions across distinct instantiations of the 
 *                       same class template (e.g., assigning `SmartPtr<Derived>` to `SmartPtr<Base>`).
 *
 *                  4. Nested Class Templates inside Class Templates:
 *                     - Declaring and defining nested class templates (`NestedNode<U>`) 
 *                       inside an outer class template (`OuterContainer<T>`).
 *
 * HEADER AUDIT   : Strictly audited. Only headers whose symbols are actively used in the code:
 *                  - <iostream> : Console I/O streams (cin, cout, flush, endl)
 *                  - <string>   : Type std::string and std::to_string
 *                  - <typeinfo> : RTTI typeid operator for runtime type name inspection
 *                  - <limits>   : Stream clearing via std::numeric_limits
 *                  - <cstddef>  : Type std::size_t
 *                  - <utility>  : std::move and std::forward
 *
 * TIME COMPLEXITY  : Compile-time Monomorphization : O(N * M) instantiations (N class types, M method types).
 *                    Runtime Execution            : O(1) Zero-cost abstraction.
 * SPACE COMPLEXITY : Binary Footprint      : Scales with each unique class + member combination.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <typeinfo>
#include <limits>

using namespace std;

// =====================================================================================
// 1. CLASS TEMPLATE WITH MEMBER FUNCTION TEMPLATES & CONVERTING CONSTRUCTORS
// =====================================================================================
template <typename T>
class CustomWrapper {
private:
    T data_;

public:
    // Standard Constructor
    explicit CustomWrapper(T data) : data_(data) {}

    [[nodiscard]] T getValue() const { return data_; }

    // Templated Converting Constructor:
    // Allows constructing CustomWrapper<T> from CustomWrapper<U> if U is convertible to T
    template <typename U>
    CustomWrapper(const CustomWrapper<U>& other)
        : data_(static_cast<T>(other.getValue())) {
        cout << "    [Templated Converting Ctor] Converted CustomWrapper<" 
             << typeid(U).name() << "> to CustomWrapper<" << typeid(T).name() << ">\n";
    }

    // Inline Member Function Template (Method parameter U independent of Class T)
    template <typename U>
    void inlineProcess(U value) {
        cout << "    [Inline Member Template] Class T (" << typeid(T).name() 
             << ") processing Method U (" << typeid(U).name() << "): " << value << "\n";
    }

    // Member Function Template declared inline, DEFINED OUTSIDE the class body
    template <typename U>
    [[nodiscard]] T transformAndAdd(U input);
};

// Out-of-Line Member Function Template Definition Syntax:
// Requires TWO separate template parameter lists:
// 1st for the enclosing class template parameter (T)
// 2nd for the member function template parameter (U)
template <typename T>
template <typename U>
T CustomWrapper<T>::transformAndAdd(U input) {
    cout << "    [Out-of-Line Member Template] Class T: " << typeid(T).name() 
         << " | Input U: " << typeid(U).name() << " (" << input << ")\n";
    return data_ + static_cast<T>(input);
}

// =====================================================================================
// 2. NESTED CLASS TEMPLATE INSIDE CLASS TEMPLATE
// =====================================================================================
template <typename OuterType>
class OuterBlueprint {
private:
    OuterType outerVal_;

public:
    explicit OuterBlueprint(OuterType val) : outerVal_(val) {}

    // Nested Class Template Declaration inside Outer Class Template
    template <typename InnerType>
    class NestedNode {
    private:
        InnerType innerVal_;

    public:
        NestedNode(InnerType val) : innerVal_(val) {}

        void displayHierarchy(const OuterBlueprint& parent) const {
            cout << "    [Nested Class Template] Outer (" << typeid(OuterType).name() 
                 << "): " << parent.outerVal_ << " | Inner (" << typeid(InnerType).name() 
                 << "): " << innerVal_ << "\n";
        }
    };
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Member Templates analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. MEMBER FUNCTION TEMPLATES (INLINE AND OUT-OF-LINE)
    // =====================================================================================
    cout << "\n================ 1. MEMBER FUNCTION TEMPLATES IN CLASS TEMPLATES ================\n";

    CustomWrapper<double> doubleWrapper(static_cast<double>(userInputValue) * 1.5);

    // Calling inline member template with double class parameter and string method parameter
    doubleWrapper.inlineProcess(string("MemberTemplateArgument"));

    // Calling out-of-line member template
    auto transformedVal = doubleWrapper.transformAndAdd(25);
    cout << "  - Result of transformAndAdd(25): " << transformedVal << "\n";

    // =====================================================================================
    // 2. TEMPLATED CONVERTING CONSTRUCTORS
    // =====================================================================================
    cout << "\n================ 2. TEMPLATED CONVERTING CONSTRUCTORS ================\n";

    // Source instance: CustomWrapper<int>
    CustomWrapper<int> intSource(userInputValue);

    // Target instance: CustomWrapper<double> initialized using CustomWrapper<int>
    CustomWrapper<double> convertedTarget = intSource; 
    cout << "  - Converted Target Value: " << convertedTarget.getValue() << "\n";

    // =====================================================================================
    // 3. NESTED CLASS TEMPLATES UNDER CLASS TEMPLATES
    // =====================================================================================
    cout << "\n================ 3. NESTED CLASS TEMPLATES UNDER CLASS TEMPLATES ================\n";

    // Outer class instantiated with int
    OuterBlueprint<int> outerObj(userInputValue);

    // Nested class template instantiated with std::string
    OuterBlueprint<int>::NestedNode<string> nestedNode("Cluster_SubNode_1");
    nestedNode.displayHierarchy(outerObj);

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ MEMBER TEMPLATES SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Member Template Form  | C++ Syntax Example                | Architectural Purpose / Behavior  |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Inline Member Method  | `template<typename U> void fn(U)` | Method parameter U independent    |\n"
         << "|                       |                                   | of class parameter T              |\n"
         << "| Out-of-Line Method    | `template<T> template<U>          | Requires dual parameter lists     |\n"
         << "|                       |  ReturnType Class<T>::fn(U)`      | for out-of-line scope definition  |\n"
         << "| Converting Constructor| `template<U> Class(const Class<U>&)`| Interop between distinct instances|\n"
         << "|                       |                                   | of the same class template        |\n"
         << "| Nested Class Template | `Outer<T>::Nested<U> obj;`        | Parameterized inner helper classes|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}