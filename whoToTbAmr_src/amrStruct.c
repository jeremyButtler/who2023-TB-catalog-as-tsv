/*########################################################
# Name: amrStruct
#   - Holds functions to support the amrStruct used in
#     tbAmr. This includes processing the tbAmr format,
#     sorting the structures, looking up structures and
#     memory managment
########################################################*/

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
' SOF: Start Of File
'   o header:
'     - Include libraries
'   o .h st-01: amrStruct
'     - Holds the information for a single amr mutation
'       that was extracted from the WHO catalog
'   o .h fun-01: blankAmrStruct
'     - Sets all non-pointer values in amrStructPtr to 0
'   o .h fun-02: initAmrStruct
'     - Sets all values, including pointers in the
'       amrStruct structure to 0
'   o fun-03: freeAmrStructStack
'     - Frees the geneIdStr, refSeqStr, and amrSeqStr
'       arrays and sets all values to 0 in the input
'       amrStruct 
'   o fun-04: freeAmrStruct
'     - Frees an heap allocated amrStruct structure
'   o fun-05: freeAmrStructArray
'     - Frees an heap allocated array of amrStruct
'       structures
'   o .h fun-06: swapAmrStructs
'     - Swaps the values in two amrStruct structures
'   o fun-07: sortAmrStructArray
'     - Sort on an amrStruct array structures by reference
'       coordiante (uses shell sort)
'   o fun-08: geneIdSortAmrSTAry
'     - Sort on an amrStruct array structures by the
'       gene names (ids) (uses shell short)
'   o fun-09: findNearestAmr
'      - Finds the nearest amr at or after the input query
'   o fun-10: pAmrDB
'     - Print out the amr database used
'   o fun-11: readTbAmrTbl
'     - Gets data from a tbAmr tsv file output from pAmrDB
'       (fun-10)
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

#include "amrStruct.h"

#include <stdio.h>

#include "drug_str_ary.h"

/*Only .h files*/
#include "../generalLib/dataTypeShortHand.h"
#include "../generalLib/ulCpStr.h"
#include "../generalLib/base10StrToNum.h"

/*-------------------------------------------------------\
| Fun-03: freeAmrStructStack
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
| Fun-04: freeAmrStruct
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
| Fun-05: freeAmrStructArray
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
| Fun-07: sortAmrStructArray
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
   ' Fun-07 TOC:
   '   - Sort on an amrStruct array structures by
   '     reference coordiante (uses shell sort)
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
   '    - Sort the amrStruct array by reference position
   \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  
  /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
  ^ Fun-07 Sec-01:
  ^   - Variable declerations
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
  ^ Fun-07 Sec-02:
  ^   - Find the max search value (number rounds to sort)
  \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
  
  /*Recursion formula: h[0] = 1, h[n] = 3 * h[n - 1] +1*/
  subUL = 1; /*Initialzie first array*/
  while(subUL < numElmUL - 1) subUL = (3 * subUL) + 1;
  
  /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  ^ Fun-07 Sec-03:
  ^   - Sort the amrStruct array by reference position
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
} /*sortAmrStructArray*/

