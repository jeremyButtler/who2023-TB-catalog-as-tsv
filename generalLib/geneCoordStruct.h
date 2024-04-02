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
'   o st: geneCoords
'     - Holds the arrays for the gene coordinates and ids 
'       in a paf file
'   o fun-01: freeGeneCoordsStack
'     - Frees the arrays in a geneCoords structure
'   o fun-02: freeGeneCoords
'     - Frees the a heap alloacted geneCoords structure
'   o fun-03: initGeneCoords
'     - Initializes a geneCoords structure
'   o fun-04: makeGeneCoords
'     - Makes a heap allocated geneCoords structure
'   o fun-05: getPafGene
'     - Get the id and coordinates for a gene from a paf
'       file
'   o fun-06: swapGeneCoord (.c only)
'     - Swaps two array items in a geneCoords structure
'       around
'   o fun-07: geneCoordsSort
'     - Sorts the arrays in a genesCoord structure by
'       starting positiion with shell short.
'   o fun-08: findStartCoordInGeneCoord
'     - Does a binary search by starting coordinate for a
'       potentail gene in a geneCoords structure
'   o fun-09: pafGetGeneCoords
'     - Gets the gene coordinates from a paf file
'   o license:
'     - Licensing for this code (public domain / mit)
\~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*-------------------------------------------------------\
| Header:
|   - Header guards
\-------------------------------------------------------*/

#ifndef GENE_COORD_STRUCT_H
#define GENE_COORD_STRUCT_H

/*-------------------------------------------------------\
| ST: geneCoords
|   - Holds the arrays for the gene coordinates and ids 
|     in a paf file
\-------------------------------------------------------*/
typedef struct geneCoords{
   char (*idStrAry)[64];
   unsigned int *startAryUI;
   unsigned int *endAryUI;
}geneCoords;


/*-------------------------------------------------------\
| Fun-01: freeGeneCoordsStack
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
| Fun-02: freeGeneCoords
|    - Frees the a heap alloacted geneCoords structure
| Input:
|   - geneCoordsST:
|     o Pointer to a geneCoords structure to free
| Output:
|   - Frees:
|     o geneCoordsST (and sets to 0)
\-------------------------------------------------------*/
void
freeGeneCoords(
   struct geneCoords **geneCoordsST
);

/*-------------------------------------------------------\
| Fun-03: initGeneCoords
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
} /*initGeneCoords*/

/*-------------------------------------------------------\
| Fun-04: makeGeneCoords
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
| Fun-05: getPafGene
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
| Fun-07: geneCoordsSort
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
| Fun-08: findStartCoordInGeneCoord
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
| Fun-09: pafGetGeneCoords
|  - Gets the gene coordinates from a paf file
| Input:
|  - pafFILE:
|    o Pointer to paf FILE to get gene coordinates from
|  - numGenesUI:
|    o Number of genes extracted
| Output:
|  - Returns:
|    o Pointer to an sorted geneCoords structure with the 
|      gene coordinates
|    o 0 for memory error
|  - Modifies:
|    o numGenesI to have the number of genes (index 0)
|      extracted
\-------------------------------------------------------*/
struct geneCoords *
pafGetGeneCoords(
   void *pafFILE, /*Paf file get gene coordinates from*/\
   int *numGenesI /*Number of genes extracted*/\
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
