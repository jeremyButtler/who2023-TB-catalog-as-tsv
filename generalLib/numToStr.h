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
   unsigned long macNumUL = (numUL);\
   int macRetI = 0;\
   char *startMacStr = 0;\
   char *endMacStr = 0;\
   char swapMacC = 0;\
   \
   do{\
      (cstr)[macRetI] = (macNumUL % 10) + 48;\
      ++macRetI;\
      macNumUL /= 10;\
   } while(macNumUL);\
   \
   (cstr)[macRetI] = '\0';\
   \
   endMacStr = (cstr) + (macRetI >> 1);\
   startMacStr = endMacStr - (! (macRetI & 1));\
   \
   /*Swap around even digit cases*/\
   do{ /*Loop: Reverse array*/\
      swapMacC = *endMacStr;\
      *endMacStr++ = *startMacStr;\
      *startMacStr-- = swapMacC;\
   } while(startMacStr >= (cstr)); /*Loop: reverse array*/\
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
   do{\
      (cstr)[macNumDigI] = (macNumUL % 10) + 48;\
      ++macNumDigI;\
      macNumUL /= 10;\
   } while(macNumUL);\
   \
   (cstr)[macNumDigI] = '\0';\
   macNumDigI;\
}) /*backNumToStr*/

#endif
