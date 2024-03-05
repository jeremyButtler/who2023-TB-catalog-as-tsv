#include <stdlib.h>
#include <stdio.h>

#include "amrStruct.h"
#include "../generalLib/dataTypeShortHand.h"
#include "drug_str_ary.h"

/*-------------------------------------------------------\
| Fun-04: freeAmrStructStack
|   - Frees the geneIdStr, refSeqStr, and amrSeqStr arrays
|     in amrStructPtr and sets all values to 0
| Input:
|   - ampStructPtr:
|     - Pointer to amrStruct to free memory from
| Output:
|   - Frees:
|     o refSeqStr, amrSeqStr, and geneIdStr (one buffer)
|   - Modifies:
|     o All values in amrStructPtr to be 0
\-------------------------------------------------------*/
void freeAmrStructStack(struct amrStruct *amrStructPtr){
   if((amrStructPtr)->geneIdStr != 0)
     free((amrStructPtr)->geneIdStr);
   
   if((amrStructPtr)->refSeqStr != 0)
     free((amrStructPtr)->refSeqStr);
   
   if((amrStructPtr)->amrSeqStr != 0)
     free((amrStructPtr)->amrSeqStr);
   
   if((amrStructPtr)->varIdStr != 0)
     free((amrStructPtr)->varIdStr);
   
   if((amrStructPtr)->effectStr != 0)
     free((amrStructPtr)->effectStr);
   
   if((amrStructPtr)->commentStr != 0)
     free((amrStructPtr)->commentStr);
   
   if((amrStructPtr)->refAaStr != 0)
      free((amrStructPtr)->refAaStr);
   
   if((amrStructPtr)->amrAaStr != 0)
      free((amrStructPtr)->amrAaStr);

   if((amrStructPtr)->needsGeneStr != 0)
      free((amrStructPtr)->needsGeneStr);
   
   initAmrStruct((amrStructPtr));
} /*freeAmrStructStack*/

/*-------------------------------------------------------\
| Fun-05: freeAmrStruct
|   - Frees an heap allocated amrStruct structure
| Input:
|   - ampStructPtr:
|     - Pointer to amrStruct to free
| Output:
|   - Frees:
|     o amrStructPtr and the variables inside amrStructPtr
\-------------------------------------------------------*/
void freeAmrStruct(struct amrStruct **amrStructPtr){
   freeAmrStructStack((*amrStructPtr));
   free(*amrStructPtr);
   *amrStructPtr = 0;
} /*freeAmrStruct*/

/*-------------------------------------------------------\
| Fun-06: freeAmrStructArray
|   - Frees an heap allocated array of amrStruct
|     structures
| Input:
|   - amrSTAry:
|     - Pointer to start of amrStruct array to free
| Output:
|   - Frees:
|     o The amrStructPtr array and the variables inside
|       each amrStructPtr
\-------------------------------------------------------*/
void freeAmrStructArray(
   struct amrStruct **amrSTAry, /*heap Array to free*/
   ulong numElmUL               /*number of elements*/
){
   ulong ulElm = 0;

   for(ulElm = 0; ulElm < numElmUL; ++ulElm)
      freeAmrStructStack(&(*amrSTAry)[ulElm]);
      /*This avoids odd memory access errors*/
   
   free(*amrSTAry);
   *amrSTAry = 0;
} /*freeAmrStructArray*/

