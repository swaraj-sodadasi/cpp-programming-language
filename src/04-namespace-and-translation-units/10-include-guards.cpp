/*
 * =====================================================================================
 * CONCEPT        : Include Guards (Header Guards) in C++
 * DESCRIPTION    : Comprehensive implementation detailing Include Guards:
 *                  1. The Double Inclusion Problem : Including the same header multiple times
 *                                                   in a translation unit leads to 
 *                                                   redefinition / ODR (One Definition Rule) errors.
 *                  2. Traditional Include Guards   : `#ifndef HEADER_NAME_H`
 *                                                   `#define HEADER_NAME_H`
 *                                                   ... declarations ...
 *                                                   `#endif`
 *                  3. Modern Alternative          : `#pragma once` (compiler directive).
 *                  4. How Preprocessor Works      : Macro checks evaluate before compilation;
 *                                                   subsequent includes are skipped.
 *                  5. Best Naming Conventions     : Unique macro names based on file paths/namespaces
 *                                                   to prevent guard collisions across projects.
 *
 * TIME COMPLEXITY  : Best Case: O(1) runtime; compile-time preprocessing overhead.
 * SPACE COMPLEXITY : Best Case: O(1) stack allocation.
 * =====================================================================================
 */

#include <iostream>
#include <string>

using namespace std;

// =====================================================================================
// 1. SIMULATED HEADER 1: TRADITIONAL INCLUDE GUARD (#ifndef / #define / #endif)
// In a real project, this entire block lives in "DatabaseConfig.h".
// =====================================================================================

#ifndef DATABASE_CONFIG_H
#define DATABASE_CONFIG_H

struct DatabaseConfig {
    string serverAddress;
    int port;
    int timeoutSec;

    [[nodiscard]] string getConnectionString() const {
        return serverAddress + ":" + to_string(port) + "?timeout=" + to_string(timeoutSec) + "s";
    }
};

void printDatabaseConfig(const DatabaseConfig& config);

#endif // DATABASE_CONFIG_H

// -------------------------------------------------------------------------------------
// SIMULATED DUPLICATE INCLUSION OF "DatabaseConfig.h"
// In a real project, this happens when Header A includes Header B, and main.cpp includes both.
// Thanks to the include guard above, the preprocessor skips this second block completely!
// -------------------------------------------------------------------------------------
#ifndef DATABASE_CONFIG_H
#define DATABASE_CONFIG_H

// If include guards were missing, defining 'DatabaseConfig' again here would cause a
// COMPILER FATAL ERROR: redefinition of 'struct DatabaseConfig'
struct DatabaseConfig {
    string serverAddress;
    int port;
};

#endif // DATABASE_CONFIG_H


// =====================================================================================
// 2. SIMULATED HEADER 2: MODERN COMPILER DIRECTIVE (#pragma once)
// In a real project, `#pragma once` is placed at the very first line of a header file.
// =====================================================================================

// #pragma once  // (Uncommented in actual standalone header files)

struct ApplicationSettings {
    string appName;
    int maxThreads;
    bool debugMode;
};


// =====================================================================================
// FUNCTION DEFINITIONS (Simulating corresponding .cpp implementation)
// =====================================================================================

void printDatabaseConfig(const DatabaseConfig& config) {
    cout << "  - [DatabaseConfig] Connection String: " << config.getConnectionString() << "\n";
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    string serverInput;
    int portInput = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter database server address (e.g., db.production.internal): " << flush;
    if (!(cin >> serverInput) || serverInput.empty()) {
        serverInput = "db.production.internal";
    }

    cout << "Enter database port number (e.g., 5432): " << flush;
    if (!(cin >> portInput) || portInput <= 0) {
        cout << "Invalid port. Defaulting port to 5432." << endl;
        portInput = 5432;
    }

    // 1. DEMONSTRATING GUARDED STRUCT USAGE
    cout << "\n================ 1. TRADITIONAL INCLUDE GUARD DEMO ================" << endl;
    
    DatabaseConfig dbConfig{serverInput, portInput, 30};
    printDatabaseConfig(dbConfig);

    cout << "  - Status: Header included multiple times without compiler redefinition errors!\n";

    // 2. DEMONSTRATING MODERN #pragma once USAGE
    cout << "\n================ 2. MODERN #pragma once DEMO ================" << endl;
    
    ApplicationSettings appSettings{"DataProcessorApp", 8, true};
    cout << "  - Application Name : " << appSettings.appName << "\n";
    cout << "  - Max Thread Count : " << appSettings.maxThreads << "\n";
    cout << "  - Debug Mode       : " << (appSettings.debugMode ? "ENABLED" : "DISABLED") << "\n";

    // 3. STEP-BY-STEP EXPLANATION OF PREPROCESSOR MECHANICS
    cout << "\n================ 3. HOW INCLUDE GUARDS WORK ================" << endl;
    cout << "  1. First Inclusion  : Preprocessor checks `#ifndef DATABASE_CONFIG_H`.\n"
         << "                        Macro is NOT defined yet, so execution enters the block.\n";
    cout << "  2. Macro Definition : `#define DATABASE_CONFIG_H` registers the guard symbol.\n";
    cout << "  3. Code Expansion   : Structs/classes/prototypes are expanded into the TU.\n";
    cout << "  4. Second Inclusion : Preprocessor checks `#ifndef DATABASE_CONFIG_H` again.\n"
         << "                        Macro IS defined, so preprocessor skips to `#endif`.\n";

    // 4. COMPARISON & BEST PRACTICES
    cout << "\n================ INCLUDE GUARDS BEST PRACTICES ================" << endl;
    cout << "1. Naming Standard   : Use unique names (e.g., `PROJECT_MODULE_FILENAME_H`).\n";
    cout << "2. Guard Every Header: Every `.h` / `.hpp` file MUST have an include guard.\n";
    cout << "3. `#pragma once`    : Widely supported, cleaner (1 line), and reduces typo bugs.\n";
    cout << "4. Combined Guards   : Some projects use BOTH `#pragma once` AND `#ifndef` guards\n"
         << "                       for maximum portability across ancient or obscure compilers.\n";

    return 0;
}