/**
 * @file logger.h
 * @brief Logging and debugging utilities.
 *
 * Provides file-aware logging and debug functions that record the source
 * file and line number for each message. Intended for compiler diagnostics
 * and development-time tracing.
 */

#pragma once

#include <stdarg.h>

/**
 * @brief Log a message with source location metadata.
 *
 * Records the calling file and line number alongside the formatted
 * message. Use the log() macro instead of calling this directly.
 *
 * @param file  Source file name (__FILE__).
 * @param line  Source line number (__LINE__).
 * @param fmt   printf-style format string.
 */
void log_impl(const char* file, int line, const char* fmt, ...);

/**
 * @brief Log a debug message with source location metadata.
 *
 * Similar to log_impl() but intended for debug-level output that may
 * be conditionally compiled out. Use the debug() macro instead of
 * calling this directly.
 *
 * @param file  Source file name (__FILE__).
 * @param line  Source line number (__LINE__).
 * @param fmt   printf-style format string.
 */
void debug_impl(const char* file, int line, const char* fmt, ...);

/**
 * @brief Log a message, automatically capturing the source file and line.
 *
 * Equivalent to calling log_impl(__FILE__, __LINE__, fmt, ...) with
 * the current file and line filled in by the preprocessor.
 */
#define log(...) log_impl(__FILE__, __LINE__, __VA_ARGS__)

/**
 * @brief Log a debug message, automatically capturing the source file and line.
 *
 * Equivalent to calling debug_impl(__FILE__, __LINE__, fmt, ...) with
 * the current file and line filled in by the preprocessor.
 */
#define debug(...) debug_impl(__FILE__, __LINE__, __VA_ARGS__)