/*-------------------------------------------------------\
| Fun-07: swapAmrStructs
|   - Swaps the values in two amrStruct structures
| Input:
|   - firstAmrSTPtr:
|     - Pointer to first amrStruct to swap values in
|   - secAmrSTPtr:
|     - Pointer to second amrStruct to swap values in
| Output:
|   - Modifies:
|     o fristAmrSTPtr to have the values of secAmrSTPtr
|     o secAmrSTPtr to have the values of frstAmrSTPtr
\-------------------------------------------------------*/
#define swapAmrStructs(firstAmrSTPtr, secAmrSTPtr){\
   char tmpMacC = 0;\
   char *tmpMacStr = 0;\
   uint tmpMacUI = 0;\
   ulong tmpMacUL = 0;\
   int iAmrMac = 0;\
   \
   \
   tmpMacUI = (firstAmrSTPtr).refPosUI;\
   (firstAmrSTPtr).refPosUI = (secAmrSTPtr).refPosUI;\
   (secAmrSTPtr).refPosUI = tmpMacUI;\
   \
   tmpMacUI = (firstAmrSTPtr).codonPosUI;\
   (firstAmrSTPtr).codonPosUI = (secAmrSTPtr).codonPosUI;\
   (secAmrSTPtr).codonPosUI = tmpMacUI;\
   \
   tmpMacUI = (firstAmrSTPtr).codonNumUI;\
   (firstAmrSTPtr).codonNumUI = (secAmrSTPtr).codonNumUI;\
   (secAmrSTPtr).codonNumUI = tmpMacUI;\
   \
   tmpMacUI = (firstAmrSTPtr).endCodonNumUI;\
   (firstAmrSTPtr).endCodonNumUI =\
      (secAmrSTPtr).endCodonNumUI;\
   (secAmrSTPtr).endCodonNumUI = tmpMacUI;\
   \
   tmpMacUI = (firstAmrSTPtr).geneFirstRefUI;\
   (firstAmrSTPtr).geneFirstRefUI =\
      (secAmrSTPtr).geneFirstRefUI;\
   (secAmrSTPtr).geneFirstRefUI = tmpMacUI;\
   \
   tmpMacUI = (firstAmrSTPtr).geneLastRefUI;\
   (firstAmrSTPtr).geneLastRefUI =\
      (secAmrSTPtr).geneLastRefUI;\
   (secAmrSTPtr).geneLastRefUI = tmpMacUI;\
   \
   tmpMacUI = (firstAmrSTPtr).codonNumUI;\
   (firstAmrSTPtr).codonNumUI = (secAmrSTPtr).codonNumUI;\
   (secAmrSTPtr).codonNumUI = tmpMacUI;\
   \
   \
   tmpMacStr = (firstAmrSTPtr).refAaStr;\
   (firstAmrSTPtr).refAaStr = (secAmrSTPtr).refAaStr;\
   (secAmrSTPtr).refAaStr = tmpMacStr;\
   \
   tmpMacUI = (firstAmrSTPtr).lenRefAaUI;\
   (firstAmrSTPtr).lenRefAaUI = (secAmrSTPtr).lenRefAaUI;\
   (secAmrSTPtr).lenRefAaUI = tmpMacUI;\
   \
   \
   tmpMacStr = (firstAmrSTPtr).amrAaStr;\
   (firstAmrSTPtr).amrAaStr = (secAmrSTPtr).amrAaStr;\
   (secAmrSTPtr).amrAaStr = tmpMacStr;\
   \
   tmpMacUI = (firstAmrSTPtr).lenAmrAaUI;\
   (firstAmrSTPtr).lenAmrAaUI = (secAmrSTPtr).lenAmrAaUI;\
   (secAmrSTPtr).lenAmrAaUI = tmpMacUI;\
   \
   \
   tmpMacC = (firstAmrSTPtr).aaDelBl;\
   (firstAmrSTPtr).aaDelBl = (secAmrSTPtr).aaDelBl;\
   (secAmrSTPtr).aaDelBl = tmpMacC;\
   \
   tmpMacC = (firstAmrSTPtr).frameshiftBl;\
   (firstAmrSTPtr).frameshiftBl =\
      (secAmrSTPtr).frameshiftBl;\
   (secAmrSTPtr).frameshiftBl = tmpMacC;\
   \
   \
   tmpMacStr = (firstAmrSTPtr).geneIdStr;\
   (firstAmrSTPtr).geneIdStr = (secAmrSTPtr).geneIdStr;\
   (secAmrSTPtr).geneIdStr = tmpMacStr;\
   \
   tmpMacUI = (firstAmrSTPtr).lenGeneIdUI;\
   (firstAmrSTPtr).lenGeneIdUI=(secAmrSTPtr).lenGeneIdUI;\
   (secAmrSTPtr).lenGeneIdUI = tmpMacUI;\
   \
   \
   tmpMacStr = (firstAmrSTPtr).refSeqStr;\
   (firstAmrSTPtr).refSeqStr = (secAmrSTPtr).refSeqStr;\
   (secAmrSTPtr).refSeqStr = tmpMacStr;\
   \
   tmpMacUI = (firstAmrSTPtr).lenRefSeqUI;\
   (firstAmrSTPtr).lenRefSeqUI = (secAmrSTPtr).lenRefSeqUI;\
   (secAmrSTPtr).lenRefSeqUI = tmpMacUI;\
   \
   \
   tmpMacStr = (firstAmrSTPtr).amrSeqStr;\
   (firstAmrSTPtr).amrSeqStr = (secAmrSTPtr).amrSeqStr;\
   (secAmrSTPtr).amrSeqStr = tmpMacStr;\
   \
   tmpMacUI = (firstAmrSTPtr).lenAmrSeqUI;\
   (firstAmrSTPtr).lenAmrSeqUI=(secAmrSTPtr).lenAmrSeqUI;\
   (secAmrSTPtr).lenAmrSeqUI = tmpMacUI;\
   \
   \
   tmpMacStr = (firstAmrSTPtr).varIdStr;\
   (firstAmrSTPtr).varIdStr = (secAmrSTPtr).varIdStr;\
   (secAmrSTPtr).varIdStr = tmpMacStr;\
   \
   tmpMacUI = (firstAmrSTPtr).lenVarIdUI;\
   (firstAmrSTPtr).lenVarIdUI = (secAmrSTPtr).lenVarIdUI;\
   (secAmrSTPtr).lenVarIdUI = tmpMacUI;\
   \
   \
   tmpMacStr = (firstAmrSTPtr).effectStr;\
   (firstAmrSTPtr).effectStr = (secAmrSTPtr).effectStr;\
   (secAmrSTPtr).effectStr = tmpMacStr;\
   \
   tmpMacUI = (firstAmrSTPtr).lenEffectUI;\
   (firstAmrSTPtr).lenEffectUI=(secAmrSTPtr).lenEffectUI;\
   (secAmrSTPtr).lenEffectUI = tmpMacUI;\
   \
   \
   tmpMacStr = (firstAmrSTPtr).commentStr;\
   (firstAmrSTPtr).commentStr = (secAmrSTPtr).commentStr;\
   (secAmrSTPtr).commentStr = tmpMacStr;\
   \
   tmpMacUI = (firstAmrSTPtr).lenCommentUI;\
   (firstAmrSTPtr).lenCommentUI =\
      (secAmrSTPtr).lenCommentUI;\
   (secAmrSTPtr).lenCommentUI = tmpMacUI;\
   \
   \
   tmpMacC = (firstAmrSTPtr).dirFlag;\
   (firstAmrSTPtr).dirFlag = (secAmrSTPtr).dirFlag;\
   (secAmrSTPtr).dirFlag = tmpMacC;\
   \
   tmpMacC = (firstAmrSTPtr).gradeC;\
   (firstAmrSTPtr).gradeC = (secAmrSTPtr).gradeC;\
   (secAmrSTPtr).gradeC = tmpMacC;\
   \
   \
   tmpMacUI = (firstAmrSTPtr).numSupReadsUI;\
   (firstAmrSTPtr).numSupReadsUI =\
       (secAmrSTPtr).numSupReadsUI;\
   (secAmrSTPtr).numSupReadsUI = tmpMacUI;\
   \
   tmpMacUI = (firstAmrSTPtr).numMapReadsUI;\
   (firstAmrSTPtr).numMapReadsUI =\
       (secAmrSTPtr).numMapReadsUI;\
   (secAmrSTPtr).numMapReadsUI = tmpMacUI;\
   \
   \
   tmpMacC = (firstAmrSTPtr).mutTypeStr[0];\
   \
   (firstAmrSTPtr).mutTypeStr[0]\
      = (secAmrSTPtr).mutTypeStr[0];\
   \
   (secAmrSTPtr).mutTypeStr[0] = tmpMacC;\
   \
   tmpMacC = (firstAmrSTPtr).mutTypeStr[1];\
   \
   (firstAmrSTPtr).mutTypeStr[1]\
      = (secAmrSTPtr).mutTypeStr[1];\
   \
   (secAmrSTPtr).mutTypeStr[1] = tmpMacC;\
   \
   tmpMacC = (firstAmrSTPtr).mutTypeStr[2];\
   \
   (firstAmrSTPtr).mutTypeStr[2]\
      = (secAmrSTPtr).mutTypeStr[2];\
   \
   (secAmrSTPtr).mutTypeStr[2] = tmpMacC;\
   \
   \
   tmpMacStr = (firstAmrSTPtr).needsGeneStr;\
   (firstAmrSTPtr).needsGeneStr =\
      (secAmrSTPtr).needsGeneStr;\
   (secAmrSTPtr).needsGeneStr = tmpMacStr;\
   \
   tmpMacUI = (firstAmrSTPtr).lenNeedsGeneUI;\
   (firstAmrSTPtr).lenNeedsGeneUI =\
      (secAmrSTPtr).lenNeedsGeneUI;\
   (secAmrSTPtr).lenNeedsGeneUI = tmpMacUI;\
   \
   tmpMacC = (firstAmrSTPtr).highResBl;\
   (firstAmrSTPtr).highResBl = (secAmrSTPtr).highResBl;\
   (secAmrSTPtr).highResBl = tmpMacC;\
   \
   tmpMacC = (firstAmrSTPtr).lowResBl;\
   (firstAmrSTPtr).lowResBl = (secAmrSTPtr).lowResBl;\
   (secAmrSTPtr).lowResBl = tmpMacC;\
   \
   tmpMacC = (firstAmrSTPtr).additiveResBl;\
   (firstAmrSTPtr).additiveResBl =\
      (secAmrSTPtr).additiveResBl;\
   (secAmrSTPtr).additiveResBl = tmpMacC;\
   \
   \
   for(\
      iAmrMac = 0;\
      iAmrMac < def_amrST_maxDrugs;\
      ++iAmrMac\
   ){ /*Loop: Blank the antibiotic flags*/\
      tmpMacUL = (firstAmrSTPtr).amrFlagsUL[iAmrMac];\
      \
      (firstAmrSTPtr).amrFlagsUL[iAmrMac] =\
         (secAmrSTPtr).amrFlagsUL[iAmrMac];\
      \
      (secAmrSTPtr).amrFlagsUL[iAmrMac] = tmpMacUL;\
      \
      \
      tmpMacUL =(firstAmrSTPtr).crossResFlagsUL[iAmrMac];\
      \
      (firstAmrSTPtr).crossResFlagsUL[iAmrMac] =\
         (secAmrSTPtr).crossResFlagsUL[iAmrMac];\
      \
      (secAmrSTPtr).crossResFlagsUL[iAmrMac] = tmpMacUL;\
   } /*Loop: Blank the antibiotic flags*/\
} /*swapAmrStructs*/

