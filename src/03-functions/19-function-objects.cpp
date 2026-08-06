/*
 * =====================================================================================
 * CONCEPT        : Function Objects (Functors) in C++
 * DESCRIPTION    : Comprehensive implementation detailing C++ function objects:
 *                  1. Basic Anatomy          : Classes/structs overloading operator() to act like functions.
 *                  2. Stateful Functors      : Maintaining persistent internal state across calls
 *                                              (unlike plain functions without static vars).
 *                  3. Standard Functors      : Using built-in functors from <functional> 
 *                                              (std::greater, std::plus, std::multiplies).
 *                  4. STL Algorithm Usage    : Passing functors as custom predicates and transformers
 *                                              to algorithms (std::sort, std::transform, std::for_each).
 *                  5. Comparison             : Functors vs Lambdas vs Function Pointers vs std::function.
 *
 * TIME COMPLEXITY  : Best Case: O(1) scalar call; O(N log N) or O(N) when used in STL algorithms.
 * SPACE COMPLEXITY : Best Case: O(1) stack frame allocation for class instances.
 * =====================================================================================
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <iomanip>

using namespace std;

// =====================================================================================
// 1. BASIC & STATEFUL FUNCTION OBJECTS (FUNCTORS)
// =====================================================================================

/**
 * @brief Basic Functor storing a multiplier factor.
 */
class MultiplyBy {
private:
    double factor_;

public:
    explicit MultiplyBy(double factor) : factor_(factor) {}

    // Overloading function call operator ()
    double operator()(double value) const {
        return value * factor_;
    }
};

/**
 * @brief Stateful Functor maintaining running statistics across multiple invocations.
 */
class RunningStatsAccumulator {
private:
    int count_;
    double sum_;
    double minVal_;
    double maxVal_;

public:
    RunningStatsAccumulator() 
        : count_(0), sum_(0.0), minVal_(1e9), maxVal_(-1e9) {}

    // Overloaded operator() mutates internal member state
    void operator()(double value) {
        ++count_;
        sum_ += value;
        if (value < minVal_) minVal_ = value;
        if (value > maxVal_) maxVal_ = value;
    }

    // Getters for inspecting persistent state
    [[nodiscard]] int getCount() const { return count_; }
    [[nodiscard]] double getSum() const { return sum_; }
    [[nodiscard]] double getAverage() const { return (count_ > 0) ? (sum_ / count_) : 0.0; }
    [[nodiscard]] double getMin() const { return (count_ > 0) ? minVal_ : 0.0; }
    [[nodiscard]] double getMax() const { return (count_ > 0) ? maxVal_ : 0.0; }
};

/**
 * @brief Predicate Functor used for filtering elements based on a threshold.
 */
struct GreaterThanThreshold {
    double threshold;

    explicit GreaterThanThreshold(double thresh) : threshold(thresh) {}

