/*########################################################
# Name: geneCoordStruct
#   - Holds the geneCoords struct and its supporting
#     functions I use to read in gene positions form a paf
#     file
########################################################*/

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
' SOF: Start Of File
'   o header:
'     - Includes and definitions
'   o st: geneCoords
'     - Holds the arrays for the gene coordinates and ids 
'       in a paf file
'   o fun-01: freeGeneCoordsStack
'     - Frees the arrays in a geneCoords structure
'   o fun-02: freeGeneCoords
'     - Frees the a heap alloacted geneCoords structure
'   o fun-03: initGeneCoords (.h only)
'     - Initializes a geneCoords structure
'   o fun-04: makeGeneCoords
'     - Makes a heap allocated geneCoords structure
'   o fun-05: getPafGene
'     - Get the id and coordinates for a gene from a paf
'       file
'   o fun-06: swapGeneCoord
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

#include "geneCoordStruct.h"

#include <stdio.h>

#include "dataTypeShortHand.h"
#include "base10StrToNum.h"
#include "genMath.h"

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
){
   if((geneCoordsST)->idStrAry != 0)
   { /*If: there are ids to free*/
      free((geneCoordsST)->idStrAry);
      (geneCoordsST)->idStrAry = 0;
   } /*If: there are ids to free*/
   
   if((geneCoordsST)->startAryUI != 0)
   { /*If: there are starting positions to free*/
      free((geneCoordsST)->startAryUI);
      (geneCoordsST)->startAryUI = 0;
   } /*If: there are starting positions to free*/
   
   if((geneCoordsST)->endAryUI != 0)
   { /*If: there are ending positions to free*/
      free((geneCoordsST)->endAryUI);
      (geneCoordsST)->endAryUI = 0;
   } /*If: there are ending positions to free*/
} /*freeGeneCoordsStack*/

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
){
   freeGeneCoordsStack(*(geneCoordsST));
   free(*(geneCoordsST));
   *(geneCoordsST) = 0;
} /*freeGeneCoords*/

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
){
   struct geneCoords *retST =
      malloc(sizeof(struct geneCoords));
   
   if(retST != 0)
   { /*If: I had allocated a structure*/
      
      initGeneCoords(retST);
      
      retST->idStrAry =
         malloc((numGenesUI) * sizeof(*retST->idStrAry));
      
      if(retST->idStrAry != 0)
      { /*If: I had no errors for the gene ids array*/
         retST->startAryUI =
            malloc((numGenesUI) * sizeof(uint));
         
         if(retST->startAryUI ==0)
           {freeGeneCoords(&retST);}
         
         else
         { /*Else: I can try getting memory for the end*/
            retST->endAryUI =
               malloc((numGenesUI) * sizeof(uint));
            
            if(retST->startAryUI == 0)
               {freeGeneCoords(&retST);}
         } /*Else: I can try getting memory for the end*/
      } /*If: I had no errors for the gene ids array*/
   } /*If: I had allocated a structure*/
   
   return retST;
} /*makeGeneCoords*/

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
){ /*getPagGene*/
   uchar ucEntry = 0;
   uint uiChar = 0;
   char *tmpStr = 0;
   
   for(uiChar = 0; (pafLineStr)[uiChar] > 32; ++uiChar)
      (geneCoordsST)->idStrAry[(posUI)][uiChar] =
         (pafLineStr)[uiChar];
   
   (geneCoordsST)->idStrAry[(posUI)][uiChar] = '\0';
   ++uiChar;
   
   for(ucEntry = 1; ucEntry < 7; ++ucEntry)
   { /*Loop: Move to the reference start position*/
      for(
         uiChar = uiChar;
         (pafLineStr)[uiChar] > 32;
         ++uiChar
      ){} /*Move to the next entry (tab)*/
      
      ++uiChar;
   } /*Loop: Move to the reference start position*/
   
   tmpStr =
      base10StrToUI(
         &(pafLineStr)[uiChar],
         (geneCoordsST)->startAryUI[(posUI)]
      ); /*Get the starting position*/
   
   ++tmpStr; /*Move off the tab*/
   ++ucEntry;
   
   tmpStr =
      base10StrToUI(
         tmpStr,
         (geneCoordsST)->endAryUI[(posUI)]
      ); /*Get the ending position*/
   
   ++tmpStr; /*Move off the tab*/
   ++ucEntry;
   
   /*Get the alignment type (P for primary)*/
   uiChar = 0;
   while(tmpStr[uiChar] != 't'
      && tmpStr[uiChar + 1] != 'p'
      && tmpStr[uiChar + 2] != ':'
   ) ++uiChar;

   *(typeC) = tmpStr[uiChar + 5];
} /*getPafGene*/