/*-------------------------------------------------------\
| Fun-08: sortAmrStructArray
|   - Sort on an amrStruct array structures by reference
|     coordiante (uses shell sort)
| Input:
|   - amrAryST:
|     - Pointer to start of amrStruct array to sort
| Output:
|   - Modifies:
|     o The amrAryST to be sorted by starting
|       reference coordiante
\-------------------------------------------------------*/
void sortAmrStructArray(
   struct amrStruct *amrAryST,
   uint startUI,
   uint endUI
){ /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
   ' Fun-08 TOC: sortGenIndice
   '  - Sorts the arrays in a genIndice struct by variant
   '    id with shell short.
   '  - Shell sort taken from:
   '    - Adam Drozdek. 2013. Data Structures and
   '      Algorithims in c++. Cengage Leraning. fourth
   '      edition. pages 505-508
   '    - I made some minor changes, but is mostly the
   '      same
   '  o fun-08 sec-01:
   '    - Variable declerations
   '  o fun-08 sec-02:
   '    - Find the number of rounds to sort for
   '  o fun-08 sec-03:
   '    - Sort the arrays in genIndiceST
   \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  
  /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
  ^ Fun-08 Sec-01:
  ^  - Variable declerations
  \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
  
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
  ^ Fun-08 Sec-02:
  ^  - Find the max search value (number rounds to sort)
  \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
  
  /*Recursion formula: h[0] = 1, h[n] = 3 * h[n - 1] +1*/
  subUL = 1; /*Initialzie first array*/
  while(subUL < numElmUL - 1) subUL = (3 * subUL) + 1;
  
  /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  ^ Fun-08 Sec-03:
  ^  - Sort the arrays in genIndiceST
  \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
  
  while(subUL > 0)
  { /*loop trhough all sub arrays sort the subarrays*/
    for(ulIndex = 0; ulIndex <= subUL; ++ulIndex)
    { /*For each element in the subarray*/
      for(ulElm = ulIndex;
          ulElm + subUL <= endUI;
          ulElm += subUL
      ){ /*Loop; swap each nth element of the subarray*/
        nextElmUL = ulElm + subUL;
        
        if(  amrAryST[ulElm].refPosUI
           > amrAryST[nextElmUL].refPosUI
        ){ /*If I need to swap an element*/
          swapAmrStructs(
             amrAryST[ulElm],
             amrAryST[nextElmUL]
          ); /*Swap the values in the two structuers*/
          
          lastElmUL = ulElm;
          elmOnUL = ulElm;
          
          while(lastElmUL >= subUL)
          { /*loop; move swapped element back*/
            lastElmUL -= subUL;
            
            if(  amrAryST[elmOnUL].refPosUI
               > amrAryST[lastElmUL].refPosUI
            ) break; /*Positioned the element*/
            
             swapAmrStructs(
                amrAryST[elmOnUL],
                amrAryST[lastElmUL]
             ); /*Swap the values in the two structuers*/
            
            elmOnUL = lastElmUL;
          } /*loop; move swapped element back*/
        } /*If I need to swap elements*/
      } /*Loop; swap each nth element of the subarray*/
    } /*For each element in the subarray*/
    
    subUL = (subUL - 1) / 3; /*Move to the next round*/
  } /*loop through all sub arrays to sort the subarrays*/
} /*sortGenIndice*/

