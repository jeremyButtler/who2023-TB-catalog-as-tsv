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
'   o fun-01: noBranchHiBit2
'     - Does a branchles method to find the higest bit in
'       the element (integer log base 2)
'   o fun-02: noBranchAB
'     - Does a branchless absolute value
'   o fun-03: noBranchMax
'     - Find the maximum value (branchless)
'   o fun-04: noBranchIfMax
'     - Set a value (ret) to a value based on which value
'       is greater.
'   o fun-05: noBranchMin
'     - Find the Minimum value (branchless)
'   o fun-06: noBranchIfMin
'     - Set a value (ret) to a value based on which value
'       is less than.
\~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*-------------------------------------------------------\
| Header:
|  - Inluded libraries and defintions/global variables
\-------------------------------------------------------*/

#ifndef GENERAL_MATH_FUNCTIONS_H
#define GENERAL_MATH_FUNCTIONS_H

/*-------------------------------------------------------\
| Fun-01: noBranchHiBit2
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
| Fun-02: noBranchAB
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
| Fun-03: noBranchMax
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
| Fun-04: noBranchIfMax
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
| Fun-05: noBranchMin
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
| Fun-06: noBranchIfMin
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