/*-------------------------------------------------------\
| Fun-06: swapGeneCoord
|  - Swaps two array items in a geneCoords structure
|    around
| Input:
|  - geneCoordST:
|    o Pointer to the geneCoord structure to swap elements
|      in
|  - posOne:
|    o The position (index) of the first gene to swap
|  - posTwo:
|    o The position (index) of the second gene to swap
| Output:
|  - Modifies:
|    o Swaps the start, end, and gene id around
\-------------------------------------------------------*/
#define swapGeneCoord(geneCoordST, posOne, posTwo){ \
  uint macStartUI = (geneCoordST)->startAryUI[(posOne)];\
  uint macEndUI = (geneCoordST)->endAryUI[(posOne)];\
  char macSwapC = 0;\
  uchar cMac = 0;\
  \
  (geneCoordST)->startAryUI[(posOne)] =\
     (geneCoordST)->startAryUI[(posTwo)];\
   \
  (geneCoordST)->endAryUI[(posOne)] =\
     (geneCoordST)->endAryUI[(posTwo)];\
  \
  (geneCoordST)->startAryUI[(posTwo)] = macStartUI;\
  (geneCoordST)->endAryUI[(posTwo)] = macEndUI;\
  \
  while(\
        (geneCoordST)->idStrAry[(posOne)][cMac] != '\0'\
     || (geneCoordST)->idStrAry[(posTwo)][cMac] != '\0'\
  ){ /*Loop: copy the gene name*/\
     macSwapC =(geneCoordST)->idStrAry[(posOne)][cMac];\
     \
     (geneCoordST)->idStrAry[(posOne)][cMac] =\
        (geneCoordST)->idStrAry[(posTwo)][cMac];\
     \
     (geneCoordST)->idStrAry[(posTwo)][cMac] = macSwapC;\
     ++cMac;\
  } /*Loop: copy the gene name*/\
  \
  (geneCoordST)->idStrAry[(posOne)][cMac] = '\0';\
  (geneCoordST)->idStrAry[(posTwo)][cMac] = '\0';\
} /*swapScoreSTs*/

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
){ /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   ' Fun-07 TOC: sortGeneCoords
   '  - Sorts the arrays in a geneCoords struct by
   '    starting position with shell short.
   '  - Shell sort taken from:
   '    - Adam Drozdek. 2013. Data Structures and
   '      Algorithims in c++. Cengage Leraning. fourth
   '      edition. pages 505-508
   '    - I made some minor changes, but is mostly the
   '      same
   '  o fun-07 sec-01:
   '    - Variable declerations
   '  o fun-07 sec-02:
   '    - Find the number of rounds to sort for
   '  o fun-07 sec-03:
   '    - Sort the arrays in geneCoordST
   \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  
  /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
  ^ Fun-07 Sec-01:
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
  ^ Fun-07 Sec-02:
  ^  - Find the max search value (number rounds to sort)
  \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
  
  /*Recursion formula: h[0] = 1, h[n] = 3 * h[n - 1] +1*/
  subUL = 1; /*Initialzie first array*/
  while(subUL < numElmUL - 1) subUL = (3 * subUL) + 1;
  
  /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  ^ Fun-07 Sec-03:
  ^  - Sort the arrays in geneCoordST
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
        
        if(   (geneCoordST)->startAryUI[ulElm]
            > (geneCoordST)->startAryUI[nextElmUL]
        ){ /*If I need to swap an element*/
          swapGeneCoord((geneCoordST),ulElm,nextElmUL);
          
          lastElmUL = ulElm;
          elmOnUL = ulElm;
          
          while(lastElmUL >= subUL)
          { /*loop; move swapped element back*/
            lastElmUL -= subUL;
            
            if(   (geneCoordST)->startAryUI[elmOnUL]
                > (geneCoordST)->startAryUI[lastElmUL]
            ) break; /*Positioned the element*/
            
            swapGeneCoord((geneCoordST),elmOnUL,lastElmUL);
            
            elmOnUL = lastElmUL;
          } /*loop; move swapped element back*/
        } /*If I need to swap elements*/
      } /*Loop; swap each nth element of the subarray*/
    } /*For each element in the subarray*/
    
    subUL = (subUL - 1) / 3; /*Move to the next round*/
  } /*loop through all sub arrays to sort the subarrays*/
} /*sortGeneCoords*/

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
   uint qryUI,
   int numGenesI
){ /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   ' Fun-08 TOC: findStartCoordInGeneCoords
   '  - Finds the starting coordinate (query) in a
   '    geneCoords structure
   '  o fun-08 sec-01:
   '    - Variable declerations
   '  o fun-08 sec-02:
   \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

   int midI = 0;
   int rightHalfI = numGenesI - 1;
   int leftHalfI = 0;

   while(leftHalfI <= rightHalfI)
   { /*Loop: Search for the starting coordinate*/
      midI = (leftHalfI + rightHalfI) >> 1;

      if(   qryUI < geneST->startAryUI[midI]
         && qryUI < geneST->endAryUI[midI]
      )  rightHalfI = midI - 1;

      else if(
            qryUI > geneST->startAryUI[midI]
         && qryUI > geneST->endAryUI[midI]
      ) leftHalfI = midI + 1;

     else return midI;
   } /*Loop: Search for the starting coordinate*/

   if(   qryUI < geneST->startAryUI[midI]
      && qryUI < geneST->endAryUI[midI]
   ) return -1;

   if(   qryUI > geneST->startAryUI[midI]
      && qryUI > geneST->endAryUI[midI]
   ) return -1;

   return midI;
} /*findStartCoordInGeneCoords*/

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
   int *numGenesI  /*Number of genes extracted*/\
){
   ushort lenBuffUS = 1024;
   char buffStr[lenBuffUS];
   
   char alnTypeC = 0;     /*To check alignment type*/
   ulong numLinesUL = 0;
   
   struct geneCoords *genesST = 0;
   
   /*Find the number of entries in the paf file*/
   while(fgets(buffStr, lenBuffUS, (FILE *) pafFILE))
      ++numLinesUL;
   
   /*Extract each entry*/
   fseek((pafFILE), 0, SEEK_SET);
   genesST = makeGeneCoords(numLinesUL);
   
   if(genesST)
   { /*If: I did not have a memory error*/
      while(fgets(buffStr, lenBuffUS, (FILE *) pafFILE)
      ){ /*Loop: Get entries from the paf file*/
         /*Get the gene locations from the paf line*/
         getPafGene(
            genesST,
            *(numGenesI),
            &alnTypeC,
            buffStr
         );
         
         (*numGenesI) += (alnTypeC == 'P');
      } /*Loop: Get entries from the paf file*/
      
      --(*numGenesI); /*Convert to index 0*/
      sortGeneCoords(genesST, 0, *(numGenesI));
      fseek((FILE *) pafFILE, 0, SEEK_SET);
   } /*If: I did not have a memory error*/

   return genesST;
}

