/*
 * =====================================================================================
 * CONCEPT        : Common Loop Patterns in C++
 * DESCRIPTION    : Comprehensive implementation illustrating essential loop patterns:
 *                  1. Accumulation / Reduction : Aggregating elements (sum, product, running average).
 *                  2. Linear Search & Extremas : Scanning for targets, min/max values, and indices.
 *                  3. Counting & Filtering     : Tallying condition matches and creating sub-collections.
 *                  4. In-Place Transformation  : Modifying container contents during iteration.
 *                  5. Sentinel / Flag Loops    : Dynamic stream processing terminating on a sentinel value.
 *                  6. Two-Pointer & Pairwise   : Multi-index iteration (array reversal, adjacent differences).
 *
 * TIME COMPLEXITY  : Best Case: O(N) — Linear time single passes over N dynamic container elements.
 * SPACE COMPLEXITY : Best Case: O(N) — Memory footprint proportional to dynamic filtered collections.
 * =====================================================================================
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <limits>

using namespace std;

/**
 * @brief Pattern 1: Accumulation / Reduction Pattern
 * Aggregates sequence elements into scalar values (Sum, Product, Average).
 */
void demonstrateAccumulation(const vector<int>& data) {
    cout << "\n================ 1. ACCUMULATION / REDUCTION PATTERN ================" << endl;
    
    long long totalSum = 0;
    long long totalProduct = 1;

    for (const auto& val : data) {
        totalSum += val;
        totalProduct *= val;
    }

    double average = data.empty() ? 0.0 : static_cast<double>(totalSum) / static_cast<double>(data.size());

    cout << "Dataset                     : ";
    for (int v : data) { cout << v << " "; }
    cout << "\nTotal Accumulated Sum       : " << totalSum << endl;
    cout << "Total Product               : " << totalProduct << endl;
    cout << "Calculated Average          : " << fixed << setprecision(2) << average << endl;
}

/**
 * @brief Pattern 2: Linear Search & Extreme Values (Min/Max Tracking)
 * Locates specific element targets and tracks maximum/minimum bounds with index position.
 */
void demonstrateSearchAndExtremas(const vector<int>& data, int target) {
    cout << "\n================ 2. LINEAR SEARCH & EXTREMAS PATTERN ================" << endl;

    int minValue = numeric_limits<int>::max();
    int maxValue = numeric_limits<int>::min();
    size_t minIdx = 0;
    size_t maxIdx = 0;

    int targetIndex = -1;

    for (size_t i = 0; i < data.size(); ++i) {
        // Minimum / Maximum tracking pattern
        if (data[i] < minValue) {
            minValue = data[i];
            minIdx = i;
        }
        if (data[i] > maxValue) {
            maxValue = data[i];
            maxIdx = i;
        }

        // Linear search match pattern
        if (targetIndex == -1 && data[i] == target) {
            targetIndex = static_cast<int>(i);
        }
    }

    cout << "Minimum Element             : " << minValue << " (at index " << minIdx << ")" << endl;
    cout << "Maximum Element             : " << maxValue << " (at index " << maxIdx << ")" << endl;
    if (targetIndex != -1) {
        cout << "Target Value (" << target << ")              : FOUND at index " << targetIndex << endl;
    } else {
        cout << "Target Value (" << target << ")              : NOT FOUND in collection" << endl;
    }
}

/**
 * @brief Pattern 3: Counting & Filtering Pattern
 * Tallies elements satisfying a condition and extracts matching elements into a new vector.
 */
void demonstrateCountingAndFiltering(const vector<int>& data) {
    cout << "\n================ 3. COUNTING & FILTERING PATTERN ================" << endl;

    int evenCount = 0;
    vector<int> filteredEvens;

    for (const auto& val : data) {
        if (val % 2 == 0) {
            ++evenCount;                  // Tally / Count match
            filteredEvens.push_back(val); // Filter / Collect match
        }
    }

    cout << "Total Even Count            : " << evenCount << " element(s)" << endl;
    cout << "Filtered Evens Collection   : ";
    for (int e : filteredEvens) { cout << e << " "; }
    cout << endl;
}

