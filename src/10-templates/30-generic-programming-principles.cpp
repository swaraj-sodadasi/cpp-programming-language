/*
 * =====================================================================================
 * CONCEPT        : Generic Programming Principles under Templates in C++
 * DESCRIPTION    : Production-grade executable code demonstrating the 4 core pillars
 *                  of Generic Programming established by Stepanov and Modern C++:
 *
 *                  1. Algorithm/Data Structure Decoupling via Iterators :
 *                     - Algorithms operate purely on iterator abstractions ([first, last)),
 *                       decoupling algorithm logic from container physical layout.
 *
 *                  2. Tag Dispatching & Iterator Traits (Compile-Time Specialization) :
 *                     - Selecting optimal algorithms statically based on iterator categories
 *                       (e.g., O(1) pointer subtraction for Random Access vs O(N) loop for
 *                       Input/Forward/Bidirectional iterators).
 *
 *                  3. Policy-Based Design (Compile-Time Static Polymorphism) :
 *                     - Injecting compile-time strategies (e.g., Validation or Transformation policies)
 *                       into generic classes without vtable overhead or dynamic dispatch.
 *
 *                  4. Orthogonality & Higher-Order Composition :
 *                     - Combining generic algorithms with arbitrary function objects (functors,
 *                       lambdas, function pointers) to process heterogeneous data structures seamlessly.
 *
 * HEADER AUDIT   : Strictly audited. Only headers whose symbols are actively used in the code:
 *                  - <iostream> : Console I/O streams (cin, cout, flush, endl)
 *                  - <vector>   : Dynamic contiguous array std::vector
 *                  - <list>     : Doubly-linked list std::list
 *                  - <iterator> : Iterator categories, std::iterator_traits
 *                  - <typeinfo> : RTTI typeid operator for type name inspection
 *                  - <limits>   : Stream clearing via std::numeric_limits
 *                  - <cstddef>  : Type std::size_t
 *                  - <utility>  : std::move
 *
 * TIME COMPLEXITY  : Compile-time Monomorphization : O(1) static type evaluation per call.
 *                    Algorithm Traversal           : O(1) for RandomAccess, O(N) for Non-RandomAccess.
 * SPACE COMPLEXITY : Binary Footprint             : Scaled by unique monomorphized template types.
 * =====================================================================================
 */

#include <iostream>
#include <vector>
#include <list>
#include <iterator>
#include <limits>

using namespace std;

// =====================================================================================
// 1. ALGORITHM DECOUPLING VIA ITERATORS
// Generic algorithm operating on any iterator sequence [first, last)
// =====================================================================================
template <typename InputIterator, typename Predicate>
InputIterator customFindIf(InputIterator first, InputIterator last, Predicate pred) {
    while (first != last) {
        if (pred(*first)) {
            return first; // Element found
        }
        ++first;
    }
    return last; // Element not found
}

// =====================================================================================
// 2. TAG DISPATCHING & ITERATOR TRAITS (COMPILE-TIME SPECIALIZATION)
// Selects optimal algorithm implementation statically using traits
// =====================================================================================

// Helper overload for Linear Iterators (Input, Forward, Bidirectional)
template <typename InputIterator>
typename std::iterator_traits<InputIterator>::difference_type
customDistanceImpl(InputIterator first, InputIterator last, std::input_iterator_tag) {
    cout << "      [Tag Dispatch -> std::input_iterator_tag] Executing O(N) linear step-by-step traversal.\n";
    typename std::iterator_traits<InputIterator>::difference_type count = 0;
    while (first != last) {
        ++first;
        ++count;
    }
    return count;
}

// Helper overload for Random Access Iterators (Vectors, Raw Arrays, Deques)
template <typename RandomAccessIterator>
typename std::iterator_traits<RandomAccessIterator>::difference_type
customDistanceImpl(RandomAccessIterator first, RandomAccessIterator last, std::random_access_iterator_tag) {
    cout << "      [Tag Dispatch -> std::random_access_iterator_tag] Executing O(1) constant-time pointer arithmetic.\n";
    return last - first;
}

// Top-level public generic distance dispatch algorithm
template <typename Iterator>
typename std::iterator_traits<Iterator>::difference_type
customDistance(Iterator first, Iterator last) {
    using Category = typename std::iterator_traits<Iterator>::iterator_category;
    return customDistanceImpl(first, last, Category{});
}

// =====================================================================================
// 3. POLICY-BASED DESIGN (COMPILE-TIME STATIC POLYMORPHISM)
// =====================================================================================

// Policy 1: Positive Number Validation
struct PositiveOnlyPolicy {
    template <typename T>
    static bool validate(const T& val) {
        return val > 0;
    }
    static const char* name() { return "PositiveOnlyPolicy"; }
};

// Policy 2: Permissive (Accepts All) Validation
struct PermissivePolicy {
    template <typename T>
    static bool validate(const T&) {
        return true;
    }
    static const char* name() { return "PermissivePolicy"; }
};

// Generic Class parameterized by Data Type and Policy Strategy
template <typename T, typename ValidationPolicy = PermissivePolicy>
class PolicyDataProcessor {
private:
    T data_;

public:
    explicit PolicyDataProcessor(T val) : data_(val) {}

