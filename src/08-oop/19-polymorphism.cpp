/*
 * =====================================================================================
 * CONCEPT        : Polymorphism in Object-Oriented Programming (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the two major forms of Polymorphism, virtual tables, and modern specifiers:
 *
 *                  1. Compile-Time Polymorphism (Static Dispatch):
 *                     - Function Overloading : Resolving same-named methods by parameter signature.
 *                     - Operator Overloading : Defining custom behavior for built-in operators.
 *                  2. Runtime Polymorphism (Dynamic Dispatch):
 *                     - Virtual Functions    : Methods resolved at runtime via vtable lookup.
 *                     - Pure Virtual Functions: Abstract interface contracts (`= 0`) requiring implementation.
 *                     - Abstract Base Classes : Classes containing pure virtual methods (cannot be instantiated).
 *                     - Virtual Destructors  : Guarantees safe derived object cleanup through base pointers.
 *                     - `override` Specifier : Compiler-checked assurance of valid method overriding.
 *                     - `final` Specifier    : Blocks further derived overriding or class inheritance.
 *
 * TIME COMPLEXITY  : Compile-Time Static Calls : O(1) constant time (resolved at compile-time).
 *                    Runtime Dynamic Calls     : O(1) constant time (via vtable indirection).
 * SPACE COMPLEXITY : Class Instance Overhead  : Sum of data members + 8 bytes vptr (for virtual classes).
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <limits>

using namespace std;

// =====================================================================================
// 1. COMPILE-TIME POLYMORPHISM (STATIC DISPATCH)
// Demonstrates Function Overloading and Operator Overloading resolved at compile time.
// =====================================================================================
class DistanceScalar {
private:
    double meters_;

public:
    explicit DistanceScalar(double m = 0.0) : meters_(m) {}

    // Method Overloading 1: Single double parameter
    void scale(double factor) noexcept {
        meters_ *= factor;
        cout << "    [Static Overload: scale(double)] Scaled by factor " << factor 
             << " -> Meters: " << meters_ << " m\n";
    }

    // Method Overloading 2: Two parameters (factor and offset)
    void scale(double factor, double offsetMeters) noexcept {
        meters_ = (meters_ * factor) + offsetMeters;
        cout << "    [Static Overload: scale(double, double)] Scaled by " << factor 
             << " + Offset " << offsetMeters << " m -> Meters: " << meters_ << " m\n";
    }

    // Method Overloading 3: String unit scaling
    void scale(const string& unitName) {
        if (unitName == "km") {
            meters_ /= 1000.0;
            cout << "    [Static Overload: scale(string)] Converted to Kilometers: " << meters_ << " km\n";
        } else {
            cout << "    [Static Overload: scale(string)] Unknown unit '" << unitName << "'\n";
        }
    }

    // Operator Overloading: Binary '+' for DistanceScalar addition
    DistanceScalar operator+(const DistanceScalar& rhs) const noexcept {
        return DistanceScalar(this->meters_ + rhs.meters_);
    }

    // Stream Operator Overloading
    friend ostream& operator<<(ostream& os, const DistanceScalar& d) {
        os << d.meters_ << " meters";
        return os;
    }

    [[nodiscard]] double getMeters() const noexcept { return meters_; }
};

// =====================================================================================
// 2. RUNTIME POLYMORPHISM (DYNAMIC DISPATCH)
// Abstract Base Class defining Interface Contract with Pure Virtual Functions & Virtual Destructor.
// =====================================================================================
class ISensorNode {
protected:
    int sensorId_;
    string locationTag_;

public:
    ISensorNode(int id, string location)
        : sensorId_(id), locationTag_(std::move(location)) {
        cout << "    [ISensorNode Base Ctor] Created Base Sensor ID: " << sensorId_ << "\n";
    }

    // ESSENTIAL VIRTUAL DESTRUCTOR
    // Guarantees derived destructors fire when deleting via base pointers (`unique_ptr<ISensorNode>`)
    virtual ~ISensorNode() noexcept {
        cout << "    [ISensorNode Base Dtor] Cleaned Base Sensor ID: " << sensorId_ << "\n";
    }

    // PURE VIRTUAL FUNCTIONS (Defines mandatory interface contract)
    virtual void initializeHardware() = 0;
    [[nodiscard]] virtual double readTelemetry() const = 0;
    [[nodiscard]] virtual string getSensorType() const = 0;

    // VIRTUAL FUNCTION WITH DEFAULT IMPLEMENTATION
    virtual void printDiagnosticReport() const {
        cout << "    [BASE DIAGNOSTIC] Sensor ID: " << sensorId_ 
             << " | Location: '" << locationTag_ 
             << "' | Status: Operational\n";
    }

    [[nodiscard]] int getSensorId() const noexcept { return sensorId_; }
};

// =====================================================================================
// DERIVED CLASS 1: TemperatureSensor
// Fulfills Pure Virtual contract using `override` specifier.
// =====================================================================================
class TemperatureSensor : public ISensorNode {
private:
    double currentTempCelsius_{22.5};

public:
    TemperatureSensor(int id, string location, double initialTemp)
        : ISensorNode(id, std::move(location)), currentTempCelsius_(initialTemp) {
        cout << "    [TemperatureSensor Derived Ctor] Initial Temp: " << currentTempCelsius_ << " C\n";
    }

    ~TemperatureSensor() noexcept override {
        cout << "    [TemperatureSensor Derived Dtor] Releasing thermal hardware bus...\n";
    }

    // Overriding Pure Virtual Functions
    void initializeHardware() override {
        cout << "    [TEMP OVERRIDE] Calibrating thermistor probe on ID " << sensorId_ << "...\n";
    }

    [[nodiscard]] double readTelemetry() const override {
        return currentTempCelsius_;
    }

    [[nodiscard]] string getSensorType() const override {
        return "Thermal / Temperature Sensor";
    }

    // Overriding Default Virtual Function
    void printDiagnosticReport() const override {
        cout << "    [TEMP DIAGNOSTIC] ID: " << sensorId_ 
             << " | Type: " << getSensorType() 
             << " | Current Reading: " << currentTempCelsius_ << " C\n";
    }
};

// =====================================================================================
// DERIVED CLASS 2: PressureSensor
// Demonstrates `override` and uses `final` to block further method overrides.
// =====================================================================================
class PressureSensor : public ISensorNode {
protected:
    double pressureBar_{1.013};

public:
    PressureSensor(int id, string location, double pressure)
        : ISensorNode(id, std::move(location)), pressureBar_(pressure) {
        cout << "    [PressureSensor Derived Ctor] Pressure: " << pressureBar_ << " bar\n";
    }

    ~PressureSensor() noexcept override {
        cout << "    [PressureSensor Derived Dtor] Disarming piezoresistive diaphragm...\n";
    }

    void initializeHardware() override {
        cout << "    [PRESSURE OVERRIDE] Zeroing pressure sensor chamber on ID " << sensorId_ << "...\n";
    }

    [[nodiscard]] double readTelemetry() const override {
        return pressureBar_;
    }

    [[nodiscard]] string getSensorType() const override {
        return "Barometric Pressure Sensor";
    }

    // `final` OVERRIDDEN METHOD: Prevents derived classes from overriding this method further!
    void printDiagnosticReport() const override final {
        cout << "    [FINAL PRESSURE DIAGNOSTIC] ID: " << sensorId_ 
             << " | Pressure: " << pressureBar_ << " bar (OVERRIDE BLOCKED BELOW)\n";
    }
};

// =====================================================================================
// DERIVED CLASS 3: HighPrecisionPressureSensor
// Inherits from PressureSensor; demonstrates `final` method enforcement.
// =====================================================================================
class HighPrecisionPressureSensor : public PressureSensor {
private:
    double calibrationOffset_{0.0005};

public:
    HighPrecisionPressureSensor(int id, string location, double pressure, double offset)
        : PressureSensor(id, std::move(location), pressure), calibrationOffset_(offset) {}

    ~HighPrecisionPressureSensor() noexcept override {
        cout << "    [HighPrecisionPressure Dtor] Releasing precision sub-system...\n";
    }

    // Overriding telemetry to apply precision offset
    [[nodiscard]] double readTelemetry() const override {
        return pressureBar_ + calibrationOffset_;
    }

    // CANNOT OVERRIDE printDiagnosticReport() HERE!
    // void printDiagnosticReport() const override; // COMPILE ERROR: Cannot override 'final' function!
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for Polymorphism analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. COMPILE-TIME POLYMORPHISM DEMONSTRATION (STATIC DISPATCH)
    // =====================================================================================
    cout << "\n================ 1. COMPILE-TIME POLYMORPHISM (STATIC DISPATCH) ================\n";

    DistanceScalar dist1(100.0);
    DistanceScalar dist2(50.0);

    cout << "  - Initial Distances: dist1 = " << dist1 << ", dist2 = " << dist2 << "\n";

    cout << "\n  - Demonstrating Method Overloading (Static Dispatch):\n";
    dist1.scale(2.5);                   // Calls scale(double)
    dist1.scale(1.5, 10.0);             // Calls scale(double, double)
    dist1.scale("km");                  // Calls scale(string)

    cout << "\n  - Demonstrating Operator Overloading (`dist1 + dist2`):\n";
    DistanceScalar distSum = dist1 + dist2;
    cout << "    Sum Result: " << distSum << "\n";

    // =====================================================================================
    // 2. RUNTIME POLYMORPHISM DEMONSTRATION (DYNAMIC DISPATCH)
    // =====================================================================================
    cout << "\n================ 2. RUNTIME POLYMORPHISM (DYNAMIC DISPATCH) ================\n";

    {
        cout << "  - Storing heterogeneous derived sensors in `vector<unique_ptr<ISensorNode>>`...\n";
        vector<unique_ptr<ISensorNode>> sensorNetwork;

        // ISensorNode abstract = ISensorNode(1, "Fail"); // COMPILE ERROR: Cannot instantiate Abstract Class!

        sensorNetwork.push_back(make_unique<TemperatureSensor>(userInputValue, "Engine_Bay", 85.4));
        sensorNetwork.push_back(make_unique<PressureSensor>(userInputValue + 1, "Manifold_Pipe", 2.45));
        sensorNetwork.push_back(make_unique<HighPrecisionPressureSensor>(userInputValue + 2, "Lab_Chamber", 1.013, 0.002));

        cout << "\n  - Initializing hardware polymorphically across sensor network:\n";
        for (const auto& sensor : sensorNetwork) {
            sensor->initializeHardware(); // Dynamic dispatch selects derived method at runtime via vtable
        }

        cout << "\n  - Executing polymorphic telemetry reads and diagnostic reporting:\n";
        for (const auto& sensor : sensorNetwork) {
            sensor->printDiagnosticReport();
            cout << "    -> Telemetry Value: " << sensor->readTelemetry() << "\n";
        }

        cout << "\n  - Exiting polymorphic vector scope (Virtual Destructors fire cleanly):\n";
    } // Polymorphic cleanup runs in sequence

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ POLYMORPHISM IN OOP SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Polymorphism Type     | Implementation Mechanism          | Primary Architectural Benefit     |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Function Overloading  | Same name, different signatures   | Static dispatch; clear API syntax |\n"
         << "| Operator Overloading  | `operator+`, `operator<<`, etc.   | Custom domain mathematical syntax |\n"
         << "| Virtual Functions     | `virtual void func();`            | Dynamic runtime dispatch (vtable) |\n"
         << "| Pure Virtual Funcs    | `virtual void func() = 0;`        | Defines mandatory type contract   |\n"
         << "| Abstract Base Class   | Class with >= 1 pure virtual func | Prevents instantiation of bases   |\n"
         << "| Virtual Destructor    | `virtual ~Base() noexcept;`       | Safe polymorphic memory cleanup   |\n"
         << "| `override` Specifier  | `void func() override;`           | Compile-time check for valid override|\n"
         << "| `final` Specifier     | `void func() override final;`     | Blocks further overrides/derived  |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}