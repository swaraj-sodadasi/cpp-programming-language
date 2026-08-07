/*
 * =====================================================================================
 * CONCEPT        : Template Class Initialization under Templates in Modern C++
 * DESCRIPTION    : Production-grade executable code demonstrating all modern methods and
 *                  mechanics for initializing template classes:
 *
 *                  1. Direct & Uniform (Brace) Constructor Initialization
 *                     - Instantiating template classes using traditional parentheses `()`
 *                       and C++11 uniform brace initialization `{}`.
 *
 *                  2. `std::initializer_list` Constructor Initialization
 *                     - Enabling variadic element sequence initialization for template containers.
 *
 *                  3. Class Template Argument Deduction (CTAD - C++17) & Deduction Guides
 *                     - Implicitly deducing template type parameters during initialization
 *                       without explicit `<T>` qualification, including explicit guides.
 *
 *                  4. Aggregate Initialization of Class Templates
 *                     - Direct member initialization of template structs/aggregates without ctors.
 *
 *                  5. Template Class Static Member Initialization
 *                     - Defining and initializing static data members of class templates.
 *
 * HEADER AUDIT   : Strictly audited. Only headers whose symbols are actively used in the code:
 *                  - <iostream>         : Console I/O streams (cin, cout, flush)
 *                  - <string>           : Type std::string
 *                  - <initializer_list> : Type std::initializer_list for sequence initialization
 *                  - <typeinfo>         : RTTI typeid operator for type inspection
 *                  - <limits>           : Stream clearing via std::numeric_limits
 *                  - <cstddef>          : Type std::size_t
 *
 * TIME COMPLEXITY  : Compile-time Monomorphization : O(N) distinct class template instantiations.
 *                    Runtime Execution            : O(1) Zero-cost abstraction for initialization.
 * SPACE COMPLEXITY : Binary footprint scales with each unique instantiated class template type.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <initializer_list>
#include <typeinfo>
#include <limits>
#include <cstddef>

using namespace std;

// =====================================================================================
// 1. CLASS TEMPLATE WITH CONSTRUCTOR, UNIFORM, AND INITIALIZER_LIST INITIALIZATION
// =====================================================================================
template <typename T>
class SequenceContainer {
private:
    T* data_{nullptr};
    std::size_t size_{0};

public:
    // A. Direct Value Constructor
    SequenceContainer(std::size_t size, const T& defaultValue)
        : data_(new T[size]), size_(size) {
        for (std::size_t i = 0; i < size_; ++i) {
            data_[i] = defaultValue;
        }
    }

    // B. std::initializer_list Constructor for Brace Sequence Initialization
    SequenceContainer(std::initializer_list<T> list)
        : data_(new T[list.size()]), size_(list.size()) {
        std::size_t index = 0;
        for (const auto& item : list) {
            data_[index++] = item;
        }
    }

    ~SequenceContainer() noexcept {
        delete[] data_;
    }

    // Disable copy for simplicity, enable move
    SequenceContainer(const SequenceContainer&) = delete;
    SequenceContainer& operator=(const SequenceContainer&) = delete;

    SequenceContainer(SequenceContainer&& other) noexcept
        : data_(other.data_), size_(other.size_) {
        other.data_ = nullptr;
        other.size_ = 0;
    }

    [[nodiscard]] std::size_t size() const noexcept { return size_; }

    void print(const string& label) const {
        cout << "    [" << label << "] SequenceContainer<" << typeid(T).name() 
             << "> (Size: " << size_ << ") -> { ";
        for (std::size_t i = 0; i < size_; ++i) {
            cout << data_[i] << (i + 1 < size_ ? ", " : " ");
        }
        cout << "}\n";
    }
};

// =====================================================================================
// 2. CTAD (CLASS TEMPLATE ARGUMENT DEDUCTION) & USER-DEFINED DEDUCTION GUIDES
// =====================================================================================
template <typename T>
class SmartValueHolder {
private:
    T value_;

public:
    // Value constructor
    explicit SmartValueHolder(T val) : value_(val) {}

    // Pointer-to-value constructor (Demonstrates explicit CTAD Guide needs)
    SmartValueHolder(const T* ptrVal, std::size_t count) 
        : value_((ptrVal && count > 0) ? *ptrVal : T{}) {}

    [[nodiscard]] T getValue() const { return value_; }

    void print(const string& label) const {
        cout << "    [" << label << "] SmartValueHolder<" << typeid(T).name() 
             << "> Value: " << value_ << "\n";
    }
};

// Custom Deduction Guide for CTAD:
// When initialized with (const T*, size_t), deduce SmartValueHolder<T> instead of SmartValueHolder<const T*>
template <typename T>
SmartValueHolder(const T*, std::size_t) -> SmartValueHolder<T>;

// =====================================================================================
// 3. TEMPLATE AGGREGATE STRUCT & STATIC MEMBER INITIALIZATION
// =====================================================================================
// Aggregate struct template (no user-declared constructors)
template <typename K, typename V>
struct TemplateAggregate {
    K key;
    V value;

    void print(const string& label) const {
        cout << "    [" << label << "] Aggregate<" << typeid(K).name() << ", " 
             << typeid(V).name() << "> -> Key: " << key << " | Value: " << value << "\n";
    }
};

// Class Template with Static Data Member
template <typename T>
class StaticMemberTemplate {
public:
    static inline T staticValue{}; // Inline C++17 static initialization
    static int instanceCounter;     // Out-of-line static member declaration

    StaticMemberTemplate() {
        ++instanceCounter;
    }
};

// Out-of-line Template Static Member Initialization
template <typename T>
int StaticMemberTemplate<T>::instanceCounter = 0;

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Template Class Initialization analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. CONSTRUCTOR & INITIALIZER_LIST INITIALIZATION
    // =====================================================================================
    cout << "\n================ 1. DIRECT, UNIFORM & INITIALIZER_LIST INITIALIZATION ================\n";

    // Direct Initialization using Parentheses ()
    SequenceContainer<int> directObj(3, userInputValue);
    directObj.print("Direct Initialization (size, default)");

    // Uniform / Brace Initialization {}
    SequenceContainer<double> braceObj{4, static_cast<double>(userInputValue) * 1.5};
    braceObj.print("Uniform Brace Initialization");

    // std::initializer_list Sequence Initialization
    SequenceContainer<int> initListObj{userInputValue, userInputValue + 10, userInputValue + 20, userInputValue + 30};
    initListObj.print("Initializer List Sequence");

    // =====================================================================================
    // 2. CLASS TEMPLATE ARGUMENT DEDUCTION (CTAD) & DEDUCTION GUIDES
    // =====================================================================================
    cout << "\n================ 2. CLASS TEMPLATE ARGUMENT DEDUCTION (CTAD) ================\n";

    // Standard CTAD (No explicit <int> or <std::string> template argument required)
    SmartValueHolder ctadInt(userInputValue * 2);
    ctadInt.print("Standard CTAD (Deduced <int>)");

    SmartValueHolder ctadString(string("Modern_Cpp_CTAD"));
    ctadString.print("Standard CTAD (Deduced <std::string>)");

    // CTAD guided by User-Defined Deduction Guide
    int rawValue = userInputValue + 99;
    SmartValueHolder guidedObj(&rawValue, 1); // Deduces SmartValueHolder<int> instead of SmartValueHolder<int*>
    guidedObj.print("Custom Guided CTAD (&int -> <int>)");

    // =====================================================================================
    // 3. TEMPLATE AGGREGATE & STATIC MEMBER INITIALIZATION
    // =====================================================================================
    cout << "\n================ 3. AGGREGATE & STATIC MEMBER INITIALIZATION ================\n";

    // Aggregate Initialization of Template Struct
    TemplateAggregate<int, string> aggregateObj{userInputValue, "Node_Cluster_Alpha"};
    aggregateObj.print("Aggregate Struct Initialization");

    // Static Member Initialization in Class Templates
    StaticMemberTemplate<double>::staticValue = static_cast<double>(userInputValue) * 0.01;
    StaticMemberTemplate<double> statObj1;
    StaticMemberTemplate<double> statObj2;

    cout << "    [Static Template Member] StaticMemberTemplate<double>::staticValue = " 
         << StaticMemberTemplate<double>::staticValue << "\n";
    cout << "    [Static Template Member] StaticMemberTemplate<double>::instanceCounter = " 
         << StaticMemberTemplate<double>::instanceCounter << " instances created\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ TEMPLATE CLASS INITIALIZATION SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Initialization Method | Syntax Example                    | Key Mechanics / C++ Feature       |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Direct ()             | `Class<T> obj(arg1, arg2);`       | Invokes explicit matching ctor    |\n"
         << "| Uniform Brace {}      | `Class<T> obj{arg1, arg2};`       | Prevents narrowing conversions    |\n"
         << "| Initializer List      | `Class<T> obj{1, 2, 3, 4};`       | Uses std::initializer_list ctor   |\n"
         << "| CTAD (C++17)          | `Class obj(10);`                  | Deduces <T> without explicit tags |\n"
         << "| Custom Deduction Guide| `Class(const T*, size_t) -> ...`  | Custom mapping rules for CTAD     |\n"
         << "| Aggregate Init        | `TemplateStruct<K,V> s{k, v};`    | Direct member initialization      |\n"
         << "| Static Member Init    | `template<T> Type Class<T>::var;` | Distinct static per type instance |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}