/*-------------------------------------------------------\
| Fun-08: geneIdSortAmrSTAry
|   - Sort on an amrStruct array structures by the gene
|     names (ids) (uses shell short)
| Input:
|   - amrAryST:
|     - Pointer to start of amrStruct array to sort
| Output:
|   - Modifies:
|     o The amrAryST to be sorted by starting
|       gene names (ids)
\-------------------------------------------------------*/
void geneIdSortAmrSTAry(
   struct amrStruct *amrAryST,
   unsigned int startUI,
   unsigned int endUI
){ /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
   ' Fun-08 TOC:
   '  - Sorts an array of amrStructs by gene names (ids)
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
   '    - Sort the amrStructs by variant id
   \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  
  /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
  ^ Fun-08 Sec-01:
  ^   - Variable declerations
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
  ^   - Find the max search value (number rounds to sort)
  \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
  
  /*Recursion formula: h[0] = 1, h[n] = 3 * h[n - 1] +1*/
  subUL = 1; /*Initialzie first array*/
  while(subUL < numElmUL - 1) subUL = (3 * subUL) + 1;
  
  /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  ^ Fun-08 Sec-03:
  ^   - Sort the amrStructs by variant id
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
        
        if( cStrEql(
               amrAryST[ulElm].geneIdStr,
               amrAryST[nextElmUL].geneIdStr,
               '\0'
            )
          > 0
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
            
            if( cStrEql(
                   amrAryST[elmOnUL].geneIdStr,
                   amrAryST[lastElmUL].geneIdStr,
                   '\0'
                )
              > 0
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
} /*geneIdSortAmrSTAry*/

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
        while(midI > 0)
        { /*Loop: Make sure I am on the first amr*/
           if(amrAryST[midI - 1].refPosUI < qryUI) break;
           --midI;
        } /*Loop: Make sure I am on the first amr*/

        return midI;
     } /*Else: I found the query*/

     /*branchless whith an if return was slower here*/
   } /*Loop: Search for the starting coordinate*/

   if(midI >= numAmrI) return -1;

   /*Make sure it is alwasy greater than*/
   midI += (amrAryST[midI].refPosUI < qryUI);

   if(midI >= numAmrI) return -1;

   /*I may have the same coordinates repeated*/
   while((midI >0) && (amrAryST[midI -1].refPosUI >qryUI))
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
){ /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
   ' Fun-10 TOC: pAmrDB
   '   - Prints out the AMRs to an tbAmr database
   '   o fun-10 sec-01:
   '     - Variable declerations
   '   o fun-10 sec-02:
   '     - Open output file check
   '   o fun-10 sec-03:
   '     - Print the header
   '   o fun-10 sec-04:
   '     - Print the AMRs
   '   o fun-10 sec-05:
   '     - Close file and exit
   \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-10 Sec-01:
   ^   - Variable declerations
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   ulong ulAmr = 0;  /*For looping through amrSTAry*/

   int iDrug = 0;    /*For looping through drugStrAry*/
   char *drugStr = 0;/*For printing out the drug id's*/
   ulong ulRes = 0;  /*For checking each amr's resitance*/
   ulong ulCross = 0;/*check each amr's cross resitance*/
   int iAmrSet = 0;/*Flag set on*/
   int iFlag = 0;  /*Flag on in a set*/
   int iDrugOn = 0; /*Drug on*/
   char resBl = 0;

   char *tmpStr = 0;

   FILE *outFILE = 0;

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-10 Sec-02:
   ^   - Open output file check
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

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
   fprintf(outFILE,"\tcodonNumber\tlastCodon\trefAA");
   fprintf(outFILE, "\tamrAA\tgeneStart\tgeneEnd");
   fprintf(outFILE, "\thighResistance\tlowResitance");
   fprintf(outFILE, "\tadditiveResitance\tneedsGene");

   for(iDrug = 0; iDrug < numDrugsI; ++iDrug)
   { /*Loop: Print out the antibiotic names*/
      drugStr = getDrugFromDrugAry(drugStrAry, iDrug);
      fprintf(outFILE, "\t%s", drugStr);
   } /*Loop: Print out the antibiotic names*/

   fprintf(outFILE,"\tendAntibiotics\teffect\tcomment");
   fprintf(outFILE, "\tgrade\tentireGene\tunkownEntry");
   fprintf(outFILE, "\n");

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-10 Sec-04:
   ^   - Print the AMRs
   ^   o fun-10 sec-04 sub-01:
   ^     - Start loop; print out ids & reference position
   ^   o fun-10 sec-04 sub-02:
   ^     - Print out direction and AMR type/sequence
   ^   o fun-10 sec-04 sub-03:
   ^     - Print out amino acid sequence and coordinates
   ^   o fun-10 sec-04 sub-04:
   ^     - Print out gene coordinates
   ^   o fun-10 sec-04 sub-05:
   ^     - Print out resistance level/if additive
   ^   o fun-10 sec-04 sub-06:
   ^     - Print out the drugs resistant to
   ^   o fun-10 sec-04 sub-07:
   ^     - Print out the effect entry
   ^   o fun-10 sec-04 sub-08:
   ^     - Print out the comment entry
   ^   o fun-10 sec-04 sub-09:
   ^     - Print grade (main drug), if the effect
   ^       appies to entire gene, and if unknown entry
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   /*****************************************************\
   * Fun-10 Sec-04 Sub-01:
   *   - Start loop; print out ids and reference position
   \*****************************************************/

   for(ulAmr = 0; ulAmr < numAmrUL; ++ulAmr)
   { /*Loop: Print out each  amr*/
      fprintf(
         outFILE,
         "%s\t%s\t%u",
         amrSTAry[ulAmr].geneIdStr, /*gene name*/
         amrSTAry[ulAmr].varIdStr,  /*variant name*/
         amrSTAry[ulAmr].refPosUI + 1   /*ref position*/
      ); /*Print out the first few entries*/

      /**************************************************\
      * Fun-10 Sec-04 Sub-02:
      *   - Print out direction and AMR type/sequence
      \**************************************************/

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

      /**************************************************\
      * Fun-10 Sec-04 Sub-03:
      *   - Print out amino acid sequence and coordinates
      \**************************************************/

      if(amrSTAry[ulAmr].refAaStr != 0)
      { /*If: I had an amino acid change*/
         fprintf(
            outFILE,
            "\t%u\t%u\t%u",
            amrSTAry[ulAmr].codonPosUI + 1,
            amrSTAry[ulAmr].codonNumUI,
            amrSTAry[ulAmr].endCodonNumUI
         );

         /*Check if I have an reference aa sequence*/
         if(amrSTAry[ulAmr].refAaStr != 0)
            fprintf(
                outFILE,
                "\t%s",
                amrSTAry[ulAmr].refAaStr
            );

         else fprintf(outFILE, "\t0");

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

      /**************************************************\
      * Fun-10 Sec-04 Sub-04:
      *   - Print out gene coordinates
      \**************************************************/

      /*Print out the gene position*/
      if(amrSTAry[ulAmr].geneLastRefUI > 0)
         fprintf(
            outFILE,
            "\t%u\t%u",
            amrSTAry[ulAmr].geneFirstRefUI + 1,
            amrSTAry[ulAmr].geneLastRefUI + 1
         );

      else fprintf(outFILE, "\tNA\tNA");
            
      /**************************************************\
      * Fun-10 Sec-04 Sub-05:
      *   - Print out resistance level/if additive
      \**************************************************/

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

      /**************************************************\
      * Fun-10 Sec-04 Sub-06:
      *   - Print out the drugs resistant to
      \**************************************************/

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

         /***********************************************\
         * Fun-10 Sec-04 Sub-07:
         *   - Print out the effect entry
         \***********************************************/

         if(! amrSTAry[ulAmr].effectStr)
            fprintf(outFILE, "\tNA");
         else
         { /*Else: If I may have an comment*/

            /*For some odd reason there are some blank
            `  effect entries in the catalog that escape
            `  me
            */
            tmpStr = amrSTAry[ulAmr].effectStr;
            while(*tmpStr++ < 32) if(*tmpStr=='\0') break;

            if(*tmpStr == '\0') fprintf(outFILE, "\tNA");

            else
               fprintf(
                  outFILE,
                  "\t%s",
                  amrSTAry[ulAmr].effectStr
               );
         } /*Else: If I may have an comment*/
 
         /***********************************************\
         * Fun-10 Sec-04 Sub-08:
         *   - Print out the comment entry
         \***********************************************/

         if(! amrSTAry[ulAmr].commentStr)
            fprintf(outFILE, "\tNA");
         else
         { /*Else: If I may have an comment*/
            /*For some odd reason there are some blank
            `  comment entries in the catalog that escape
            `  me
            */
            tmpStr = amrSTAry[ulAmr].commentStr;
            while(*tmpStr++ < 32) if(*tmpStr=='\0') break;

            if(*tmpStr == '\0') fprintf(outFILE, "\tNA");

            else
               fprintf(
                  outFILE,
                  "\t%s",
                  amrSTAry[ulAmr].commentStr
               );
         } /*Else: If I may have an comment*/
 
         /***********************************************\
         * Fun-10 Sec-04 Sub-09:
         *   - Print grade (main drug), if the effect
         *     appies to entire gene, and if unknown entry
         \***********************************************/

         fprintf(
            outFILE,
            "\t%i\t%i\t%i",
            amrSTAry[ulAmr].gradeC,
            amrSTAry[ulAmr].wholeGeneFlag,
            amrSTAry[ulAmr].unknownBl
         ); /*Print out the final columns*/

         /*Print out the newline*/
         fprintf(outFILE, "\n");
      } /*Loop: Print out the AMR's resitant to*/

   } /*Loop: Print out each  amr*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-10 Sec-05:
   ^   - Close file and exit
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   if(outFILE != stdout) fclose(outFILE);

   return 0;
} /*pAmrDB*/

/*-------------------------------------------------------\
| Fun-11: readTbAmrTbl
|   - Gets data from a tbAmr tsv file output from pAmrDB
|     (fun-10)
| Input:
|   - tbAmrTblStr:
|     o C-string with path to the AMR database/table
|   - numAmrUI:
|     o Changed to hold the number of AMRs in tbAmrTblStr
|   - durgStrAry:
|     o Pointer to a c-string to hold the drug names in
|   - numDrugsI
|     o Pointer to uint to hold the number of unique
|       drugs in drugStrAry
|   - maxDrugsI:
|     o The maximum number of drugs the current drugStrAry
|       can hold. This is changed if drugStrAry is resized
|   - errC:
|     o Changed to hold the error message
| Output:
|   - Modifies:
|     o numAmrUI to hold the number of extracted amrs
|     o drugStrAry to hold the name of each drug. This is
|       resized as needed.
|     o numDrugsI to hold the number of drugs currently
|       in drugStrAry
|     o maxDrugsI to hold the maximum number of drugs
|       the current drugStrAry has
|     o errC to hold the error message
|       - 0 for no errors
|       - def_amrST_invalidFILE for could not open the
|         file
|       - def_amrST_memError for memory errors
|   - Returns:
|     o Array of amrStruct structures with the AMRs
|     o 0 for an error (see errC for specific error)
\-------------------------------------------------------*/
struct amrStruct * readTbAmrTbl(
   char *tbAmrTblStr,    /*Path to tbAmr tsv with AMR(s)*/
   unsigned int *numAmrUI,  /*Number of AMR(s) in tsv*/
   char **drugStrAry,       /*Holds drug names*/
   int *numDrugsI,          /*number drugs in drugStrAry*/
   int *maxDrugsI,          /*Max drugs for drugStrAry*/
   char *errC               /*Holds errors*/
){ /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
   ' Fun-11 TOC: readTbAmrTbl
   '   - Gets data from a tbAmr tsv file output from
   '     pAmrDB (fun-10)
   '   o fun-11 sec-01:
   '     - Variable declerations
   '   o fun-11 sec-02:
   '     - Process the header
   '   o fun-11 sec-03:
   '     - Process the header and get the number of lines
   '   o fun-11 sec-04:
   '     - Prepare buffers for extracting AMRs
   '   o fun-11 sec-05:
   '     - Extract the information from the file
   '   o fun-11 sec-06:
   '     - Clean up
   \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-11 Sec-01:
   ^   - Variable declerations
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   uint lenBuffUI = 1 << 11; /*Around 4096 bytes*/
   char *buffStr = 0;        /*Buffer to read file into*/
   char *tmpStr = 0;         /*For temporary operations*/
   ulong ulTab = ulCpMakeDelim('\t'); /*for ulCpStrDelim*/

   int maxLineLenI = 0;
   int tmpI = 0;

   /*Boolean to check if I need to make a new drug array*/
   char newDrugAryBl = (*drugStrAry == 0);
   int iEndDrug = 0; /*Marks end of the drug entry*/
   int iStartDrug = 0; /*Marks start of the drug entry*/
   int iDrug = 0;  /*Iterator for loops*/

   struct amrStruct *amrSTAry = 0;
   uint uiAmr = 0;
   ulong *drugResUL = 0;
   ulong *drugCrossResUL = 0;

   FILE *amrFILE = 0;

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-11 Sec-02:
   ^   - Check if file eixists and set up the buffer
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   /*Check if my file exists*/
   amrFILE = fopen(tbAmrTblStr, "r");

   if(amrFILE == 0) goto fileErr_sec06_sub03_readTbAmrTbl;

   /*Set up my buffer*/
   buffStr = malloc(lenBuffUI * sizeof(char));

   if(buffStr == 0) goto memErr_sec06_sub02_readTbAmrTbl;

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-11 Sec-03:
   ^   - Process the header and get the number of lines
   ^   o fun-11 sec-03 sub-01:
   ^     - Get the number of lines in the file
   ^   o fun-11 sec-03 sub-02:
   ^     - Read in the header
   ^   o fun-11 sec-03 sub-03:
   ^     - Get the number of antibiotics
   ^   o fun-11 sec-03 sub-04:
   ^     - Allocate memory for the antibiotics
   ^   o fun-11 sec-03 sub-05:
   ^     - Copy the antibiotics to the drug array
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   /*****************************************************\
   * Fun-11 Sec-03 Sub-01:
   *   - Get the number of lines in the file
   \*****************************************************/

   *numAmrUI = 0;

   while(
      fread(buffStr, sizeof(char), lenBuffUI - 1, amrFILE)
   ){ /*Loop: Get number of amrs from the file*/
      
      buffStr[lenBuffUI - 1] = '\0';
      tmpStr = buffStr;

      while(*tmpStr != '\0')
      { /*Loop: Find the number of new lines*/
         while(*tmpStr & ~ 10)
         { /*Loop: Find the end of the line*/
            ++tmpI;
            ++tmpStr;
         } /*Loop: Find the end of the line*/

         if(tmpI > maxLineLenI) maxLineLenI = tmpI;

         tmpI &= (int) -(*tmpStr != '\n');
         (*numAmrUI) += (*tmpStr == '\n');
         tmpStr += (*tmpStr == '\n');
      } /*Loop: Find the number of new lines*/
   } /*Loop: Get number of amrs from the file*/

   ++(*numAmrUI);

   /*Check if there were any AMR(s) in the file*/
   if(! *numAmrUI) goto fileErr_sec06_sub03_readTbAmrTbl;

   /*Go back to start of file*/
   fseek(amrFILE, 0, SEEK_SET);

   free(buffStr);
   buffStr = malloc((maxLineLenI + 2) * sizeof(char));
   lenBuffUI = maxLineLenI + 1;

   if(buffStr == 0) goto memErr_sec06_sub02_readTbAmrTbl;

   /*****************************************************\
   * Fun-11 Sec-03 Sub-02:
   *   - Read in the header
   \*****************************************************/

   buffStr[lenBuffUI - 2] = '\0';
   tmpStr = fgets(buffStr, lenBuffUI, amrFILE);

   if(! tmpStr) goto fileErr_sec06_sub03_readTbAmrTbl;
  
   /*~10 sets '\n', backspace, and "start of text" to 0.
   `   So, every realisitic character in a file, except
   `  '\0' and '\n' will have at least one bit set
   */
   while(tmpStr[lenBuffUI - 2] & ~10)
   { /*Loop: Get the full line*/
      tmpStr =
         realloc(buffStr, (lenBuffUI <<1) * sizeof(char));

      if(! tmpStr) goto memErr_sec06_sub02_readTbAmrTbl;

      buffStr = tmpStr;
      tmpStr += lenBuffUI;
      lenBuffUI <<= 1;

      /*Add my marker back in*/
      buffStr[lenBuffUI - 2] = '\0';

      tmpStr = fgets(buffStr, lenBuffUI, amrFILE);

      /*Check if I am at the end of the file*/
      if(! tmpStr) goto fileErr_sec06_sub03_readTbAmrTbl;
   } /*Loop: Get the full line*/
   
   /*****************************************************\
   * Fun-11 Sec-03 Sub-03:
   *   - Get the number of antibiotics
   \*****************************************************/

   for(iDrug = 0; iDrug < 19; ++iDrug)
      while(buffStr[iStartDrug++] > 31) {}

   iDrug = *numDrugsI;
   iEndDrug = iStartDrug;

   while(
      cStrMatch(
         &buffStr[iEndDrug],
         "endAntibiotics",
         '\t',
         iEndDrug
      )
   ){ /*Loop: Find the end of the antibiotics columns*/
      ++(*numDrugsI);
      while(buffStr[iEndDrug] != '\t') ++iEndDrug;
      ++iEndDrug;
   } /*Loop: Find the end of the antibiotics columns*/

   /*****************************************************\
   * Fun-11 Sec-03 Sub-04:
   *   - Allocate memory for the antibiotics
   \*****************************************************/

   if(newDrugAryBl)
   { /*If: this is a new drug array*/
      *drugStrAry = mallocDrugAryStr(*numDrugsI);

      if(! *drugStrAry)
         goto memErr_sec06_sub02_readTbAmrTbl;
   } /*If: this is a new drug array*/

   else if((*numDrugsI > *maxDrugsI))
   { /*If: I need to add more memory to the drug array*/

      if(reallocDrugAryStrNoFree(drugStrAry, *numDrugsI))
         goto memErr_sec06_sub02_readTbAmrTbl;

   } /*If: I need to add more memory to the drug array*/

   /*****************************************************\
   * Fun-11 Sec-03 Sub-05:
   *   - Copy the antibiotics to the drug array
   \*****************************************************/

   while(iDrug < *numDrugsI)
   { /*Loop: Read in the antibiotic entries*/
      iStartDrug +=
         cpDrugToDrugAry(
            *drugStrAry, 
            &buffStr[iStartDrug],
            iDrug,
            '\t'
         ); /*Copy the drug to the drug array*/

      ++iStartDrug; /*Get off the tab*/
      ++iDrug;
   } /*Loop: Read in the antibiotic entries*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-11 Sec-04:
   ^   - Prepare buffers for extracting AMRs
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   amrSTAry= malloc(*numAmrUI * sizeof(struct amrStruct));
   if(! amrSTAry) goto memErr_sec06_sub02_readTbAmrTbl;

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-11 Sec-05:
   ^   - Extract the information from the file
   ^   o fun-11 sec-05 sub-02:
   ^     - Get first entry, start read in loop, &
   ^       initailze the new variables
   ^   o fun-11 sec-05 sub-02:
   ^     - Read in the gene id
   ^   o fun-11 sec-05 sub-03:
   ^     - Read in the variaint id
   ^   o fun-11 sec-05 sub-04:
   ^     - Read in the refernce positon
   ^   o fun-11 sec-05 sub-05:
   ^     - Read in the direction
   ^   o fun-11 sec-05 sub-06:
   ^     - Read in the mutation type
   ^   o fun-11 sec-05 sub-07:
   ^     - Read in the frame shift entry
   ^   o fun-11 sec-05 sub-08:
   ^     - Read in the reference sequence
   ^   o fun-11 sec-05 sub-09:
   ^     - Read in the amr sequence
   ^   o fun-11 sec-05 sub-10:
   ^     - Read in the frist codon base in reference
   ^   o fun-11 sec-05 sub-11:
   ^     - Read in the starting codon number
   ^   o fun-11 sec-05 sub-12:
   ^     - Read in the ending codon number
   ^   o fun-11 sec-05 sub-13:
   ^     - Read in the reference amino acid sequence
   ^   o fun-11 sec-05 sub-14:
   ^     - Read in the amr amino acid sequence
   ^   o fun-11 sec-05 sub-15:
   ^     - Read in the gene starting position
   ^   o fun-11 sec-05 sub-16:
   ^     - Read in the gene ending position
   ^   o fun-11 sec-05 sub-17:
   ^     - Check if is a high resistance gene
   ^   o fun-11 sec-05 sub-18:
   ^     - Check if is a low resistance gene
   ^   o fun-11 sec-05 sub-19:
   ^     - Check if resistance is additive
   ^   o fun-11 sec-05 sub-20:
   ^     - Read in the if it needs a functional gene
   ^   o fun-11 sec-05 sub-21:
   ^     - Read in the antibiotic flags
   ^   o fun-11 sec-05 sub-22:
   ^     - Read in the effect entry
   ^   o fun-11 sec-05 sub-23:
   ^     - Read in the commenht entry
   ^   o fun-11 sec-05 sub-24:
   ^     - Read in the grade entry
   ^   o fun-11 sec-05 sub-25:
   ^     - Read in if the entry effects the entire gene
   ^   o fun-11 sec-05 sub-26:
   ^     - Read in the unkown type entry
   ^   o fun-11 sec-05 sub-27:
   ^     - Get the next line
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   /*****************************************************\
   * Fun-11 Sec-05 Sub-01:
   *   - Get first entry, start read in loop, & initailze`
   \*****************************************************/

   /*Get past the header*/
   tmpStr = fgets(buffStr, lenBuffUI, amrFILE);
   tmpStr = fgets(buffStr, lenBuffUI, amrFILE);

   for(uiAmr = 0; uiAmr < *numAmrUI; ++uiAmr)
   { /*Loop: Read in the file*/

      initAmrStruct(&amrSTAry[uiAmr]);
      tmpStr = buffStr;

      /**************************************************\
      * Fun-11 Sec-05 Sub-02:
      *   - Read in the gene id
      \**************************************************/

      amrSTAry[uiAmr].lenGeneIdUI = cLenStr(tmpStr, '\t');

      amrSTAry[uiAmr].geneIdStr =
         malloc(
              (amrSTAry[uiAmr].lenGeneIdUI + 1)
            * sizeof(char)
         ); /*Allocate memory for the gene name*/

      if(! amrSTAry[uiAmr].geneIdStr)
         goto memErr_sec06_sub02_readTbAmrTbl;

      cCpStr(
         amrSTAry[uiAmr].geneIdStr,
         tmpStr,
         amrSTAry[uiAmr].lenGeneIdUI
      );

      tmpStr += amrSTAry[uiAmr].lenGeneIdUI + 1;

      /**************************************************\
      * Fun-11 Sec-05 Sub-03:
      *   - Read in the variaint id
      \**************************************************/

      amrSTAry[uiAmr].lenVarIdUI =
         ulLenStr(tmpStr, ulTab, '\t');

      amrSTAry[uiAmr].varIdStr =
         malloc(
              (amrSTAry[uiAmr].lenVarIdUI + 1)
            * sizeof(char)
         ); /*Allocate memory for the variant id*/

      if(! amrSTAry[uiAmr].varIdStr)
         goto memErr_sec06_sub02_readTbAmrTbl;

      ulCpStr(
         amrSTAry[uiAmr].varIdStr,
         tmpStr,
         amrSTAry[uiAmr].lenVarIdUI
      );

      tmpStr += amrSTAry[uiAmr].lenVarIdUI + 1;

      /**************************************************\
      * Fun-11 Sec-05 Sub-04:
      *   - Read in the refernce positon
      \**************************************************/

      tmpStr =
         base10StrToUI(tmpStr, amrSTAry[uiAmr].refPosUI);

      --amrSTAry[uiAmr].refPosUI; /*Convert to index 0*/

      ++tmpStr; /*get off the tab*/

      /**************************************************\
      * Fun-11 Sec-05 Sub-05:
      *   - Read in the direction
      \**************************************************/

      if(*tmpStr == 'F') 
         amrSTAry[uiAmr].dirFlag = def_amrST_forwardDir;
      else if(*tmpStr == 'R') 
         amrSTAry[uiAmr].dirFlag = def_amrST_revCompDir;

      else amrSTAry[uiAmr].dirFlag = def_amrST_unkownDir;
     
      /*Move to the next entry*/
      while(*tmpStr++ != '\t') {}

      /**************************************************\
      * Fun-11 Sec-05 Sub-06:
      *   - Read in the mutation type
      \**************************************************/

      if(*tmpStr == '\t')
      { /*If: this was a blank entry*/
         amrSTAry[uiAmr].mutTypeStr[0] = 'N';
         amrSTAry[uiAmr].mutTypeStr[1] = 'A';
         amrSTAry[uiAmr].mutTypeStr[2] = '\0';

         goto endMut_fuh11_sec05_sub06;
      } /*If: this was a blank entry*/

      amrSTAry[uiAmr].mutTypeStr[0] = *tmpStr++;

      if(*tmpStr == '\t')
      { /*If: this is a short entry*/
         amrSTAry[uiAmr].mutTypeStr[1] = '\0';
         goto endMut_fuh11_sec05_sub06;
      } /*If: this is a short entry*/

      amrSTAry[uiAmr].mutTypeStr[1] = *tmpStr++;

      if(*tmpStr == '\t')
      { /*If: this is a short entry*/
         amrSTAry[uiAmr].mutTypeStr[2] = '\0';
         goto endMut_fuh11_sec05_sub06;
      } /*If: this is a short entry*/

      /*This is a full length entry*/
      amrSTAry[uiAmr].mutTypeStr[2] = *tmpStr++;
      amrSTAry[uiAmr].mutTypeStr[3] = '\0';

      /*Move to the next entry*/
      endMut_fuh11_sec05_sub06:;

      while(*tmpStr++ != '\t') {}

      /**************************************************\
      * Fun-11 Sec-05 Sub-07:
      *   - Read in the frame shift entry
      \**************************************************/

      amrSTAry[uiAmr].frameshiftBl = (*tmpStr == '1');

      while(*tmpStr++ != '\t') {}

      /**************************************************\
      * Fun-11 Sec-05 Sub-08:
      *   - Read in the reference sequence
      \**************************************************/

      amrSTAry[uiAmr].lenRefSeqUI = cLenStr(tmpStr, '\t');

      amrSTAry[uiAmr].refSeqStr =
         malloc(
              (amrSTAry[uiAmr].lenRefSeqUI + 1)
            * sizeof(char)
         ); /*Allocate memory for the refernce sequence*/

      if(! amrSTAry[uiAmr].refSeqStr)
         goto memErr_sec06_sub02_readTbAmrTbl;

      cCpStr(
         amrSTAry[uiAmr].refSeqStr,
         tmpStr,
         amrSTAry[uiAmr].lenRefSeqUI
      );

      tmpStr += amrSTAry[uiAmr].lenRefSeqUI + 1;

      /**************************************************\
      * Fun-11 Sec-05 Sub-09:
      *   - Read in the amr sequence
      \**************************************************/

      amrSTAry[uiAmr].lenAmrSeqUI = cLenStr(tmpStr, '\t');

      amrSTAry[uiAmr].amrSeqStr =
         malloc(
              (amrSTAry[uiAmr].lenAmrSeqUI + 1)
            * sizeof(char)
         ); /*Allocate memory for the amrernce sequence*/

      if(! amrSTAry[uiAmr].amrSeqStr)
         goto memErr_sec06_sub02_readTbAmrTbl;

      cCpStr(
         amrSTAry[uiAmr].amrSeqStr,
         tmpStr,
         amrSTAry[uiAmr].lenAmrSeqUI
      );

      tmpStr += amrSTAry[uiAmr].lenAmrSeqUI + 1;

      /**************************************************\
      * Fun-11 Sec-05 Sub-10:
      *   - Read in the frist codon base in reference
      \**************************************************/

      if(*tmpStr == 'N')
      { /*If: this is missing*/
         amrSTAry[uiAmr].codonPosUI = 0;

         while(*tmpStr++ != '\t') {}
      } /*If: this is missing*/

      else
      { /*Else: I have a number*/
         tmpStr =
            base10StrToUI(
               tmpStr,
               amrSTAry[uiAmr].codonPosUI
            );

         --amrSTAry[uiAmr].codonPosUI; /*to index 0*/
         ++tmpStr;
      } /*Else: I have a number*/

      /**************************************************\
      * Fun-11 Sec-05 Sub-11:
      *   - Read in the starting codon number
      \**************************************************/

      if(*tmpStr == 'N')
      { /*If: this is missing*/
         amrSTAry[uiAmr].codonNumUI = 0;

         while(*tmpStr++ != '\t') {}
      } /*If: this is missing*/

      else
      { /*Else: I have a number*/
         tmpStr =
            base10StrToUI(
               tmpStr,
               amrSTAry[uiAmr].codonNumUI
            );

         ++tmpStr;
      } /*Else: I have a number*/


      /**************************************************\
      * Fun-11 Sec-05 Sub-12:
      *   - Read in the ending codon number
      \**************************************************/

      if(*tmpStr == 'N')
      { /*If: this is missing*/
         amrSTAry[uiAmr].endCodonNumUI = 0;

         while(*tmpStr++ != '\t') {}
      } /*If: this is missing*/

      else
      { /*Else: I have a number*/
         tmpStr =
            base10StrToUI(
               tmpStr,
               amrSTAry[uiAmr].endCodonNumUI
            );

         ++tmpStr;
      } /*Else: I have a number*/


      /**************************************************\
      * Fun-11 Sec-05 Sub-13:
      *   - Read in the reference amino acid sequence
      \**************************************************/

      amrSTAry[uiAmr].lenRefAaUI= cLenStr(tmpStr,'\t');

      amrSTAry[uiAmr].refAaStr =
         malloc(
              (amrSTAry[uiAmr].lenRefAaUI + 1)
            * sizeof(char)
         ); /*Allocate memory for refernce aa seq*/
   
      if(! amrSTAry[uiAmr].refAaStr)
         goto memErr_sec06_sub02_readTbAmrTbl;

      cCpStr(
         amrSTAry[uiAmr].refAaStr,
         tmpStr,
         amrSTAry[uiAmr].lenRefAaUI
      );

      tmpStr += amrSTAry[uiAmr].lenRefAaUI + 1;

      /**************************************************\
      * Fun-11 Sec-05 Sub-14:
      *   - Read in the amr amino acid sequence
      \**************************************************/

      amrSTAry[uiAmr].lenAmrAaUI= cLenStr(tmpStr,'\t');

      amrSTAry[uiAmr].amrAaStr =
         malloc(
              (amrSTAry[uiAmr].lenAmrAaUI + 1)
            * sizeof(char)
         ); /*Allocate memory for amr aa seq*/
   
      if(! amrSTAry[uiAmr].amrAaStr)
         goto memErr_sec06_sub02_readTbAmrTbl;

      cCpStr(
         amrSTAry[uiAmr].amrAaStr,
         tmpStr,
         amrSTAry[uiAmr].lenAmrAaUI
      );

      tmpStr += amrSTAry[uiAmr].lenAmrAaUI + 1;
         
      /**************************************************\
      * Fun-11 Sec-05 Sub-15:
      *   - Read in the gene starting position
      \**************************************************/

      if(*tmpStr == 'N')
      { /*If: this is missing*/
         amrSTAry[uiAmr].geneFirstRefUI = 0;

         while(*tmpStr++ != '\t') {}
      } /*If: this is missing*/

      else
      { /*Else: I have a number*/
         tmpStr =
            base10StrToUI(
               tmpStr,
               amrSTAry[uiAmr].geneFirstRefUI
            );

         --amrSTAry[uiAmr].geneFirstRefUI; /*to index 0*/
         ++tmpStr;
      } /*Else: I have a number*/

      /**************************************************\
      * Fun-11 Sec-05 Sub-16:
      *   - Read in the gene ending position
      \**************************************************/

      if(*tmpStr == 'N')
      { /*If: this is missing*/
         amrSTAry[uiAmr].geneLastRefUI = 0;

         while(*tmpStr++ != '\t') {}
      } /*If: this is missing*/

      else
      { /*Else: I have a number*/
         tmpStr =
            base10StrToUI(
               tmpStr,
               amrSTAry[uiAmr].geneLastRefUI
            );

         --amrSTAry[uiAmr].geneLastRefUI; /*to index 0*/
         ++tmpStr;
      } /*Else: I have a number*/

      /**************************************************\
      * Fun-11 Sec-05 Sub-17:
      *   - Check if is a high resistance gene
      \**************************************************/
      
      amrSTAry[uiAmr].highResBl = (*tmpStr == '1');

      while(*tmpStr++ != '\t') {}

      /**************************************************\
      * Fun-11 Sec-05 Sub-18:
      *   - Check if is a low resistance gene
      \**************************************************/
      
      amrSTAry[uiAmr].lowResBl = (*tmpStr == '1');

      while(*tmpStr++ != '\t') {}

      /**************************************************\
      * Fun-11 Sec-05 Sub-19:
      *   - Check if resistance is additive
      \**************************************************/
      
      amrSTAry[uiAmr].additiveResBl = (*tmpStr == '1');

      while(*tmpStr++ != '\t') {}

      /**************************************************\
      * Fun-11 Sec-05 Sub-20:
      *   - Read in the if it needs a functional gene
      \**************************************************/

      /*Nothing in the column; should not happen*/
      if(*tmpStr == '\t')
      { /*If: there is no entry*/
         
         amrSTAry[uiAmr].lenNeedsGeneUI = 2;

         amrSTAry[uiAmr].needsGeneStr =
            malloc( 3 * sizeof(char));

         if(! amrSTAry[uiAmr].needsGeneStr)
            goto memErr_sec06_sub02_readTbAmrTbl;

        amrSTAry[uiAmr].needsGeneStr[0] = 'N';
        amrSTAry[uiAmr].needsGeneStr[1] = 'A';
        amrSTAry[uiAmr].needsGeneStr[2] = '\0';

        ++tmpStr;
      } /*If: there is no entry*/

      else
      { /*Else if: there  an entry (may be NA)*/
         amrSTAry[uiAmr].lenNeedsGeneUI =
            cLenStr(tmpStr, '\t');

         amrSTAry[uiAmr].needsGeneStr =
            malloc(
                 (amrSTAry[uiAmr].lenNeedsGeneUI + 1)
               * sizeof(char)
            ); /*Allocate memory for the gene name*/

         if(! amrSTAry[uiAmr].needsGeneStr)
            goto memErr_sec06_sub02_readTbAmrTbl;

         cCpStr(
            amrSTAry[uiAmr].needsGeneStr,
            tmpStr,
            amrSTAry[uiAmr].lenNeedsGeneUI
         );

         tmpStr += amrSTAry[uiAmr].lenNeedsGeneUI + 1;
      } /*Else if: there  an entry (may be NA)*/

      /**************************************************\
      * Fun-11 Sec-05 Sub-21:
      *   - Read in the antibiotic flags
      \**************************************************/

      iDrug = 0;
      drugResUL = amrSTAry[uiAmr].amrFlagsUL;
      drugCrossResUL = amrSTAry[uiAmr].amrFlagsUL;

      while(*tmpStr != '*')
      { /*Loop: Check for antibiotic resistance*/
         if(iDrug >= def_amrST_maxFlags)
         { /*If: I need to start a new array*/
            iDrug = 0;
            ++drugResUL;
            ++drugCrossResUL;
         } /*If: I need to start a new array*/

         *drugResUL |=
            ((*tmpStr - 48) & def_amrST_resFlag) << iDrug;

         *drugCrossResUL |=
               ((*tmpStr - 48) & def_amrST_crossResFlag)
            << iDrug;

         ++iDrug;

         /*Move to the enxt antibiotic*/
         while(*tmpStr++ != '\t') {}
      } /*Loop: Check for antibiotic resistance*/
     
      /*Move to the effect entry*/
      while(*tmpStr++ != '\t') {}

      /**************************************************\
      * Fun-11 Sec-05 Sub-22:
      *   - Read in the effect entry
      \**************************************************/

      amrSTAry[uiAmr].lenEffectUI =
         ulLenStr(tmpStr, ulTab, '\t');

      amrSTAry[uiAmr].effectStr =
         malloc(
              (amrSTAry[uiAmr].lenEffectUI + 1)
            * sizeof(char)
         ); /*Allocate memory for the variant id*/

      if(! amrSTAry[uiAmr].effectStr)
         goto memErr_sec06_sub02_readTbAmrTbl;

      ulCpStr(
         amrSTAry[uiAmr].effectStr,
         tmpStr,
         amrSTAry[uiAmr].lenEffectUI
      );

      tmpStr += amrSTAry[uiAmr].lenEffectUI + 1;

      /**************************************************\
      * Fun-11 Sec-05 Sub-23:
      *   - Read in the comment entry
      \**************************************************/

      amrSTAry[uiAmr].lenCommentUI = 0;

      while(tmpStr[amrSTAry[uiAmr].lenCommentUI] > 31)
         ++amrSTAry[uiAmr].lenCommentUI;

      amrSTAry[uiAmr].commentStr =
         malloc(
              (amrSTAry[uiAmr].lenCommentUI + 1)
            * sizeof(char)
         ); /*Allocate memory for the variant id*/

      if(! amrSTAry[uiAmr].commentStr)
         goto memErr_sec06_sub02_readTbAmrTbl;

      ulCpStr(
         amrSTAry[uiAmr].commentStr,
         tmpStr,
         amrSTAry[uiAmr].lenCommentUI
      );

      tmpStr += amrSTAry[uiAmr].lenCommentUI + 1;

      while(*tmpStr > 31) ++tmpStr;

      /**************************************************\
      * Fun-11 Sec-05 Sub-24:
      *   - Read in the grade entry
      \**************************************************/

      if(*tmpStr == '\n') goto nextLine_readAmrST;
      ++tmpStr; /*Get off the tab*/

      amrSTAry[uiAmr].gradeC = 48 - *tmpStr;

      while(*tmpStr > 31) ++tmpStr;

      /**************************************************\
      * Fun-11 Sec-05 Sub-25:
      *   - Read in if the entry effects the entire gene
      \**************************************************/

      if(*tmpStr == '\n') goto nextLine_readAmrST;
      ++tmpStr; /*Get off the tab*/

      amrSTAry[uiAmr].wholeGeneFlag = 48 - *tmpStr;

      while(*tmpStr > 31) ++tmpStr;

      /**************************************************\
      * Fun-11 Sec-05 Sub-26:
      *   - Read in the unkown type entry
      \**************************************************/

      if(*tmpStr == '\n') goto nextLine_readAmrST;
      ++tmpStr; /*Get off the tab*/

      amrSTAry[uiAmr].unknownBl = 48 - *tmpStr;

      /**************************************************\
      * Fun-11 Sec-05 Sub-27:
      *   - Get the next line
      \**************************************************/

      nextLine_readAmrST:;

      tmpStr = fgets(buffStr, lenBuffUI, amrFILE);
   } /*Loop: Read in the file*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-11 Sec-06:
   ^   - Clean up
   ^   o fun-11 sec-06 sub-01:
   ^     - Clean up when everything worked
   ^   o fun-11 sec-06 sub-02:
   ^     - Clean up for memory erorrs
   ^   o fun-11 sec-06 sub-03:
   ^     - Clean up for file erorrs
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   /*****************************************************\
   * Fun-11 Sec-06 Sub-01:
   *   - Clean up when everything worked
   \*****************************************************/

   free(buffStr);
   fclose(amrFILE);

   ++(*numDrugsI); /*Convert index 0 to index 1*/

   *errC = 0;

   sortAmrStructArray(amrSTAry, 0, *numAmrUI - 1);

   return amrSTAry;

   /*****************************************************\
   * Fun-11 Sec-06 Sub-02:
   *   - Clean up for memory erorrs
   \*****************************************************/

   memErr_sec06_sub02_readTbAmrTbl:;

   fclose(amrFILE);
   if(buffStr != 0) free(buffStr);

   if(newDrugAryBl)
   { /*If: I made a new drug array*/
      if(*drugStrAry != 0) free(*drugStrAry);
      *drugStrAry = 0;
      *numDrugsI = 0;
      *maxDrugsI = 0;
   } /*If: I made a new drug array*/

   if(amrSTAry) freeAmrStructArray(&amrSTAry, *numAmrUI);


   *errC = def_amrST_memError;

   return 0;

   /*****************************************************\
   * Fun-11 Sec-06 Sub-03:
   *   - Clean up for file erorrs
   \*****************************************************/

   fileErr_sec06_sub03_readTbAmrTbl:;

   if(amrFILE != 0) fclose(amrFILE);
   if(buffStr != 0) free(buffStr);

   if(newDrugAryBl)
   { /*If: I made a new drug array*/
      if(*drugStrAry != 0) free(*drugStrAry);
      *drugStrAry = 0;
      *numDrugsI = 0;
      *maxDrugsI = 0;
   } /*If: I made a new drug array*/

   if(amrSTAry) freeAmrStructArray(&amrSTAry, *numAmrUI);

   *errC = def_amrST_invalidFILE;

   return 0;
} /*readTbAmrTbl*/

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
