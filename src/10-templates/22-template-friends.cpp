/*
 * =====================================================================================
 * CONCEPT        : Template Friends under Templates in Modern C++
 * DESCRIPTION    : Production-grade executable code demonstrating all four primary forms
 *                  of friend declarations inside class templates:
 *
 *                  1. Non-Template Friend Functions Defined Inside (Hidden Friend Idiom) :
 *                     - A non-template function injected into the enclosing namespace via Argument-Dependent
 *                       Lookup (ADL). Most common for binary operators like `operator<<` or `operator+`.
 *
 *                  2. Bound Template Friend Functions (1-to-1 Match) :
 *                     - A specific instantiation `fn<T>` is a friend of `Class<T>`. Requires forward
 *                       declarations of both the class template and function template.
 *
 *                  3. Unbound Template Friend Functions (1-to-Many / Any-to-Any Match) :
 *                     - All instantiations of `fn<U>` are friends of `Class<T>` regardless of type parameter mismatch.
 *
 *                  4. Friend Class Templates (Cross-Instantiation Friend Access) :
 *                     - Granting friendship to another class template or across different instantiations 
 *                       of the same class template (e.g., allowing `Node<int>` to access private members of `Node<double>`).
 *
 * HEADER AUDIT   : Strictly audited. Only headers whose symbols are actively used in the code:
 *                  - <iostream> : Console I/O streams (cin, cout, flush, endl, ostream)
 *                  - <string>   : Type std::string
 *                  - <typeinfo> : RTTI typeid operator for runtime type name inspection
 *                  - <limits>   : Stream clearing via std::numeric_limits
 *
 * TIME COMPLEXITY  : Compile-time Friendship Resolution : O(1) lookup during overload resolution.
 *                    Runtime Execution                 : O(1) Zero-cost abstraction.
 * SPACE COMPLEXITY : Binary Footprint                   : Zero runtime memory overhead.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <typeinfo>
#include <limits>

using namespace std;

// =====================================================================================
// FORWARD DECLARATIONS FOR BOUND TEMPLATE FRIEND FUNCTIONS
// Required so the compiler recognizes the function template before the class definition.
// =====================================================================================
template <typename T>
class BoundFriendBox;

template <typename T>
void printBoundBox(const BoundFriendBox<T>& box);

// =====================================================================================
// 1. NON-TEMPLATE FRIEND FUNCTION (HIDDEN FRIEND IDIOM)
// =====================================================================================
template <typename T>
class HiddenFriendBox {
private:
    T secretData_;

public:
    explicit HiddenFriendBox(T val) : secretData_(val) {}

    // Non-template friend defined inside the class template body.
    // Injected into the surrounding namespace via Argument-Dependent Lookup (ADL).
    friend ostream& operator<<(ostream& os, const HiddenFriendBox& box) {
        os << "HiddenFriendBox<" << typeid(T).name() << "> [Secret Data: " << box.secretData_ << "]";
        return os;
    }
};

// =====================================================================================
// 2. BOUND TEMPLATE FRIEND FUNCTION (1-TO-1 MATCHING TYPE PARAMETER)
// =====================================================================================
template <typename T>
class BoundFriendBox {
private:
    T boundValue_;

public:
    explicit BoundFriendBox(T val) : boundValue_(val) {}

    // Friends ONLY printBoundBox<T> with the matching type parameter T
    friend void printBoundBox<T>(const BoundFriendBox<T>& box);
};

// Definition of Bound Template Friend Function
template <typename T>
void printBoundBox(const BoundFriendBox<T>& box) {
    cout << "    [Bound Template Friend fn<T>] Accessing private boundValue_: " 
         << box.boundValue_ << " (Type: " << typeid(T).name() << ")\n";
}

// =====================================================================================
// 3. UNBOUND TEMPLATE FRIEND FUNCTION (ANY-TO-ANY MATCH)
// =====================================================================================
template <typename T>
class UnboundFriendBox {
private:
    T value_;

public:
    explicit UnboundFriendBox(T val) : value_(val) {}

    // Grants friendship to ALL instantiations of printUnboundBox<U> regardless of U
    template <typename U>
    friend void printUnboundBox(const UnboundFriendBox<U>& box);
};

// Unbound Template Friend Function Definition
template <typename U>
void printUnboundBox(const UnboundFriendBox<U>& box) {
    cout << "    [Unbound Template Friend fn<U>] Accessing private value_: " 
         << box.value_ << " (Type: " << typeid(U).name() << ")\n";
}

// =====================================================================================
// 4. FRIEND CLASS TEMPLATES (CROSS-INSTANTIATION CONVERSION)
// =====================================================================================
template <typename ElementType>
class FlexibleNode {
private:
    ElementType data_;

    // Grants friendship to ALL instantiations of FlexibleNode<OtherType>
    template <typename OtherType>
    friend class FlexibleNode;

public:
    explicit FlexibleNode(ElementType data) : data_(data) {}

    [[nodiscard]] ElementType getData() const { return data_; }

    // Allows converting/copying private data_ from a FlexibleNode<OtherType>
    template <typename OtherType>
    void copyFromOtherNode(const FlexibleNode<OtherType>& other) {
        // Direct private access to other.data_ enabled by class template friendship
        this->data_ = static_cast<ElementType>(other.data_);
        cout << "    [Friend Class Template Access] Successfully copied private data_ (" 
             << other.data_ << ") from FlexibleNode<" << typeid(OtherType).name() 
             << "> to FlexibleNode<" << typeid(ElementType).name() << ">\n";
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Template Friends analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. NON-TEMPLATE FRIEND (HIDDEN FRIEND IDIOM)
    // =====================================================================================
    cout << "\n================ 1. HIDDEN FRIEND INLINE OPERATOR OVERLOAD ================\n";

    HiddenFriendBox<int> intHiddenBox(userInputValue);
    HiddenFriendBox<string> strHiddenBox("Modern_Cpp_Friend_Templates");

    // Found via Argument-Dependent Lookup (ADL)
    cout << "  - Outputting via ADL Hidden Friend: " << intHiddenBox << "\n";
    cout << "  - Outputting via ADL Hidden Friend: " << strHiddenBox << "\n";

    // =====================================================================================
    // 2. BOUND TEMPLATE FRIEND FUNCTIONS
    // =====================================================================================
    cout << "\n================ 2. BOUND TEMPLATE FRIEND FUNCTIONS (1-TO-1 MATCH) ================\n";

    BoundFriendBox<int> boundIntBox(userInputValue * 2);
    BoundFriendBox<double> boundDoubleBox(static_cast<double>(userInputValue) * 3.14159);

    printBoundBox(boundIntBox);       // Invokes printBoundBox<int>
    printBoundBox(boundDoubleBox);    // Invokes printBoundBox<double>

    // =====================================================================================
    // 3. UNBOUND TEMPLATE FRIEND FUNCTIONS
    // =====================================================================================
    cout << "\n================ 3. UNBOUND TEMPLATE FRIEND FUNCTIONS (ANY-TO-ANY) ================\n";

    UnboundFriendBox<int> unboundIntBox(userInputValue + 500);
    UnboundFriendBox<string> unboundStrBox("Unbound_Friend_Access");

    printUnboundBox(unboundIntBox);
    printUnboundBox(unboundStrBox);

    // =====================================================================================
    // 4. FRIEND CLASS TEMPLATES (CROSS-INSTANTIATION ACCESS)
    // =====================================================================================
    cout << "\n================ 4. FRIEND CLASS TEMPLATES (CROSS-INSTANTIATION ACCESS) ================\n";

    FlexibleNode<double> doubleNode(static_cast<double>(userInputValue) * 1.5);
    FlexibleNode<int> intNode(0);

    cout << "  - Initial intNode value: " << intNode.getData() << "\n";
    intNode.copyFromOtherNode(doubleNode); // intNode accesses private data_ of doubleNode
    cout << "  - Updated intNode value: " << intNode.getData() << "\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ TEMPLATE FRIENDS SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Friend Category       | Declaration Syntax Example        | Architectural Purpose & Access    |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Hidden Friend (ADL)   | `friend os& op<<(os&, const C&)`  | Non-template inlined operator     |\n"
         << "|                       | defined inside class body         | resolved via ADL                  |\n"
         << "| Bound Friend Fn       | `friend void fn<T>(const C<T>&);` | Grants access ONLY to `fn<T>` for |\n"
         << "|                       | Requires forward declarations     | matching type parameter T         |\n"
         << "| Unbound Friend Fn     | `template<U> friend void fn(...)` | Grants access to `fn<U>` for ALL  |\n"
         << "|                       | inside class body                 | instantiations regardless of U    |\n"
         << "| Class Template Friend | `template<U> friend class Node;`  | Grants cross-instantiation access |\n"
         << "|                       | inside class body                 | between `Node<T>` and `Node<U>`   |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}