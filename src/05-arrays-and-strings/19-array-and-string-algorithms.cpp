/*
 * =====================================================================================
 * CONCEPT        : Core Array and String Algorithms in Modern C++
 * DESCRIPTION    : Comprehensive implementation of essential algorithmic paradigms for 
 *                  arrays and strings:
 *                  1. Kadane's Algorithm           : Maximum contiguous subarray sum in O(N) time.
 *                  2. Two-Pointers Technique       : Pair searching on sorted array in O(N) time & O(1) space.
 *                  3. Sliding Window Technique     : Longest substring without repeating characters in O(N) time.
 *                  4. Dutch National Flag Algorithm: 3-way partitioning / 0-1-2 sorting in O(N) time & O(1) space.
 *                  5. Prefix Sum Array Algorithm   : Range sum queries in O(1) time after O(N) precomputation.
 *                  6. String In-Place Word Reversal: Reversing individual words in a sentence in O(N) time.
 *
 * TIME COMPLEXITY  : Most operations run in linear time O(N).
 * SPACE COMPLEXITY : O(1) auxiliary space for in-place paradigms, O(N) for prefix sum buffers.
 * =====================================================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <limits>
#include <cstddef>
#include <utility>

using namespace std;

// =====================================================================================
// 1. KADANE'S ALGORITHM (MAXIMUM SUBARRAY SUM)
// Paradigms: Dynamic Programming / Greedy. Finds maximum sum of a contiguous subarray.
// =====================================================================================
int maxSubArraySum(const vector<int>& nums, int& startIdx, int& endIdx) {
    int maxSoFar = nums[0];
    int currentMax = nums[0];
    
    int tempStart = 0;
    startIdx = 0;
    endIdx = 0;

    for (size_t i = 1; i < nums.size(); ++i) {
        if (nums[i] > currentMax + nums[i]) {
            currentMax = nums[i];
            tempStart = static_cast<int>(i);
        } else {
            currentMax += nums[i];
        }

        if (currentMax > maxSoFar) {
            maxSoFar = currentMax;
            startIdx = tempStart;
            endIdx = static_cast<int>(i);
        }
    }
    return maxSoFar;
}

// =====================================================================================
// 2. TWO-POINTERS TECHNIQUE (FIND PAIR WITH TARGET SUM)
// Requires sorted input array. Moves two converging pointers from head and tail.
// =====================================================================================
pair<int, int> twoSumSorted(const vector<int>& sortedNums, int target) {
    int left = 0;
    int right = static_cast<int>(sortedNums.size()) - 1;

    while (left < right) {
        int currentSum = sortedNums[left] + sortedNums[right];
        if (currentSum == target) {
            return {left, right}; // Found pair indices
        }
        if (currentSum < target) {
            ++left;
        } else {
            --right;
        }
    }
    return {-1, -1}; // Not found
}

// =====================================================================================
// 3. SLIDING WINDOW (LONGEST SUBSTRING WITHOUT REPEATING CHARACTERS)
// Dynamic window shrinking and expanding using character index tracking.
// =====================================================================================
int lengthOfLongestSubstring(const string& s) {
    unordered_map<char, int> lastSeenIndex;
    int maxLength = 0;
    int windowStart = 0;

    for (int windowEnd = 0; windowEnd < static_cast<int>(s.length()); ++windowEnd) {
        char currentChar = s[windowEnd];

        // If character was seen inside current window, shrink window
        if (lastSeenIndex.find(currentChar) != lastSeenIndex.end() && lastSeenIndex[currentChar] >= windowStart) {
            windowStart = lastSeenIndex[currentChar] + 1;
        }

        lastSeenIndex[currentChar] = windowEnd;
        maxLength = max(maxLength, windowEnd - windowStart + 1);
    }
    return maxLength;
}

// =====================================================================================
// 4. DUTCH NATIONAL FLAG ALGORITHM (3-WAY PARTITIONING)
// Sorts an array containing only 0s, 1s, and 2s in a single pass in-place.
// =====================================================================================
void dutchNationalFlagSort(vector<int>& nums) {
    int low = 0;
    int mid = 0;
    int high = static_cast<int>(nums.size()) - 1;

    while (mid <= high) {
        if (nums[mid] == 0) {
            swap(nums[low], nums[mid]);
            ++low;
            ++mid;
        } else if (nums[mid] == 1) {
            ++mid;
        } else { // nums[mid] == 2
            swap(nums[mid], nums[high]);
            --high;
        }
    }
}

// =====================================================================================
// 5. PREFIX SUM ARRAY ALGORITHM
// Precomputes prefix cumulative sums allowing O(1) range query evaluations.
// =====================================================================================
class PrefixSumArray {
private:
    vector<int> prefixSum_;

public:
    explicit PrefixSumArray(const vector<int>& arr) {
        prefixSum_.resize(arr.size() + 1, 0);
        for (size_t i = 0; i < arr.size(); ++i) {
            prefixSum_[i + 1] = prefixSum_[i] + arr[i];
        }
    }

    // Range Query Sum between index L and R (inclusive)
    [[nodiscard]] int queryRange(size_t L, size_t R) const {
        if (L > R || R >= prefixSum_.size() - 1) return 0;
        return prefixSum_[R + 1] - prefixSum_[L];
    }
};

// =====================================================================================
// 6. IN-PLACE STRING WORD REVERSAL
// Reverses individual words in a string while preserving word position boundaries.
// =====================================================================================
void reverseWordsInSentence(string& str) {
    size_t n = str.length();
    size_t start = 0;

    for (size_t end = 0; end <= n; ++end) {
        if (end == n || str[end] == ' ') {
            // Reverse character range [start, end - 1] in-place
            reverse(str.begin() + static_cast<ptrdiff_t>(start), str.begin() + static_cast<ptrdiff_t>(end));
            start = end + 1;
        }
    }
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int targetSumInput = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter target sum integer for Two-Pointer search (e.g., 35): " << flush;
    if (!(cin >> targetSumInput) || targetSumInput == 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid or zero input. Defaulting target sum to 35." << endl;
        targetSumInput = 35;
    }

    // =====================================================================================
    // 1. KADANE'S ALGORITHM DEMO
    // =====================================================================================
    cout << "\n================ 1. KADANE'S ALGORITHM (MAX SUBARRAY) ================\n";
    vector<int> kadaneData = {-2, 1, -3, 4, -1, 2, 1, -5, 4};
    int startIdx = 0, endIdx = 0;
    int maxSubSum = maxSubArraySum(kadaneData, startIdx, endIdx);

    cout << "  - Input Array       : [ ";
    for (int v : kadaneData) cout << v << " ";
    cout << "]\n";
    cout << "  - Max Subarray Sum  : " << maxSubSum << "\n";
    cout << "  - Subarray Span     : Indices [" << startIdx << " to " << endIdx << "] -> [ ";
    for (int i = startIdx; i <= endIdx; ++i) cout << kadaneData[i] << " ";
    cout << "]\n";

    // =====================================================================================
    // 2. TWO-POINTERS TECHNIQUE DEMO
    // =====================================================================================
    cout << "\n================ 2. TWO-POINTERS TECHNIQUE ================\n";
    vector<int> sortedData = {5, 10, 15, 20, 25, 30, 40, 50};
    auto pairIndices = twoSumSorted(sortedData, targetSumInput);

    cout << "  - Sorted Array      : [ ";
    for (int v : sortedData) cout << v << " ";
    cout << "]\n";
    cout << "  - Target Sum Value  : " << targetSumInput << "\n";
    if (pairIndices.first != -1) {
        cout << "  - Pair Found        : " << sortedData[pairIndices.first] 
             << " + " << sortedData[pairIndices.second] << " = " << targetSumInput 
             << " (Indices: [" << pairIndices.first << ", " << pairIndices.second << "])\n";
    } else {
        cout << "  - Pair Result       : No two elements sum to " << targetSumInput << "\n";
    }

    // =====================================================================================
    // 3. SLIDING WINDOW DEMO
    // =====================================================================================
    cout << "\n================ 3. SLIDING WINDOW TECHNIQUE ================\n";
    string sampleText = "abcabcbb";
    int longestSubstrLen = lengthOfLongestSubstring(sampleText);

    cout << "  - Input String      : \"" << sampleText << "\"\n";
    cout << "  - Longest Unique    : " << longestSubstrLen << " characters (e.g., \"abc\")\n";

    // =====================================================================================
    // 4. DUTCH NATIONAL FLAG ALGORITHM DEMO
    // =====================================================================================
    cout << "\n================ 4. DUTCH NATIONAL FLAG (3-WAY PARTITION) ================\n";
    vector<int> flagData = {2, 0, 2, 1, 1, 0, 2, 1, 0};
    cout << "  - Before Sorting    : [ ";
    for (int v : flagData) cout << v << " ";
    cout << "]\n";

    dutchNationalFlagSort(flagData);

    cout << "  - After O(N) Sort   : [ ";
    for (int v : flagData) cout << v << " ";
    cout << "]\n";

    // =====================================================================================
    // 5. PREFIX SUM ARRAY DEMO
    // =====================================================================================
    cout << "\n================ 5. PREFIX SUM ARRAY (RANGE SUM) ================\n";
    vector<int> rangeData = {3, 2, 4, 5, 1, 1, 5, 3};
    PrefixSumArray prefEngine(rangeData);

    size_t qL = 2, qR = 5;
    int rangeSum = prefEngine.queryRange(qL, qR);

    cout << "  - Input Array       : [ ";
    for (int v : rangeData) cout << v << " ";
    cout << "]\n";
    cout << "  - Range Query [" << qL << ".." << qR << "] : Sum = " << rangeSum 
         << " (O(1) execution time)\n";

    // =====================================================================================
    // 6. IN-PLACE WORD REVERSAL DEMO
    // =====================================================================================
    cout << "\n================ 6. IN-PLACE STRING WORD REVERSAL ================\n";
    string sentence = "Fast High Performance Engine";
    cout << "  - Original Sentence : \"" << sentence << "\"\n";

    reverseWordsInSentence(sentence);

    cout << "  - Reversed Words    : \"" << sentence << "\"\n";

    // =====================================================================================
    // SUMMARY TABLE
    // =====================================================================================
    cout << "\n================ ARRAY & STRING ALGORITHMS SUMMARY ================\n";
    cout << "+-----------------------+-------------------+-------------------+-----------------------------------+\n"
         << "| Algorithm / Pattern   | Time Complexity   | Space Complexity  | Primary Application               |\n"
         << "+-----------------------+-------------------+-------------------+-----------------------------------+\n"
         << "| Kadane's Algorithm    | O(N) Linear       | O(1) Auxiliary    | Max contiguous sum in 1D array    |\n"
         << "| Two-Pointers Search   | O(N) Linear       | O(1) Auxiliary    | Pair matching on sorted arrays    |\n"
         << "| Sliding Window        | O(N) Linear       | O(K) Aux Hashmap  | Substring / Subarray constraint   |\n"
         << "| Dutch National Flag   | O(N) Single Pass  | O(1) Auxiliary    | 3-way partitioning / 0-1-2 sort   |\n"
         << "| Prefix Sum Array      | O(1) Query        | O(N) Precomputed  | Frequent range sum queries        |\n"
         << "| In-Place Word Reverse | O(N) Linear       | O(1) Auxiliary    | Word-level string transformation  |\n"
         << "+-----------------------+-------------------+-------------------+-----------------------------------+\n";

    return 0;
}