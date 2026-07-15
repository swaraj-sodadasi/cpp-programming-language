/**
 * @file common.hpp
 * @brief Global utilities, benchmarking tools, and structural configurations 
 *        shared across all learning modules.
 */

#pragma once // Standard preprocessor directive to prevent duplicate inclusion errors

// Standard Library Inclusions
#include <iostream>
#include <chrono>
#include <string>
#include <string_view>
#include <cstdint>

// ============================================================================
// 1. Enterprise Standard Type Aliases (Fixed-width primitives)
// ============================================================================
using u8   = std::uint8_t;
using u16  = std::uint16_t;
using u32  = std::uint32_t;
using u64  = std::uint64_t;

using i8   = std::int8_t;
using i16  = std::int16_t;
using i32  = std::int32_t;
using i64  = std::int64_t;

using f32  = float;
using f64  = double;

// ============================================================================
// 2. High-Performance Compiler Branch Predictors (C++20 Attribute Wrappers)
// ============================================================================
#define likely(x)   (x) [[likely]]
#define unlikely(x) (x) [[unlikely]]

namespace cpp_learning {

    // ============================================================================
    // 3. High-Precision RAII Microsecond Benchmarking Engine
    // ============================================================================
    /**
     * @brief Scoped timer utilizing Resource Acquisition Is Initialization.
     *        Instantiate this at the start of a block to benchmark execution velocity.
     */
    class ScopedTimer {
    public:
        explicit ScopedTimer(std::string_view operational_name)
            : m_name(operational_name), 
              m_start_time(std::chrono::high_resolution_clock::now()) {
            std::cout << "[TIMER START] Execution tracking initialized for: " << m_name << "\n";
        }

        ~ScopedTimer() {
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - m_start_time).count();
            
            std::cout << "[TIMER END] " << m_name << " completed execution in: " 
                      << f64(duration) / 1000.0 << " ms (" << duration << " us)\n";
        }

        // Prevent copying and moving to ensure metrics integrity
        ScopedTimer(const ScopedTimer&) = delete;
        ScopedTimer& operator=(const ScopedTimer&) = delete;
        ScopedTimer(ScopedTimer&&) = delete;
        ScopedTimer& operator=(ScopedTimer&&) = delete;

    private:
        std::string m_name;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_start_time;
    };

    // ============================================================================
    // 4. Thread-Safe Clean Printing Utilities
    // ============================================================================
    /**
     * @brief Formatted terminal logging helper to standardize console outputs.
     */
    inline void log_status(std::string_view prefix, std::string_view message) {
        std::cout << "[" << prefix << "] " << message << "\n";
    }

} // namespace cpp_learning