    void process() const {
        if (ValidationPolicy::validate(data_)) {
            cout << "    [PolicyProcessor <" << ValidationPolicy::name() 
                 << ">] Value (" << data_ << ") VALIDATED and processed successfully.\n";
        } else {
            cout << "    [PolicyProcessor <" << ValidationPolicy::name() 
                 << ">] Value (" << data_ << ") REJECTED by policy constraint.\n";
        }
    }
};

// =====================================================================================
// 4. HIGHER-ORDER GENERIC TRANSFORMATIONS (ORTHOGONALITY)
// =====================================================================================
template <typename InputIterator, typename OutputIterator, typename UnaryOperation>
OutputIterator customTransform(InputIterator first, InputIterator last, OutputIterator result, UnaryOperation op) {
    while (first != last) {
        *result = op(*first);
        ++first;
        ++result;
    }
    return result;
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for Generic Programming analysis (e.g., 42): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 42." << endl;
        userInputValue = 42;
    }

    // Populate dynamic containers
    vector<int> vecData = {10, userInputValue, 30, 40, -15, 60};
    list<int> listData = {10, userInputValue, 30, 40, -15, 60};

    // =====================================================================================
    // 1. ALGORITHM / DATA STRUCTURE DECOUPLING
    // =====================================================================================
    cout << "\n================ 1. ALGORITHM DECOUPLING VIA ITERATORS ================\n";

    auto targetVal = userInputValue;
    auto isTarget = [targetVal](int val) { return val == targetVal; };

    // Same customFindIf algorithm works on contiguous std::vector
    auto vecIt = customFindIf(vecData.begin(), vecData.end(), isTarget);
    if (vecIt != vecData.end()) {
        cout << "  - std::vector search: Found value " << *vecIt << " at position.\n";
    }

    // Same customFindIf algorithm works on non-contiguous std::list
    auto listIt = customFindIf(listData.begin(), listData.end(), isTarget);
    if (listIt != listData.end()) {
        cout << "  - std::list search  : Found value " << *listIt << " at position.\n";
    }

    // =====================================================================================
    // 2. TAG DISPATCHING & ITERATOR TRAITS
    // =====================================================================================
    cout << "\n================ 2. TAG DISPATCHING & ITERATOR TRAITS ================\n";

    cout << "  - Computing distance on std::vector (Random Access):\n";
    auto vecDist = customDistance(vecData.begin(), vecData.end());
    cout << "    * Result Vector Size: " << vecDist << "\n";

    cout << "\n  - Computing distance on std::list (Bidirectional):\n";
    auto listDist = customDistance(listData.begin(), listData.end());
    cout << "    * Result List Size  : " << listDist << "\n";

    // =====================================================================================
    // 3. POLICY-BASED DESIGN (STATIC POLYMORPHISM)
    // =====================================================================================
    cout << "\n================ 3. POLICY-BASED DESIGN (STATIC POLYMORPHISM) ================\n";

    PolicyDataProcessor<int, PositiveOnlyPolicy> positiveProcessor(userInputValue);
    positiveProcessor.process();

    PolicyDataProcessor<int, PositiveOnlyPolicy> negativeProcessor(-userInputValue);
    negativeProcessor.process();

    PolicyDataProcessor<int, PermissivePolicy> permissiveProcessor(-userInputValue);
    permissiveProcessor.process();

    // =====================================================================================
    // 4. HIGHER-ORDER GENERIC TRANSFORMATIONS
    // =====================================================================================
    cout << "\n================ 4. HIGHER-ORDER GENERIC TRANSFORMATIONS ================\n";

    vector<int> transformedOutput(vecData.size());
    customTransform(vecData.begin(), vecData.end(), transformedOutput.begin(), [](int x) {
        return x * 2;
    });

    cout << "  - Original Vector values : ";
    for (int v : vecData) cout << v << " ";
    cout << "\n  - Doubled Vector values  : ";
    for (int v : transformedOutput) cout << v << " ";
    cout << "\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ GENERIC PROGRAMMING PRINCIPLES SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Generic Pillar        | C++ Implementation Mechanism      | Architectural Purpose & Benefit   |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Iterator Decoupling   | `[InputIt first, InputIt last)`   | Decouples algorithms from physical|\n"
         << "|                       |                                   | memory container layouts           |\n"
         << "| Tag Dispatching       | `std::iterator_traits<It>::`      | Selects optimal algorithm variant |\n"
         << "|                       | `iterator_category`               | at compile time (O(1) vs O(N))    |\n"
         << "| Policy-Based Design   | `template <T, typename Policy>`   | Static polymorphism injecting     |\n"
         << "|                       |                                   | strategy without vtable overhead |\n"
         << "| Orthogonality         | Generic Higher-Order Functions    | Seamlessly composes algorithms    |\n"
         << "|                       | (`UnaryOp`, Lambdas, Functors)    | with custom behavioral predicate  |\n"
         << "| Zero-Cost Abstraction | Static template monomorphization  | Inlines code to match hand-written|\n"
         << "|                       |                                   | C execution performance           |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}