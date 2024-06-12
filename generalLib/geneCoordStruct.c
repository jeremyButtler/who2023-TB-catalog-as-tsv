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
'   o .h st: geneCoords
'     - Holds the arrays for the gene coordinates and ids 
'       in gene table
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
#include "ulCpStr.h"

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
){
   free((geneCoordsST)->idStrAry);
   (geneCoordsST)->idStrAry = 0;
   
   free((geneCoordsST)->startAryUI);
   (geneCoordsST)->startAryUI = 0;
   
   free((geneCoordsST)->endAryUI);
   (geneCoordsST)->endAryUI = 0;

   free((geneCoordsST)->dirAryUC);
   (geneCoordsST)->dirAryUC = 0;
} /*freeGeneCoordsStack*/

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
){
   if(geneCoordsST) freeGeneCoordsStack(geneCoordsST);
   free(geneCoordsST);
} /*freeGeneCoords*/

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
){
   struct geneCoords *retST =
      malloc(sizeof(struct geneCoords));
   
   if(! retST)
      goto memErr_fun04;

   initGeneCoords(retST);
   
   retST->idStrAry =
      malloc((numGenesUI) * sizeof(*retST->idStrAry));
   
   if(! retST->idStrAry)
      goto memErr_fun04;

   retST->startAryUI= malloc((numGenesUI) * sizeof(uint));
   
   if(! retST->startAryUI)
      goto memErr_fun04;
   
   retST->endAryUI = malloc((numGenesUI) * sizeof(uint));
   
   if(! retST->endAryUI)
      goto memErr_fun04;

   retST->dirAryUC = malloc((numGenesUI) * sizeof(uchar));
   
   if(! retST->dirAryUC)
      goto memErr_fun04;

   return retST;

   memErr_fun04:;

   freeGeneCoords(retST);
   retST = 0;
   return 0;
} /*makeGeneCoords*/

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
      /*Move off the tab*/
      while((pafLineStr)[uiChar++] > 32) ;
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
| Fun06: swapGeneCoord
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
  char macSwapC =(char)(geneCoordST)->dirAryUC[(posOne)];\
  uchar cMac = 0;\
  \
  (geneCoordST)->startAryUI[(posOne)] =\
     (geneCoordST)->startAryUI[(posTwo)];\
  \
  (geneCoordST)->endAryUI[(posOne)] =\
     (geneCoordST)->endAryUI[(posTwo)];\
  \
  (geneCoordST)->dirAryUC[(posOne)] =\
     (geneCoordST)->dirAryUC[(posTwo)];\
  \
  (geneCoordST)->startAryUI[(posTwo)] = macStartUI;\
  (geneCoordST)->endAryUI[(posTwo)] = macEndUI;\
  (geneCoordST)->dirAryUC[(posTwo)] = (uchar) macSwapC;\
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
){ /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   ' Fun07 TOC: sortGeneCoords
   '  - Sorts the arrays in a geneCoords struct by
   '    starting position with shell short.
   '  - Shell sort taken from:
   '    - Adam Drozdek. 2013. Data Structures and
   '      Algorithims in c++. Cengage Leraning. fourth
   '      edition. pages 505-508
   '    - I made some minor changes, but is mostly the
   '      same
   '  o fun07 sec01:
   '    - Variable declerations
   '  o fun07 sec02:
   '    - Find the number of rounds to sort for
   '  o fun07 sec03:
   '    - Sort the arrays in geneCoordST
   \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  
  /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
  ^ Fun07 Sec01:
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
  ^ Fun07 Sec02:
  ^  - Find the max search value (number rounds to sort)
  \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
  
  /*Recursion formula: h[0] = 1, h[n] = 3 * h[n - 1] +1*/
  subUL = 1; /*Initialzie first array*/
  while(subUL < numElmUL - 1) subUL = (3 * subUL) + 1;
  
  /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  ^ Fun07 Sec03:
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
   uint qryUI,
   int numGenesI
){ /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   ' Fun08 TOC: findStartCoordInGeneCoords
   '  - Finds the starting coordinate (query) in a
   '    geneCoords structure
   '  o fun08 sec01:
   '    - Variable declerations
   '  o fun08 sec02:
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
){ /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   ' Fun09 TOC:
   '  - Sorts the arrays in a genesCoord structure by
   '    gene name
   '  - Shell sort taken from:
   '    - Adam Drozdek. 2013. Data Structures and
   '      Algorithims in c++. Cengage Leraning. fourth
   '      edition. pages 505-508
   '    - I made some minor changes, but is mostly the
   '      same
   '  o fun09 sec01:
   '    - Variable declerations
   '  o fun09 sec02:
   '    - Find the number of rounds to sort for
   '  o fun09 sec03:
   '    - Sort the arrays in geneCoordST
   \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  
  /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
  ^ Fun09 Sec01:
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
  ^ Fun09 Sec02:
  ^  - Find the max search value (number rounds to sort)
  \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
  
  /*Recursion formula: h[0] = 1, h[n] = 3 * h[n - 1] +1*/
  subUL = 1; /*Initialzie first array*/
  while(subUL < numElmUL - 1) subUL = (3 * subUL) + 1;
  
  /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  ^ Fun09 Sec03:
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
        
        if(
            cStrEql(
               geneCoordST->idStrAry[ulElm],
               geneCoordST->idStrAry[nextElmUL],
               0
            ) > 0
        ){ /*If I need to swap an element*/
          swapGeneCoord((geneCoordST),ulElm,nextElmUL);
          
          lastElmUL = ulElm;
          elmOnUL = ulElm;
          
          while(lastElmUL >= subUL)
          { /*loop; move swapped element back*/
            lastElmUL -= subUL;
            
            if(
                cStrEql(
                   geneCoordST->idStrAry[elmOnUL],
                   geneCoordST->idStrAry[lastElmUL],
                   0
                ) > 0
            ) break; /*Positioned the element*/
            
            swapGeneCoord((geneCoordST),elmOnUL,lastElmUL);
            
            elmOnUL = lastElmUL;
          } /*loop; move swapped element back*/
        } /*If I need to swap elements*/
      } /*Loop; swap each nth element of the subarray*/
    } /*For each element in the subarray*/
    
    subUL = (subUL - 1) / 3; /*Move to the next round*/
  } /*loop through all sub arrays to sort the subarrays*/
} /*geneCoords_sortNames*/

