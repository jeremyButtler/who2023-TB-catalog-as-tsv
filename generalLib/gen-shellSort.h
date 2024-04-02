/*########################################################
# Name: gen-shellShort
#   - Holds functions for doing shell sorts on arrays.
#   - Currently is numeric arrays only
########################################################*/

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
' SOF: Start Of File
'  o header:
'    - Header guards
'  o fun-01: arySortNumeric
'    - Sorts the input array from least to greatest with
'      shell sort. This uses numeric input.
'  o fun-02: twoArySortNumeric
'    - Sorts the first input array from least to greatest
'      and keeps the second input arrar in order with the
'      first array. This is set up for numeric input using
'      shell short. Both arrays must be the same size.
'  o fun-03: threeArySortNumeric
'    - Sorts three arrays by the first input array (as
'      numeric) with shell short. All three arrays must be
'      the same size.
'   o license:
'     - Licensing for this code (public domain / mit)
\~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*-------------------------------------------------------\
| Header:
|   - Header guards
\-------------------------------------------------------*/

#ifndef GENERAL_SHELL_SHORT_H
#define GENERAL_SHELL_SHORT_H

/*-------------------------------------------------------\
| Fun-01: arySortNumeric
|  - Sorts the input array from least to greatest with
|    shell sort. This uses numeric input.
| Input:
|  - firstAry:
|    o First array, this is the array to sort
|  - startUL:
|    o First element to start sorting at
|  - endUL:
|    o Last element to sort (index 0)
| Output:
|  - Modifies:
|    o First array to be sorted form least to greatest
\-------------------------------------------------------*/
#define \
arySortNumeric( \
   firstAry, \
   startUL, \
   endUL \
){ /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
   ' Fun-01 TOC:
   '  - Sorts the input array from least to greatest with
   '    shell sort. This uses numeric input.
   '  - Shell sort taken from:
   '    - Adam Drozdek. 2013. Data Structures and
   '      Algorithims in c++. Cengage Leraning. fourth
   '      edition. pages 505-508
   '    - I made some minor changes, but is mostly the
   '      same
   '  o fun-01 sec-01:
   '    - Variable declerations
   '  o fun-01 sec-02:
   '    - Find the number of rounds to sort for
   '  o fun-01 sec-03:
   '    - Sort the arrays
   \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/\
  \
  /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
  ^ Fun-01 Sec-01:
  ^  - Variable declerations
  \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/\
  \
  /*Number of elements to sort*/\
  unsigned long numElmMacUL = (endUL) - (startUL);\
  long swapMacL = 0;\
  \
  /*Number of sorting rounds*/\
  unsigned long subMacUL = 0;\
  unsigned long nextElmMacUL = 0;\
  unsigned long lastElmMacUL = 0;\
  unsigned long elmOnMacUL = 0;\
  \
  /*Get arrays to sort from the matrix (for sanity)*/\
  \
  /*Variables to incurment loops*/\
  unsigned long ulIndexMac = 0;\
  unsigned long ulElmMac = 0;\
  \
  /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
  ^ Fun-01 Sec-02:
  ^  - Find the max search value (number rounds to sort)
  \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/\
  \
  /*Recursion formula: h[0] = 1, h[n] = 3 * h[n - 1] +1*/\
  subMacUL = 1; /*Initialzie first array*/\
  \
  while(subMacUL < numElmMacUL - 1)\
     subMacUL = (3 * subMacUL) + 1;\
  \
  /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
  ^ Fun-01 Sec-03:
  ^  - Sort the arrays in genIndiceST
  \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/\
  \
  while(subMacUL > 0)\
  { /*loop trhough all sub arrays sort the subarrays*/\
    for( \
       ulIndexMac = 0;\
       ulIndexMac <= subMacUL;\
       ++ulIndexMac\
    ){ /*For each element in the subarray*/\
      for(ulElmMac = ulIndexMac;\
          ulElmMac + subMacUL <= endUL;\
          ulElmMac += subMacUL\
      ){ /*Loop; swap each nth element of the subarray*/\
        nextElmMacUL = ulElmMac + subMacUL;\
        \
        if((firstAry)[ulElmMac]>(firstAry)[nextElmMacUL])\
        { /*If I need to swap an element*/\
          swapMacL = (firstAry)[ulElmMac];\
          (firstAry)[ulElmMac]= (firstAry)[nextElmMacUL];\
          (firstAry)[nextElmMacUL] = swapMacL;\
          \
          lastElmMacUL = ulElmMac;\
          elmOnMacUL = ulElmMac;\
          \
          while(lastElmMacUL >= subMacUL)\
          { /*loop; move swapped element back*/\
            lastElmMacUL -= subMacUL;\
            \
            if(  (firstAry)[elmOnMacUL]\
               > (firstAry)[lastElmMacUL]\
            ) break; /*Positioned the element*/\
            \
            \
            swapMacL = (firstAry)[elmOnMacUL];\
            (firstAry)[elmOnMacUL] =\
               (firstAry)[lastElmMacUL];\
            (firstAry)[lastElmMacUL] = swapMacL;\
            \
            elmOnMacUL = lastElmMacUL;\
          } /*loop; move swapped element back*/\
        } /*If I need to swap elements*/\
      } /*Loop; swap each nth element of the subarray*/\
    } /*For each element in the subarray*/\
    \
    subMacUL = (subMacUL - 1) / 3; /*Move to next round*/\
  } /*loop through all sub arrays to sort the subarrays*/\
} /*arySortNumeric*/

