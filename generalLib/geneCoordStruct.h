/*########################################################
# Name: geneCoordStruct
#   - Holds the geneCoords struct and its supporting
#     functions I use to read in gene positions form a paf
#     file
# Libraries:
#   - "base10StrToNum.h"
#   o "dataTypeShortHand.h"
#   - "genMath.h"
# C Standard Libraries:
########################################################*/

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
' SOF: Start Of File
'   o header:
'     - Header guards
'   o .h st: geneCoords
'     - Holds the arrays for the gene coordinates and ids 
'       in an gene coordiantes table
'   o fun01: freeGeneCoordsStack
'     - Frees the arrays in a geneCoords structure
'   o fun02: freeGeneCoords
'     - Frees the a heap alloacted geneCoords structure
'   o .h fun03: initGeneCoords
'     - Initializes a geneCoords structure
'   o fun04: makeGeneCoords
'     - Makes a heap allocated geneCoords structure
'   o fun05: getPafGene
'     - Get the id and coordinates for a gene from a paf
'       file
'   o .c fun06: swapGeneCoord
'     - Swaps two array items in a geneCoords structure
'       around
'   o fun07: geneCoordsSort
'     - Sorts the arrays in a genesCoord structure by
'       starting positiion with shell short.
'   o fun08: findStartCoordInGeneCoord
'     - Does a binary search by starting coordinate for a
'       potentail gene in a geneCoords structure
'   o fun09: geneCoords_sortNames
'     - Sorts the arrays in a genesCoord structure by
'       gene name
'   o fun10: geneCoords_findName
'     - Does a binary search to find an gene name in an
'       gene geneCoords structer (must be sorted by name)
'   o fun11: getGeneCoords
'     - Gets the gene coordinates from an gene coordinates
'       table
'   o license:
'     - Licensing for this code (public domain / mit)
\~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*-------------------------------------------------------\
| Header:
|   - Header guards
\-------------------------------------------------------*/

#ifndef GENE_COORD_STRUCT_H
#define GENE_COORD_STRUCT_H

#define def_fileErr_geneCoord 2
#define def_invalidEntry_geneCoord 4
#define def_memErr_geneCoord 64

/*-------------------------------------------------------\
| ST: geneCoords
|   - Holds the arrays for the gene coordinates and ids 
|     in an gene coordinates table
\-------------------------------------------------------*/
typedef struct geneCoords{
   char (*idStrAry)[64];
   unsigned int *startAryUI;
   unsigned int *endAryUI;
   unsigned char *dirAryUC;
}geneCoords;


/*-------------------------------------------------------\
| Fun01: freeGeneCoordsStack
|    - Frees the arrays in a geneCoords structure
| Input:
|   - geneCoordsST:
|     o Pointer to a geneCoords structure with arrays to
|       free
| Output:
|   - Frees:
|     o idStrAry, startAryUI, and endAryUI (and sets to 0)
\-------------------------------------------------------*/
void
freeGeneCoordsStack(
   struct geneCoords *geneCoordsST
);

/*-------------------------------------------------------\
| Fun02: freeGeneCoords
|    - Frees the a heap alloacted geneCoords structure
| Input:
|   - geneCoordsST:
|     o Pointer to a geneCoords structure to free
| Output:
|   - Frees:
|     o geneCoordsST
\-------------------------------------------------------*/
void
freeGeneCoords(
   struct geneCoords *geneCoordsST
);

/*-------------------------------------------------------\
| Fun03: initGeneCoords
|    - Initializes a geneCoords structure
| Input:
|   - geneCoordsST:
|     o Pointer to a geneCoords structure to initialize
| Output:
|   - Sets:
|     o idStrAry, startAryUI, and endAryUI to 0
\-------------------------------------------------------*/
#define initGeneCoords(geneCoordsST){\
   (geneCoordsST)->idStrAry = 0;\
   (geneCoordsST)->startAryUI = 0;\
   (geneCoordsST)->endAryUI = 0;\
   (geneCoordsST)->dirAryUC = 0;\
} /*initGeneCoords*/

/*-------------------------------------------------------\
| Fun04: makeGeneCoords
|    - Makes a heap allocated geneCoords structure
| Input:
|   - numGenesUI:
|     o Length of the arrays in the new geneCoords
|       structure
| Output:
|   - Returns:
|     o geneCoords structure with allocated arrays
| Note:
|   - Do not use initGeneCoords on the returned structure
\-------------------------------------------------------*/
struct geneCoords *
makeGeneCoords(
   unsigned int numGenesUI
);

