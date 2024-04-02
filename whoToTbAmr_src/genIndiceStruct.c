/*#######################################################\
# Name: geneIndiceStruct
#   - Has functions for reading the genome indice (tab 2)
#     of the WHO's 2023 TB catalog.
\#######################################################*/

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
' SOF: Start Of File
'   o header: 
'     - Has included libraries
'   o .h st-01: genIndice
'     - Structure to hold a single genome indice from
'       the who 2023 catalog
'   o .h fun-01: blankGeneIndice
'     - Blanks the positon in an gene indice structure
'   o .h fun-02: initGeneIndice
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
'   o .c fun-07: cmpGenIndices
'     - Compares to see if one genIndice structure is
'       less than another
'   o .c fun-08: cmpGenIndiceToStr
'     - Compares to see if a genIndice structure is
'       less than a string
'   o .c fun-09: swapGenIndices
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
'   o license:
'     - Licensing for this code (public domain / mit)
\~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*-------------------------------------------------------\
| Header:
|   - Has the included libraries
\-------------------------------------------------------*/

#include "genIndiceStruct.h"

#ifdef PLAN9
   #include <u.h>
   #include <libc.h>
#else
   #include <stdlib.h>
#endif

#include <stdio.h>

/*All of these are just .h files*/
#include "../generalLib/ulCpStr.h"
#include "../generalLib/base10StrToNum.h"
#include "../generalLib/dataTypeShortHand.h"

/*-------------------------------------------------------\
| Fun-03: makeGenIndice
|   - Makes a new genIndice
| Input:
| Output:
|   - Returns:
|     o genIndice on heap
\-------------------------------------------------------*/
struct genIndice * makeGenIndice(){
   struct genIndice *retST =
      malloc(sizeof(struct genIndice));
   
   initGenIndice(retST);
   return retST;
} /*makeGenIndice*/

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
void freeGeneIndiceStack(struct genIndice *geneIndiceST){
   if((geneIndiceST)->varIdStr)
      free((geneIndiceST)->varIdStr);
   
   if((geneIndiceST)->refSeqStr)
      free((geneIndiceST)->refSeqStr);
   
   if((geneIndiceST)->amrSeqStr)
      free((geneIndiceST)->amrSeqStr);
   
   initGenIndice(geneIndiceST);
} /*freeGeneIndiceStack*/

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
void freeGeneIndice(struct genIndice **geneIndiceST){
   freeGeneIndiceStack(*(geneIndiceST));
   free(geneIndiceST);
   geneIndiceST = 0;
} /*freeGenIndice*/

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
){
   uint uiElm = 0;
   for(uiElm = 0; uiElm < numElm; ++uiElm)
      freeGeneIndiceStack(&(*genIndiceST)[uiElm]);
      /*This avoids odd memory access errors*/
   
   free(*genIndiceST);
   *genIndiceST = 0;
} /*freeGenIndiceAry*/

/*-------------------------------------------------------\
| Fun-07: cmpGenIndices
|   - Compares to see if one genIndice structure is
|     less than another
| Input:
|   - qryGenIndicePtr:
|     o Pointer to a query genIndice to compare
|   - refGenIndicePtr:
|     o Pointer to a reference genIndice to compare
| Output:
|   - Returns:
|     o < 0 if the query is less than the reference
|     o 0 if the query is the same as the reference
|     o > 0 if the query is greater than the reference
\-------------------------------------------------------*/
#define cmpGenIndices(qryGenIndicePtr, refGenIndicePtr)(\
   cStrEql(\
      (qryGenIndicePtr)->varIdStr,\
      (refGenIndicePtr)->varIdStr,\
      '\0'\
   )\
) /*cmpGenIndicies*/

/*-------------------------------------------------------\
| Fun-08: cmpGenIndiceToStr
|   - Compares to see if a genIndice structure is
|     less than a string
| Input:
|   - qryGenIndicePtr:
|     o Pointer to a query genIndice to compare
|   - refStr:
|     o C-string to compare
| Output:
|   - Returns:
|     o < 0 if the query is less than the reference
|     o 0 if the query is the same as the reference
|     o > 0 if the query is greater than the reference
\-------------------------------------------------------*/
#define cmpGenIndiceToStr(qryStr, refGenIndicePtr)(\
   cStrEql(qryStr, (refGenIndicePtr)->varIdStr, '\0')\
) /*cmpGenIndicies*/

