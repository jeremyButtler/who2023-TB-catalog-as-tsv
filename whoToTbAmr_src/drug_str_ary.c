/*########################################################
# Name: drug_str_ary
#   - Holds fuctions to maintain an array of 32 length
#     c-strings. This is in the from of a matrix so you
#     can free using free(drug_cstring_array);
########################################################*/

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
' SOF: Start Of File
'   o header:
'     - Included libraries
'   o fun-01: mallocDrugAryStr
'     - Returns a pointer to allocated memory for drug
'       c-string array
'   o fun-02: reallocDrugAryStr
'     - Returns a pointer to rellocated memory for an
'       drug c-string array
'   o fun-02: reallocDrugAryStrNoFree
'     - Returns a pointer to rellocated memory for
'       an drug c-string array, but does not free the
'       c-string array when there is an memory error
'   o .h fun-03: cpDrugToDrugAry
'     - Copies an antibiotic to a drug c-string array
'   o .h fun-04: getDrugFromDrugAry
'     - Gets the pointer to a drug c-string in a drug
'       c-string array
'   o .h fun-05: findDrug_in_drugAryStr()
'       - Finds an antibiotic in a drug c-string array
'   o license:
'     - Licensing for this code (public domain / mit)
\~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*-------------------------------------------------------\
| Header:
|   - Included libraries
\-------------------------------------------------------*/

#ifdef PLAN9
   #include <u.h>
   #include <libc.h>
#else
   #include <stdlib.h>
#endif

#include "drug_str_ary.h"

/*-------------------------------------------------------\
| Fun-01: mallocDrugAryStr
|   - Returns a pointer to allocated memory for drugAryStr
| Input:
|   - numStringsUI:
|     o Number of strings to make
| Output:
|   - Returns:
|     o An pointer to the new arrray
|     o 0 for memory error
\-------------------------------------------------------*/
char * mallocDrugAryStr(unsigned int numStringsUI){
   return
      malloc(numStringsUI * defMaxDrugLen * sizeof(char));
} /*mallocDrugAryStr*/

/*-------------------------------------------------------\
| Fun-02: reallocDrugAryStr
|   - Returns a pointer to rellocated memory for
|     drugAryStr
| Input:
|   - drugAryStr:
|     o Pionter to drugAryStr to reallocate memory for
|   - numStringsUI:
|     o Number of strings to make
| Output:
|   - Modifies:
|     o drugAryStr to be resized
|     o frees and sets drugAryStr to 0 if memory error
\-------------------------------------------------------*/
void reallocDrugAryStr(
   char **drugAryStr,
   unsigned int numStrUI
){ /*reallocDrugAryStr*/
   char * tmpMacStr = 0;

   tmpMacStr =
      realloc(
         (*drugAryStr),
         (numStrUI) * defMaxDrugLen * sizeof(char)
      );
   
   if(!tmpMacStr)
   { /*If: I had a memory error*/
      free((*drugAryStr));
      (*drugAryStr) = 0;
   } /*If: I had a memory error*/
   
   else (*drugAryStr) = tmpMacStr;
} /*reallocDrugAryStr*/

/*-------------------------------------------------------\
| Fun-02: reallocDrugAryStrNoFree
|   - Returns a pointer to rellocated memory for
|     drugAryStr, but does not free drugStrAry when there
|     is a memory error
| Input:
|   - drugAryStr:
|     o Pionter to drugAryStr to reallocate memory for
|   - numStringsUI:
|     o Number of strings to make
| Output:
|   - Modifies:
|     o drugAryStr to be resized
|     o frees and sets drugAryStr to 0 if memory error
|   - Returns:
|     o 0 for no problems
|     o 1 for memory errors
\-------------------------------------------------------*/
char reallocDrugAryStrNoFree(
   char **drugAryStr,
   unsigned int numStrUI
){ /*reallocDrugAryStrNoFree*/
   char * tmpMacStr = 0;

   tmpMacStr =
      realloc(
         (*drugAryStr),
         (numStrUI) * defMaxDrugLen * sizeof(char)
      );
   
   if(!tmpMacStr) return 1;
   
  return 0;
} /*reallocDrugAryStrNoFree*/

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
