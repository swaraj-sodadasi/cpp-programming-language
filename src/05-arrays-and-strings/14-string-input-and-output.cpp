/*
 * =====================================================================================
 * CONCEPT        : String Input and Output Operations in C++ (`std::string` & C-Strings)
 * DESCRIPTION    : Comprehensive guide and implementation of string I/O mechanisms:
 *                  1. Formatted Extraction (`cin >> str`) : Reads single whitespace-delimited words;
 *                                                           skips leading whitespaces.
 *                  2. Unformatted Line Input (`getline`)  : Reads entire sentences including spaces 
 *                                                           until a newline ('\n').
 *                  3. Custom Delimited Input             : `getline(cin, str, delim)` for parsing 
 *                                                           tokens (e.g., CSV data).
 *                  4. Stream Buffer Cleanup               : Handling leftover newlines in `cin` buffer 
 *                                                           via `cin.ignore()` and `cin.clear()`.
 *                  5. String Output Techniques            : `cout << str`, `cout.write()`, and legacy `c_str()`.
 *                  6. In-Memory String Streams (`sstream`): Formatting and parsing numbers/strings 
 *                                                           via `std::stringstream`.
 *
 * TIME COMPLEXITY  : Reading / Writing a string of length N: O(N)
 * SPACE COMPLEXITY : Dynamic input buffer allocation: O(N)
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <sstream>
#include <limits>
#include <iomanip>

using namespace std;

// =====================================================================================
// HELPER FUNCTION: STREAM BUFFER CLEANUP
// Flushes leftover whitespace/newlines from cin buffer after formatted input (`cin >>`)
// =====================================================================================
void flushInputStreamBuffer() {
    if (cin.fail()) {
        cin.clear(); // Clear error state flags
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    // =====================================================================================
    // 1. FORMATTED INPUT (`cin >> word`) vs UNFORMATTED LINE INPUT (`getline`)
    // =====================================================================================
    cout << "================ 1. FORMATTED (`cin >>`) VS UNFORMATTED (`getline`) ================\n";

    string singleWord;
    cout << "Enter a single word (e.g., Alpha): " << flush;
    cin >> singleWord;

    cout << "  - Formatted Output (`cin >>`)    : \"" << singleWord << "\"\n";

    // CRITICAL C++ I/O LESSON: `cin >>` leaves the trailing newline ('\n') in the buffer!
    // We MUST flush the buffer before calling `getline()`, otherwise `getline()` reads the leftover '\n'!
    flushInputStreamBuffer();

    string fullSentence;
    cout << "\nEnter a full sentence with spaces (e.g., Modern C++ Engine 2026): " << flush;
    getline(cin, fullSentence);

    if (fullSentence.empty()) {
        fullSentence = "Modern C++ Engine 2026";
        cout << "Empty line detected. Defaulted to: \"" << fullSentence << "\"\n";
    }

    cout << "  - Unformatted Output (`getline`) : \"" << fullSentence << "\"\n";

    // =====================================================================================
    // 2. CUSTOM DELIMITED INPUT (`getline(cin, str, delim)`)
    // =====================================================================================
    cout << "\n================ 2. DELIMITED INPUT (CSV PARSING) ================\n";

    string csvInput = "CPU,GPU,RAM,Storage";
    stringstream csvStream(csvInput); // Simulated delimited stream input
    string token;

    cout << "  - Parsing simulated CSV stream (\"" << csvInput << "\") via delimiter ',':\n";
    int tokenCount = 1;
    while (getline(csvStream, token, ',')) {
        cout << "    Token " << tokenCount++ << ": \"" << token << "\"\n";
    }

    // =====================================================================================
    // 3. STRING OUTPUT TECHNIQUES & FORMATTING
    // =====================================================================================
    cout << "\n================ 3. STRING OUTPUT TECHNIQUES ================\n";

    string outputText = "High-Performance Computing";

    // Standard insertion operator
    cout << "  - Standard Output (`cout <<`)      : " << outputText << "\n";

    // Direct buffer write using write() method
    cout << "  - Direct Stream `.write()` (First 16): ";
    cout.write(outputText.data(), 16);
    cout << "\n";

    // Width alignment and padding using <iomanip>
    cout << "  - Right-Aligned (`std::setw(35)`): " << right << setw(35) << outputText << "\n";
    cout << "  - Left-Aligned  (`std::setfill('*')`): " << left << setfill('*') << setw(35) << outputText << "\n";
    cout << setfill(' '); // Reset fill character

    // Legacy C-API Interoperability
    cout << "  - C-API Compatibility (`.c_str()`): " << outputText.c_str() << "\n";

    // =====================================================================================
    // 4. IN-MEMORY STRING STREAMS (`std::stringstream`)
    // =====================================================================================
    cout << "\n================ 4. IN-MEMORY STRING STREAMS (`std::stringstream`) ================\n";

    // Formatting mixed data types into a single string (ostringstream behavior)
    stringstream formattedStream;
    string systemName = "ClusterNode";
    int nodeId = 402;
    double loadFactor = 98.65;

    formattedStream << "System: " << systemName << " | ID: " << nodeId << " | Load: " << fixed << setprecision(2) << loadFactor << "%";
    string compositeString = formattedStream.str();

    cout << "  - Formatted Stream Composition     : \"" << compositeString << "\"\n";

    // Parsing structured values out of a string (istringstream behavior)
    string rawData = "Latency 15 89.4";
    stringstream parseStream(rawData);

    string metricName;
    int metricCount;
    double metricValue;

    parseStream >> metricName >> metricCount >> metricValue;
    cout << "  - Parsed Stream Breakdown          : Metric=" << metricName 
         << " | Count=" << metricCount 
         << " | Value=" << metricValue << "\n";

    // =====================================================================================
    // SUMMARY COMPARISON TABLE
    // =====================================================================================
    cout << "\n================ STRING I/O MECHANICS SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Mechanism / Function  | Input Trait / Stop Condition      | Primary Use Case                  |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| `cin >> str`          | Stops at whitespace (' ', '\\t', '\\n')| Reading single words or tokens    |\n"
         << "| `getline(cin, str)`   | Stops at newline ('\\n')            | Reading full lines with spaces    |\n"
         << "| `getline(cin, s, d)`  | Stops at custom delimiter `d`     | CSV and token parsing             |\n"
         << "| `cin.ignore()`        | Flushes remaining buffer bytes    | Resolving newline bugs after `cin`|\n"
         << "| `std::stringstream`   | In-memory formatting / parsing    | Converting numbers <-> strings    |\n"
         << "| `cout.write(ptr, N)`  | Writes exact N bytes without check| Binary string/raw buffer output   |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}