/*-------------------------------------------------------\
| Fun-09: findNearestAmr
|  - Does a binary search for the nearest amr at or after
|    to the input query coordiante
| Input:
|  - amrAryST:
|    o Pointer to an array of amrStruct structures to
|      search
|  - qryUI:
|    o Starting coordinate (query) to search for in
|      amrAryST
|  - numGenesUI:
|    o Number of amrStruct's in amrAryST (index 0)
| Output:
|  - Returns:
|    o The nearest index at or after qryUI
|    o -1 for no amr genes after qryUI
\-------------------------------------------------------*/
int findNearestAmr(
   struct amrStruct *amrAryST,
   uint qryUI,
   int numAmrI
){ /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   ' Fun-09 TOC: findNearestAmr
   '   - Finds the nearest amr at or after the input query
   '     coordiante
   \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

   int midI = 0; /*divide by 2*/
   int rightHalfI = numAmrI - 1;
   int leftHalfI = 0;

   while(leftHalfI <= rightHalfI)
   { /*Loop: Search for the starting coordinate*/
      midI = (leftHalfI + rightHalfI) >> 1;

     if(qryUI > amrAryST[midI].refPosUI)
         leftHalfI = midI + 1;

      else if(qryUI < amrAryST[midI].refPosUI)
         rightHalfI = midI - 1;

     else
     { /*Else: I found the query*/
        if(midI == 0) return 0;

        while(midI > 0)
        { /*Loop: Make sure I am on the first amr*/
           if(amrAryST[midI - 1].refPosUI < qryUI) break;
           --midI;
        } /*Loop: Make sure I am on the first amr*/

        return midI;
     } /*Else: I found the query*/

     /*branchless whith an if return was slower here*/
   } /*Loop: Search for the starting coordinate*/

   /*Make sure it is alwasy greater than*/
   midI += (amrAryST[midI].refPosUI < qryUI);

   /*I may have the same coordinates repeated*/
   while((midI >0) & (amrAryST[midI -1].refPosUI >qryUI))
      --midI;

   while(midI > 0)
   { /*Loop: Make sure I am on the first amr*/
      if(amrAryST[midI - 1].refPosUI < qryUI) break;
      --midI;
   } /*Loop: Make sure I am on the first amr*/

   /*Make sure not at end of vector*/
   if(midI >= numAmrI) return -1;
   midI += (amrAryST[midI].refPosUI < qryUI);

   if(midI >= numAmrI) return -1;
   if(amrAryST[midI].refPosUI < qryUI) return -1;


   return midI;
} /*findNearestAmr*/

