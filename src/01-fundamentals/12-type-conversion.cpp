/*
 * =====================================================================================
 * CONCEPT        : Type Conversion in C++ (Implicit vs Explicit / Named Casts)
 * DESCRIPTION    : Comprehensive implementation explaining modern C++ type conversions:
 *                  1. Implicit Conversion (Coercion) : Automatic type promotion/demotion and 
 *                                                      preventing unwanted conversions via 'explicit'.
 *                  2. static_cast                     : Compile-time safe conversions between related 
 *                                                      types (numeric, void*, base/derived pointers).
 *                  3. dynamic_cast                    : Runtime safe downcasting for polymorphic class 
 *                                                      hierarchies using RTTI (Run-Time Type Information).
 *                  4. const_cast                      : Adding or removing 'const' / 'volatile' qualifiers.
 *                  5. reinterpret_cast                : Low-level reinterpretation of bit patterns 
 *                                                      (e.g., pointer-to-integer, unrelated pointers).
 *
 * TIME COMPLEXITY  : Best Case: O(1) — Constant time execution for scalar type casts, pointer 
 *                    reinterpretation, and polymorphic dynamic downcasting.
 * SPACE COMPLEXITY : Best Case: O(1) — Fixed stack/heap memory allocation for test objects and primitives.
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <string>
#include <cstdint>

using namespace std;

// POLYMORPHIC HIERARCHY FOR DYNAMIC_CAST DEMONSTRATION
class BaseDevice {
public:
    virtual ~BaseDevice() = default; // Virtual destructor enables RTTI for dynamic_cast
    
    virtual void identify() const {
        cout << "[BaseDevice] Generic Hardware Device" << endl;
    }
};

class SmartSensor : public BaseDevice {
private:
    double sensorValue;

public:
    explicit SmartSensor(double val) noexcept : sensorValue(val) {}

    void identify() const override {
        cout << "[SmartSensor] IoT Sensor Unit | Telemetry: " << sensorValue << endl;
    }

    void calibrate() const {
        cout << "[SmartSensor] Executing self-calibration routine..." << endl;
    }
};

class UnrelatedDevice {
public:
    void ping() const {
        cout << "[UnrelatedDevice] Network Ping Received" << endl;
    }
};

// EXPLICIT CONSTRUCTOR DEMONSTRATION CLASS
class MemoryBuffer {
private:
    size_t bufferSize;

public:
    // 'explicit' keyword prevents implicit conversion from size_t / int to MemoryBuffer
    explicit MemoryBuffer(size_t bytes) : bufferSize(bytes) {}

    size_t getSize() const noexcept {
        return bufferSize;
    }
};

// CONST_CAST HELPER FUNCTION (Simulating legacy C-style API expecting non-const pointer)
void legacyPrintData(char* rawDataBuffer) {
    cout << "Legacy API Read Buffer: " << rawDataBuffer << endl;
}

int main() {
    double dynamicInputDouble = 0.0;
    int dynamicInputInt = 0;

    // 1. DYNAMIC INPUT COLLECTION WITH EXPLICIT FLUSHING
    cout << "Enter a floating-point number (e.g., 98.76): " << flush;
    if (!(cin >> dynamicInputDouble)) {
        cout << "Invalid floating-point input. Program terminated." << endl;
        return 0;
    }

    cout << "Enter an integer for bit-pattern reinterpretation (e.g., 1024): " << flush;
    if (!(cin >> dynamicInputInt)) {
        cout << "Invalid integer input. Program terminated." << endl;
        return 0;
    }

    // 2. IMPLICIT TYPE CONVERSION (AUTOMATIC COERCION)
    cout << "\n================ 1. IMPLICIT TYPE CONVERSION ================" << endl;
    // Numeric demotion / truncation (implicit double -> int)
    int implicitTruncatedInt = dynamicInputDouble;
    // Numeric promotion (implicit int -> double)
    double implicitPromotedDouble = dynamicInputInt;

    cout << "Original Double Value       : " << dynamicInputDouble << endl;
    cout << "Implicitly Cast to Int      : " << implicitTruncatedInt << " (Data truncated)" << endl;
    cout << "Implicitly Promoted Double  : " << implicitPromotedDouble << endl;

    // Demonstrating explicit constructor safety:
    // MemoryBuffer buf = 50; // COMPILE ERROR: Implicit conversion blocked by 'explicit'
    MemoryBuffer validBuffer(50); // Direct initialization required
    cout << "Explicit Constructor Size   : " << validBuffer.getSize() << " bytes" << endl;

    // 3. STATIC_CAST (COMPILE-TIME SAFE CAST)
    cout << "\n================ 2. STATIC_CAST ================" << endl;
    // Explicit numeric conversion without compiler narrowing warnings
    int explicitInt = static_cast<int>(dynamicInputDouble);
    double exactRatio = static_cast<double>(dynamicInputInt) / 3.0;

    // Void pointer conversion to typed pointer
    void* voidAddress = &dynamicInputInt;
    int* typedPtr = static_cast<int*>(voidAddress);

    cout << "static_cast<int>(double)    : " << explicitInt << endl;
    cout << "static_cast ratio (int/3.0) : " << exactRatio << endl;
    cout << "Value via static_cast void*: " << *typedPtr << endl;

    // 4. DYNAMIC_CAST (RUNTIME SAFE DOWNCASTING WITH RTTI)
    cout << "\n================ 3. DYNAMIC_CAST ================" << endl;
    // Polymorphic pointer holding derived instance
    unique_ptr<BaseDevice> polymorphicDevice = make_unique<SmartSensor>(dynamicInputDouble);
    polymorphicDevice->identify();

    // Safe downcast to derived class
    SmartSensor* sensorPtr = dynamic_cast<SmartSensor*>(polymorphicDevice.get());
    if (sensorPtr != nullptr) {
        cout << "dynamic_cast SUCCESSFUL    : BaseDevice* downcasted to SmartSensor*" << endl;
        sensorPtr->calibrate();
    } else {
        cout << "dynamic_cast FAILED        : Pointer is not of type SmartSensor" << endl;
    }

    // Attempting invalid downcast via a BaseDevice pointer pointing to a base instance
    unique_ptr<BaseDevice> genericDevice = make_unique<BaseDevice>();
    SmartSensor* invalidSensorPtr = dynamic_cast<SmartSensor*>(genericDevice.get());
    cout << "Invalid dynamic_cast Result: " 
         << (invalidSensorPtr == nullptr ? "nullptr (Safe runtime failure)" : "Valid") << endl;

    // 5. CONST_CAST (QUALIFIER MODIFICATION)
    cout << "\n================ 4. CONST_CAST ================" << endl;
    const string constMessage = "Immutable C++ String";

    // Safely casting away constness to interface with legacy read-only C APIs
    // Note: Modifying an originally 'const' declared object leads to Undefined Behavior.
    char* mutableStringPtr = const_cast<char*>(constMessage.c_str());
    legacyPrintData(mutableStringPtr);

    // 6. REINTERPRET_CAST (LOW-LEVEL BIT REINTERPRETATION)
    cout << "\n================ 5. REINTERPRET_CAST ================" << endl;
    // Converting pointer address to integral uintptr_t for inspection
    uintptr_t integerAddress = reinterpret_cast<uintptr_t>(&dynamicInputInt);
    
    // Reinterpreting integral address back to pointer
    int* reconstructedPtr = reinterpret_cast<int*>(integerAddress);

    cout << "Original Variable Value     : " << dynamicInputInt << endl;
    cout << "Address as Integer (Hex)    : 0x" << hex << uppercase << integerAddress << dec << endl;
    cout << "Reconstructed Ptr Value    : " << *reconstructedPtr << endl;

    return 0;
}