    bool operator()(double val) const {
        return val > threshold;
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    double userMultiplier = 0.0;
    double userThreshold = 0.0;

    // Dynamic input collection with stream flushing
    cout << "Enter a scalar multiplier factor (e.g., 2.5): " << flush;
    if (!(cin >> userMultiplier)) {
        cout << "Invalid multiplier. Defaulting to 2.5." << endl;
        userMultiplier = 2.5;
    }

    cout << "Enter a filtering threshold value (e.g., 15.0): " << flush;
    if (!(cin >> userThreshold)) {
        cout << "Invalid threshold. Defaulting to 15.0." << endl;
        userThreshold = 15.0;
    }

    vector<double> dataset = {10.0, 5.5, 20.0, 15.0, 30.0, 2.5, 18.0};

    // 1. BASIC FUNCTOR INVOCATION
    cout << "\n================ 1. BASIC FUNCTOR INVOCATION ================" << endl;
    MultiplyBy multiplier(userMultiplier); // Construct functor instance with state
    
    double sampleVal = 10.0;
    // Invoking functor like a function call
    double scaledResult = multiplier(sampleVal); 
    cout << "  - multiplier(" << sampleVal << ") with factor " << userMultiplier 
         << " = " << fixed << setprecision(2) << scaledResult << endl;

    // 2. STATEFUL FUNCTOR WITH std::for_each
    cout << "\n================ 2. STATEFUL FUNCTOR IN STL ALGORITHMS ================" << endl;
    cout << "  - Processing dataset through RunningStatsAccumulator via std::for_each..." << endl;
    
    // std::for_each returns a copy of the accumulator containing updated state
    RunningStatsAccumulator stats = std::for_each(dataset.begin(), dataset.end(), RunningStatsAccumulator());

    cout << "  - Statistics Accumulated:" << endl;
    cout << "    |- Elements Processed : " << stats.getCount() << endl;
    cout << "    |- Total Sum          : " << stats.getSum() << endl;
    cout << "    |- Average Value      : " << stats.getAverage() << endl;
    cout << "    |- Minimum Value      : " << stats.getMin() << endl;
    cout << "    |- Maximum Value      : " << stats.getMax() << endl;

    // 3. TRANSFORMING DATA WITH FUNCTORS (std::transform)
    cout << "\n================ 3. TRANSFORMING DATA WITH FUNCTORS ================" << endl;
    vector<double> transformedData(dataset.size());

    // Apply MultiplyBy functor across vector elements
    std::transform(dataset.begin(), dataset.end(), transformedData.begin(), MultiplyBy(userMultiplier));

    cout << "  - Original Dataset : ";
    for (double v : dataset) cout << v << " ";
    cout << "\n  - Scaled Dataset   : ";
    for (double v : transformedData) cout << v << " ";
    cout << endl;

    // 4. BUILT-IN STANDARD LIBRARY FUNCTORS (<functional>)
    cout << "\n================ 4. BUILT-IN STANDARD FUNCTORS (<functional>) ================" << endl;
    
    // Using std::greater<double>() functor for descending order sorting
    vector<double> sortedData = dataset;
    std::sort(sortedData.begin(), sortedData.end(), std::greater<double>());

    cout << "  - Sorted Descending (std::greater)  : ";
    for (double v : sortedData) cout << v << " ";
    cout << endl;

    // Using std::multiplies<double>() binary functor with std::transform
    vector<double> squaredData(dataset.size());
    std::transform(dataset.begin(), dataset.end(), dataset.begin(), squaredData.begin(), std::multiplies<double>());

    cout << "  - Element-wise Square (std::multiplies): ";
    for (double v : squaredData) cout << v << " ";
    cout << endl;

    // 5. CUSTOM PREDICATE FUNCTOR WITH std::count_if
    cout << "\n================ 5. CUSTOM PREDICATE FUNCTOR ================" << endl;
    GreaterThanThreshold filterPredicate(userThreshold);

    ptrdiff_t matchCount = std::count_if(dataset.begin(), dataset.end(), filterPredicate);
    cout << "  - Count of elements strictly > " << userThreshold << " : " << matchCount << endl;

    // 6. WRAPPING FUNCTORS WITH std::function
    cout << "\n================ 6. UNIFIED STORAGE VIA std::function ================" << endl;
    std::function<double(double)> genericCallable = MultiplyBy(3.0);
    cout << "  - Wrapped MultiplyBy(3.0) inside std::function: genericCallable(4.0) = " 
         << genericCallable(4.0) << endl;

    cout << "\n================ FUNCTION OBJECTS SUMMARY ================" << endl;
    cout << "1. Core Concept   : Any class/struct that overloads `operator()` is a Function Object (Functor)." << endl;
    cout << "2. Statefulness   : Functors can hold member variables, retaining state across calls without globals." << endl;
    cout << "3. Inlining Advantage: Functor calls can be aggressively inlined by compilers, outperforming function pointers." << endl;
    cout << "4. Standard Lib   : `<functional>` provides built-in functors like `std::greater`, `std::plus`, `std::multiplies`." << endl;
    cout << "5. Relation to Lambdas: C++ Lambdas are syntactic sugar that compiler compiles into anonymous Functor classes." << endl;

    return 0;
}