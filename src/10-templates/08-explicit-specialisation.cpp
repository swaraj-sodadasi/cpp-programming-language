/*
 * =====================================================================================
 * CONCEPT        : Explicit Specialisation (Full Specialization) under Templates in C++
 * DESCRIPTION    : Production-grade executable code demonstrating the syntax, mechanics,
 *                  and applications of Explicit (Full) Template Specialisation in C++:
 *
 *                  1. Explicit Function Template Specialization :
 *                     - Customizing function implementation for specific target types 
 *                       using `template <> void functionName<SpecificType>(...)`.
 *
 *                  2. Explicit Class Template Specialization :
 *                     - Overriding the entire class template blueprint for a specific type
 *                       (`template <> class ClassName<SpecificType>`), allowing unique data 
 *                       structures, member functions, or layouts (e.g., `std::vector<bool>`).
 *
 *                  3. Explicit Member Function Specialization :
 *                     - Specializing individual member methods of a primary class template 
 *                       for a specific type without specializing the entire class.
 *
 * HEADER AUDIT   : Strictly audited. Only headers whose symbols are actively used in the code:
 *                  - <iostream> : Console I/O streams (cin, cout, flush)
 *                  - <string>   : Type std::string
 *                  - <typeinfo> : RTTI typeid operator for type inspection
 *                  - <limits>   : Stream clearing via std::numeric_limits
 *                  - <cstddef>  : Type std::size_t
 *
 * TIME COMPLEXITY  : Compile-time Selection : O(1) exact match lookup by compiler.
 *                    Runtime Execution     : O(1) Zero-cost abstraction.
 * SPACE COMPLEXITY : Binary Footprint      : Generates separate binary code for specialized types.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <typeinfo>
#include <limits>
#include <cstddef>

using namespace std;

// =====================================================================================
// 1. EXPLICIT FUNCTION TEMPLATE SPECIALIZATION
// Primary function template provides generic behavior; specializations override specific types.
// =====================================================================================
class FunctionSpecializationDemo {
public:
    // Primary Function Template
    template <typename T>
    static void printValue(T val) {
        cout << "    [Primary Function Template] Type (" << typeid(T).name() 
             << ") | Raw Value: " << val << "\n";
    }
};

// Explicit Specialization of printValue for `bool`
template <>
void FunctionSpecializationDemo::printValue<bool>(bool val) {
    cout << "    [Explicit Specialization <bool>] Formatted Boolean: " 
         << (val ? "TRUE (1)" : "FALSE (0)") << "\n";
}

// Explicit Specialization of printValue for `std::string`
template <>
void FunctionSpecializationDemo::printValue<string>(string val) {
    std::size_t strLen = val.length();
    cout << "    [Explicit Specialization <std::string>] String (Length: " << strLen 
         << ") | Content: \"" << val << "\"\n";
}

// =====================================================================================
// 2. EXPLICIT CLASS TEMPLATE SPECIALIZATION
// Replacing the primary class layout entirely for a specific type.
// =====================================================================================

// Primary Class Template
template <typename T>
class TypeFormatter {
private:
    T data_;

public:
    explicit TypeFormatter(T data) : data_(data) {}

    void render() const {
        cout << "    [Primary Class Template<T>] Data: " << data_ << "\n";
    }
};

// Explicit Class Template Specialization for `bool`
// Demonstrates that specialized classes can have completely different members and signatures.
template <>
class TypeFormatter<bool> {
private:
    bool bitState_;

public:
    explicit TypeFormatter(bool data) : bitState_(data) {}

    void render() const {
        cout << "    [Specialized Class Template<bool>] Bit Mask Status: " 
             << (bitState_ ? "[ACTIVE / HIGH]" : "[INACTIVE / LOW]") << "\n";
    }

    // Unique method available ONLY in bool specialization
    [[nodiscard]] bool getBit() const noexcept {
        return bitState_;
    }
};

// =====================================================================================
// 3. EXPLICIT MEMBER FUNCTION SPECIALIZATION
// Specializing a single member method without specializing the entire class template.
// =====================================================================================
template <typename T>
class StorageEngine {
private:
    T value_;

public:
    explicit StorageEngine(T val) : value_(val) {}

    void processStorage() const {
        cout << "    [Primary Member Engine<T>] Executing standard generic write for: " 
             << value_ << "\n";
    }
};

// Explicit Specialization of single member method for `double`
template <>
void StorageEngine<double>::processStorage() const {
    cout << "    [Specialized Member Engine<double>] Executing high-precision floating point write: " 
         << value_ << "\n";
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Explicit Specialisation analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. EXPLICIT FUNCTION TEMPLATE SPECIALIZATION
    // =====================================================================================
    cout << "\n================ 1. EXPLICIT FUNCTION TEMPLATE SPECIALIZATION ================\n";

    cout << "  - Calling printValue with `int` (Uses Primary Template):\n";
    FunctionSpecializationDemo::printValue(userInputValue);

    cout << "\n  - Calling printValue with `bool` (Uses Explicit Specialization <bool>):\n";
    FunctionSpecializationDemo::printValue(true);

    cout << "\n  - Calling printValue with `std::string` (Uses Explicit Specialization <std::string>):\n";
    FunctionSpecializationDemo::printValue(string("Modern_Cpp_Templates"));

    // =====================================================================================
    // 2. EXPLICIT CLASS TEMPLATE SPECIALIZATION
    // =====================================================================================
    cout << "\n================ 2. EXPLICIT CLASS TEMPLATE SPECIALIZATION ================\n";

    cout << "  - Instantiating TypeFormatter<int> (Primary Class):\n";
    TypeFormatter<int> intFormatter(userInputValue);
    intFormatter.render();

    cout << "\n  - Instantiating TypeFormatter<bool> (Specialized Class):\n";
    TypeFormatter<bool> boolFormatter(false);
    boolFormatter.render();
    cout << "    Unique method call `getBit()` -> " << boolFormatter.getBit() << "\n";

    // =====================================================================================
    // 3. EXPLICIT MEMBER FUNCTION SPECIALIZATION
    // =====================================================================================
    cout << "\n================ 3. EXPLICIT MEMBER FUNCTION SPECIALIZATION ================\n";

    cout << "  - Instantiating StorageEngine<int> (Primary Member Method):\n";
    StorageEngine<int> intStorage(userInputValue);
    intStorage.processStorage();

    cout << "\n  - Instantiating StorageEngine<double> (Specialized Member Method):\n";
    StorageEngine<double> doubleStorage(static_cast<double>(userInputValue) * 3.14159);
    doubleStorage.processStorage();

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ EXPLICIT SPECIALISATION SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Specialization Type   | Syntax Example                    | Architectural Effect / Rule       |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Function Template     | `template <> void fn<bool>(bool)` | Overrides function for bool type  |\n"
         << "| Class Template        | `template <> class Custom<bool>`  | Completely replaces class layout  |\n"
         << "| Member Function       | `template <> void Class<dbl>::fn()`| Overrides one method, keeps class |\n"
         << "| Unique Class Members  | Added methods in specialized class| Specialization can add new APIs   |\n"
         << "| Overload vs Spec      | Functions prefer overloading      | Prefer function overloading over  |\n"
         << "|                       |                                   | function template specialization  |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}