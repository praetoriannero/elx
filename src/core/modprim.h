/**
 * @file modprim.h
 * @brief Primitive type aggregation header.
 *
 * Includes and re-exports all fundamental type definitions from
 * modfloat.h, modint.h, and modsize.h. This header should be used
 * as the single entry point for primitive type imports.
 */

#pragma once

// IWYU pragma: begin_exports
#include "modfloat.h"
#include "modint.h"
#include "modsize.h"
// IWYU pragma: end_exports