/**
 * @brief Pattern 4: In-Place Transformation Pattern
 * Mutates elements directly within their existing memory layout using non-const reference binding.
 */
void demonstrateInPlaceTransformation(vector<int> data, int multiplier) {
    cout << "\n================ 4. IN-PLACE TRANSFORMATION PATTERN ================" << endl;

    cout << "Original Vector             : ";
    for (int v : data) { cout << v << " "; }
    cout << endl;

    // In-place element mutation via auto& reference binding
    for (auto& val : data) {
        val *= multiplier;
    }

    cout << "Transformed (* " << multiplier << ")            : ";
    for (int v : data) { cout << v << " "; }
    cout << endl;
}

/**
 * @brief Pattern 5: Two-Pointer & Pairwise Processing
 * Leverages multi-index tracking for operations like element swapping and adjacent differences.
 */
void demonstrateTwoPointerAndPairwise(vector<int> data) {
    cout << "\n================ 5. TWO-POINTER & PAIRWISE PATTERNS ================" << endl;

    // A. Two-Pointer Array Reversal
    vector<int> reversedCopy = data;
    size_t left = 0;
    size_t right = reversedCopy.empty() ? 0 : reversedCopy.size() - 1;

    while (left < right) {
        swap(reversedCopy[left], reversedCopy[right]);
        ++left;
        --right;
    }

    cout << "Two-Pointer Reversed Vector : ";
    for (int v : reversedCopy) { cout << v << " "; }
    cout << endl;

    // B. Pairwise Adjacent Difference Inspection
    cout << "Pairwise Adjacent Diffs     : ";
    for (size_t i = 0; i + 1 < data.size(); ++i) {
        int diff = data[i + 1] - data[i];
        cout << "[" << data[i] << "->" << data[i + 1] << " : " << diff << "] ";
    }
    cout << endl;
}

int main() {
    size_t elementCount = 0;
    int searchTarget = 0;
    int scaleFactor = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter number of elements to generate (3 - 10): " << flush;
    if (!(cin >> elementCount) || elementCount < 3 || elementCount > 10) {
        cout << "Invalid count provided. Defaulting to 5 elements." << endl;
        elementCount = 5;
    }

    cout << "Enter a target search integer: " << flush;
    if (!(cin >> searchTarget)) {
        cout << "Invalid search target. Setting search target to 10." << endl;
        searchTarget = 10;
    }

    cout << "Enter dynamic scale multiplier factor: " << flush;
    if (!(cin >> scaleFactor)) {
        cout << "Invalid factor. Setting default factor to 2." << endl;
        scaleFactor = 2;
    }

    // Populate baseline dataset dynamically
    vector<int> numbers;
    numbers.reserve(elementCount);
    for (size_t i = 1; i <= elementCount; ++i) {
        numbers.push_back(static_cast<int>(i * 3 + 1));
    }

    // Execute Common Loop Patterns
    demonstrateAccumulation(numbers);
    demonstrateSearchAndExtremas(numbers, searchTarget);
    demonstrateCountingAndFiltering(numbers);
    demonstrateInPlaceTransformation(numbers, scaleFactor);
    demonstrateTwoPointerAndPairwise(numbers);

    // Pattern 6: Sentinel / Flag Control Loop
    cout << "\n================ 6. SENTINEL / FLAG CONTROL LOOP ================" << endl;
    cout << "Simulating sentinel loop processing (Enter numbers, enter -1 to STOP):" << endl;
    
    int sentinelValue = 0;
    int entryCounter = 0;
    while (true) {
        cout << "  Input item #" << (entryCounter + 1) << " (or -1 to terminate sentinel loop): " << flush;
        if (!(cin >> sentinelValue) || sentinelValue == -1) {
            cout << "  [Sentinel Triggered] Termination signal received. Exiting loop." << endl;
            break;
        }
        ++entryCounter;
        cout << "    -> Processed entry: " << sentinelValue << endl;
    }

    return 0;
}