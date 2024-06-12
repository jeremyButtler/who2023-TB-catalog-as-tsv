/*########################################################
# Name: refSwap
#   - has functions to swap the reference for an tbAmr
#    database
# Basic usage:
#   - call cnvtCoords_refSwap (fun01) to convert an set of
#     coordinates
#   - call swapAmrAryRef_refSwap (fun02) to convert an
#     amr structure array
# Basic free at end:
#   - you will have to free the sam entry structure
#     (../../generalLib/samEntryStruct.h) and amrStruct
#     array (../amrStruct.h)
########################################################*/

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
' SOF:
'   o header:
'     - included libraries
'   o fun01: cnvtCoords_refSwap
'     - converts an set of coordinates from one reference
'       to another reference
'   o fun02: swapAmrAryRef_refSwap
'     - swaps the reference coordinates for an array of
'       amr structures
'   o license:
'     - licensing for this code (public domain / mit)
\~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*-------------------------------------------------------\
| Header:
|   - included libraries
\-------------------------------------------------------*/

#ifdef PLAN9
   #include <u.h>
   #include <libc.h>
#endif

#include "refSwap.h"

#include "../generalLib/samEntryStruct.h"
#include "../generalLib/amrStruct.h"

/*.h files only*/
#include "../generalLib/dataTypeShortHand.h"

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\
! Hidden libraries
!   - std. #include <stdio.h>
!   - std. #include <stdlib.h>
!   - .c #include "../generalLib/drug_str_ary.h"
!   - .h #include "../generalLib/base10StrToNum.h"
!   - .h #include "../generalLib/ulCpStr.h"
!   - .h #include "../generalLib/numToStr.h"
!   - .h #include "../generalLib/ntToBit.h"
\%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/*-------------------------------------------------------\
| Fun01: cnvtCoords_refSwap
|   - converts an set of coordinates from one reference
|     to another reference
| Input:
|   - samSTPtr:
|     o pointer to an samEntry structure with the new
|       reference mapped to the old reference
|   - posArySI:
|     o unsigned int array with the current position
|       in the sam entry struct
|     o posAryUI[0] = current cigar entry on
|     o posAryUI[1] = bases not checked in current cigar
|       entry
|     o posAryUI[2] = reference position at currently
|     o posAryUI[3] = sequence (new ref) position at
|       currently
|   - refPosSI:
|     o unsigned int with position to change
| Output:
|   - Modifies:
|     o newCoordsAryUI to hold the new start coordinate
|       (index 0) and new ending coordinate (index 1)
|     o posAryUI to be moved to the start of the current
|       entry
|   - Returns
|     o new coordinate
|     o -1 if the coordinate could not be found
\-------------------------------------------------------*/
signed int
cnvtCoords_refSwap(
   struct samEntry *samSTPtr,
   signed int posArySI[],
   signed int refStartSI
){
   /*If: this is the first time*/
   if(posArySI[0] == 0 && posArySI[1] == 0)
      posArySI[1] = samSTPtr->cigValAryI[posArySI[0]];

   if(posArySI[2] >= samSTPtr->refEndUI)
      goto outOfBounds_fun01_sec05;

   else if(posArySI[3] >= samSTPtr->readLenUI)
      goto outOfBounds_fun01_sec05;

   samEntryFindRefPos(
      samSTPtr,
      posArySI[0],   /*cigar entry*/
      posArySI[1],   /*number bases left in cigar entry*/
      refStartSI,    /*start of target*/
      posArySI[2],   /*reference position*/
      posArySI[3]    /*sequence position*/
   );

   return posArySI[3];

   outOfBounds_fun01_sec05:;

   return -1;
} /*cnvtCoords_refSwap*/

