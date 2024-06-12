/*########################################################
# Name: drug_str_ary
#   - Holds fuctions to maintain an array of 32 length
#     c-strings. This is in the from of a matrix so you
#     can free using free(drug_cstring_array);
########################################################*/

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
' SOF: Start Of File
'   o header:
'     - Defined variables and header guards
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
|   - Defined varialbes and header guard
\-------------------------------------------------------*/

#ifndef DRUG_STRING_ARRAY_H
#define DRUG_STRING_ARRAY_H

#define defMaxDrugLen 32

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
char * mallocDrugAryStr(unsigned int numStringsUI);

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
);

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
);

/*-------------------------------------------------------\
| Fun-03: cpDrugToDrugAry
|   - Copies an antibiotic to a drug array
| Input:
|   - drugAryStr:
|     o Drug array to copy the antibiotic name to
|   - drugStr:
|     o Drug to copy
|   - indexUI:
|     o Index to copy drug to
|   - delimC:
|     o Deliminator to stop copy at
| Output:
|   - Modifies:
|     o dubAryStr to hold the new drug name
|   - Returns:
|     o The length of the copied string
\-------------------------------------------------------*/
#define cpDrugToDrugAry(\
   drugAryStr,\
   drugStr,\
   indexUI,\
   delimC\
)({\
   char *dupMacStr =\
       (drugAryStr) + (defMaxDrugLen * (indexUI));\
   \
   int iMacChar = 0;\
   \
   for(iMacChar=0; iMacChar < defMaxDrugLen; ++iMacChar)\
   { /*Loop: Copy over the drug names*/\
      if((drugStr)[iMacChar] == (delimC)) break;\
      dupMacStr[iMacChar] = (drugStr)[iMacChar] | 32;\
   } /*Loop: Copy over the drug names*/\
   \
   dupMacStr[iMacChar] = '\0';\
   iMacChar;\
}) /*cpDrugToDrugAry*/

/*-------------------------------------------------------\
| Fun-04: getDrugFromDrugAry
|   - Gets the pointer to a drug c-string in a drugAry
| Input:
|   - drugAryStr:
|     o Drug array to get the antibiotic name pointer
|   - indexUI:
|     o Index to grab
| Output:
|   - Returns:
|     o An pointer to the correct position
|       resitant (grade 1 and 2) mutations
\-------------------------------------------------------*/
#define getDrugFromDrugAry(drugAryStr, indexUI)(\
   (char *) (drugAryStr) + ((indexUI) * defMaxDrugLen)\
) /*getDrugFromDrugAry*/

/*-------------------------------------------------------\
| Fun-05: findDrug_in_drugAryStr()
|   - Finds an antibiotic in a drugAray c-string
| Input:
|   - qryStr:
|     o Drug to search for in drugAryStr
|   - drugAryStr:
|     o Drug array (c-string; treated as char[][32]) to
|       get the antibiotic name pointer
|   - numDrugsI:
|     o Number of antibiotics in drugAryStr
| Output:
|   - Returns:
|     o index of the drug if qryStr is in drugAryStr
|     o -1 if the drug is not in drugAry
| Note:
|    - Do to the way this is procesed, I can not use a
|      fast search method like a binary search, since
|      (there is no garuntee of order).
\-------------------------------------------------------*/
#define findDrug_in_drugAryStr(\
   qryStr,     /*Drug to search for*/\
   drugAryStr, /*Array of drugs to search*/\
   numDrugsI,  /*Number of drugs in array*/\
   delimC      /*Deliminator to stop at*/\
)({ /*findDrug_in_durgAryStr*/\
   int iIndexMac = 0;\
   char *drugMacStr = 0;\
   char *qryMacStr = (qryStr);\
   \
   for(iIndexMac=0; iIndexMac < (numDrugsI); ++iIndexMac)\
   { /*Loop: find qryStr in drugAryStr*/\
      drugMacStr =\
         getDrugFromDrugAry((drugAryStr), iIndexMac);\
      \
      while((*qryMacStr | 32) == (*drugMacStr | 32))\
      { /*Loop: Check if this is the same string*/\
         if((*drugMacStr=='\0') | (*qryMacStr==(delimC)))\
            break;\
         ++qryMacStr;\
         ++drugMacStr;\
      } /*Loop: Check if this is the same string*/\
      \
      if((*drugMacStr=='\0') & (*qryMacStr==(delimC)))\
         break;\
      \
      qryMacStr = (qryStr);\
   } /*Loop: find qryStr in drugAryStr*/\
   \
   iIndexMac | ( (int) -(iIndexMac >= (numDrugsI)) );\
      /*Logic:
      `   - iIndexMac >= numDrugsI
      `     o 1 when I did not find the query
      `     o 0 when I found the query
      `   - -(iIndexMac >= numDrugsI)
      `     o Changes 1 to -1 (all bits set) and 0 to 0
      `       (no bits set)
      `   - iIndexMac | -(iIndexMac >= numDrugsI)
      `     o Converts iIndexMac to -1 when the query was
      `       not found [-(iIndexMac >= numDragsI) is -1]
      `     o Does nothing to iIndexMac when the query was
      `       found [-(iIndexMac >= numDragsI) is 0]
      */\
}) /*findDrug_in_drugAryStr*/

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
