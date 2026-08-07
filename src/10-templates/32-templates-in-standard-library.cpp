/*
 * =====================================================================================
 * CONCEPT        : Templates in the Standard Library (STL) under Templates
 * DESCRIPTION    : Production-grade executable code demonstrating how C++ templates form 
 *                  the foundational architecture of the C++ Standard Template Library (STL):
 *
 *                  1. Container Class Templates (Type & Non-Type Parameters) :
 *                     - Dynamic memory sequence containers (`std::vector<T>`).
 *                     - Fixed-size compile-time stack containers (`std::array<T, N>`).
 *
 *                  2. Utility & Smart Pointer Templates :
 *                     - Heterogeneous tuple/pair abstractions (`std::pair<T1, T2>`).
 *                     - RAII memory ownership wrappers (`std::unique_ptr<T>`).
 *
 *                  3. Algorithm Function Templates :
 *                     - Generic, container-agnostic sequence operations (`std::sort`, `std::transform`).
 *
 *                  4. Type Traits & Metaprogramming Templates :
 *                     - Compile-time type introspection and trait transformation (`std::is_same_v`, 
 *                       `std::decay_t`).
 *
 * HEADER AUDIT   : Strictly audited. Only headers whose symbols are actively used in the code:
 *                  - <iostream>    : Console I/O streams (cin, cout, flush, endl)
 *                  - <vector>      : Class template std::vector<T>
 *                  - <array>       : Class template std::array<T, N>
 *                  - <memory>      : Class template std::unique_ptr<T> and std::make_unique
 *                  - <algorithm>   : Function templates std::sort, std::transform
 *                  - <utility>     : Class template std::pair<T1, T2> and std::make_pair
 *                  - <type_traits> : Type trait templates std::is_same_v, std::decay_t
 *                  - <string>      : Type std::string
 *                  - <typeinfo>    : RTTI typeid operator for type name inspection
 *                  - <limits>      : Stream clearing via std::numeric_limits
 *                  - <cstddef>     : Type std::size_t
 *
 * TIME COMPLEXITY  : Compile-time Monomorphization : O(1) type deduction and code expansion per template type.
 *                    Runtime Execution            : Container / Algorithm matching STL complexities.
 * SPACE COMPLEXITY : Binary Footprint             : Scaled linearly with unique monomorphized type instantiations.
 * =====================================================================================
 */

#include <iostream>
#include <vector>
#include <array>
#include <memory>
#include <algorithm>
#include <utility>
#include <type_traits>
#include <string>
#include <typeinfo>
#include <limits>
#include <cstddef>

using namespace std;

// =====================================================================================
// 1. CONTAINER CLASS TEMPLATES (std::vector & std::array)
// =====================================================================================
template <typename T, std::size_t N>
void demonstrateContainerTemplates(T initialValue) {
    // std::vector: Class template with type parameter T (Allocates on heap dynamically)
    std::vector<T> dynamicVec;
    dynamicVec.push_back(initialValue);
    dynamicVec.push_back(static_cast<T>(initialValue * 2));
    dynamicVec.push_back(static_cast<T>(initialValue * 3));

    // std::array: Class template with type parameter T AND Non-Type Template Parameter (NTTP) N
    std::array<T, N> fixedArray{};
    for (std::size_t i = 0; i < N; ++i) {
        fixedArray[i] = static_cast<T>(initialValue + i * 10);
    }

    cout << "    [std::vector<" << typeid(T).name() << ">] Dynamic Elements: { ";
    for (const auto& elem : dynamicVec) cout << elem << " ";
    cout << "}\n";

    cout << "    [std::array<" << typeid(T).name() << ", " << N << ">] Stack Elements: { ";
    for (const auto& elem : fixedArray) cout << elem << " ";
    cout << "}\n";
}

// =====================================================================================
// 2. UTILITY & SMART POINTER TEMPLATES (std::pair & std::unique_ptr)
// =====================================================================================
template <typename KeyType, typename ValueType>
void demonstrateUtilityTemplates(KeyType key, ValueType val) {
    // std::pair: Class template combining two heterogeneous types
    std::pair<KeyType, ValueType> entry = std::make_pair(key, val);

    // std::unique_ptr: Smart pointer class template managing single-owner memory lifetime
    std::unique_ptr<KeyType> smartPtr = std::make_unique<KeyType>(key);

    cout << "    [std::pair<" << typeid(KeyType).name() << ", " << typeid(ValueType).name() << ">] Pair Key: " 
         << entry.first << " | Value: " << entry.second << "\n";

    cout << "    [std::unique_ptr<" << typeid(KeyType).name() << ">] Heap Object Address: " 
         << static_cast<const void*>(smartPtr.get()) << " | Dereferenced Value: " << *smartPtr << "\n";
}

