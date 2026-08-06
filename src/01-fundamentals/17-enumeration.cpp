/*
 * =====================================================================================
 * CONCEPT        : Enumerations in C++ (Unscoped enum vs Scoped enum class, Underlying Types, Bitmask Flags)
 * DESCRIPTION    : Comprehensive implementation explaining modern C++ enumerations:
 *                  1. Unscoped Enums (enum)     : Legacy C-style enumerations with implicit integer 
 *                                                 conversions and global namespace scope pollution.
 *                  2. Scoped Enums (enum class) : Strongly-typed, strongly-scoped enumerations requiring 
 *                                                 explicit scope access and casting.
 *                  3. Custom Underlying Types  : Specifying exact integer storage size (e.g., uint8_t 
 *                                                 for 1-byte memory optimization).
 *                  4. Bitmask Enum Flags       : Overloading bitwise operators (| , &) for scoped enums 
 *                                                 to manage combined permission/state flags.
 *                  5. Dynamic Input Mapping    : Converting runtime user input safely to enum instances.
 *
 * TIME COMPLEXITY  : Best Case: O(1) — Constant-time dynamic enum conversion, switch-case evaluation, 
 *                    and bitwise flag operations.
 * SPACE COMPLEXITY : Best Case: O(1) — Memory footprint explicitly optimized to uint8_t (1 byte per enum).
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <cstdint>
#include <type_traits>

using namespace std;

// 1. UNSCOPED ENUM (LEGACY C-STYLE)
// Leaks enumerator names into enclosing scope; implicitly converts to int
enum LegacyColor {
    RED,   // 0
    GREEN, // 1
    BLUE   // 2
};

// 2. SCOPED ENUM CLASS WITH CUSTOM UNDERLYING STORAGE TYPE (1 BYTE)
// Strong typing prevents implicit conversion to int and isolates scope
enum class ProcessStatus : uint8_t {
    Pending    = 1,
    Processing = 2,
    Completed  = 3,
    Failed     = 4
};

// 3. BITMASK SCOPED ENUM CLASS FOR PERMISSION FLAGS
enum class PermissionFlags : uint8_t {
    None    = 0,
    Read    = 1 << 0, // 0b00000001 (1)
    Write   = 1 << 1, // 0b00000010 (2)
    Execute = 1 << 2  // 0b00000100 (4)
};

// BITWISE OPERATOR OVERLOADS FOR SCOPED BITMASK ENUM
constexpr PermissionFlags operator|(PermissionFlags lhs, PermissionFlags rhs) noexcept {
    using UnderlyingType = underlying_type_t<PermissionFlags>;
    return static_cast<PermissionFlags>(
        static_cast<UnderlyingType>(lhs) | static_cast<UnderlyingType>(rhs)
    );
}

constexpr PermissionFlags operator&(PermissionFlags lhs, PermissionFlags rhs) noexcept {
    using UnderlyingType = underlying_type_t<PermissionFlags>;
    return static_cast<PermissionFlags>(
        static_cast<UnderlyingType>(lhs) & static_cast<UnderlyingType>(rhs)
    );
}

/**
 * @brief Helper function to check if a specific flag is set in a bitmask.
 */
constexpr bool hasPermission(PermissionFlags mask, PermissionFlags flag) noexcept {
    return (mask & flag) == flag;
}

/**
 * @brief Converts ProcessStatus enum instance to human-readable string representation.
 */
string statusToString(ProcessStatus status) {
    switch (status) {
        case ProcessStatus::Pending:    return "Pending";
        case ProcessStatus::Processing: return "Processing";
        case ProcessStatus::Completed:  return "Completed";
        case ProcessStatus::Failed:     return "Failed";
        default:                        return "Unknown Status";
    }
}

int main() {
    int rawStatusChoice = 0;
    int rawPermissionChoice = 0;

    // 4. DYNAMIC INPUT COLLECTION WITH STREAM FLUSHING
    cout << "Select Process Status [1: Pending, 2: Processing, 3: Completed, 4: Failed]: " << flush;
    if (!(cin >> rawStatusChoice) || rawStatusChoice < 1 || rawStatusChoice > 4) {
        cout << "Invalid status choice provided. Program terminated." << endl;
        return 0;
    }

    cout << "Enter Permission Combination Bitmask [1: Read, 2: Write, 3: Read+Write, 7: Read+Write+Exec]: " << flush;
    if (!(cin >> rawPermissionChoice) || rawPermissionChoice < 0 || rawPermissionChoice > 7) {
        cout << "Invalid permission bitmask input. Program terminated." << endl;
        return 0;
    }

    // 5. UNSCOPED ENUM DEMONSTRATION
    cout << "\n================ 1. UNSCOPED ENUM (LEGACY) ================" << endl;
    LegacyColor color = GREEN;
    // Implicit integer conversion occurs naturally with unscoped enums
    int colorVal = color; 
    cout << "Legacy Unscoped Enum Value (GREEN) : " << colorVal << endl;
    cout << "Memory Size (Default int)          : " << sizeof(LegacyColor) << " Bytes" << endl;

    // 6. SCOPED ENUM CLASS DEMONSTRATION
    cout << "\n================ 2. SCOPED ENUM CLASS (MODERN) ================" << endl;
    // Converting dynamic runtime integer to scoped enum via explicit static_cast
    auto status = static_cast<ProcessStatus>(rawStatusChoice);

    cout << "Dynamic Status Selected          : " << statusToString(status) << endl;
    cout << "Underlying Value (Explicit Cast) : " << static_cast<int>(status) << endl;
    cout << "Memory Size (uint8_t optimized)   : " << sizeof(ProcessStatus) << " Byte(s)" << endl;

    // 7. BITMASK SCOPED ENUM DEMONSTRATION
    cout << "\n================ 3. SCOPED ENUM BITMASK FLAGS ================" << endl;
    auto userPermissions = static_cast<PermissionFlags>(rawPermissionChoice);

    cout << "Active Permission Flags (Raw Bitmask Value: " << static_cast<int>(userPermissions) << "):" << endl;
    cout << "  |- Read Permission    : " << (hasPermission(userPermissions, PermissionFlags::Read) ? "GRANTED" : "DENIED") << endl;
    cout << "  |- Write Permission   : " << (hasPermission(userPermissions, PermissionFlags::Write) ? "GRANTED" : "DENIED") << endl;
    cout << "  |- Execute Permission : " << (hasPermission(userPermissions, PermissionFlags::Execute) ? "GRANTED" : "DENIED") << endl;

    return 0;
}