/*-------------------------------------------------------\
| Fun-02: twoArySortNumeric
|  - Sorts the first input array from least to greatest
|    and keeps the second input arrar in order with the
|    first array. This is set up for numeric input using
|    shell short. Both arrays must be the same size.
| Input:
|  - firstAry:
|    o First array, this is the array to sort
|  - secAry:
|    o Second array to keep sorted with first array
|  - startUL:
|    o First element to start sorting at
|  - endUL:
|    o Last element to sort (index 0)
| Output:
|  - Modifies:
|    o First array to be sorted form least to greatest
|    o secAry to be kept in order with firstAry
\-------------------------------------------------------*/
#define \
twoArySortNumeric( \
   firstAry, \
   secAry, \
   startUL, \
   endUL \
){ /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
   ' Fun-02 TOC:
   '  - Sorts the first input array from least to greatest
   '    and keeps the second input arrar in order with the
   '    first array.
   '  - Shell sort taken from:
   '    - Adam Drozdek. 2013. Data Structures and
   '      Algorithims in c++. Cengage Leraning. fourth
   '      edition. pages 505-508
   '    - I made some minor changes, but is mostly the
   '      same
   '  o fun-02 sec-01:
   '    - Variable declerations
   '  o fun-02 sec-02:
   '    - Find the number of rounds to sort for
   '  o fun-02 sec-03:
   '    - Sort the arrays
   \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/\
  \
  /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
  ^ Fun-02 Sec-01:
  ^  - Variable declerations
  \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/\
  \
  /*Number of elements to sort*/\
  unsigned long numElmMacUL = (endUL) - (startUL);\
  long swapMacL = 0;\
  \
  /*Number of sorting rounds*/\
  unsigned long subMacUL = 0;\
  unsigned long nextElmMacUL = 0;\
  unsigned long lastElmMacUL = 0;\
  unsigned long elmOnMacUL = 0;\
  \
  /*Get arrays to sort from the matrix (for sanity)*/\
  \
  /*Variables to incurment loops*/\
  unsigned long ulIndexMac = 0;\
  unsigned long ulElmMac = 0;\
  \
  /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
  ^ Fun-02 Sec-02:
  ^  - Find the max search value (number rounds to sort)
  \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/\
  \
  /*Recursion formula: h[0] = 1, h[n] = 3 * h[n - 1] +1*/\
  subMacUL = 1; /*Initialzie first array*/\
  \
  while(subMacUL < numElmMacUL - 1)\
     subMacUL = (3 * subMacUL) + 1;\
  \
  /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
  ^ Fun-02 Sec-03:
  ^  - Sort the arrays in genIndiceST
  \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/\
  \
  while(subMacUL > 0)\
  { /*loop trhough all sub arrays sort the subarrays*/\
    for( \
       ulIndexMac = 0;\
       ulIndexMac <= subMacUL;\
       ++ulIndexMac\
    ){ /*For each element in the subarray*/\
      for(ulElmMac = ulIndexMac;\
          ulElmMac + subMacUL <= endUL;\
          ulElmMac += subMacUL\
      ){ /*Loop; swap each nth element of the subarray*/\
        nextElmMacUL = ulElmMac + subMacUL;\
        \
        if((firstAry)[ulElmMac]>(firstAry)[nextElmMacUL])\
        { /*If I need to swap an element*/\
          swapMacL = (firstAry)[ulElmMac];\
          (firstAry)[ulElmMac]= (firstAry)[nextElmMacUL];\
          (firstAry)[nextElmMacUL] = swapMacL;\
          \
          swapMacL = (secAry)[ulElmMac];\
          (secAry)[ulElmMac] = (secAry)[nextElmMacUL];\
          (secAry)[nextElmMacUL] = swapMacL;\
          \
          lastElmMacUL = ulElmMac;\
          elmOnMacUL = ulElmMac;\
          \
          while(lastElmMacUL >= subMacUL)\
          { /*loop; move swapped element back*/\
            lastElmMacUL -= subMacUL;\
            \
            if(  (firstAry)[elmOnMacUL]\
               > (firstAry)[lastElmMacUL]\
            ) break; /*Positioned the element*/\
            \
            \
            swapMacL = (firstAry)[elmOnMacUL];\
            (firstAry)[elmOnMacUL] =\
               (firstAry)[lastElmMacUL];\
            (firstAry)[lastElmMacUL] = swapMacL;\
            \
            swapMacL = (secAry)[elmOnMacUL];\
            (secAry)[elmOnMacUL]= (secAry)[lastElmMacUL];\
            (secAry)[lastElmMacUL] = swapMacL;\
            \
            elmOnMacUL = lastElmMacUL;\
          } /*loop; move swapped element back*/\
        } /*If I need to swap elements*/\
      } /*Loop; swap each nth element of the subarray*/\
    } /*For each element in the subarray*/\
    \
    subMacUL = (subMacUL - 1) / 3; /*Move to next round*/\
  } /*loop through all sub arrays to sort the subarrays*/\
} /*twoArySortNumeric*/