// =====================================================================================
// 3. ALGORITHM FUNCTION TEMPLATES (std::sort & std::transform)
// =====================================================================================
template <typename Container>
void demonstrateAlgorithmTemplates(Container& data) {
    cout << "    [Original Sequence]: ";
    for (const auto& v : data) cout << v << " ";
    cout << "\n";

    // std::sort: Function template deduced on iterator type Iterator
    std::sort(data.begin(), data.end(), std::greater<typename Container::value_type>());

    cout << "    [std::sort (Descending)]: ";
    for (const auto& v : data) cout << v << " ";
    cout << "\n";

    // std::transform: Function template applying unary operation
    std::transform(data.begin(), data.end(), data.begin(), [](auto val) {
        return val * 2;
    });

    cout << "    [std::transform (Doubled)]: ";
    for (const auto& v : data) cout << v << " ";
    cout << "\n";
}

// =====================================================================================
// 4. TYPE TRAIT TEMPLATES (<type_traits>)
// =====================================================================================
template <typename T>
void demonstrateTypeTraits() {
    using CleanType = std::decay_t<T>;

    constexpr bool isInt = std::is_same_v<CleanType, int>;
    constexpr bool isDouble = std::is_same_v<CleanType, double>;
    constexpr bool isString = std::is_same_v<CleanType, std::string>;

    cout << "    [Type Traits Inspection for " << typeid(T).name() << "]\n"
         << "      - std::is_same_v<T, int>         : " << (isInt ? "TRUE" : "FALSE") << "\n"
         << "      - std::is_same_v<T, double>      : " << (isDouble ? "TRUE" : "FALSE") << "\n"
         << "      - std::is_same_v<T, std::string> : " << (isString ? "TRUE" : "FALSE") << "\n";
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for STL Templates analysis (e.g., 50): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 50." << endl;
        userInputValue = 50;
    }

    // =====================================================================================
    // 1. CONTAINER CLASS TEMPLATES
    // =====================================================================================
    cout << "\n================ 1. STL CONTAINER CLASS TEMPLATES ================\n";
    demonstrateContainerTemplates<int, 4>(userInputValue);

    // =====================================================================================
    // 2. UTILITY & SMART POINTER TEMPLATES
    // =====================================================================================
    cout << "\n================ 2. UTILITY & SMART POINTER TEMPLATES ================\n";
    demonstrateUtilityTemplates(userInputValue, string("STL_Template_Resource"));

    // =====================================================================================
    // 3. ALGORITHM FUNCTION TEMPLATES
    // =====================================================================================
    cout << "\n================ 3. ALGORITHM FUNCTION TEMPLATES ================\n";
    std::vector<int> sampleNumbers = {userInputValue, 15, 80,userInputValue / 2, 95};
    demonstrateAlgorithmTemplates(sampleNumbers);

    // =====================================================================================
    // 4. TYPE TRAITS & METAPROGRAMMING TEMPLATES
    // =====================================================================================
    cout << "\n================ 4. TYPE TRAIT METAPROGRAMMING TEMPLATES ================\n";
    demonstrateTypeTraits<int>();
    demonstrateTypeTraits<const string&>();

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ STL TEMPLATES ARCHITECTURE SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| STL Template Category | Example Standard Library Component| Key Architectural Purpose        |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Container Templates   | `std::vector<T>`, `std::array<T,N>`| Generic memory and element layouts|\n"
         << "| Utility Templates     | `std::pair<T1,T2>`, `std::tuple`  | Generic type compositions         |\n"
         << "| Smart Pointer Templates| `std::unique_ptr<T>`, `shared_ptr`| Type-safe RAII memory management  |\n"
         << "| Algorithm Templates   | `std::sort`, `std::transform`     | Decoupled sequence algorithms     |\n"
         << "| Type Trait Templates  | `std::is_same_v`, `std::decay_t`  | Compile-time type introspection   |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}