/*
 * =====================================================================================
 * CONCEPT        : Literals in C++ (Integer, Floating-Point, Character, String, Boolean, 
 *                  Raw Strings, and User-Defined Literals)
 * DESCRIPTION    : Comprehensive implementation showcasing all standard C++ literal types:
 *                  1. Integer Literals       : Decimal (100), Hexadecimal (0x64), Octal (0144), 
 *                                              Binary (0b01100100), Suffixes (U, L, LL, ULL).
 *                  2. Floating-Point Literals: Standard (3.14159), Scientific (1.5e-3), 
 *                                              Suffixes (f, L).
 *                  3. Character Literals     : Single ('A'), Escape ('\n'), Wide (L'A'), UTF-8 (u8'A').
 *                  4. String Literals        : Standard ("C++"), Raw String Literals R"(...)".
 *                  5. Boolean Literals       : true, false.
 *                  6. User-Defined Literals  : Custom suffix operator (e.g., 5.0_km -> meters).
 *
 * TIME COMPLEXITY  : Best Case: O(1) — Direct constant literal evaluations and dynamic comparisons 
 *                    execute in constant time.
 * SPACE COMPLEXITY : Best Case: O(1) — Fixed stack memory footprint for dynamic user variables.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

// 1. USER-DEFINED LITERAL (UDL) DEFINITION
// Converts kilometers to meters at compile-time/runtime via custom suffix _km
constexpr long double operator""_km(long double val) noexcept {
    return val * 1000.0;
}

// Converts meters to kilometers
constexpr long double operator""_m(long double val) noexcept {
    return val / 1000.0;
}

int main() {
    // Dynamic runtime variables for input mapping
    double dynamicDistanceMeters = 0.0;
    string dynamicSearchTerm{};

    // 2. DYNAMIC INPUT COLLECTION WITH EXPLICIT STREAM FLUSHING
    cout << "Enter a distance in meters (e.g., 2500): " << flush;
    if (!(cin >> dynamicDistanceMeters) || dynamicDistanceMeters < 0.0) {
        cout << "Invalid distance provided. Program terminated." << endl;
        return 0;
    }

    cout << "Enter a single-word keyword to match against raw string literal: " << flush;
    cin >> dynamicSearchTerm;

    // 3. INTEGER LITERALS DEMONSTRATION (Different Bases & Suffixes)
    constexpr int decimalLit     = 100;          // Base-10 Decimal Literal
    constexpr int hexLit         = 0x64;         // Base-16 Hexadecimal Literal (0x64 = 100)
    constexpr int octalLit       = 0144;         // Base-8 Octal Literal (0144 = 100)
    constexpr int binaryLit      = 0b01100100;   // Base-2 Binary Literal (0b01100100 = 100)
    constexpr unsigned long long intSuffixLit = 10000000000ULL; // Unsigned Long Long Suffix

    // 4. FLOATING-POINT LITERALS DEMONSTRATION
    constexpr double standardFloatLit = 3.1415926535; // Double-precision literal (default)
    constexpr float singleFloatLit    = 2.71828f;     // Single-precision literal ('f' suffix)
    constexpr double scientificLit   = 1.5e-3;       // Scientific notation literal (0.0015)

    // 5. CHARACTER & BOOLEAN LITERALS DEMONSTRATION
    constexpr char charLit         = 'Z';           // Single character literal
    constexpr char newlineCharLit  = '\n';          // Escape character literal
    constexpr bool boolTrueLit     = true;          // Boolean true literal
    constexpr bool boolFalseLit    = false;         // Boolean false literal

    // 6. STRING & RAW STRING LITERALS DEMONSTRATION
    const string standardStrLit = "Modern C++ Programming";
    
    // Raw String Literal R"(...)" ignores escape sequences like \n, \t, and unescaped quotes "
    const string rawStrLit = R"(C:\Program Files\App\Data.json -> {"status": "SUCCESS", "code": 200})";

    // 7. USER-DEFINED LITERALS DEMONSTRATION
    constexpr double fiveKmInMeters = 5.0_km;       // 5.0 kilometers converted to 5000.0 meters
    double dynamicKmVal = dynamicDistanceMeters * 1.0_m; // Convert dynamic meters to km

    // 8. FORMATTED REPORT OF LITERAL EVALUATIONS
    cout << "\n================ 1. INTEGER LITERALS (SAME VALUE: 100) ================" << endl;
    cout << "Decimal (100)        : " << decimalLit << endl;
    cout << "Hexadecimal (0x64)   : " << hexLit << endl;
    cout << "Octal (0144)         : " << octalLit << endl;
    cout << "Binary (0b01100100)  : " << binaryLit << endl;
    cout << "ULL Suffix (10000...): " << intSuffixLit << endl;

    cout << "\n================ 2. FLOATING-POINT LITERALS ================" << endl;
    cout << "Standard Double      : " << setprecision(10) << standardFloatLit << endl;
    cout << "Single Float ('f')   : " << singleFloatLit << endl;
    cout << "Scientific (1.5e-3)  : " << scientificLit << endl;

    cout << "\n================ 3. CHARACTER & BOOLEAN LITERALS ================" << endl;
    cout << "Character Literal    : '" << charLit << "' (ASCII: " << static_cast<int>(charLit) << ")" << newlineCharLit;
    cout << "Escape Char Literal  : '\\n' (ASCII: " << static_cast<int>(newlineCharLit) << ")" << endl;
    cout << "Boolean Literals     : " << boolalpha << boolTrueLit << " / " << boolFalseLit << endl;

    cout << "\n================ 4. STRING & RAW STRING LITERALS ================" << endl;
    cout << "Standard String      : " << standardStrLit << endl;
    cout << "Raw String Literal   : " << rawStrLit << endl;
    
    // Dynamic match against Raw String
    bool matchFound = (rawStrLit.find(dynamicSearchTerm) != string::npos);
    cout << "Dynamic Search Match : Keyword \"" << dynamicSearchTerm << "\" " 
         << (matchFound ? "FOUND" : "NOT FOUND") << " in raw string literal." << endl;

    cout << "\n================ 5. USER-DEFINED LITERALS (UDLs) ================" << endl;
    cout << "Compile-Time UDL     : 5.0_km = " << fiveKmInMeters << " meters" << endl;
    cout << "Dynamic Input UDL    : " << dynamicDistanceMeters << " meters = " 
         << fixed << setprecision(3) << dynamicKmVal << " km (calculated via 1.0_m operator)" << endl;

    return 0;
}