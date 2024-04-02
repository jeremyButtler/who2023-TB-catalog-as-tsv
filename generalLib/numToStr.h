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
'   o license:
'     - Licensing for this code (public domain / mit)
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
