// This is a line comment
/*
   This is a block comment
*/

#include <stdio.h>
#define PI 3.14
#ifdef DEBUG
  #define LOG(x) printf(x)
#else
  #define LOG(x)
#endif

#pragma once

