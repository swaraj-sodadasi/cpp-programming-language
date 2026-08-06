/*
 * =====================================================================================
 * CONCEPT        : Fundamentals of Array Operations in C++
 * DESCRIPTION    : Comprehensive guide and implementation of primary Array operations:
 *                  1. Traversal        : Sequential iteration over all active array elements.
 *                  2. Insertion        : Inserting an element at a specific index by shifting 
 *                                        subsequent elements right (Time: O(N)).
 *                  3. Deletion         : Removing an element at a specific index by shifting 
 *                                        subsequent elements left (Time: O(N)).
 *                  4. Updating         : Direct modification of element value at index (Time: O(1)).
 *                  5. Linear Search    : Unsorted array lookup comparing elements sequentially (Time: O(N)).
 *                  6. Binary Search    : Efficient lookup on sorted arrays using divide-and-conquer (Time: O(log N)).
 *                  7. Array Reversal   : Two-pointer in-place element swapping (Time: O(N), Space: O(1)).
 *                  8. Array Rotation   : Circular shifting of elements left/right by K positions (Time: O(N)).
 *
 * TIME COMPLEXITIES:
 * - Access / Update : O(1)
 * - Traversal       : O(N)
 * - Insertion       : Worst/Average O(N) | Best O(1) at end
 * - Deletion        : Worst/Average O(N) | Best O(1) at end
 * - Linear Search   : O(N)
 * - Binary Search   : O(log N) [Requires Sorted Array]
 * - Reversal        : O(N)
 * - Rotation        : O(N)
 *
 * SPACE COMPLEXITY : Fixed Capacity Buffer: O(N)
 * =====================================================================================
 */

#include <iostream>
#include <algorithm>
#include <iomanip>
#include <cstddef>
#include <utility>

using namespace std;

// =====================================================================================
// FIXED-CAPACITY ARRAY STRUCTURE FOR ELEMENT SHIFTING DEMONSTRATIONS
// Demonstrates how raw contiguous arrays handle insertion and deletion memory mechanics.
// =====================================================================================
template <typename T, size_t CAPACITY>
class ArrayOperationsEngine {
private:
    T data_[CAPACITY];
    size_t size_{0};

public:
    ArrayOperationsEngine() = default;

    // Helper constructor for array initialization
    ArrayOperationsEngine(const T* initialValues, size_t initialSize) {
        if (initialSize > CAPACITY) {
            initialSize = CAPACITY;
        }
        for (size_t i = 0; i < initialSize; ++i) {
            data_[i] = initialValues[i];
        }
        size_ = initialSize;
    }

    [[nodiscard]] size_t getSize() const noexcept { return size_; }
    [[nodiscard]] size_t getCapacity() const noexcept { return CAPACITY; }

    // 1. TRAVERSAL OPERATION
    void traverse(const string& label) const {
        cout << "  - [" << left << setw(18) << label << "] Size: " << size_ << "/" << CAPACITY << " | Elements: [ ";
        for (size_t i = 0; i < size_; ++i) {
            cout << data_[i] << (i + 1 < size_ ? ", " : " ");
        }
        cout << "]\n";
    }

    // 2. INSERTION OPERATION (Shifts elements right)
    bool insertAt(size_t index, T value) {
        if (size_ >= CAPACITY) {
            cout << "  - [ERROR] Cannot insert! Array capacity reached.\n";
            return false;
        }
        if (index > size_) {
            cout << "  - [ERROR] Insert index " << index << " out of bounds!\n";
            return false;
        }

        // Shift elements right starting from the tail down to index
        for (size_t i = size_; i > index; --i) {
            data_[i] = data_[i - 1];
        }

        data_[index] = value;
        ++size_;
        return true;
    }

    // 3. DELETION OPERATION (Shifts elements left)
    bool deleteAt(size_t index, T& deletedValue) {
        if (size_ == 0) {
            cout << "  - [ERROR] Cannot delete! Array is empty.\n";
            return false;
        }
        if (index >= size_) {
            cout << "  - [ERROR] Delete index " << index << " out of bounds!\n";
            return false;
        }

        deletedValue = data_[index];

        // Shift elements left starting from index to tail
        for (size_t i = index; i < size_ - 1; ++i) {
            data_[i] = data_[i + 1];
        }

        --size_;
        return true;
    }

    // 4. UPDATE OPERATION
    bool updateAt(size_t index, T newValue) {
        if (index >= size_) {
            cout << "  - [ERROR] Update index " << index << " out of bounds!\n";
            return false;
        }
        data_[index] = newValue;
        return true;
    }

    // 5. LINEAR SEARCH OPERATION (Unsorted Search)
    [[nodiscard]] int linearSearch(T target) const {
        for (size_t i = 0; i < size_; ++i) {
            if (data_[i] == target) {
                return static_cast<int>(i); // Found index
            }
        }
        return -1; // Not found
    }

    // 6. BINARY SEARCH OPERATION (Requires Sorted Array)
    [[nodiscard]] int binarySearch(T target) const {
        int low = 0;
        int high = static_cast<int>(size_) - 1;

        while (low <= high) {
            int mid = low + (high - low) / 2;
            if (data_[mid] == target) {
                return mid;
            }
            if (data_[mid] < target) {
                low = mid + 1;
            } else {
                high = mid - 1;
            }
        }
        return -1; // Not found
    }

