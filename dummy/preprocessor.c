// Single-line comment at top

/* Multi-line
   comment at top */

/* Nested-style (not actually nested in C)
   still part of the original comment
*/

#pragma once // Prevent multiple inclusion

#pragma GCC diagnostic push /* Pushing diagnistic thing */
#pragma GCC diagnostic ignored "-Wunused-variable"/* Ignoring
some
wawrning */

#pragma GCC diagnostic pop /*
pop
*/

#pragma pack(push, 1)   
#pragma pack(pop)

#include <stdio.h>
#include <stdint.h>
#include "my_header.h"

// #include "commented_out.h" (should be ignored by preprocessor)

/**************
 * Decorative comment
 **************/

#define FEATURE_ENABLED
#define ENABLE_LOGGING

#undef ENABLE_LOGGING

#ifdef FEATURE_ENABLED
  // This block is active
#else
  // This block is inactive
#endif

#ifndef DEBUG
  // DEBUG not defined
#endif

#if 1
  // Constant expression true
#endif

#if 0
  // Constant expression false
#elif 1
  // Alternative true branch
#else
  // Should be skipped
#endif

#line 100 "virtual_file.c"

#warning "This is a test warning"
#error "This is a test error"

/* End of file multi-line comment */
