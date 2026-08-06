/*
 * =====================================================================================
 * CONCEPT        : One Definition Rule (ODR) in C++
 * DESCRIPTION    : Comprehensive program demonstrating the three pillars of the ODR:
 *                  1. Single Translation Unit (TU) Rule :
 *                     - A variable, function, class, enum, or template can have at most 
 *                       ONE definition per Translation Unit.
 *                  2. Program-Wide Rule (Non-Inline Symbols) :
 *                     - Non-inline global functions and non-inline global variables 
 *                       must have EXACTLY ONE definition across the ENTIRE program.
 *                  3. Multi-TU Identical Definition Rule (Inline / Header Symbols) :
 *                     - Classes, structures, inline functions, inline variables (C++17), 
 *                       and templates CAN be defined in multiple TUs, provided that 
 *                       every definition is TOKEN-FOR-TOKEN IDENTICAL and resolves to 
 *                       the exact same meanings.
 *
 * TIME COMPLEXITY  : Best Case: O(1) runtime; ODR enforcement occurs at compile & link time.
 * SPACE COMPLEXITY : Best Case: O(1) stack frame footprint.
 * =====================================================================================
 */

#include <iostream>

using namespace std;

// =====================================================================================
// RULE 1: EXACTLY ONE DEFINITION PER TRANSLATION UNIT (SINGLE TU)
// =====================================================================================

// Function Declaration (Prototypes can occur MULTIPLE times in a TU)
void processTask(int count);
void processTask(int count); // Duplicate declaration is 100% legal!

// Function Definition (Must occur AT MOST ONCE in this TU)
void processTask(int count) {
    cout << "  - [Single Definition] Processing task with count: " << count << endl;
}

/*
 * ODR VIOLATION EXAMPLE 1 (Within Single TU):
 *
 * void processTask(int count) { ... } 
 * // COMPILER FATAL ERROR: Redefinition of 'void processTask(int)'!
 */


// =====================================================================================
// RULE 2: EXACTLY ONE DEFINITION PER ENTIRE PROGRAM (EXTERNAL LINKAGE)
// Non-inline entities exported to the symbol table can only exist once across all .cpp files.
// =====================================================================================

// Global variable definition (Non-inline, External Linkage)
int g_globalAppStatus = 200; 

// Global function definition (Non-inline, External Linkage)
void executeGlobalAppRoutine() {
    cout << "  - [Global Symbol] Routine executing with App Status: " << g_globalAppStatus << endl;
}

/*
 * ODR VIOLATION EXAMPLE 2 (Across Multiple TUs):
 *
 * If another file (e.g., 'Helper.cpp') also contains:
 *     int g_globalAppStatus = 200;
 * 
 * LINKER FATAL ERROR: 'symbol g_globalAppStatus is multiply defined'!
 */


// =====================================================================================
// RULE 3: MULTI-TU ALLOWED DEFINITIONS (INLINE FUNCTIONS, INLINE VARIABLES & CLASSES)
// Must be token-for-token identical across every header inclusion site.
// =====================================================================================

// A. CLASS DEFINITIONS (Allowed in multiple TUs / headers if identical)
class NetworkPacket {
private:
    int packetId_;
    size_t payloadSize_;

public:
    NetworkPacket(int id, size_t size) : packetId_(id), payloadSize_(size) {}

    // Inline member function defined inside class body (Implicitly inline)
    void displayPacket() const {
        cout << "  - [Class Definition] Packet ID: " << packetId_ 
             << " | Payload: " << payloadSize_ << " bytes\n";
    }
};

// B. INLINE FUNCTIONS (Allowed in multiple TUs / headers)
// The compiler merges duplicate inline function definitions into a single call address.
inline double calculateTax(double amount, double rate) {
    return amount * rate;
}

// C. C++17 INLINE VARIABLES (Allowed in headers without duplicate symbol linker errors!)
inline constexpr double g_piConstant = 3.141592653589793;
inline int g_sharedHeaderCounter = 1000;

/*
 * ODR VIOLATION EXAMPLE 3 (Token-for-Token Mismatch across TUs):
 *
 * File1.cpp:
 *   struct DataRecord { int x; double y; };
 *
 * File2.cpp:
 *   struct DataRecord { double y; int x; }; // Order reversed!
 *
 * UNDEFINED BEHAVIOR / LINKER WARNING: Silent data corruption or crashes at runtime!
 */


// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userPacketCount = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter number of network packets to simulate (e.g., 3): " << flush;
    if (!(cin >> userPacketCount) || userPacketCount <= 0) {
        cout << "Invalid input. Defaulting packet count to 3." << endl;
        userPacketCount = 3;
    }

    // 1. SINGLE TU & GLOBAL SYMBOLS DEMONSTRATION
    cout << "\n================ 1. SINGLE TU & GLOBAL DEFINITIONS ================" << endl;
    processTask(userPacketCount);
    executeGlobalAppRoutine();

    // 2. INLINE & CLASS DEFINITIONS (MULTI-TU SAFE)
    cout << "\n================ 2. MULTI-TU INLINE & CLASS DEFINITIONS ================" << endl;
    
    for (int i = 1; i <= userPacketCount; ++i) {
        NetworkPacket packet(100 + i, static_cast<size_t>(i * 512));
        packet.displayPacket();
    }

    double taxVal = calculateTax(150.00, 0.08);
    cout << "  - [Inline Function] calculateTax(150.0, 0.08) = $" << taxVal << endl;
    cout << "  - [C++17 Inline Variable] g_piConstant        = " << g_piConstant << endl;
    cout << "  - [C++17 Inline Variable] g_sharedHeaderCounter = " << g_sharedHeaderCounter << endl;

    // 3. SUMMARY OF ONE DEFINITION RULE (ODR)
    cout << "\n================ ONE DEFINITION RULE (ODR) SUMMARY ================" << endl;
    cout << "+-----------------------+-------------------+-------------------+-----------------------------------+\n"
         << "| Construct Type        | Single TU Limit   | Whole Program     | Multi-TU Inclusion Mechanism      |\n"
         << "+-----------------------+-------------------+-------------------+-----------------------------------+\n"
         << "| Non-Inline Variable   | Max 1 Definition  | EXACTLY 1 Def     | Declare with `extern int x;`      |\n"
         << "| Non-Inline Function   | Max 1 Definition  | EXACTLY 1 Def     | Put prototype in `.h`, def in `.cpp`|\n"
         << "| `inline` Function     | Max 1 Definition  | Multiple Allowed  | Define in header; merged by Linker|\n"
         << "| `inline` Variable     | Max 1 Definition  | Multiple Allowed  | Define in header (C++17)          |\n"
         << "| Class / Struct        | Max 1 Definition  | Multiple Allowed  | Define in header (Token Identical)|\n"
         << "| Template Class/Func   | Max 1 Definition  | Multiple Allowed  | Define in header (Token Identical)|\n"
         << "+-----------------------+-------------------+-------------------+-----------------------------------+\n";

    return 0;
}