    // Sort helper for Binary Search
    void sortArray() {
        std::sort(data_, data_ + size_);
    }

    // 7. IN-PLACE REVERSAL OPERATION (Two-Pointer Technique)
    void reverse() {
        if (size_ <= 1) return;
        size_t left = 0;
        size_t right = size_ - 1;

        while (left < right) {
            std::swap(data_[left], data_[right]);
            ++left;
            --right;
        }
    }

    // 8. LEFT ROTATION OPERATION (Triple-Reversal Algorithm)
    void rotateLeft(size_t k) {
        if (size_ <= 1 || k == 0) return;
        k = k % size_; // Handle rotations > size

        // Reversal algorithm: reverse(0, k-1), reverse(k, size-1), reverse(0, size-1)
        std::reverse(data_, data_ + k);
        std::reverse(data_ + k, data_ + size_);
        std::reverse(data_, data_ + size_);
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int insertValueInput = 0;
    int searchTargetInput = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter an integer value to insert into array (e.g., 25): " << flush;
    if (!(cin >> insertValueInput)) {
        cout << "Invalid input. Defaulting insert value to 25." << endl;
        insertValueInput = 25;
    }

    cout << "Enter an integer target value to search (e.g., 40): " << flush;
    if (!(cin >> searchTargetInput)) {
        cout << "Invalid input. Defaulting search target to 40." << endl;
        searchTargetInput = 40;
    }

    // Initialize array with 5 elements inside a capacity-10 buffer
    int initialData[] = {10, 20, 30, 40, 50};
    ArrayOperationsEngine<int, 10> arr(initialData, 5);

    // 1. TRAVERSAL DEMO
    cout << "\n================ 1. TRAVERSAL OPERATION ================\n";
    arr.traverse("Initial Array");

    // 2. INSERTION DEMO
    cout << "\n================ 2. INSERTION OPERATION (Index Shifting) ================\n";
    cout << "Inserting " << insertValueInput << " at Index [2]...\n";
    arr.insertAt(2, insertValueInput);
    arr.traverse("After Insertion");

    // 3. DELETION DEMO
    cout << "\n================ 3. DELETION OPERATION (Index Shifting) ================\n";
    int removedVal = 0;
    if (arr.deleteAt(4, removedVal)) {
        cout << "Deleted element '" << removedVal << "' from Index [4].\n";
    }
    arr.traverse("After Deletion");

    // 4. UPDATE DEMO
    cout << "\n================ 4. UPDATE OPERATION ================\n";
    cout << "Updating element at Index [0] to 99...\n";
    arr.updateAt(0, 99);
    arr.traverse("After Update");

    // 5. SEARCHING DEMO (Linear vs Binary Search)
    cout << "\n================ 5. SEARCH OPERATIONS ================\n";
    int linearIdx = arr.linearSearch(searchTargetInput);
    if (linearIdx != -1) {
        cout << "  - Linear Search: Target " << searchTargetInput << " FOUND at Index [" << linearIdx << "]\n";
    } else {
        cout << "  - Linear Search: Target " << searchTargetInput << " NOT FOUND.\n";
    }

    // Sorting array to prepare for Binary Search
    cout << "Sorting array elements for Binary Search...\n";
    arr.sortArray();
    arr.traverse("Sorted Array");

    int binaryIdx = arr.binarySearch(searchTargetInput);
    if (binaryIdx != -1) {
        cout << "  - Binary Search: Target " << searchTargetInput << " FOUND at Index [" << binaryIdx << "]\n";
    } else {
        cout << "  - Binary Search: Target " << searchTargetInput << " NOT FOUND.\n";
    }

    // 6. REVERSAL DEMO
    cout << "\n================ 6. REVERSAL OPERATION (Two Pointers) ================\n";
    arr.reverse();
    arr.traverse("Reversed Array");

    // 7. ROTATION DEMO
    cout << "\n================ 7. LEFT ROTATION OPERATION ================\n";
    cout << "Rotating array left by 2 positions...\n";
    arr.rotateLeft(2);
    arr.traverse("After 2 Left Shift");

    // =====================================================================================
    // SUMMARY TABLE
    // =====================================================================================
    cout << "\n================ ARRAY OPERATIONS COMPLEXITY SUMMARY ================\n";
    cout << "+-----------------------+-------------------+-------------------+-----------------------------------+\n"
         << "| Operation             | Time Complexity   | Space Complexity  | Key Mechanism / Requirement       |\n"
         << "+-----------------------+-------------------+-------------------+-----------------------------------+\n"
         << "| Random Access / Update| O(1)              | O(1)              | Direct memory offset calculation  |\n"
         << "| Traversal             | O(N)              | O(1)              | Sequential index iteration        |\n"
         << "| Insertion (Middle)    | O(N)              | O(1)              | Requires right element shifting   |\n"
         << "| Deletion (Middle)     | O(N)              | O(1)              | Requires left element shifting    |\n"
         << "| Linear Search         | O(N)              | O(1)              | Works on unsorted arrays          |\n"
         << "| Binary Search         | O(log N)          | O(1)              | REQUIRES SORTED ARRAY             |\n"
         << "| In-Place Reversal     | O(N)              | O(1)              | Two pointers swap from ends       |\n"
         << "| K-Left Rotation       | O(N)              | O(1)              | Triple-reversal technique         |\n"
         << "+-----------------------+-------------------+-------------------+-----------------------------------+\n";

    return 0;
}