/*-------------------------------------------------------\
| Fun-09: swapGenIndices
|  - Swaps two array items in a genIndice structure
|    around
| Input:
|  - genIndiceAry:
|    o Pointer to the geneIndice structure array to to
|      swap elements in
|  - posOne:
|    o The position (index) of the first gene to swap
|  - posTwo:
|    o The position (index) of the second gene to swap
| Output:
|  - Modifies:
|    o Swaps the pionters and values in the two structers
|      around
\-------------------------------------------------------*/
#define swapGenIndices(genIndiceAry, posOne, posTwo){ \
  uint macSwapUI = (genIndiceAry)[posOne].posUI;\
  char *macSwapStr = 0;\
  \
  /*Swap the reference position*/\
  (genIndiceAry)[posOne].posUI =\
     (genIndiceAry)[posTwo].posUI;\
  \
  (genIndiceAry)[posTwo].posUI = macSwapUI;\
  \
  \
  /*Swap the variant id*/\
  macSwapUI = (genIndiceAry)[posOne].lenVarIdUI;\
  macSwapStr = (genIndiceAry)[posOne].varIdStr;\
  \
  (genIndiceAry)[posOne].varIdStr =\
     (genIndiceAry)[posTwo].varIdStr;\
  \
  (genIndiceAry)[posOne].lenVarIdUI =\
     (genIndiceAry)[posTwo].lenVarIdUI;\
  \
  (genIndiceAry)[posTwo].varIdStr = macSwapStr;\
  (genIndiceAry)[posTwo].lenVarIdUI = macSwapUI;\
  \
  \
  /*Swap the reference sequence*/\
  macSwapUI = (genIndiceAry)[posOne].lenRefSeqUI;\
  macSwapStr = (genIndiceAry)[posOne].refSeqStr;\
  \
  (genIndiceAry)[posOne].lenRefSeqUI =\
     (genIndiceAry)[posTwo].lenRefSeqUI;\
  \
  (genIndiceAry)[posOne].refSeqStr =\
     (genIndiceAry)[posTwo].refSeqStr;\
  \
  (genIndiceAry)[posTwo].lenRefSeqUI = macSwapUI;\
  (genIndiceAry)[posTwo].refSeqStr = macSwapStr;\
  \
  \
  /*Swap the amr sequence*/\
  macSwapUI = (genIndiceAry)[posOne].lenAmrSeqUI;\
  macSwapStr = (genIndiceAry)[posOne].amrSeqStr;\
  \
  (genIndiceAry)[posOne].lenAmrSeqUI =\
     (genIndiceAry)[posTwo].lenAmrSeqUI;\
  \
  (genIndiceAry)[posOne].amrSeqStr =\
     (genIndiceAry)[posTwo].amrSeqStr;\
  \
  (genIndiceAry)[posTwo].lenAmrSeqUI = macSwapUI;\
  (genIndiceAry)[posTwo].amrSeqStr = macSwapStr;\
} /*swapScoreSTs*/

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
){ /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   ' Fun-10 TOC: sortGenIndice
   '  - Sorts the arrays in a genIndice struct by variant
   '    id with shell short.
   '  - Shell sort taken from:
   '    - Adam Drozdek. 2013. Data Structures and
   '      Algorithims in c++. Cengage Leraning. fourth
   '      edition. pages 505-508
   '    - I made some minor changes, but is mostly the
   '      same
   '  o fun-10 sec-01:
   '    - Variable declerations
   '  o fun-10 sec-02:
   '    - Find the number of rounds to sort for
   '  o fun-10 sec-03:
   '    - Sort the arrays in genIndiceST
   \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  
  /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
  ^ Fun-10 Sec-01:
  ^  - Variable declerations
  \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
  
  /*Number of elements to sort*/
  ulong numElmUL = (endUI) - (startUI);
  
  /*Number of sorting rounds*/
  ulong subUL = 0;
  ulong nextElmUL = 0;
  ulong lastElmUL = 0;
  ulong elmOnUL = 0;
  
  /*Get arrays to sort from the matrix (for sanity)*/
  
  /*Variables to incurment loops*/
  ulong ulIndex = 0;
  ulong ulElm = 0;
  
  /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
  ^ Fun-10 Sec-02:
  ^  - Find the max search value (number rounds to sort)
  \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
  
  /*Recursion formula: h[0] = 1, h[n] = 3 * h[n - 1] +1*/
  subUL = 1; /*Initialzie first array*/
  while(subUL < numElmUL - 1) subUL = (3 * subUL) + 1;
  
  /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  ^ Fun-10 Sec-03:
  ^  - Sort the arrays in genIndiceST
  \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
  
  while(subUL > 0)
  { /*loop trhough all sub arrays sort the subarrays*/
    for(ulIndex = 0; ulIndex <= subUL; ++ulIndex)
    { /*For each element in the subarray*/
      for(ulElm = ulIndex;
          ulElm + subUL <= endUI;
          ulElm += subUL
      ){ /*Loop; swap each nth element of the subarray*/
        nextElmUL = ulElm + subUL;
        
        if(cmpGenIndices(
              &(genIndiceST)[ulElm],
              &(genIndiceST)[nextElmUL]
           ) > 0
        ){ /*If I need to swap an element*/
          swapGenIndices((genIndiceST),ulElm,nextElmUL);
          
          lastElmUL = ulElm;
          elmOnUL = ulElm;
          
          while(lastElmUL >= subUL)
          { /*loop; move swapped element back*/
            lastElmUL -= subUL;
            
            if(cmpGenIndices(
                  &(genIndiceST)[elmOnUL],
                  &(genIndiceST)[lastElmUL]
               ) > 0
            ) break; /*Positioned the element*/
            
            swapGenIndices(
               (genIndiceST),
               elmOnUL,
               lastElmUL
            ); /*swap the two indices*/
            
            elmOnUL = lastElmUL;
          } /*loop; move swapped element back*/
        } /*If I need to swap elements*/
      } /*Loop; swap each nth element of the subarray*/
    } /*For each element in the subarray*/
    
    subUL = (subUL - 1) / 3; /*Move to the next round*/
  } /*loop through all sub arrays to sort the subarrays*/
} /*sortGenIndice*/

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
){ /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   ' Fun-11 TOC: findGenIndiceVariant
   '   - Finds the nearest amr at or after the input query
   '     coordiante
   \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

   int strDiffI = 0;
   int midI = 0; /*divide by 2*/
   int rightHalfI = lenAryI - 1;
   int leftHalfI = 0;

   while(leftHalfI <= rightHalfI)
   { /*Loop: Search for the starting coordinate*/
      midI = (leftHalfI + rightHalfI) >> 1;

      strDiffI =
         cmpGenIndiceToStr(qryStr, &genIndiceAry[midI]);

      if(strDiffI > 0) leftHalfI = midI + 1;
      else if(strDiffI < 0) rightHalfI = midI - 1;

     else
     { /*Else: I found the query*/
        foundResult:;

        while(midI > 0)
        { /*Loop: Make sure I am on the first amr*/

           strDiffI =
             cmpGenIndiceToStr(
                qryStr,
                &genIndiceAry[midI - 1]
             );

           if(strDiffI) break; /*At start of variant*/

           --midI;
        } /*Loop: Make sure I am on the first amr*/

        return midI;
     } /*Else: I found the query*/

     /*branchless whith an if return was slower here*/
   } /*Loop: Search for the starting coordinate*/

   if(!cmpGenIndiceToStr(qryStr, &genIndiceAry[midI]))
      goto foundResult;

   return -1; /*The variant is not in the array*/
} /*findGenIndiceVariant*/

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
|     o 0 for memory error or file errors
|     o Pointer to an array of genIndice structures
\-------------------------------------------------------*/
struct genIndice * read_who2023_indiceTabTsv(
   char *indiceStr,
   unsigned int *numIndicesUI
){ /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
   ' Fun-12 TOC: read_who2023_indiceTab
   '   - Reads in the genome coordinates and variants from
   '     the 2023 WHO TB catalog genome indice tab (saved
   '     as a tsv)
   '   o fun-12 Sec-01:
   '     - Variable declerations
   '   o fun-12 Sec-02:
   '     - Get the number of entries in the file
   '   o fun-12 Sec-03:
   '     - Read in each entry in the file
   '   o fun-12 Sec-04:
   '     - Clean up and return
   \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-12 Sec-01:
   ^   - Variable declerations
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   ushort lenBuffUS = 4096;
   char buffStr[lenBuffUS];
   char *tmpStr = 0;

   uint uiElm = 0;  /*indice entry on (excluding header)*/
   uint uiPos = 0; /*Positon in buffStr at*/
   struct genIndice *indiceSTAry =  0;

   FILE *indiceFILE = fopen(indiceStr, "r");

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-12 Sec-02:
   ^   - Get the number of entries in the file
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   if(indiceFILE == 0) return 0;

   *numIndicesUI = 0;

   while(fgets(buffStr, lenBuffUS, indiceFILE))
      ++(*numIndicesUI);

   fseek(indiceFILE, 0, SEEK_SET); /*go to start of file*/

   /*Ignore the header*/
   tmpStr = fgets(buffStr, lenBuffUS, indiceFILE);

   indiceSTAry =
      malloc(*numIndicesUI * sizeof(struct genIndice));

   if(!indiceSTAry)
   { /*If: I had a memory error*/
      fclose(indiceFILE);
      return 0; /*memory error*/
   } /*If: I had a memory error*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-12 Sec-03:
   ^   - Read in each entry in the file
   ^   o fun-12 sec-03 sub-01:
   ^     - Read in the variant id (also start loop)
   ^   o fun-12 sec-03 sub-02:
   ^     - Move past the reference/chromosome id
   ^   o fun-12 sec-03 sub-03:
   ^     - Get the refernce position
   ^   o fun-12 sec-03 sub-04:
   ^     - Get the refernce sequence
   ^   o fun-12 sec-03 sub-05:
   ^     - Get the amr sequence
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   /*****************************************************\
   * Fun-12 Sec-03 Sub-01:
   *   - Read in the variant id (also start loop)
   \*****************************************************/

   while(fgets(buffStr, lenBuffUS, indiceFILE))
   { /*Loop: Get each entry from the file*/
      initGenIndice(&indiceSTAry[uiElm]);
      uiPos = 0;

      indiceSTAry[uiElm].lenVarIdUI =
         cLenStr(&buffStr[uiPos],'\t');

      indiceSTAry[uiElm].varIdStr =
         malloc(
             (indiceSTAry[uiElm].lenVarIdUI + 1)
           * sizeof(char)
         ); /*Make the buffer for the reference sequence*/

      /*Check for memory errors*/
      if(!indiceSTAry[uiElm].varIdStr) goto memoryError;

      cCpStr(
         indiceSTAry[uiElm].varIdStr,
         &buffStr[uiPos],
         indiceSTAry[uiElm].lenVarIdUI
      ); /*Copy the variant id*/

      uiPos = indiceSTAry[uiElm].lenVarIdUI + 1;
        /*+1 to get of tab*/

      /**************************************************\
      * Fun-12 Sec-03 Sub-02:
      *   - Move past the reference/chromosome id
      \**************************************************/

      /*Move past the chromosome entry*/
      while(buffStr[uiPos] > 31) ++uiPos;

      ++uiPos; /*Get of tab*/

      /**************************************************\
      * Fun-12 Sec-03 Sub-03:
      *   - Get the refernce position
      \**************************************************/

      tmpStr = 
         base10StrToUI(
            &buffStr[uiPos],
           indiceSTAry[uiElm].posUI
         ); /*Get the genome coordiant*/

       /*Convert index 1 to index 0*/
       --(indiceSTAry[uiElm].posUI);

       uiPos = tmpStr + 1 - buffStr;

      /**************************************************\
      * Fun-12 Sec-03 Sub-04:
      *   - Get the refernce sequence
      \**************************************************/

      indiceSTAry[uiElm].lenRefSeqUI =
         cLenStr(&buffStr[uiPos],'\t');

      indiceSTAry[uiElm].refSeqStr =
         malloc(
             (indiceSTAry[uiElm].lenRefSeqUI + 1)
           * sizeof(char)
         ); /*Make the buffer for the reference sequence*/

      /*Check for memory errors*/
      if(!indiceSTAry[uiElm].refSeqStr) goto memoryError;

      cCpStr(
         indiceSTAry[uiElm].refSeqStr,
         &buffStr[uiPos],
         indiceSTAry[uiElm].lenRefSeqUI
      ); /*Copy the reference sequence*/

      uiPos += indiceSTAry[uiElm].lenRefSeqUI + 1;
         /*+1 to get off the tab*/

      /**************************************************\
      * Fun-12 Sec-03 Sub-05:
      *   - Get the amr sequence
      \**************************************************/

      indiceSTAry[uiElm].lenAmrSeqUI = uiPos;

      while(buffStr[indiceSTAry[uiElm].lenAmrSeqUI] > 31)
         ++indiceSTAry[uiElm].lenAmrSeqUI;

      indiceSTAry[uiElm].lenAmrSeqUI -= uiPos;

      indiceSTAry[uiElm].amrSeqStr =
         malloc(
             (indiceSTAry[uiElm].lenAmrSeqUI + 1)
           * sizeof(char)
         ); /*Make the buffer for the amr sequence*/

      /*Check for memory errors*/
      if(!indiceSTAry[uiElm].amrSeqStr) goto memoryError;

      cCpStr(
         indiceSTAry[uiElm].amrSeqStr,
         &buffStr[uiPos],
         indiceSTAry[uiElm].lenAmrSeqUI
      ); /*Copy the amr sequence*/

       ++uiElm;
   } /*Loop: Get each entry from the file*/

   goto cleanUp; /*No errors*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-12 Sec-04:
   ^   - Clean up and return
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   memoryError:;

   fclose(indiceFILE);
   freeGeneIndiceAry(&indiceSTAry, uiElm);
   return 0;

   cleanUp:;

   /*Sort the array by the variant; this is for easier
   ` lookups later
   */
   sortGenIndice(indiceSTAry, 0, uiElm - 1);
   *numIndicesUI = uiElm;
   
   fclose(indiceFILE);

   return indiceSTAry;
} /*read_who2023_indiceTabTsv*/

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