/*-------------------------------------------------------\
| Fun-03: threeArySortNumeric
|  - Sorts three arrays by the first input array (as
|    numeric) with shell short. All three arrays must be
|    the same size.
| Input:
|  - firstAry:
|    o First array, this is the array to sort
|  - secAry:
|    o Second array to keep sorted with first array
|  - thirdAry:
|    o Third arra to keep sorted with the first array
|  - startUL:
|    o First element to start sorting at
|  - endUL:
|    o Last element to sort (index 0)
| Output:
|  - Modifies:
|    o First array to be sorted form least to greatest
|    o secAry to be kept in order with firstAry
|    o thirdAry to be kept in order with firstAry
\-------------------------------------------------------*/
#define \
threeArySortNumeric( \
   firstAry, \
   secAry, \
   thirdAry,\
   startUL, \
   endUL \
){ /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
   ' Fun-03 TOC:
   '  - Sorts three arrays by the first input array (as
   '    numeric) with shell short. All three arrays must
   '    be the same size.
   '  - Shell sort taken from:
   '    - Adam Drozdek. 2013. Data Structures and
   '      Algorithims in c++. Cengage Leraning. fourth
   '      edition. pages 505-508
   '    - I made some minor changes, but is mostly the
   '      same
   '  o fun-03 sec-01:
   '    - Variable declerations
   '  o fun-03 sec-02:
   '    - Find the number of rounds to sort for
   '  o fun-03 sec-03:
   '    - Sort the arrays
   \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/\
  \
  /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
  ^ Fun-03 Sec-01:
  ^  - Variable declerations
  \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/\
  \
  /*Number of elements to sort*/\
  unsigned long numElmMacUL = (endUL) - (startUL);\
  long swapMacL = 0;\
  \
  /*Number of sorting rounds*/\
  unsigned long subMacUL = 0;\
  unsigned long nextElmMacUL = 0;\
  unsigned long lastElmMacUL = 0;\
  unsigned long elmOnMacUL = 0;\
  \
  /*Get arrays to sort from the matrix (for sanity)*/\
  \
  /*Variables to incurment loops*/\
  unsigned long ulIndexMac = 0;\
  unsigned long ulElmMac = 0;\
  \
  /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
  ^ Fun-03 Sec-02:
  ^  - Find the max search value (number rounds to sort)
  \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/\
  \
  /*Recursion formula: h[0] = 1, h[n] = 3 * h[n - 1] +1*/\
  subMacUL = 1; /*Initialzie first array*/\
  \
  while(subMacUL < numElmMacUL - 1)\
     subMacUL = (3 * subMacUL) + 1;\
  \
  /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
  ^ Fun-03 Sec-03:
  ^  - Sort the arrays in genIndiceST
  \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/\
  \
  while(subMacUL > 0)\
  { /*loop trhough all sub arrays sort the subarrays*/\
    for( \
       ulIndexMac = 0;\
       ulIndexMac <= subMacUL;\
       ++ulIndexMac\
    ){ /*For each element in the subarray*/\
      for(ulElmMac = ulIndexMac;\
          ulElmMac + subMacUL <= endUL;\
          ulElmMac += subMacUL\
      ){ /*Loop; swap each nth element of the subarray*/\
        nextElmMacUL = ulElmMac + subMacUL;\
        \
        if((firstAry)[ulElmMac]>(firstAry)[nextElmMacUL])\
        { /*If I need to swap an element*/\
          swapMacL = (firstAry)[ulElmMac];\
          (firstAry)[ulElmMac]= (firstAry)[nextElmMacUL];\
          (firstAry)[nextElmMacUL] = swapMacL;\
          \
          swapMacL = (secAry)[ulElmMac];\
          (secAry)[ulElmMac] = (secAry)[nextElmMacUL];\
          (secAry)[nextElmMacUL] = swapMacL;\
          \
          swapMacL = (thirdAry)[ulElmMac];\
          (thirdAry)[ulElmMac]= (thirdAry)[nextElmMacUL];\
          (thirdAry)[nextElmMacUL] = swapMacL;\
          \
          lastElmMacUL = ulElmMac;\
          elmOnMacUL = ulElmMac;\
          \
          while(lastElmMacUL >= subMacUL)\
          { /*loop; move swapped element back*/\
            lastElmMacUL -= subMacUL;\
            \
            if(  (firstAry)[elmOnMacUL]\
               > (firstAry)[lastElmMacUL]\
            ) break; /*Positioned the element*/\
            \
            \
            swapMacL = (firstAry)[elmOnMacUL];\
            (firstAry)[elmOnMacUL] =\
               (firstAry)[lastElmMacUL];\
            (firstAry)[lastElmMacUL] = swapMacL;\
            \
            swapMacL = (secAry)[elmOnMacUL];\
            (secAry)[elmOnMacUL]= (secAry)[lastElmMacUL];\
            (secAry)[lastElmMacUL] = swapMacL;\
            \
            swapMacL = (thirdAry)[elmOnMacUL];\
            (thirdAry)[elmOnMacUL] =\
               (thirdAry)[lastElmMacUL];\
            (thirdAry)[lastElmMacUL] = swapMacL;\
            \
            elmOnMacUL = lastElmMacUL;\
          } /*loop; move swapped element back*/\
        } /*If I need to swap elements*/\
      } /*Loop; swap each nth element of the subarray*/\
    } /*For each element in the subarray*/\
    \
    subMacUL = (subMacUL - 1) / 3; /*Move to next round*/\
  } /*loop through all sub arrays to sort the subarrays*/\
} /*threeArySortNumeric*/

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