/*-------------------------------------------------------\
| Fun-10: pAmrDB
|  - Print out the amr database used
| Input:
|  - amrAryST:
|    o Pointer to an array of amrStruct structures to
|      print out as a tsv file (database)
|  - numAmrUL:
|    o Number of amrStructs in amrAryST
|  - drugStrAry:
|    o C-string array with drug names. Use the functions
|       drug_str_ary.h to make this
|  - numDrugsI:
|    o number of antibiotics in drugStrAry
|  - outStr:
|    o C-string with path to output to.
|    o a "-" or null (0) is stdout
| Output:
|  - Prints:
|    o Elements in amrSTAry to outStr
|  - Returns:
|    o 0 for success
|    o def_amrST_invalidFILE if outStr could not be opend
\-------------------------------------------------------*/
char pAmrDB(
   struct amrStruct *amrSTAry,
   ulong numAmrUL,
   char *drugStrAry,
   int numDrugsI,
   char *outStr
){
   ulong ulAmr = 0;  /*For looping through amrSTAry*/

   int iDrug = 0;    /*For looping through drugStrAry*/
   char *drugStr = 0;/*For printing out the drug id's*/
   ulong ulRes = 0;  /*For checking each amr's resitance*/
   ulong ulCross = 0;/*check each amr's cross resitance*/
   int iAmrSet = 0;/*Flag set on*/
   int iFlag = 0;  /*Flag on in a set*/
   int iDrugOn = 0; /*Drug on*/
   char resBl = 0;

   FILE *outFILE = 0;

   if(! outStr) outFILE = stdout;
   else if(*outStr == '-') outFILE = stdout;
   else outFILE = fopen(outStr, "w");

   if(outFILE == 0) return def_amrST_invalidFILE;

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-10 Sec-03:
   ^   - Print the header
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   fprintf(outFILE,"gene\tvariantID\trefPosition");
   fprintf(outFILE,"\tdirection\tmutation\tframeshift");
   fprintf(outFILE,"\trefSeq\tamrSeq\tcodonRefPos");
   fprintf(outFILE,"\tcodonNumber\tlastCodon\trefeAA");
   fprintf(outFILE, "\tamrAA\tgeneStart\tgeneEnd");
   fprintf(outFILE, "\thighResistance\tlowResitance");
   fprintf(outFILE, "\tadditiveResitance\tneedsGene");

   for(iDrug = 0; iDrug < numDrugsI; ++iDrug)
   { /*Loop: Print out the antibiotic names*/
      drugStr = getDrugFromDrugAry(drugStrAry, iDrug);
      fprintf(outFILE, "\t%s", drugStr);
   } /*Loop: Print out the antibiotic names*/

   fprintf(outFILE,"\tendAntibiotics\teffect\tcomment\n");

   for(ulAmr = 0; ulAmr < numAmrUL; ++ulAmr)
   { /*Loop: Print out each  amr*/
      fprintf(
         outFILE,
         "%s\t%s\t%u",
         amrSTAry[ulAmr].geneIdStr, /*gene name*/
         amrSTAry[ulAmr].varIdStr,  /*variant name*/
         amrSTAry[ulAmr].refPosUI + 1   /*ref position*/
      ); /*Print out the first few entries*/

      /*Print out the reading frame of the gene*/
      if(amrSTAry[ulAmr].dirFlag == def_amrST_forwardDir)
         fprintf(outFILE, "\tF");

      else if(
         amrSTAry[ulAmr].dirFlag == def_amrST_revCompDir
      ) fprintf(outFILE, "\tR");

      else fprintf(outFILE, "\tNA");

      /*Print out mutation type, if is a frame shift, 
      `   and nucleotide sequences
      */
      fprintf(
         outFILE,
         "\t%s\t%u\t%s\t%s",
         amrSTAry[ulAmr].mutTypeStr,
         amrSTAry[ulAmr].frameshiftBl,
         amrSTAry[ulAmr].refSeqStr,
         amrSTAry[ulAmr].amrSeqStr
      );

      if(amrSTAry[ulAmr].refAaStr != 0)
      { /*If: I had an amino acid change*/
         fprintf(
            outFILE,
            "\t%u\t%u\t%u\t%s",
            amrSTAry[ulAmr].codonPosUI + 1,
            amrSTAry[ulAmr].codonNumUI,
            amrSTAry[ulAmr].endCodonNumUI,
            amrSTAry[ulAmr].refAaStr
         );

         /*Check if I have an amr aa sequence*/
         if(amrSTAry[ulAmr].amrAaStr != 0)
            fprintf(
                outFILE,
                "\t%s",
                amrSTAry[ulAmr].amrAaStr
            );

         else fprintf(outFILE, "\t0");
      } /*If: I had an amino acid change*/

      else fprintf(outFILE, "\tNA\tNA\tNA\t0\t0");

      /*Print out the gene position*/
      if(amrSTAry[ulAmr].geneLastRefUI > 0)
         fprintf(
            outFILE,
            "\t%u\t%u",
            amrSTAry[ulAmr].geneFirstRefUI,
            amrSTAry[ulAmr].geneLastRefUI 
         );

      else fprintf(outFILE, "\tNA\tNA");
            
      fprintf(
         outFILE,
        "\t%i\t%i\t%i",
        amrSTAry[ulAmr].highResBl,
        amrSTAry[ulAmr].lowResBl,
        amrSTAry[ulAmr].additiveResBl
      ); /*Print out the who comment parsed data*/
         
      /*I should not need to do this, but for some
      `    odd reason I get blanks/(null) if I do not
      */
      if(! amrSTAry[ulAmr].needsGeneStr)
         fprintf(outFILE, "\tNA");
      else if(amrSTAry[ulAmr].needsGeneStr[0] < 32)
         fprintf(outFILE, "\tNA");
      else
        fprintf(
           outFILE,
           "\t%s",
           amrSTAry[ulAmr].needsGeneStr
        );

      iDrugOn = 0;

      for(
         iAmrSet = 0;
         iAmrSet < 1+numDrugsI/(sizeof(unsigned long)<<3);
         ++iAmrSet
      ){ /*Loop: Print out the AMR's resitant to*/
         ulRes = amrSTAry[ulAmr].amrFlagsUL[iAmrSet];
         ulCross=amrSTAry[ulAmr].crossResFlagsUL[iAmrSet];

         for(iFlag=0; iFlag < def_amrST_maxFlags; ++iFlag)
         { /*Loop: print restaincet for each antibiotic*/
            /*check if this buffer was completely filled*/
            ++iDrugOn;
            if(iDrugOn > numDrugsI) break;

            /*Add in the resitance entry*/
            resBl =
               (char)((-(ulRes &1) ) & def_amrST_resFlag);

            /*Add in the cross resistance entry*/
            resBl |=
               (char)
               ((-(ulCross &1) ) &def_amrST_crossResFlag);

            /*Logic (both lines)
            `  - ulRes & 1 or ulCross & 1:
            `    o 1 if there is resitance (bit set)
            `    o 0 if there is no resitance
            `  - -(ulRes & 1) or -(ulCross & 1):
            `    o -1 if there is resitance (all bits set)
            `    o 0 if there is no resitance
            `  - -(ulRes & 1) & def_amrST_resFlag:
            `    o Changes -1 to def_amrST_resFlag
            `      - for cross resitance I use the
            `        def_amrST_crossResFlag
            `    o Stays 0 if there is no resitance
            `  - resBl |=
            `    o Meges the two (restiance and cross
            `      resitance) result together
            */

            fprintf(outFILE, "\t%i", resBl);

            /*Move to the next antibiotic flag*/
            ulRes >>= 1;
            ulCross >>= 1;
         } /*Loop: print restaincet for each antibiotic*/

         /*Mark end of antibiotics*/
         fprintf(outFILE, "\t*");

         /*Print out the effect entry*/
         if(! amrSTAry[ulAmr].effectStr)
            fprintf(outFILE, "\tNA");
         else if(*amrSTAry[ulAmr].effectStr < 32)
            fprintf(outFILE, "\tNA");
         else
            fprintf(
               outFILE,
               "\t%s",
               amrSTAry[ulAmr].effectStr
             );
 
         /*Print out the comment entry*/
         if(! amrSTAry[ulAmr].commentStr)
            fprintf(outFILE, "\tNA");
         else if(*amrSTAry[ulAmr].commentStr < 32)
            fprintf(outFILE, "\tNA");
         else
            fprintf(
               outFILE,
               "\t%s",
               amrSTAry[ulAmr].commentStr
             );
 

         /*Print out the newline*/
         fprintf(outFILE, "\n");
      } /*Loop: Print out the AMR's resitant to*/

   } /*Loop: Print out each  amr*/

   if(outFILE != stdout) fclose(outFILE);

   return 0;
} /*pAmrDB*/
