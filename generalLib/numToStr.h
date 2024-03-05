/*########################################################
# Name: numToStr
#   - Has function(s) to convert a number to a string
# Libraries:
# C Standard Libraries:
########################################################*/

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
' SOF: Start Of File
'   o header:
'     - Included libraries (none) and definitions (none)
'   o fun-01: numToStr
'      - Converts a number to a c-string
'   o fun-02: backNumToStr
'     - Converts a number to a c-string, but keeps the
'       number backwards
\~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*-------------------------------------------------------\
| Header:
|   - Included libraries (none) and definitions (none)
\-------------------------------------------------------*/

#ifndef NUMBER_TO_CSTRING
#define NUMBER_TO_CSTRING

/*-------------------------------------------------------\
| Fun-01: numToStr
|   - Converts a number to a c-string
| Input:
|   - cstr:
|     o C-string to hold the converted number
|   - numUL:
|     o Number to convert
| Output:
|   - Modifies:
|     o cstr to have the number (adds a '\0' at the end)
|   - Returns:
|     o Number of characters copied to cstr
\-------------------------------------------------------*/
#define numToStr(cstr, numUL)({\
   unsigned long macNumUL;\
   int macNumDigI = 0;\
   int macSwapI = 0;\
   int macStartI = 0;\
   int macRetI = 0;\
   int macMidI = 0;\
   \
   for(macNumUL=(numUL); macNumUL; macNumUL /= 10)\
   { /*Loop: Copy number to c-string*/\
      (cstr)[macNumDigI] = (macNumUL % 10) + 48;\
      ++macNumDigI;\
   } /*Loop: Copy number to c-string*/\
    \
   (cstr)[macNumDigI] = '\0';\
   macRetI = macNumDigI;\
   \
   macMidI = macRetI >> 1;\
   macStartI = 0;\
   \
   if(!(macNumDigI & 1)) \
   { /*If: I only have two digits*/\
      macSwapI = (cstr)[macMidI];\
      (cstr)[macMidI] = (cstr)[macMidI - 1];\
      (cstr)[macMidI - 1] = macSwapI;\
   } /*If: I only have two digits*/\
   \
   for(\
      macNumDigI = macNumDigI - 1;\
      macNumDigI > macMidI;\
     --macNumDigI\
   ){ /*Loop: Swap digits around*/\
      macSwapI = (cstr)[macStartI];\
      (cstr)[macStartI] = (cstr)[macNumDigI];\
      (cstr)[macNumDigI] = macSwapI;\
      ++macStartI;\
   } /*Loop: Swap digits around*/\
   \
   \
   macRetI;\
}) /*numToStr*/

/*-------------------------------------------------------\
| Fun-02: backNumToStr
|   - Converts a number to a c-string, but keeps the
|     number backwards
| Input:
|   - cstr:
|     o C-string to hold the converted number
|   - numUL:
|     o Number to convert
| Output:
|   - Modifies:
|     o cstr to have the backwards number (adds a '\0' at
|       the end)
|   - Returns:
|     o Number of characters copied to cstr
\-------------------------------------------------------*/
#define backNumToStr(cstr, numUL)({\
   unsigned long macNumUL;\
   int macNumDigI = 0;\
   \
   for(macNumUL=(numUL); macNumUL; macNumUL /= 10)\
   { /*Loop: Copy number to c-string*/\
      (cstr)[macNumDigI] = (macNumUL % 10) + 48;\
      ++macNumDigI;\
   } /*Loop: Copy number to c-string*/\
    \
   (cstr)[macNumDigI] = '\0';\
   macNumDigI;\
}) /*backNumToStr*/

#endif
