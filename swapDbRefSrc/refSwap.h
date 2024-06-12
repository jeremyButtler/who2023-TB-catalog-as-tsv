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
'     - foward declerations and guards
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

#ifndef REFERENCE_SWAP_H
#define REFERENCE_SWAP_H

typedef struct samEntry samEntry;
typedef struct amrStruct amrStruct;

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
);

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
   struct amrStruct *armAryST,
   unsigned int numAmrUI,
   struct samEntry *samSTPtr
);

#endif

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