/*-------------------------------------------------------\
| Fun02: swapAmrAryRef_refSwap
|   - swaps the reference coordinates for an array of
|     amr structures
| Input:
|   - amrAryST
|     o array of amrStructs to swap reference coordinates
|       with
|   - numAmrUI:
|     o number of amrStructs in amrAryST
|   - samSTPtr:
|     o pointer to an samEntry structure with the new
|       reference mapped to the old reference
| Output:
|   - Modifies:
|     o coordinates in amrStructs
|   - Returns:
|     o the number of amrStructs that could be converted
\-------------------------------------------------------*/
unsigned int
swapAmrAryRef_refSwap(
   struct amrStruct *amrAryST,
   unsigned int numAmrUI,
   struct samEntry *samSTPtr
){ /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
   ' Fun02 TOC:
   '   o fun02 sec01:
   '     - variable declerations
   '   o fun02 sec02:
   '     - convert amr coordinates
   '   o fun02 sec03:
   '     - remove uncoverted structures at start
   '   o fun02 sec04:
   '     - return the number of amrs converted
   \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun02 Sec01:
   ^   - variable declerations
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   sint ntPosArySI[4];
   sint aaPosArySI[4];

   sint genePosArySI[4];
   sint geneStartSI = 0;
   sint geneEndSI = 0;

   sint newCoordSI = 0;
   uint uiAmr = 0;
   schar startBl = 1;
   uint firstCnvtAmrUI = 0;

   for(
      uiAmr = 0;
      uiAmr < 4;
      ++uiAmr
   ){ /*Loop: initialize arrays*/
      ntPosArySI[uiAmr] = 0;
      aaPosArySI[uiAmr] = 0;
      genePosArySI[uiAmr] = 0;
   } /*Loop: initialize arrays*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun02 Sec02:
   ^   - convert amr coordinates
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   uiAmr = 0;

   while(uiAmr < numAmrUI)
   { /*Loop: convert amr coordinates*/ 
      if(
            amrAryST[uiAmr].geneFirstRefUI != geneStartSI
         || amrAryST[uiAmr].geneLastRefUI != geneEndSI
      ){ /*If: I am changing genes*/
         geneStartSI =
            cnvtCoords_refSwap(
               samSTPtr,
               genePosArySI,
               amrAryST[uiAmr].geneFirstRefUI
            );

         if(geneStartSI < 0)
            geneStartSI = samSTPtr->readLenUI;

         geneEndSI =
            cnvtCoords_refSwap(
               samSTPtr,
               genePosArySI,
               amrAryST[uiAmr].geneLastRefUI
            );

         if(geneEndSI < 0)
            geneEndSI = samSTPtr->readLenUI;
      } /*If: I am changing genes*/

      newCoordSI =
         cnvtCoords_refSwap(
            samSTPtr,
            ntPosArySI,
            amrAryST[uiAmr].refPosUI
         );

      if(newCoordSI < 0)
      { /*If: I could not find the coordinates*/
         if(! startBl)
         { /*If: I am at the end of the reference*/
            numAmrUI = uiAmr - 1;
            break;
            /*at this point no other coordinates should
              work
            */
         } /*If: I am at the end of the reference*/

         /*count the number of amr structures I could
         `  not convert
         */
         ++firstCnvtAmrUI;
      } /*If: I could not find the coordinates*/

      amrAryST[uiAmr].refPosUI = (uint) newCoordSI;

      if(amrAryST[uiAmr].codonPosUI > 0)
      { /*If: I have an codon position*/
         newCoordSI =
            cnvtCoords_refSwap(
               samSTPtr,
               aaPosArySI,
               amrAryST[uiAmr].codonPosUI
            );

         /*move problematic structures to the end*/
         if(newCoordSI < 0)
         { /*If: I could not find the coordinates*/
            numAmrUI = uiAmr - 1;
            break;
         } /*If: I could not find the coordinates*/

         amrAryST[uiAmr].codonPosUI = (uint) newCoordSI;
      } /*If: I have an codon position*/

      amrAryST[uiAmr].geneFirstRefUI = geneStartSI;
      amrAryST[uiAmr].geneLastRefUI = geneEndSI;

      startBl = 0;
      ++uiAmr;
   } /*Loop: convert amr coordinates*/ 

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun02 Sec03:
   ^   - remove uncoverted structures at start
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   uiAmr = 0;
   while(uiAmr < firstCnvtAmrUI)
   { /*Loop: remove the uncoverted AMRs at start*/
      swapAmrStructs(
         amrAryST[uiAmr],
         amrAryST[numAmrUI]
      );

      --numAmrUI;
      ++uiAmr;
   } /*Loop: remove the uncoverted AMRs at start*/

   if(firstCnvtAmrUI > 0)
   { /*If: I need to resort the array*/
      sortAmrStructArray(
         amrAryST,
         0,
         numAmrUI
      );
   } /*If: I need to resort the array*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun02 Sec04:
   ^   - return the number of amrs converted
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   return numAmrUI;
} /*swapAmrAryRef_refSwap*/

/*=======================================================\
: License:
: 
: This code is under the unlicense (public domain).
:   However, for cases were the public domain is not
:   suitable, such as countries that do not respect the
:   public domain or were working with the public domain
:   is inconveint / not possible, this code is under the
:   MIT license
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
