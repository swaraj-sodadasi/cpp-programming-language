/*
 * =====================================================================================
 * CONCEPT        : Static Data Members and Member Functions in OOP (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the lifetime, scope rules, and architectural applications of `static`:
 *
 *                  1. Static Data Members         : Class-scoped shared storage that exists
 *                                                   independently of individual instance lifetimes.
 *                  2. Modern Inline Static        : C++17 `inline static` members permitting
 *                                                   in-class definition without separate .cpp definitions.
 *                  3. Static Member Functions     : Class-level functions executing without a `this`
 *                                                   pointer; unable to directly access non-static state.
 *                  4. Compile-Time Static Consts  : `constexpr static` constants evaluated at compile-time.
 *                  5. Static Factory Pattern      : Controlled object instantiation using static
 *                                                   factory functions with private constructors.
 *                  6. Instance State Inspection   : Passing explicit instance references to static
 *                                                   functions to inspect or manipulate state.
 *
 * TIME COMPLEXITY  : Static Member Access / Function Calls : O(1) constant time.
 * SPACE COMPLEXITY : Overhead per Object Instance          : 0 bytes (static members stored in data/BSS segment).
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <stdexcept>
#include <iomanip>
#include <limits>

using namespace std;

// =====================================================================================
// CLASS DEFINITION: ClusterNode
// Demonstrates static data members, inline initializers, compile-time constants,
// static utility functions, and static factory methods.
// =====================================================================================
class ClusterNode {
private:
    // Non-static Data Members (Per-Instance State)
    int nodeId_;
    string nodeName_;
    double allocatedMemoryGb_;
    bool isActive_;

    // 1. Static Data Members (Shared Across ALL Instances)
    // C++17 `inline static` allows header/in-class initialization without out-of-class definition
    inline static int totalActiveNodes_ = 0;
    inline static double totalClusterMemoryGb_ = 0.0;
    inline static int cumulativeNodesCreated_ = 0;

    // Private Constructor enforcing Controlled Creation via Static Factory Method
    ClusterNode(int id, string name, double memoryGb)
        : nodeId_(id), nodeName_(std::move(name)), allocatedMemoryGb_(memoryGb), isActive_(true) {
        
        // Update class-wide static tracking variables
        ++totalActiveNodes_;
        ++cumulativeNodesCreated_;
        totalClusterMemoryGb_ += allocatedMemoryGb_;

        cout << "    [ClusterNode Ctor] Created Node #" << nodeId_ 
             << " ('" << nodeName_ << "') | Allocated: " << allocatedMemoryGb_ << " GB\n";
    }

public:
    // 2. Compile-Time Static Constants
    constexpr static double MAX_SINGLE_NODE_MEMORY_GB = 512.0;
    constexpr static double MIN_SINGLE_NODE_MEMORY_GB = 4.0;
    constexpr static int MAX_CLUSTER_NODES = 100;

    // Destructor (Updates Static Shared State on Object Teardown)
    ~ClusterNode() noexcept {
        if (isActive_) {
            --totalActiveNodes_;
            totalClusterMemoryGb_ -= allocatedMemoryGb_;
            isActive_ = false;
        }
        cout << "    [ClusterNode Dtor] Teardown Node #" << nodeId_ 
             << " ('" << nodeName_ << "') | Remaining Active: " << totalActiveNodes_ << "\n";
    }

    // Delete Copy Operations to prevent ambiguous duplicate node allocations
    ClusterNode(const ClusterNode&) = delete;
    ClusterNode& operator=(const ClusterNode&) = delete;

    // Allow Move Semantics
    ClusterNode(ClusterNode&& other) noexcept
        : nodeId_(other.nodeId_),
          nodeName_(std::move(other.nodeName_)),
          allocatedMemoryGb_(other.allocatedMemoryGb_),
          isActive_(other.isActive_) {
        other.isActive_ = false; // Disarm source instance without altering global static counts
    }

    ClusterNode& operator=(ClusterNode&& other) noexcept {
        if (this != &other) {
            if (isActive_) {
                --totalActiveNodes_;
                totalClusterMemoryGb_ -= allocatedMemoryGb_;
            }
            nodeId_ = other.nodeId_;
            nodeName_ = std::move(other.nodeName_);
            allocatedMemoryGb_ = other.allocatedMemoryGb_;
            isActive_ = other.isActive_;
            other.isActive_ = false;
        }
        return *this;
    }

    // =================================================================================
    // STATIC MEMBER FUNCTIONS
    // Operates on class-level static state; does NOT possess an implicit `this` pointer.
    // =================================================================================

    // Static Factory Function (Controlled Creation with Invariant Validation)
    [[nodiscard]] static std::shared_ptr<ClusterNode> createNode(int id, const string& name, double memoryGb) {
        if (id <= 0) {
            throw std::invalid_argument("Node ID must be strictly positive!");
        }
        if (memoryGb < MIN_SINGLE_NODE_MEMORY_GB || memoryGb > MAX_SINGLE_NODE_MEMORY_GB) {
            throw std::invalid_argument("Memory allocation exceeds valid cluster limits!");
        }
        if (totalActiveNodes_ >= MAX_CLUSTER_NODES) {
            throw std::runtime_error("Cluster capacity reached! Cannot create more nodes.");
        }

        // Shared pointer managing instance lifecycle
        return std::shared_ptr<ClusterNode>(new ClusterNode(id, name, memoryGb));
    }

    // Static Query Member Functions (Accessing Private Static Data)
    [[nodiscard]] static int getTotalActiveNodes() noexcept {
        return totalActiveNodes_;
    }

    [[nodiscard]] static double getTotalClusterMemoryGb() noexcept {
        return totalClusterMemoryGb_;
    }

    [[nodiscard]] static int getCumulativeNodesCreated() noexcept {
        return cumulativeNodesCreated_;
    }

    // Static Utility Function: Computes cluster average metrics without an instance
    [[nodiscard]] static double calculateAverageMemoryPerNodeGb() noexcept {
        if (totalActiveNodes_ == 0) return 0.0;
        return totalClusterMemoryGb_ / totalActiveNodes_;
    }

    // Static Member Function Accessing Instance Data via Explicit Pointer/Reference
    static void inspectNodeState(const ClusterNode& node) {
        // Static functions CAN ACCESS private instance members if passed a valid instance reference!
        cout << "    [STATIC INSPECTION] Node ID: " << node.nodeId_
             << " | Name: " << node.nodeName_
             << " | Memory: " << node.allocatedMemoryGb_ << " GB"
             << " | Status: " << (node.isActive_ ? "ACTIVE" : "INACTIVE") << "\n";
    }

    // Instance Member Functions
    void displayStatus() const {
        cout << "    [NODE STATUS] ID: " << setw(4) << nodeId_ 
             << " | Name: " << setw(20) << left << nodeName_ 
             << " | Memory: " << setw(6) << fixed << setprecision(1) << allocatedMemoryGb_ << " GB"
             << " | Active: " << (isActive_ ? "YES" : "NO") << "\n";
    }

    [[nodiscard]] int getNodeId() const noexcept { return nodeId_; }
    [[nodiscard]] const string& getNodeName() const noexcept { return nodeName_; }
    [[nodiscard]] double getAllocatedMemoryGb() const noexcept { return allocatedMemoryGb_; }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for Static Members analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. COMPILE-TIME STATIC CONSTANTS & INITIAL STATIC STATE
    // =====================================================================================
    cout << "\n================ 1. COMPILE-TIME STATIC CONSTANTS & INITIAL STATE ================\n";

    cout << "  - Inspecting constexpr static constants (accessible without any instances):\n";
    cout << "    * Max Single Node Memory : " << ClusterNode::MAX_SINGLE_NODE_MEMORY_GB << " GB\n";
    cout << "    * Min Single Node Memory : " << ClusterNode::MIN_SINGLE_NODE_MEMORY_GB << " GB\n";
    cout << "    * Max Cluster Node Count : " << ClusterNode::MAX_CLUSTER_NODES << "\n";

    cout << "\n  - Querying static state before object instantiation:\n";
    cout << "    * Active Nodes           : " << ClusterNode::getTotalActiveNodes() << "\n";
    cout << "    * Total Memory Allocated : " << ClusterNode::getTotalClusterMemoryGb() << " GB\n";

    // =====================================================================================
    // 2. STATIC FACTORY CREATION & SHARED STATE MUTATION
    // =====================================================================================
    cout << "\n================ 2. STATIC FACTORY CREATION & SHARED STATE MUTATION ================\n";

    {
        cout << "  - Creating node instances via static factory function `ClusterNode::createNode()`...\n";
        auto node1 = ClusterNode::createNode(userInputValue, "ComputeNode_Alpha", 64.0);
        auto node2 = ClusterNode::createNode(userInputValue + 1, "ComputeNode_Beta", 128.0);
        auto node3 = ClusterNode::createNode(userInputValue + 2, "StorageNode_Gamma", 256.0);

        cout << "\n  - Displaying individual node instance statuses:\n";
        node1->displayStatus();
        node2->displayStatus();
        node3->displayStatus();

        cout << "\n  - Querying shared class-level static state post-instantiation:\n";
        cout << "    * Total Active Nodes     : " << ClusterNode::getTotalActiveNodes() << "\n";
        cout << "    * Cumulative Nodes Created: " << ClusterNode::getCumulativeNodesCreated() << "\n";
        cout << "    * Total Cluster Memory   : " << ClusterNode::getTotalClusterMemoryGb() << " GB\n";
        cout << "    * Average Memory per Node: " << ClusterNode::calculateAverageMemoryPerNodeGb() << " GB\n";

        cout << "\n  - Exiting local scope to test dynamic static teardown...\n";
    } // node1, node2, node3 destructed HERE; static trackers updated automatically

    cout << "\n  - Querying static state post-scope teardown:\n";
    cout << "    * Total Active Nodes     : " << ClusterNode::getTotalActiveNodes() << "\n";
    cout << "    * Cumulative Nodes Created: " << ClusterNode::getCumulativeNodesCreated() << "\n";
    cout << "    * Total Cluster Memory   : " << ClusterNode::getTotalClusterMemoryGb() << " GB\n";

    // =====================================================================================
    // 3. STATIC MEMBER FUNCTIONS ACCESSING INSTANCE STATE
    // =====================================================================================
    cout << "\n================ 3. STATIC FUNCTION WITH EXPLICIT INSTANCE ACCESS ================\n";

    {
        auto node4 = ClusterNode::createNode(userInputValue + 10, "EdgeNode_Delta", 32.0);

        cout << "  - Invoking static member function `ClusterNode::inspectNodeState(*node4)`:\n";
        // Demonstrates that static methods can inspect private members when passed an instance reference
        ClusterNode::inspectNodeState(*node4);
    }

    // =====================================================================================
    // 4. INVARIANT ENFORCEMENT & ERROR HANDLING
    // =====================================================================================
    cout << "\n================ 4. INVARIANT ENFORCEMENT IN STATIC FACTORIES ================\n";

    try {
        cout << "  - Attempting to create node exceeding max memory limit (1024 GB)...\n";
        auto faultyNode = ClusterNode::createNode(userInputValue + 20, "FaultyNode", 1024.0);
    } catch (const exception& e) {
        cout << "  - [INVARIANT GUARD CAUGHT] Exception: \"" << e.what() << "\"\n";
    }

    try {
        cout << "  - Attempting to create node with invalid negative ID (-5)...\n";
        auto faultyNode = ClusterNode::createNode(-5, "NegativeIdNode", 32.0);
    } catch (const exception& e) {
        cout << "  - [INVARIANT GUARD CAUGHT] Exception: \"" << e.what() << "\"\n";
    }

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ STATIC DATA MEMBERS & FUNCTIONS SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Concept / Component   | Modern C++ Implementation Syntax  | Key Behavioral Advantage / Rule   |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Inline Static Member  | `inline static int count = 0;`    | Single shared memory; C++17 inline|\n"
         << "| Static Function       | `static int getCount();`          | No `this` pointer; class-scoped   |\n"
         << "| Static Factory        | `static shared_ptr<T> create();`  | Controlled object instantiation   |\n"
         << "| Static Constexpr      | `constexpr static double MAX=100;`| Evaluated at compile-time         |\n"
         << "| Memory Overhead       | 0 bytes per object instance       | Stored in BSS/Data segment        |\n"
         << "| Private Instance Access| Static func taking `const T& obj`  | Bypasses private encapsulation    |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}