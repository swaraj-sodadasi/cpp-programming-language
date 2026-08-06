/*
 * =====================================================================================
 * CONCEPT        : Boolean Type and Character Types in C++
 * DESCRIPTION    : Comprehensive implementation covering:
 *                  1. Boolean Type (bool): Size, boolean literals (true, false),
 *                     stream formatting (boolalpha vs noboolalpha), and logical evaluation.
 *                  2. Character Types: Narrow (char), wide (wchar_t), and UTF encoding 
 *                     types (char8_t, char16_t, char32_t) metadata and sizes.
 *                  3. Character Inspection & Manipulation: Safe <cctype> classification 
 *                     (isalpha, isdigit, isalnum), case conversion, and character arithmetic.
 *
 * TIME COMPLEXITY  : Best Case: O(1) — Constant time execution for scalar variable 
 *                    evaluations, character inspections, and I/O streams.
 * SPACE COMPLEXITY : Best Case: O(1) — Fixed stack memory footprint bounded by standard scalar types.
 * =====================================================================================
 */

#include <iostream>
#include <iomanip>
#include <cctype>
#include <limits>
#include <string>
#include <uchar.h>

using namespace std;

/**
 * @brief Inspects ASCII metadata, classification, and casing for a dynamic character.
 * @param ch Input character to analyze safely.
 */
void analyzeCharacter(char ch) noexcept {
    // Cast to unsigned char to avoid undefined behavior with negative char values in <cctype>
    auto uc = static_cast<unsigned char>(ch);

    cout << "\n================ DYNAMIC CHARACTER ANALYSIS ================" << endl;
    cout << "Character Input         : '" << ch << "'" << endl;
    cout << "ASCII Value (Dec / Hex) : " << static_cast<int>(uc) 
         << " / 0x" << hex << uppercase << static_cast<int>(uc) << dec << endl;
    cout << "Is Alphabetic?          : " << boolalpha << static_cast<bool>(isalpha(uc)) << endl;
    cout << "Is Digit?               : " << static_cast<bool>(isdigit(uc)) << endl;
    cout << "Is Alphanumeric?        : " << static_cast<bool>(isalnum(uc)) << endl;
    cout << "Uppercase Conversion    : '" << static_cast<char>(toupper(uc)) << "'" << endl;
    cout << "Lowercase Conversion    : '" << static_cast<char>(tolower(uc)) << "'" << endl;

    // Perform dynamic character arithmetic
    if (isalpha(uc)) {
        char base = isupper(uc) ? 'A' : 'a';
        int alphaPosition = ch - base + 1;
        cout << "Alphabetical Position   : " << alphaPosition << endl;
    } else if (isdigit(uc)) {
        int numericValue = ch - '0';
        cout << "Converted Integer Value : " << numericValue << endl;
    }
}

int main() {
    int rawBoolInput{0};
    bool userBool{false};
    char userChar{' '};

    // 1. DYNAMIC INPUT COLLECTION WITH STREAM FLUSHING
    cout << "Enter a Boolean value (1 for true, 0 for false): " << flush;
    if (cin >> rawBoolInput) {
        userBool = static_cast<bool>(rawBoolInput);
    }

    cout << "Enter a single Character (e.g., 'K'): " << flush;
    cin >> userChar;

    // 2. BOOLEAN TYPE REPORT
    cout << "\n================ 1. BOOLEAN TYPE (& METRICS) ================" << endl;
    cout << left << setw(26) << "Property / Operation" << "Value / Representation" << endl;
    cout << string(55, '-') << endl;
    cout << left << setw(26) << "Memory Footprint" << sizeof(bool) << " Byte(s)" << endl;
    cout << left << setw(26) << "Default Stream Output" << noboolalpha << userBool << endl;
    cout << left << setw(26) << "Textual Stream Output" << boolalpha << userBool << endl;
    cout << left << setw(26) << "Logical NOT (!val)" << (!userBool) << endl;
    cout << left << setw(26) << "Logical AND (val && true)" << (userBool && true) << endl;
    cout << left << setw(26) << "Logical OR (val || false)" << (userBool || false) << endl;

    // 3. CHARACTER TYPES METADATA REPORT
    cout << "\n================ 2. CHARACTER TYPES & METADATA ================" << endl;
    cout << left << setw(14) << "Type Name"
         << setw(10) << "Size(B)"
         << setw(18) << "Signedness"
         << setw(20) << "Min Value"
         << "Max Value" << endl;
    cout << string(75, '-') << endl;

    cout << left << setw(14) << "char"
         << setw(10) << sizeof(char)
         << setw(18) << (numeric_limits<char>::is_signed ? "Signed" : "Unsigned")
         << setw(20) << static_cast<int>(numeric_limits<char>::min())
         << static_cast<int>(numeric_limits<char>::max()) << endl;

    cout << left << setw(14) << "wchar_t"
         << setw(10) << sizeof(wchar_t)
         << setw(18) << (numeric_limits<wchar_t>::is_signed ? "Signed" : "Unsigned")
         << setw(20) << static_cast<long long>(numeric_limits<wchar_t>::min())
         << static_cast<long long>(numeric_limits<wchar_t>::max()) << endl;

    cout << left << setw(14) << "char8_t"
         << setw(10) << sizeof(char8_t)
         << setw(18) << "Unsigned"
         << setw(20) << static_cast<unsigned int>(numeric_limits<char8_t>::min())
         << static_cast<unsigned int>(numeric_limits<char8_t>::max()) << endl;

    cout << left << setw(14) << "char16_t"
         << setw(10) << sizeof(char16_t)
         << setw(18) << "Unsigned"
         << setw(20) << static_cast<unsigned int>(numeric_limits<char16_t>::min())
         << static_cast<unsigned int>(numeric_limits<char16_t>::max()) << endl;

    cout << left << setw(14) << "char32_t"
         << setw(10) << sizeof(char32_t)
         << setw(18) << "Unsigned"
         << setw(20) << static_cast<unsigned long long>(numeric_limits<char32_t>::min())
         << static_cast<unsigned long long>(numeric_limits<char32_t>::max()) << endl;

    // 4. CHARACTER INSPECTION INVOCATION
    analyzeCharacter(userChar);

    return 0;
}