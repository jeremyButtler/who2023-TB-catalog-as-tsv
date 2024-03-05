/*#######################################################\
# Name: geneIndiceStruct
#   - Has functions for reading the genome indice (tab 2)
#     of the WHO's 2023 TB catalog.
#   - Based on what happened between 2021 (edition 1) and
#     2023 (edition 2), I suspect that I will have to
#     change this for whenever catalog edition 3 comes
#     out. How the WHO handled their 2023 catalog is a bug
#     of its own.
\#######################################################*/

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
' SOF: Start Of File
'   o header: 
'     - Has included libraries and defines
'   o st-01: genIndice
'     - Structure to hold a single genome indice from
'       the who 2023 catalog
'   o fun-01: blankGeneIndice
'     - Blanks all non-pointer values in a genIndice
'   o fun-02: initGeneIndice
'     - Initialize a genIndice
'   o fun-03: makeGenIndice
'     - Makes a new genIndice
'   o fun-04: freeGeneIndiceStack
'     - Frees varialbes in geneIndiceST
'   o fun-05: freeGeneIndice
'     - Frees a single geneIndiceST structure. This
'       function does 'not free the child pointers in
'       geneIndiceST
'   o fun-06: freeGeneIndiceAry
'     - Frees an array of geneIndiceST structures.
'   o fun-07: cmpGenIndices
'     - Compares to see if one genIndice structure is
'       less than another
'   o fun-08: cmpGenIndiceToStr
'     - Compares to see if a genIndice structure is
'       less than a string
'   o fun-09: swapGenIndices
'     - Swaps two array items in a genIndice structure
'       around
'   o fun-10: genIndiceSort
'     - Sorts an array of genIndice structures by variant
'       ids with shell short.
'   o fun-11: findGenIndiceVariant
'     - Does a binary search for the nearest amr at or
'       after to the input query coordiante
'   o fun-12: read_who2023_indiceTab
'     - Reads in the genome coordinates and variants from
'       the 2023 WHO TB catalog genome indice tab (saved
'       as a tsv)
\~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*-------------------------------------------------------\
| Header: 
|   - Has included libraries and defines
\-------------------------------------------------------*/

#ifndef GENOME_INDICIE_STRUCT_H
#define GENOME_INDICIE_STRUCT_H

/*-------------------------------------------------------\
| ST-01: genIndice
|   - Structure to hold a single genome indice from
|     the who 2023 catalog
\-------------------------------------------------------*/
typedef struct genIndice
{ /*genIndice*/
   unsigned int posUI;

   char *varIdStr;
   unsigned int lenVarIdUI;

   char *refSeqStr;
   unsigned int lenRefSeqUI;

   char *amrSeqStr;
   unsigned int lenAmrSeqUI;
}genIndice;

/*-------------------------------------------------------\
| Fun-01: initGeneIndice
|   - Initialize a genIndice
| Input:
|   - geneIndiceStPtr:
|     o Pointer to genIndice to initialize
| Output:
|   - Modifies:
|     o geneIndice to have values set to 0; including
|       pointers
\-------------------------------------------------------*/
#define initGenIndice(genIndiceSTPtr){\
   (genIndiceSTPtr)->posUI = 0;\
   \
   (genIndiceSTPtr)->varIdStr = 0;\
   (genIndiceSTPtr)->lenVarIdUI = 0;\
   \
   (genIndiceSTPtr)->refSeqStr = 0;\
   (genIndiceSTPtr)->lenRefSeqUI = 0;\
    \
   (genIndiceSTPtr)->amrSeqStr = 0;\
   (genIndiceSTPtr)->lenAmrSeqUI = 0;\
   \
} /*initGenIndice*/

/*-------------------------------------------------------\
| Fun-03: makeGenIndice
|   - Makes a new genIndice
| Input:
| Output:
|   - Returns:
|     o genIndice on heap
\-------------------------------------------------------*/
struct genIndice * makeGenIndice();

/*-------------------------------------------------------\
| Fun-04: freeGenIndiceStack
|   - Frees the variables in an geneIndice structure
|   - This is on the programs stack, not the stack struct
|     used in AVL trees
| Input:
|   - geneIndiceST:
|     o Pointer to genIndice to free varialbes in
| Output:
|   - Frees:
|     o Strings in the geneIndiceST structure
|   - Sets:
|     o Everything to 0
\-------------------------------------------------------*/
void freeGeneIndiceStack(struct genIndice *geneIndiceST);

/*-------------------------------------------------------\
| Fun-05: freeGeneIndice
|   - Frees a single geneIndiceST structure.
| Input:
|   - geneIndiceST:
|     o Pointer to geneIndiceST structure to free
| Output:
|   - Frees:
|     o geneIndiceST
\-------------------------------------------------------*/
void freeGeneIndice(struct genIndice **geneIndiceST);

/*-------------------------------------------------------\
| Fun-06: freeGeneIndiceAry
|   - Frees an array of geneIndiceST structures.
| Input:
|   - genIndiceST:
|     o Pointer to genIndiceST structure arrray to free
|  - numElmUI:
|     o unsigned int with the number of elements in
|       genIndiceST
| Output:
|   - Frees:
|     o genIndiceST
|   - Sets:
|     o genIndiceST to 0
\-------------------------------------------------------*/
void freeGeneIndiceAry(
   struct genIndice **genIndiceST,
   unsigned long numElm
);

/*-------------------------------------------------------\
| Fun-10: genIndiceSort
|  - Sorts an array of genIndice structures by variant ids
|    with shell short.
| Input:
|  - genIndiceST:
|    o Pointer to genIndice structure array with variants
|      to sort
|  - startUI:
|    o First element to start sorting at
|  - endUI:
|    o Last element to sort (index 0)
| Output:
|  - Modifies:
|    o Arrays in genIndiceST to be sorted by the gene
|      starting coordinate (lowest first)
\-------------------------------------------------------*/
void sortGenIndice(
   struct genIndice *genIndiceST,
   unsigned int startUI,
   unsigned int endUI
);

/*-------------------------------------------------------\
| Fun-11: findGenIndiceVariant
|  - Does a binary search for the nearest amr at or after
|    to the input query coordiante
| Input:
|  - genIndiceAry:
|    o Pointer to an genIndice structured array  to find
|      the variant in
|  - qryStr:
|    o C-string with variant to search for
|  - lenAryI:
|    o Lenth of the genIndice structure array (index 1)
| Output:
|  - Returns:
|    o The index of the variant in the array
|    o -1 if the variant is not in the array
\-------------------------------------------------------*/
int findGenIndiceVariant(
   struct genIndice *genIndiceAry,
   char *qryStr,
   int lenAryI
);

/*-------------------------------------------------------\
| Fun-12: read_who2023_indiceTab
|   - Reads in the genome coordinates and variants from
|     the 2023 WHO TB catalog genome indice tab (saved
|     as a tsv)
| Input:
|   - indiceStr:
|     o C-string with the path to the tsv with the
|       genome indice tab (tab 2) of the who 2023 catalog
|   - numIndicesUI:
|     o Poiunter to unsigned int to put the number of
|       extracted entries in
| Output:
|   - Modifies:
|     o numIndicesUI to have the number of extracted
|       genome indices
|   - Returns:
|     o 0 for memory error
|     o Pointer to an array of genIndice structures
\-------------------------------------------------------*/
struct genIndice * read_who2023_indiceTabTsv(
   char *indiceStr,
   unsigned int *numIndicesUI
);

#endif
