/*########################################################
# Name: genMath.h
#   - Has my math functions I use. These are often
#     branchless (noBrach) variations.
# Lbraries:
# C Stanard Libraries:
# Note:
#   Most of the branchless code is from or modified from:
#   - https://graphics.stanford.edu/~seander/bithacks.html
########################################################*/

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
' SOF: Start Of File
'   o header:
'     - Inluded libraries and defintions/global variables
'   o fun01: noBranchHiBit2
'     - Does a branchles method to find the higest bit in
'       the element (integer log base 2)
'   o fun02: noBranchAB
'     - Does a branchless absolute value
'   o fun03: noBranchMax
'     - Find the maximum value (branchless)
'   o fun04: noBranchIfMax
'     - Set a value (ret) to a value based on which value
'       is greater.
'   o fun05: noBranchMin
'     - Find the Minimum value (branchless)
'   o fun06: noBranchIfMin
'     - Set a value (ret) to a value based on which value
'       is less than.
'   o license:
'     - Licensing for this code (public domain / mit)
\~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*-------------------------------------------------------\
| Header:
|  - Inluded libraries and defintions/global variables
\-------------------------------------------------------*/

#ifndef GENERAL_MATH_FUNCTIONS_H
#define GENERAL_MATH_FUNCTIONS_H

/*-------------------------------------------------------\
| Fun01: noBranchHiBit2
|   - Does a branchles method to find the higest bit in
|     the element (integer log base 2)
| Input:
|   - numI:
|     o Integer (non-long) number to get the highest bit
|       for
| Output:
|   - Returns:
|     o the position of the highest bit
\-------------------------------------------------------*/
#define noBanchHiBit2(numI)({\
   unsigned int tmpNumUI = (uint) (numI);\
   unsigned int retUI = 0;\
   unsigned int shiftUI = 0;\
   \
   retUI = (tmpNumUI > 0xFFFF) << 4;\
   tmpNumUI >>= retUI;\
   \
   shiftUI = (tmpNumUI > 0xFF) << 3;\
   tmpNumUI >>= shiftUI;\
   retUI |= shiftUI;\
   \
   shiftUI = (tmpNumUI > 0xF) << 2;\
   tmpNumUI >>= shiftUI;\
   retUI |= shiftUI;\
   \
   shiftUI = (tmpNumUI > 0x3) << 1;\
   tmpNumUI >>= shiftUI;\
   retUI |= shiftUI;\
   \
   retUI | (tmpNumUI >> 1);\
})

/*-------------------------------------------------------\
| Fun02: noBranchAB
|   - Does a branchless absolute value
| Input:
|   - numI:
|     o Integer (non-long) to get the absolute value to
| Output:
|   - Returns:
|     o The absolute value of numI
\-------------------------------------------------------*/
#define noBranchAB(numI)({\
   int maskI = (numI) >> (sizeof(int) * 8 - 1);\
   ((numI) + maskI) ^ maskI;\
})


/*-------------------------------------------------------\
| Fun03: noBranchMax
|  - Find the maximum value (branchless)
| Input:
|  o ret:
|    - Value to hold the maximum value
|  o x:
|    - First value to compare, ret is set to x if x >= y
|  o y:
|    - second value to compare, ret is set to y if y > x
| Output:
|  - Sets:
|    - Sets ret to x if x is greater than or equal to y
|    - Sets ret to y if y is greather than x
\-------------------------------------------------------*/
#define noBranchMax(x, y)(\
   (x) ^ (((x) ^ (y)) & (-((x) < (y))))\
   /*Logic:
   `  x < y:
   `    if x < y I get 0 (x > y); else I get 1
   `  -(x < y):
   `    If x < y I get -1 (111...)
   `    If x >= y I get 0
   `  x ^ ((x ^ y) & 111...): (only when x < y)
   `    This gives me x
   `  x ^ (x ^ y) & 0: (only when x >= y)
   `    This gives me y
   */\
)