/*-------------------------------------------------------\
| Fun10: geneCoords_findName
|  - Does a binary search to find an gene name in an gene
|    geneCoords structer (must be sorted by name)
| Input:
|  - geneCoordST:
|    o Pointer to geneCoords structure to sort
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
){ 
   sint matchSI = 0;
   sint midSI = 0;
   sint rightHalfSI = numGenesSI - 1;
   sint leftHalfSI = 0;

   while(leftHalfSI <= rightHalfSI)
   { /*Loop: Search for the starting coordinate*/
      midSI = (leftHalfSI + rightHalfSI) >> 1;

      matchSI =
         cStrEql(
            nameStr,
            geneST->idStrAry[midSI],
            '\0'
         ); /*see if I have an match*/

      if(matchSI < 0)
         rightHalfSI = midSI - 1;

      else if(matchSI > 0)
         leftHalfSI = midSI + 1;

     else
        return midSI;
   } /*Loop: Search for the starting coordinate*/

   return -1; /*No match found*/
} /*geneCoords_findCloseGene*/

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
){ /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
   ' Fun11 TOC: getGeneCoords
   '   - Gets the gene coordinates from a gene table (tsv)
   '   o fun11 Sec01:
   '     - Variable declerations
   '   o fun11 Sec02:
   '     - Check input and allocate memory for buffer
   '   o fun11 Sec03:
   '     - Find number lines/max line length in table file
   '   o fun11 Sec04:
   '     - Allocate memory and go back to start of file
   '   o fun11 Sec05:
   '     - Read in the gene coordinates from the file
   '   o fun11 Sec06:
   '     - Clean up and return
   \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun11 Sec01:
   ^   - Variable declerations
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   int lenBuffSI = 1 << 10; /*~ 2000*/
   char *buffHeapStr = 0;
   
   char newLineBl = 0;     /*Check if on an new line*/
   char *cpStr = 0;
   char *dupStr = 0;

   int numLinesSI = 0;
   int maxLineLenSI = 0;
   int lineLenSI = 0;

   ulong bytesUL = 0;
   ulong ulByte = 0;
   int tmpUI = 0;
   
   struct geneCoords *genesHeapST = 0;

   FILE *tblFILE  = 0;
   
   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun11 Sec02:
   ^   - Check input and allocate memory for buffer
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   *numGenesSI = 0;
   *errULPtr = 0;

   tblFILE  = fopen(geneTblFileStr, "r");

   if(! tblFILE)
      goto fileErr_fun09;

   buffHeapStr  = malloc((lenBuffSI + 1) * sizeof(char));

   if(! buffHeapStr)
       goto memErr_fun09;

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun11 Sec03:
   ^   - Find number lines/max line length in table file
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   bytesUL =
      fread(
         buffHeapStr,
         sizeof(char),
         lenBuffSI,
         tblFILE
      ); /*Read in the first part of the file*/

   while(bytesUL > 0)
   { /*Loop: Scan through the file for new lines*/

      ulByte = 0;
      buffHeapStr[bytesUL] = '\0';
         /*I made sure I always have one byte to spare*/

      while(buffHeapStr[ulByte] != '\0')
      { /*Loop: Count the number of newlines in buffer*/
         tmpUI = ulEndStrLine(&buffHeapStr[ulByte]);
         ulByte += tmpUI;
         newLineBl = buffHeapStr[ulByte] == '\n';
         numLinesSI += newLineBl;

         lineLenSI += tmpUI;

         maxLineLenSI =
            noBranchMax(
               maxLineLenSI,
               lineLenSI
            );

         lineLenSI &= ((int) (newLineBl - 1));
         ulByte += newLineBl; /*get off new line*/
      } /*Loop: Count the number of newlines in buffer*/
            

      bytesUL =
         fread(
            buffHeapStr,
            sizeof(char),
            lenBuffSI,
            tblFILE
         ); /*Read in the next part of the file*/
   } /*Loop: Scan through the file for new lines*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun11 Sec04:
   ^   - Allocate memory and go back to start of file
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   /*Make sure I have a full sized buffer*/
   if(lenBuffSI < maxLineLenSI)
   { /*If: I need to make an larger buffer*/
      free(buffHeapStr);
      buffHeapStr = 0;

      lenBuffSI = maxLineLenSI;
      buffHeapStr = malloc((lenBuffSI +1) * sizeof(char));

      if(! buffHeapStr)
         goto memErr_fun09;
   } /*If: I need to make an larger buffer*/

   genesHeapST = makeGeneCoords(numLinesSI + 1);
      /*+ 1 to account for no header*/
   
   if(! genesHeapST)
      goto memErr_fun09;

   /*Extract each entry*/
   fseek((tblFILE), 0, SEEK_SET);

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun11 Sec05:
   ^   - Read in the gene coordinates from the file
   ^   o fun11 sec05 sub01:
   ^     - Start loop and copy gene name
   ^   o fun11 sec05 sub02:
   ^     - Move past the refernce id
   ^   o fun11 sec05 sub03:
   ^     - Move past the gene direction
   ^   o fun11 sec05 sub04:
   ^     - Get coordiante of frist reference base in gene
   ^   o fun11 sec05 sub05:
   ^     - Get coordiante of last reference base in gene
   ^   o fun11 sec05 sub06:
   ^     - Move to the next gene
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   /*****************************************************\
   * Fun11 Sec05 Sub01:
   *   - Start loop and copy gene name
   \*****************************************************/

   while(fgets(buffHeapStr, lenBuffSI, tblFILE))
   { /*Loop: Get entries from the paf file*/

      cpStr = buffHeapStr;
      dupStr = genesHeapST->idStrAry[*numGenesSI];

      /*Copy the gene name*/
      while(*cpStr > 32)
         *dupStr++ = *cpStr++;

      *dupStr = '\0';

      if(*cpStr == '\t') ;      /*Expected*/
      else if(*cpStr == ' ') ;  /*Odd, but works*/
      else                      /*new line or null*/
         goto invalidEntry_fun09;

      ++cpStr; /*get off the tab*/

      /**************************************************\
      * Fun11 Sec05 Sub02:
      *   - Move past the refernce id
      \**************************************************/

      while(*cpStr++ > 32) ; /*Move past reference id*/

      if(*(cpStr - 1) == '\t') ;      /*Expected*/
      else if(*(cpStr - 1) == ' ') ;  /*Odd, but works*/
      else                            /*new line or null*/
         goto invalidEntry_fun09;

      /**************************************************\
      * Fun11 Sec05 Sub03:
      *   - get the gene direction
      \**************************************************/

      genesHeapST->dirAryUC[*numGenesSI] = (*cpStr =='-');
      genesHeapST->dirAryUC[*numGenesSI] |= (*cpStr=='R');

      while(*cpStr++ > 32) ;

      if(*(cpStr - 1) == '\t') ;      /*Expected*/
      else if(*(cpStr - 1) == ' ') ;  /*Odd, but works*/
      else                            /*new line or null*/
         goto invalidEntry_fun09;

      /**************************************************\
      * Fun11 Sec05 Sub04:
      *   - Get coordiante of frist reference base in gene
      \**************************************************/

      cpStr =
         base10StrToSI(
            cpStr,
            genesHeapST->startAryUI[*numGenesSI]
         ); /*Get the genes frist reference base*/

      --genesHeapST->startAryUI[*numGenesSI];

      if(*cpStr == '\t') ;      /*Expected*/
      else if(*cpStr == ' ') ;  /*Odd, but works*/
      else                      /*new line or null*/
         goto invalidEntry_fun09;

      ++cpStr; /*Get off the tab*/

      /**************************************************\
      * Fun11 Sec05 Sub05:
      *   - Get coordiante of last reference base in gene
      \**************************************************/

      cpStr =
         base10StrToSI(
            cpStr,
            genesHeapST->endAryUI[*numGenesSI]
         ); /*Get the genes last reference base*/

      --genesHeapST->endAryUI[*numGenesSI];

      if(*cpStr > 32)
         goto invalidEntry_fun09; /*Not an tsv*/

      if(
           genesHeapST->endAryUI[*numGenesSI]
         < genesHeapST->startAryUI[*numGenesSI]
      ){ /*If: I need to coordinates*/
         genesHeapST->endAryUI[*numGenesSI] ^=
            genesHeapST->startAryUI[*numGenesSI];

         genesHeapST->startAryUI[*numGenesSI] ^=
            genesHeapST->endAryUI[*numGenesSI];

         genesHeapST->endAryUI[*numGenesSI] ^=
            genesHeapST->startAryUI[*numGenesSI];
      } /*If: I need to swap coordinates*/

      /**************************************************\
      * Fun11 Sec05 Sub06:
      *   - Move to the next gene
      \**************************************************/

      ++(*numGenesSI);
   } /*Loop: Get entries from the paf file*/
   
   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun11 Sec06:
   ^   - Clean up and return
   ^   o fun11 sec06 sub01:
   ^     - Clean up and return after success
   ^   o fun11 sec06 sub02:
   ^     - Deal with memory errors
   ^   o fun11 sec06 sub03:
   ^     - Deal with file opening errors
   ^   o fun11 sec06 sub04:
   ^     - Deal with invalid entries in files
   ^   o fun11 sec06 sub05:
   ^     - Clean up and return after an error
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   /*****************************************************\
   * Fun11 Sec06 Sub01:
   *   - Clean up and return after success
   \*****************************************************/

   --(*numGenesSI); /*Convert to index 0*/
   sortGeneCoords(genesHeapST, 0, *(numGenesSI));

   free(buffHeapStr);
   buffHeapStr = 0;

   fclose(tblFILE);
   tblFILE = 0;

   return genesHeapST;

   /*****************************************************\
   * Fun11 Sec06 Sub02:
   *   - Deal with memory errors
   \*****************************************************/

   memErr_fun09:;

   *errULPtr = def_memErr_geneCoord;
   goto errCleanUp_fun09;

   /*****************************************************\
   * Fun11 Sec06 Sub03:
   *   - Deal with file opening errors
   \*****************************************************/

   fileErr_fun09:;

   *errULPtr = def_fileErr_geneCoord;
   goto errCleanUp_fun09;

   /*****************************************************\
   * Fun11 Sec06 Sub04:
   *   - Deal with invalid entries in files
   \*****************************************************/

   invalidEntry_fun09:;

   *errULPtr = def_invalidEntry_geneCoord;
   *errULPtr |= (*numGenesSI << 8);
   goto errCleanUp_fun09;

   /*****************************************************\
   * Fun11 Sec06 Sub05:
   *   - Clean up and return after an error
   \*****************************************************/

   errCleanUp_fun09:;

   free(buffHeapStr);
   buffHeapStr = 0;

   freeGeneCoords(genesHeapST);
   genesHeapST = 0;

   if(tblFILE) fclose(tblFILE);
   tblFILE = 0;

   return 0;
} /*getGeneCoords*/

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