/*-------------------------------------------------------\
| Fun05: getPafGene
|    - Get the id and coordinates for a gene from a paf
|      file
| Input:
|   - geneCoordsST:
|     o Pointer to a geneCoordsST structure to hold the
|       new gene
|   - posUI:
|     o Position in the arrays in the geneCoords structure
|       to add the new gene at
|   - typeC:
|     o Alignment type for the extracted gene
|   - pafLineStr:
|     o Line from the paf file with the gene to add
| Output:
|   - Modifies:
|     o All arrays in geneCoordsST to hold the new gene
\-------------------------------------------------------*/
void
getPafGene(
   struct geneCoords *geneCoordsST,
   unsigned int posUI,
   char *typeC,
   char *pafLineStr
); /*getPagGene*/

/*-------------------------------------------------------\
| Fun07: geneCoordsSort
|  - Sorts the arrays in a genesCoord structure by
|    starting positiion with shell short.
| Input:
|  - geneCoordST:
|    o Pointer to geneCoords structure with gene
|      coordinates to sort
|  - startUI:
|    o First element to start sorting at
|  - endUI:
|    o Last element to sort
| Output:
|  - Modifies:
|    o Arrays in geneCoordST to be sorted by the gene
|      starting coordinate (lowest first)
\-------------------------------------------------------*/
void
sortGeneCoords(
   struct geneCoords *geneCoordST,
   unsigned int startUI,
   unsigned int endUI
);

/*-------------------------------------------------------\
| Fun08: findStartCoordInGeneCoord
|  - Does a binary search by starting coordinate for a
|    potentail gene in a geneCoords structure
| Input:
|  - geneCoordST:
|    o Pointer to geneCoords structure with starting gene
|      coordinates to search
|  - qryUI:
|    o Starting coordinate (query) to search for in
|      geneCoordST
|  - numGenesUI:
|    o Number of genes in geneCoordST (index 1)
| Output:
|  - Returns:
|    o The index of the starting position
|    o -1 if there was no gene
\-------------------------------------------------------*/
int
findStartCoordInGeneCoord(
   struct geneCoords *geneST,
   unsigned int qryUI,
   int numGenesI
);

/*-------------------------------------------------------\
| Fun09: geneCoords_sortNames
|  - Sorts the arrays in a genesCoord structure by
|    gene name
| Input:
|  - geneCoordST:
|    o Pointer to geneCoords structure with gene
|      coordinates to sort
|  - startUI:
|    o First element to start sorting at
|  - endUI:
|    o Last element to sort (index 0)
| Output:
|  - Modifies:
|    o Arrays in geneCoordST to be sorted by the gene
|      starting coordinate (lowest first)
\-------------------------------------------------------*/
void
geneCoords_sortNames(
   struct geneCoords *geneCoordST,
   unsigned int startUI,
   unsigned int endUI
);

/*-------------------------------------------------------\
| Fun10: geneCoords_findName
|  - Does a binary search to find an gene name in an gene
|    geneCoords structer (must be sorted by name)
| Input:
|  - geneCoordST:
|    o Pointer to geneCoords structure with starting gene
|      coordinates to search
|  - nameStr:
|    o c-string with name to search for
|  - numGenesSI:
|    o Number of genes in geneCoordST (index 1)
| Output:
|  - Returns:
|    o The index of gene with the same name
|    o -1 if there was no gene
\-------------------------------------------------------*/
int
geneCoords_findName(
   struct geneCoords *geneST,
   signed char *nameStr,
   signed int numGenesSI
);

/*-------------------------------------------------------\
| Fun11: getGeneCoords
|  - Gets the gene coordinates from a gene table (tsv)
| Input:
|  - geneTblFileStr:
|    o C-string with name of the gene table file to
|      extract the gene coordinates and names from
|  - numGenesSI:
|    o Will hold the Number of genes extracted
|  - errULPtr:
|    o Will hold the error return value
| Output:
|  - Returns:
|    o Pointer to an sorted geneCoords structure with the 
|      gene coordinates
|    o 0 for errors
|  - Modifies:
|    o numGenesI to have the number of genes (index 0)
|      extracted
|    o errULPtr to hold the error
|      - 0 for no errors
|      - def_fileErr_geneCoord for an file opening error
|      - def_memErr_geneCoord for an memor error
|      - (line_number << 8) | def_invalidEntry_geneCoord
|        for an invalid line in the file
|        o Get the line number with (*errULPtr >> 8)
\-------------------------------------------------------*/
struct geneCoords *
getGeneCoords(
   char *geneTblFileStr,
   int *numGenesSI, /*Number of genes extracted*/\
   unsigned long *errULPtr
);

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