/*-------------------------------------------------------\
| Fun04: noBranchIfMax
|  - Set a value (ret) to a value based on which value
|    is greater.
| Input:
|  o ret:
|    - This will hold the return value
|  o x:
|    - First value to compare, (if x >= y)
|  o y:
|    - second value to compare, (if y > x)
|  o xRet:
|    - Value to set ret of x is >= y
|  o yRet:
|    - Value to set ret of y is > x
| Output:
|  - Sets:
|    - ret to xRet if x is greater than or equal to y
|    - ret to yRet if y is greater than x
\-------------------------------------------------------*/
#define noBrachIfMax(x, y, xRet, yRet)(\
   (xRet) ^ (((xRet) ^ (yRet)) & (-((x) < (y))))\
   /*This follows the same logic as macroMax(ret, x, y),
   ' except instead of setting ret to the highest value, I
   ' set ret to xRet if x is >= to y or yRet if y > x.
   */ \
)


/*-------------------------------------------------------\
| Fun05: noBranchMin
|  - Find the Minimum value (branchless)
| Input:
|  o ret:
|    - Value to hold the maximum value
|  o x:
|    - First value to compare, ret is set to x if x <= y
|  o y:
|    - second value to compare, ret is set to y if y < x
| Output:
|  - Sets:
|    - Sets ret to x if x is less than or equal to y
|    - Sets ret to y if y is less than x
\-------------------------------------------------------*/
#define noBranchMin(x, y)(\
   (x) ^ (((x) ^ (y)) & (-((x) > (y))))\
   /*Logic:
   `  x < y:
   `    if x > y I get 1; else I get 0
   `  -(x > y):
   `    If x > y I get -1 (111...)
   `    If x <= y I get 0
   `  x ^ ((x ^ y) & 111...): (only when x > y)
   `    This gives me x
   `  x ^ (x ^ y) & 0: (only when x <= y)
   `    This gives me y
   */\
)

/*-------------------------------------------------------\
| Fun06: noBranchIfMin
|  - Set a value (ret) to a value based on which value
|    is less than.
| Input:
|  o ret:
|    - This will hold the return value
|  o x:
|    - First value to compare, (if x <= y)
|  o y:
|    - second value to compare, (if y < x)
|  o xRet:
|    - Value to set ret of x is <= y
|  o yRet:
|    - Value to set ret of y is < x
| Output:
|  - Sets:
|    - ret to xRet if x is less than or equal to y
|    - ret to yRet if y is less than x
\-------------------------------------------------------*/
#define noBrachIfMin(x, y, xRet, yRet)(\
   (xRet) ^ (((xRet) ^ (yRet)) & (-((x) > (y))))\
   /*This follows the same logic as macroMax(ret, x, y),
   ' except instead of setting ret to the highest value, I
   ' set ret to xRet if x is <= to y or yRet if y < x.
   */ \
)

#endif

/*=======================================================\
: License:
: 
: This code is under the unlicense (public domain).
:   However, for cases were the public domain is not
:   suitable, such as countries that do not respect the
:   public domain or were working with the public domain
:   is inconvient / not possible, this code is under the
:   MIT license.
: 
: Public domain:
: 
: This is free and unencumbered software released into the
:   public domain.
: 
: Anyone is free to copy, modify, publish, use, compile,
:   sell, or distribute this software, either in source
:   code form or as a compiled binary, for any purpose,
:   commercial or non-commercial, and by any means.
: 
: In jurisdictions that recognize copyright laws, the
:   author or authors of this software dedicate any and
:   all copyright interest in the software to the public
:   domain. We make this dedication for the benefit of the
:   public at large and to the detriment of our heirs and
:   successors. We intend this dedication to be an overt
:   act of relinquishment in perpetuity of all present and
:   future rights to this software under copyright law.
: 
: THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
:   ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
:   LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
:   FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO
:   EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM,
:   DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
:   CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
:   IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
:   DEALINGS IN THE SOFTWARE.
: 
: For more information, please refer to
:   <https://unlicense.org>
: 
: MIT License:
: 
: Copyright (c) 2024 jeremyButtler
: 
: Permission is hereby granted, free of charge, to any
:   person obtaining a copy of this software and
:   associated documentation files (the "Software"), to
:   deal in the Software without restriction, including
:   without limitation the rights to use, copy, modify,
:   merge, publish, distribute, sublicense, and/or sell
:   copies of the Software, and to permit persons to whom
:   the Software is furnished to do so, subject to the
:   following conditions:
: 
: The above copyright notice and this permission notice
:   shall be included in all copies or substantial
:   portions of the Software.
: 
: THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
:   ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
:   LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
:   FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO
:   EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
:   FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
:   AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
:   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
:   USE OR OTHER DEALINGS IN THE SOFTWARE.
\=======================================================*/
