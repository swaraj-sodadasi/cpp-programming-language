/*
 * =====================================================================================
 * CONCEPT        : Header Inclusion in C++ (#include, Header Guards, Pragma Once, Forward Decl)
 * DESCRIPTION    : Comprehensive code demonstrating header inclusion mechanics:
 *                  1. Inclusion Directives      : `<header>` (system search path) vs
 *                                                 `"header.h"` (local directory search path).
 *                  2. Multiple Inclusion Hazard : Including the same header multiple times
 *                                                 causes redefinition (ODR) errors.
 *                  3. Protection Mechanisms     : Traditional `#ifndef` header guards vs
 *                                                 modern `#pragma once`.
 *                  4. Forward Declarations      : Reducing `#include` overhead in headers
 *                                                 to speed up compilation and break cycles.
 *                  5. Inclusion Order Best Prac : Standard -> 3rd Party -> Local headers.
 *
 * TIME COMPLEXITY  : Best Case: O(1) runtime; compile-time cost reduced via guards/forward decls.
 * SPACE COMPLEXITY : Best Case: O(1) stack allocation.
 * =====================================================================================
 */

#include <iostream>
#include <string>

using namespace std;

// =====================================================================================
// 1. SIMULATED HEADER WITH TRADITIONAL HEADER GUARDS (#ifndef / #define / #endif)
// Prevents duplicate declaration errors if included multiple times across translation units.
// =====================================================================================

#ifndef SIMULATED_LOGGER_HEADER_H
#define SIMULATED_LOGGER_HEADER_H

class SystemLogger {
public:
    static void logInfo(const string& message) {
        cout << "  - [SystemLogger] INFO: " << message << endl;
    }
};

#endif // SIMULATED_LOGGER_HEADER_H

// Intentionally testing guard resilience: Re-including the same guard block does nothing!
#ifndef SIMULATED_LOGGER_HEADER_H
#define SIMULATED_LOGGER_HEADER_H
// This section is skipped by the preprocessor because SIMULATED_LOGGER_HEADER_H is defined.
#endif

// =====================================================================================
// 2. SIMULATED HEADER WITH `#pragma once`
// Non-standard but universally supported compiler directive to prevent duplicate inclusion.
// =====================================================================================

// #pragma once // In actual header files, placed at the top line

struct UserProfile {
    int userId;
    string userName;
};

// =====================================================================================
// 3. FORWARD DECLARATION TECHNIQUE (REDUCING HEADER INCLUSION OVERHEAD)
// Instead of including heavy headers inside another header, use forward declarations!
// =====================================================================================

// Forward declaration of class (No #include needed if using pointers or references)
class DatabaseConnection;

class UserDataService {
private:
    // Pointer requires only forward declaration (size of pointer is fixed)
    DatabaseConnection* dbConnPtr_; 

public:
    UserDataService();
    ~UserDataService();

    void fetchUser(int id, const DatabaseConnection& conn);
};

// Actual class definition (In real code, this would be in a separate header or .cpp)
class DatabaseConnection {
public:
    void connect() const {
        cout << "  - [DatabaseConnection] Connected to SQL Database successfully.\n";
    }
};

UserDataService::UserDataService() : dbConnPtr_(nullptr) {}
UserDataService::~UserDataService() = default;

void UserDataService::fetchUser(int id, const DatabaseConnection& conn) {
    conn.connect();
    cout << "  - [UserDataService] Fetched profile data for User ID: " << id << endl;
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userIdInput = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a User ID for inclusion demo (e.g., 101): " << flush;
    if (!(cin >> userIdInput) || userIdInput <= 0) {
        cout << "Invalid User ID. Defaulting to 101." << endl;
        userIdInput = 101;
    }

    // 1. ANGLE BRACKETS VS DOUBLE QUOTES
    cout << "\n================ 1. ANGLE BRACKETS VS DOUBLE QUOTES ================" << endl;
    cout << "  - `#include <header>`   : Searches compiler's system include directories first.\n"
         << "                            Used for standard library (<iostream>, <vector>).\n";
    cout << "  - `#include \"header.h\"` : Searches local source directory first, then system paths.\n"
         << "                            Used for project-specific custom headers.\n";

    // 2. HEADER GUARDS DEMONSTRATION
    cout << "\n================ 2. HEADER GUARDS (#ifndef & #pragma once) ================" << endl;
    SystemLogger::logInfo("Header inclusion guards prevented duplicate symbol errors.");
    cout << "  - `#ifndef / #define / #endif` : Standard portable preprocessor guard mechanism.\n";
    cout << "  - `#pragma once`               : Compiler directive offering cleaner single-line protection.\n";

    // 3. FORWARD DECLARATIONS DEMONSTRATION
    cout << "\n================ 3. FORWARD DECLARATION REDUCING INCLUSION ================" << endl;
    DatabaseConnection dbConn;
    UserDataService userService;

    userService.fetchUser(userIdInput, dbConn);
    cout << "  - Insight: Using forward declarations (`class X;`) in headers reduces `#include` dependencies,\n"
         << "             decreases compilation time, and prevents circular inclusion loops.\n";

    // 4. BEST PRACTICES FOR INCLUSION ORDER
    cout << "\n================ 4. BEST PRACTICES FOR INCLUSION ORDER ================" << endl;
    cout << "  Standard Order in .cpp files:\n";
    cout << "  1. Matching Header      (e.g., `#include \"MyClass.h\"`) -> Tests header self-sufficiency.\n";
    cout << "  2. C Standard Libraries (e.g., `<cstdio>`, `<cmath>`)\n";
    cout << "  3. C++ Standard Libs    (e.g., `<iostream>`, `<vector>`, `<string>`)\n";
    cout << "  4. 3rd Party Libraries  (e.g., `<boost/asio.hpp>`)\n";
    cout << "  5. Local Project Headers(e.g., `\"Utils/Logger.h\"`)\n";

    cout << "\n================ HEADER INCLUSION SUMMARY ================" << endl;
    cout << "1. Use Header Guards : Always protect headers with `#pragma once` or `#ifndef` guards." << endl;
    cout << "2. Minimal Inclusion : Include only necessary headers inside `.h` files; push includes to `.cpp`." << endl;
    cout << "3. Forward Declare   : Use forward declarations when using pointers or references in class headers." << endl;
    cout << "4. Self-Contained    : Every header file must compile independently without requiring caller pre-includes." << endl;

    return 0;
}