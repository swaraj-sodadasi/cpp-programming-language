/*
 * =====================================================================================
 * CONCEPT        : Best Practices, Common Mistakes, and Performance Considerations in OOP
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  idiomatic patterns, subtle anti-patterns, and performance tradeoffs in Modern C++:
 *
 *                  1. BEST PRACTICES:
 *                     - RAII & Rule of Zero/5 for complete resource safety.
 *                     - Strict Const-Correctness & `[[nodiscard]]`.
 *                     - Virtual Destructors (`virtual ~Base() noexcept`) and explicit `override`.
 *                     - Preventing unintended implicit type conversions (`explicit`).
 *                     - Pass-by-const-reference vs. Move semantics (`std::move`).
 *
 *                  2. COMMON MISTAKES & PITFALLS:
 *                     - Missing Virtual Destructors (incomplete cleanup / UB).
 *                     - Object Slicing when passing derived objects by value.
 *                     - Calling Virtual Functions inside Constructors/Destructors (static binding).
 *                     - Unintended copies due to missing `noexcept` on move constructors.
 *
 *                  3. PERFORMANCE CONSIDERATIONS:
 *                     - Virtual Function Overhead (vtable indirection, cache misses, no inlining)
 *                       vs. Static Polymorphism (CRTP / Templates / Concepts).
 *                     - Cache Locality: Array of Pointers (pointer chasing) vs. Contiguous
 *                       Memory Structures (Data-Oriented Design / Struct of Arrays).
 *                     - Vector Reallocation guarantees (`noexcept` move requirement).
 *
 * RESOLVED ISSUE : Changed `GoodBase::process()` from pure virtual (`= 0`) to a virtual method 
 *                  with a default implementation. This makes `GoodBase` a concrete class, 
 *                  resolving the compiler error: "Parameter type 'GoodBase' is an abstract class" 
 *                  when demonstrating pass-by-value Object Slicing.
 *
 * TIME COMPLEXITY  : Static Dispatch (CRTP/Direct)  : O(1) inlined constant time.
 *                    Dynamic Dispatch (Virtual)      : O(1) constant time with vtable lookup indirection.
 * SPACE COMPLEXITY : Class Overhead                 : Base size + 8 bytes vptr (for virtual classes).
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <cstddef>

using namespace std;

// =====================================================================================
// 1. BEST PRACTICES vs. COMMON MISTAKES
// Demonstrates `explicit`, `const`, `override`, Virtual Destructors, and Slicing Prevention.
// =====================================================================================

// --- ANTI-PATTERN / BAD PRACTICE BASE CLASS ---
class BadBase {
public:
    int baseData_{100};

    // MISTAKE 1: Missing virtual destructor in a polymorphic base class!
    // Deleting a Derived object via BadBase* will cause UNDEFINED BEHAVIOR and resource leaks.
    ~BadBase() {
        cout << "    [BadBase Dtor] Non-virtual destructor called.\n";
    }

    virtual void doWork() const {
        cout << "    [BadBase] Doing base work.\n";
    }
};

class BadDerived : public BadBase {
private:
    int* rawHeapResource_{nullptr}; // Bad practice: raw pointer management

public:
    BadDerived() : rawHeapResource_(new int[5000]) {
        cout << "    [BadDerived Ctor] Allocated 5000 raw heap integers.\n";
    }

    ~BadDerived() {
        delete[] rawHeapResource_;
        cout << "    [BadDerived Dtor] Freed raw heap memory.\n";
    }

    void doWork() const override {
        cout << "    [BadDerived] Doing specialized derived work.\n";
    }
};

// --- GOOD PRACTICE / BEST PRACTICE HIERARCHY ---
class GoodBase {
protected:
    int entityId_;

public:
    // BEST PRACTICE 1: `explicit` constructor prevents accidental implicit type conversions
    explicit GoodBase(int id) : entityId_(id) {}

    // BEST PRACTICE 2: Virtual destructor guarantees safe polymorphic cleanup
    virtual ~GoodBase() noexcept {
        cout << "    [GoodBase Dtor] Virtual destructor called safely for ID: " << entityId_ << "\n";
    }

    // BEST PRACTICE 3: Read-only accessors marked `const` and `[[nodiscard]]`
    [[nodiscard]] int getEntityId() const noexcept {
        return entityId_;
    }

    // BEST PRACTICE 4: Virtual function with default implementation
    // (Concrete base class allows instantiation/pass-by-value to demonstrate Object Slicing)
    virtual void process() const {
        cout << "    [GoodBase] Executing base processing logic for ID: " << entityId_ << "\n";
    }
};

class GoodDerived : public GoodBase {
private:
    // BEST PRACTICE 5: Smart pointers (RAII) instead of raw pointers
    unique_ptr<vector<double>> payload_;

public:
    GoodDerived(int id, size_t payloadSize)
        : GoodBase(id), payload_(make_unique<vector<double>>(payloadSize, 3.14)) {}

    // BEST PRACTICE 6: Explicit `override` and `noexcept` on virtual destructor
    ~GoodDerived() noexcept override {
        cout << "    [GoodDerived Dtor] RAII automatically cleans up payload vector.\n";
    }

    void process() const override {
        cout << "    [GoodDerived] Processing derived payload with " << payload_->size() << " elements.\n";
    }
};

// Demonstration function showing Object Slicing
// Passing by value requires GoodBase to be concrete (not abstract).
void demonstrateObjectSlicing(GoodBase slicedCopy) { // MISTAKE: Pass-by-value causes Object Slicing!
    cout << "      -> Inside function (Passed By Value - Object Sliced!):\n";
    slicedCopy.process(); // Calls GoodBase::process(), derived attributes and vtable dispatch are stripped!
}

void preventObjectSlicing(const GoodBase& refObj) { // BEST PRACTICE: Pass by const reference
    cout << "      -> Inside function (Passed By Const Reference - Polymorphic!):\n";
    refObj.process(); // Calls GoodDerived::process() via dynamic dispatch!
}

// =====================================================================================
// 2. VIRTUAL FUNCTIONS IN CONSTRUCTORS / DESTRUCTORS PITFALL
// Demonstrates why calling virtual methods during object construction/destruction fails.
// =====================================================================================
class CtorVtableBase {
public:
    CtorVtableBase() {
        cout << "    [CtorVtableBase Ctor] Calling virtual setup()...\n";
        // MISTAKE: Virtual dispatch does NOT reach derived classes during base construction!
        // The object is currently of type CtorVtableBase because Derived sub-object is not built yet.
        this->setup(); 
    }

    virtual ~CtorVtableBase() noexcept = default;

    virtual void setup() const {
        cout << "      -> Base::setup() executed (Base Vtable active).\n";
    }
};

class CtorVtableDerived : public CtorVtableBase {
private:
    string configState_{"Uninitialized"};

public:
    CtorVtableDerived() : configState_("DerivedFullyConfigured") {}

    void setup() const override {
        cout << "      -> Derived::setup() executed with state: " << configState_ << "\n";
    }
};

// =====================================================================================
// 3. PERFORMANCE CONSIDERATIONS: DYNAMIC VS STATIC POLYMORPHISM
// Comparing Runtime Dynamic Dispatch (Vtables) vs Static Dispatch (CRTP)
// =====================================================================================

// --- A. Dynamic Polymorphism (Vtable Indirection) ---
class DynamicWidget {
public:
    virtual ~DynamicWidget() noexcept = default;
    virtual void update(double dt) = 0;
};

class ConcreteDynamicWidget : public DynamicWidget {
private:
    double val_{0.0};

public:
    void update(double dt) override {
        val_ += dt * 1.5;
    }

    [[nodiscard]] double getVal() const noexcept { return val_; }
};

// --- B. Static Polymorphism via CRTP (Curiously Recurring Template Pattern) ---
// Eliminates vtable indirection, enables compiler inlining, and saves 8 bytes vptr per object.
template <typename Derived>
class StaticWidgetBase {
public:
    void update(double dt) {
        // Static dispatch at compile-time
        static_cast<Derived*>(this)->updateImpl(dt);
    }
};

class ConcreteStaticWidget : public StaticWidgetBase<ConcreteStaticWidget> {
private:
    double val_{0.0};

public:
    void updateImpl(double dt) {
        val_ += dt * 1.5; // Easily inlined by compiler
    }

    [[nodiscard]] double getVal() const noexcept { return val_; }
};

// =====================================================================================
// 4. PERFORMANCE CONSIDERATIONS: CACHE LOCALITY & VECTOR REALLOCATION
// Demonstrates `noexcept` move requirement and contiguous memory layout.
// =====================================================================================
class CacheFriendlyParticle {
private:
    double x_{0.0}, y_{0.0}, z_{0.0};
    double vx_{1.0}, vy_{1.0}, vz_{1.0};

public:
    CacheFriendlyParticle() = default;

    // BEST PRACTICE FOR PERFORMANCE: Mark move operations `noexcept`!
    // If missing `noexcept`, `std::vector::reserve`/`push_back` will fall back to expensive DEEP COPIES.
    CacheFriendlyParticle(CacheFriendlyParticle&&) noexcept = default;
    CacheFriendlyParticle& operator=(CacheFriendlyParticle&&) noexcept = default;
    CacheFriendlyParticle(const CacheFriendlyParticle&) = default;
    CacheFriendlyParticle& operator=(const CacheFriendlyParticle&) = default;

    void updatePosition(double dt) noexcept {
        x_ += vx_ * dt;
        y_ += vy_ * dt;
        z_ += vz_ * dt;
    }
};

// =====================================================================================
// MAIN ENTRY POINT & BENCHMARKING
// =====================================================================================
int main() {
    int userInputValue = 0;

    cout << "Enter a base iteration multiplier for performance benchmark (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting multiplier to 100." << endl;
        userInputValue = 100;
    }

    const size_t benchmarkIterations = static_cast<size_t>(userInputValue) * 100'000;

    // =====================================================================================
    // 1. DEMONSTRATING BEST PRACTICES & COMMON PITFALLS
    // =====================================================================================
    cout << "\n================ 1. BEST PRACTICES & COMMON MISTAKES ================\n";

    cout << "  - A. Pitfall: Missing Virtual Destructor Leak (Demonstration):\n";
    {
        // BadBase* ptr = new BadDerived();
        // delete ptr; // UNDEFINED BEHAVIOR: BadDerived Dtor is NEVER called! Memory leak occurs!
        cout << "    [WARNING] Deleting BadDerived through BadBase* skips BadDerived destructor!\n";
    }

    cout << "\n  - B. Best Practice: Virtual Destructor & RAII Cleanup:\n";
    {
        unique_ptr<GoodBase> goodPtr = make_unique<GoodDerived>(101, 1000);
        goodPtr->process();
    } // goodPtr goes out of scope -> GoodDerived Dtor -> GoodBase Dtor called cleanly!

    cout << "\n  - C. Pitfall vs Best Practice: Object Slicing Demonstration:\n";
    {
        GoodDerived derivedObj(202, 500);
        
        cout << "    1. Demonstrating Object Slicing (Pass By Value):\n";
        demonstrateObjectSlicing(derivedObj); // Slices GoodDerived down to GoodBase!

        cout << "\n    2. Preventing Object Slicing (Pass By Const Reference):\n";
        preventObjectSlicing(derivedObj); // Preserves derived type & dynamic dispatch!
    }

    // =====================================================================================
    // 2. PITFALL: VIRTUAL CALLS IN CONSTRUCTORS
    // =====================================================================================
    cout << "\n================ 2. PITFALL: VIRTUAL CALLS IN CONSTRUCTORS ================\n";

    cout << "  - Instantiating CtorVtableDerived object...\n";
    // Notice that base class constructor executes Base::setup(), NOT Derived::setup()!
    CtorVtableDerived ctorDemo;

    // =====================================================================================
    // 3. PERFORMANCE BENCHMARK: DYNAMIC VS STATIC POLYMORPHISM
    // =====================================================================================
    cout << "\n================ 3. PERFORMANCE: DYNAMIC VS STATIC DISPATCH ================\n";
    cout << "  - Running benchmark over " << benchmarkIterations << " iterations...\n";

    // Scenario A: Dynamic Polymorphism (Vtable pointer chasing)
    vector<unique_ptr<DynamicWidget>> dynamicContainer;
    dynamicContainer.reserve(1000);
    for (int i = 0; i < 1000; ++i) {
        dynamicContainer.push_back(make_unique<ConcreteDynamicWidget>());
    }

    auto startDynamic = chrono::high_resolution_clock::now();
    for (size_t iter = 0; iter < benchmarkIterations / 1000; ++iter) {
        for (auto& widget : dynamicContainer) {
            widget->update(0.016); // Indirection through vtable pointer
        }
    }
    auto endDynamic = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> dynamicTime = endDynamic - startDynamic;

    // Scenario B: Static Polymorphism via CRTP / Direct Contiguous Vector
    vector<ConcreteStaticWidget> staticContainer(1000);

    auto startStatic = chrono::high_resolution_clock::now();
    for (size_t iter = 0; iter < benchmarkIterations / 1000; ++iter) {
        for (auto& widget : staticContainer) {
            widget.update(0.016); // Inlined at compile-time! Zero vtable lookup!
        }
    }
    auto endStatic = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> staticTime = endStatic - startStatic;

    cout << "    * Dynamic Polymorphism Execution Time : " << fixed << setprecision(3) 
         << dynamicTime.count() << " ms\n";
    cout << "    * Static CRTP Polymorphism Time      : " << fixed << setprecision(3) 
         << staticTime.count() << " ms\n";
    cout << "    * Performance Speedup Ratio          : " << fixed << setprecision(2) 
         << (dynamicTime.count() / staticTime.count()) << "x Faster!\n";

    // =====================================================================================
    // 4. PERFORMANCE: DATA LOCALITY (ARRAY OF POINTERS VS CONTIGUOUS LAYOUT)
    // =====================================================================================
    cout << "\n================ 4. PERFORMANCE: CACHE LOCALITY & DATA LAYOUT ================\n";

    constexpr size_t particleCount = 100'000;

    // Bad Cache Locality: Pointer Chasing (Heap allocations scattered across memory)
    vector<CacheFriendlyParticle*> scatteredParticles;
    scatteredParticles.reserve(particleCount);
    for (size_t i = 0; i < particleCount; ++i) {
        scatteredParticles.push_back(new CacheFriendlyParticle());
    }

    auto startScattered = chrono::high_resolution_clock::now();
    for (size_t step = 0; step < 10; ++step) {
        for (size_t i = 0; i < particleCount; ++i) {
            scatteredParticles[i]->updatePosition(0.016); // High cache miss rate
        }
    }
    auto endScattered = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> scatteredTime = endScattered - startScattered;

    // Good Cache Locality: Contiguous Vector Layout
    vector<CacheFriendlyParticle> contiguousParticles(particleCount);

    auto startContiguous = chrono::high_resolution_clock::now();
    for (size_t step = 0; step < 10; ++step) {
        for (auto& p : contiguousParticles) {
            p.updatePosition(0.016); // Sequential CPU cache pre-fetching!
        }
    }
    auto endContiguous = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> contiguousTime = endContiguous - startContiguous;

    cout << "    * Scattered Pointer Allocation Time  : " << scatteredTime.count() << " ms (Pointer Chasing)\n";
    cout << "    * Contiguous Memory Array Time      : " << contiguousTime.count() << " ms (Cache-Friendly)\n";
    cout << "    * Cache Locality Speedup Ratio       : " << (scatteredTime.count() / contiguousTime.count()) << "x Faster!\n";

    // Clean up scattered raw pointers
    for (auto* p : scatteredParticles) delete p;

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ OOP BEST PRACTICES & PERFORMANCE SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Topic / Rule          | Best Practice / Solution          | Potential Penalty / Common Error  |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Resource Management   | Use Smart Pointers (`make_unique`)| Manual `new`/`delete` causes leaks|\n"
         << "| Virtual Destructors   | `virtual ~Base() noexcept`        | Missing `virtual` causes UB / leaks|\n"
         << "| Parameter Passing     | Pass by `const Base&` or `value&&`| Pass-by-value causes Object Slicing|\n"
         << "| Constructor Converts  | Mark constructors `explicit`      | Hidden temporary conversions      |\n"
         << "| Virtuals in Ctors     | Avoid virtual calls in ctors/dtors| Executes Base implementation only |\n"
         << "| Dispatch Performance  | Use CRTP / Templates for hotspots | Dynamic vtable causes cache misses|\n"
         << "| Move Operations       | Mark moves `noexcept`             | Un-noexcept moves trigger copies  |\n"
         << "| Memory Layout         | Prefer contiguous `std::vector`   | Array of pointers degrades cache  |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}