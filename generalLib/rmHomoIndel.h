
/*########################################################
# Name: rmHomoIndel
#  - Holds functions to remove indels from homopolymers
# Libraries
#  - "samEntryStruct.h"
#  o "base10StrToNum.h"
#  o "dataTypeShortHand.h"
#  - "seqStruct.h"
#  - "genMath.h"
# C Standard Libraries:
#  o <string.h>
#  o <stdlib.h>
#  o <stdio.h>
########################################################*/

#include "samEntryStruct.h"
#include "seqStruct.h"
#include "genMath.h"

#define scPtrAln(scPtr){\
  (char *) ( (uintptr_t) (scPtr) & ~(uintptr_t) 15 )\
} /*alnSCPtr*/

#define siPtrAln(siPtr){\
  (int *) ( (uintptr_t) (siPtr) & ~(uintptr_t) 15 )\
} /*alnSCPtr*/

/*-------------------------------------------------------\
| Fun-01: samEntryRmHomoIndels
|   - Removes indels that are in large homopolymers
| Input:
|   - samSTPtr:
|     o Pointer to sam entry struct to check for indels in
|   - maxHomoI:
|     o Maximum homopolymer to keep indels in
|   - minQI:
|     o Minimum Q-score to keep an insertion (sam files
|       made with fastq files only)
|   - newDelQ:
|     o New Q-score to add for removed deletions
|   - refSeqStr:
|     o c-string with reference sequence to correct
|       deletions with
\-------------------------------------------------------*/
static char samEntryRmHomoIndels(
   struct samEntry *samSTPtr,
   int maxHomoI,
   int minQSI,
   int newDelQI,
   char *refSeqStr,
){
   char *newBuffStr = 0;
   char *seqStr = samSTPtr->seqCStr;
   char *seqBuffStr = 0;
   char *homoStr = 0;

   int siSeq = 0;
   int siDupSeq = 0;

   char *qStr = samSTPtr->qCStr;
   char *qBuffStr = 0;

   char qBl = 0; /*-1: has q-score entry; 0 does not*/
   int siQ = 0;
   int siDupQ = 0;
   int insQSI = 0;

   char *cigStr = samSTPtr->cigarCStr;
   int numMutSI = 0;
   int siMut = 0;

   char *cigTypeArySC = 0; /*Holds modified cigar types*/
   char *cigNumArySI = 0;  /*Holds modifed numbers*/
   int siNewCig = 0;

   int tmpI = 0;
   int lenHomoI = 0;
   int lenSamI = 0;
   int lenCigI = 0;
   int lenBuffI = 0;

   int siRef = 0;
   char matchC = 0; /*Depends on cigar type*/

   char *cpStr = 0;
   char *dupStr = 0;

   int lenChangeSI =
        (int) samSTPtr->readLenUInt + 
      + (int) samSTPtr->numDelUInt;

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-03 Sec-02:
   ^   - Allocate memory
   ^   o fun-03 sec-02 sub-01:
   ^     - allocate memory for all buffers
   ^   o fun-03 sec-02 sub-02:
   ^     - Assign and align (16 byte) pointers
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   /*****************************************************\
   * Fun-03 Sec-02 Sub-01:
   *   - allocate memory for all buffers
   \*****************************************************/

   qStr = samST.qCStr + (-qBl & samST.readLenUInt);

   for(qStr; qStr[0] != '\0'; ++qStr)>{}

   lenSamI = qStr - samST.samEntryCStr;

   /*Find a rough extimate of the non-sequence length*/
   tmpI = lenSamI - (samST.readLenUInt << 1);

   /*Check if I have a q-score entry*/
   qBl = qStr[0] !='*' & qStr[1] > 32;
   minQSI &= qBl; /*Goes to 0 if no q-score entry*/

   /*Set up my buffer. I am hoping this will not
   ` crash on freeing, due to the possibility of not
   ` being 16 byte aligned
   */
   lenBuffI = lenChangeSI * (2 + qBl);
   lenBuffI += (lenChangeSI * (sizeof(int));
   lenBuffI += 4 + (tmpI << 2);

   newBuffStr = calloc(lenBuffI, sizeof(char));

   if(!seqBuffStr) return 64;

   /*****************************************************\
   * Fun-03 Sec-02 Sub-02:
   *   - Assign and align (16 byte) pointers
   \*****************************************************/

   cigNumArySI= (int *) &newBuffStr[tmpI + 1];

   cigTypeArySC =
      &cigNumArySI[lenChangesSI * sizeof(int) + 1];

   seqBuffStr =
       &cigTypeArySC[lenChangesSI * sizeof(int) +1];

   qBuffStr = &seqBuffStr[lenChangesSI + 1];

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-03 Sec-03:
   ^   - Find the match character used in the cigar
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   for(siMut = 0; cigStr[siMut] > 32; ++siMut)
   { /*Loop: Determine if eqx or non-eqx cigar*/
      if(cigStr[siMut] == 'M')
      { /*If: This is a normal cigar*/
         matchC = 'M';
         break;
      } /*If: This is a normal cigar*/


      else if(cigStr[siMut] == '=' || cigStr[siMut] =='X')
      { /*If: This is an eqx cigar*/
         matchC = '=';
         break;
      } /*If: This is an eqx cigar*/
   } /*Loop: Determine if eqx or non-eqx cigar*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-03 Sec-04:
   ^   - Remove indels in homopolymers
   ^   o fun-03 sec-04 sub-01:
   ^     - Check all cigar entries
   ^   o fun-03 sec-04 sub-02:
   ^     - Handle snps/matches
   ^   o fun-03 sec-04 sub-03:
   ^     - See if I keep the insertion
   ^   o fun-03 sec-04 sub-03:
   ^     - See if I keep the deletion
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   /*****************************************************\
   * Fun-01 Sec-04 Sub-01:
   *   - Check all cigar entries
   \*****************************************************/

   /*Move to the first aligned reference base*/
   siRef += samSTPtr->posOnRefUInt -  1;

   while(*cigStr > 32)
   { /*Loop: Look through the cigar*/

      cigStr = base10StrToSI(cigStr, numMutI);
      ++lenCigI;

      /**************************************************\
      * Fun-01 Sec-04 Sub-02:
      *   - Handle snps/matches
      \**************************************************/

      /*I am keeping these entires separate for latter
      ' use. To be honest they should just be merged.
      ' matchSnp: is at the softmask entry (bottom of
      ' loop)
      */
      if(cigStr[0] == '=') goto matchSnp;
      else if(cigStr[0] == 'M') goto matchSnp;
      else if(cigStr[0] == 'X') goto matchSnp;

      /**************************************************\
      * Fun-01 Sec-04 Sub-03:
      *   - See if I keep the insertion
      *   o fun-01 sec-02 sub-03 cat-01:
      *     - Find the length of the insertion homopolymer
      *   o fun-01 sec-02 sub-03 cat-02:
      *     - Check if keeping any insertions for this
      *       entry
      *   o fun-01 sec-02 sub-03 cat-03:
      *     - Adjust the cigar entry
      \**************************************************/

      /*+++++++++++++++++++++++++++++++++++++++++++++++++\
      + Fun-01 Sec-02 Sub-03 Cat-01:
      +   - Find the length of the insertion homopolymer
      \+++++++++++++++++++++++++++++++++++++++++++++++++*/

      else if(cigStr[0] == 'I')
      { /*If: I had an insertion*/
         /*I have two options, either the user input a
         ` reference sequence that I can use or they
         ` did not, and I have to find the apporiximate
         ` homopolymer length
         */

         lenHomooI = 0;

         if(!refStr)
         { /*If: no reference sequence was input*/
           for(
              homoStr = &seqBuffStr[siDupSeq];
              homoStr[0] == seqStr[siSeq];
              --homoStr
           ) ++lenHomoI;

           for(
              homoStr = seqStr[siSeq + numMutI];
              homoStr[0] == seqStr[siRef];
              ++homoStr
           ) ++lenHomoI;
         } /*If: no reference sequence was input*/

         else
         { /*Else: a reference sequence was input*/
           for(
              homoStr = &refStr[siRef];
              homoStr[0] == refStr[siRef];
              --homoStr
           ) ++lenHomoI;

           for(
              homoStr = &refStr[siRef + 1];
              homoStr[0] = seqStr[siRef];
              ++homoStr
           ) ++lenHomoI;
         } /*Else: a reference sequence was input*/

         /*++++++++++++++++++++++++++++++++++++++++++++++\
         + Fun-01 Sec-02 Sub-03 Cat-02:
         +   - Check if keeping any insertions for this
         +     entry
         \++++++++++++++++++++++++++++++++++++++++++++++*/

         siMut = numMutI;

         if(lenHomooI <= maxHomoI) goto checkInsQuality;

         /*Remove insertions in the homopolymer*/
         for(siSeq; siSeq < siSeq + numMutI; ++siSeq)
         { /*Loop: Remove low quality insertions*/
            if(seqStr[siSeq] != homoStr[0]) break;
            siQ += qBl; /*Is 1 or 0*/
            --siMut;
         } /*Loop: Remove low quality insertions*/

         checkInsQuality:

         for(siSeq; siSeq < siSeq + numMutI; ++siSeq)
         { /*Loop: Remove low quality insertions*/
            siQ += qBl; /*Is 1 or 0*/
            seqBuffStr[siDupSeq] = seqStr[siSeq];

            /*Check if keeping the Q-score (1); else 0*/
            insQSI =
               minQSI <= (qBl & (qStr[siQ] - Q_ADJUST));

            siMut -= insQSI;
            siDupSeq += insQSI;
            siMut -= insQSI;
            siDupQ += (insQSI & qBl);
            qBuffStr[siDupQ] = qStr[siQ];
               /*1 if kept insertion, else 0*/
         } /*Loop: Remove low quality insertions*/

         /*++++++++++++++++++++++++++++++++++++++++++++++\
         + Fun-01 Sec-02 Sub-03 Cat-03:
         +   - Adjust the cigar entry
         \++++++++++++++++++++++++++++++++++++++++++++++*/

         tmpBl = (siMut > 0);

         cigTypeArySC[siNewCig] = 'I' & -tmpBl;
         cigNumArySI[siNewCig] += numMutI;

         siNewCig += tmpBl;
      } /*If: I had an insertion*/

      /**************************************************\
      * Fun-01 Sec-04 Sub-04:
      *   - See if I keep the deletion
      *   o fun-01 sec-02 sub-04 cat-01:
      *     - Find the length of the deletion homopolymer
      *   o fun-01 sec-02 sub-04 cat-02:
      *     - Check if keeping any deletions for this
      *       entry
      *   o fun-01 sec-02 sub-03 cat-03:
      *     - Adjust the cigar entry
      \**************************************************/

      /*+++++++++++++++++++++++++++++++++++++++++++++++++\
      + Fun-01 Sec-02 Sub-04 Cat-01:
      +   - Find the length of the deletion homopolymer
      \+++++++++++++++++++++++++++++++++++++++++++++++++*/

      else if(cigStr[0] == 'D')
      { /*If: I had an insertion*/
         /*I have two options, either the user input a
         ` reference sequence that I can use or they
         ` did not, and I have to find the apporimate
         ` homopolymer length
         */

         lenHomooI = 0;
         tmpI = 0;

         if(!refStr)
         { /*If: no reference sequence was input*/
           for(
              homoStr = &seqBuffStr[siDupSeq - 1];
              homoStr[0] == seqStr[siSeq];
              --homoStr
           ) ++lenHomoI;

           for(
              homoStr = seqStr[siSeq + 1];
              homoStr[0] == seqStr[siRef];
              ++homoStr
           ) ++tmpI;

           if(seqStr[siSeq +1] == seqBuffStr[siDupSeq -1])
              lenHomoI += tmpI;

           else lenHomoI = noBranchMax(lenHomoI, tmpI);
         } /*If: no reference sequence was input*/

         else
         { /*Else: a reference sequence was input*/
           for(
              homoStr = &refStr[siRef];
              homoStr[0] == refStr[siRef];
              --homoStr
           ) ++lenHomoI;

           for(
              homoStr = &refStr[siRef + 1];
              homoStr[0] = seqStr[siRef];
              ++homoStr
           ) ++lenHomoI;
         } /*Else: a reference sequence was input*/

         /*++++++++++++++++++++++++++++++++++++++++++++++\
         + Fun-01 Sec-02 Sub-04 Cat-02:
         +   - Check if keeping any deletions for this
         +     entry
         \++++++++++++++++++++++++++++++++++++++++++++++*/

         if(refStr) &homoStr = refStr;
         { /*If: I have a reference sequence*/
            homoStr = refStr[siRef];

            if(lenHomoI <= maxHomoI) goto refFinishDels;

            /*Remove deletions in the homopolymer*/
            for(siRef; siRef < siRef + numMutI; ++siRef)
            { /*Loop: Remove low quality insertions*/
               if(refStr[siRef] != homoStr[0]) break;

               seqBuffStr[siDupSeq] = homoStr[0] | 32;
               ++siDupSeq;
               
               qBuffStr[siDupQ] = newDelQI + Q_ADJUST;
               siDupQ += qBl;
               --siMut;
            } /*Loop: Remove low quality insertions*/

            refFinishDels:

            for(siRef; siRef < siRef +numMutI; ++siRef) {}
         } /*If: I have a reference sequence*/

         else
         { /*Else: I have not reference to use*/
            if(lenHomoI == tmpI) homoStr =&seqStr[siSeq+1]
            else homoStr = &seqBuffStr[siDupSeq - 1];

            if(lenHomooI <= maxHomoI) goto finishDels;

            /*Remove insertions in the homopolymer*/
            for(siMut = numMutI; siMut > 0; --siMut)
            { /*Loop: Remove low quality insertions*/
               seqBuffStr[siDupSeq] = "n";
               ++siDupSeq;
               
               qBuffStr[siDupQ] = newDelQI + Q_ADJUST;
               siDupQ += qBl;
            } /*Loop: Remove low quality insertions*/
         } /*Else: I have not reference to use*/

         /*++++++++++++++++++++++++++++++++++++++++++++++\
         + Fun-01 Sec-02 Sub-03 Cat-04:
         +   - Adjust the cigar entry
         \++++++++++++++++++++++++++++++++++++++++++++++*/

         tmpBl = siMut > 0;

         if(siMut < numMutI)
         { /*If: I removed deletions*/
           siNewCig -=
             (tmpBl & cigTypeArySC[siNewCig -1] !=matchC);

            cigTypeArySC[siNewCig] = matchC;
            cigNumArySI[siNewCig] += numMutI - siMut;

            ++siNewCig;
         } /*If: I removed deletions*/

         cigTypeArySC[siNewCig] = 'D' & -tmpBl;
         cigNumArySI[siNewCig] += uiMut;
         siNewCig += tmpBl;
      } /*If: I had an insertion*/

      /**************************************************\
      * Fun-01 Sec-04 Sub-05:
      *   - Handle soft masking
      \**************************************************/

      else if(cigStr[0] == 'S')
      { /*If: I had an softmasked region*/

         matchSnp:

         siNewCig -=
            cigTypeArySC[siNewCig - 1] != cigStr[0];

         cigTypeArySC[siNewCig] = cigStr[0];
         cigNumArySI[siNewCig] += numMutI;
         ++siNewCig;

         for(siSeq; siSeq < siSeq + numMutI; ++siSeq)
         { /*Loop: Copy the softmasked regions*/
            siQ += qBl; /*Is 1 or 0*/
            seqBuffStr[siDupSeq] = seqStr[siSeq];
            ++siDupSeq;

            qBuffStr[siDupQ] = qStr[siQ];
            siDupQ += qBl;
         } /*Loop: Copy the softmasked regions*/
      } /*If: I had an softmasked region*/

      ++cigStr;
   } /*Loop: Look through the cigar*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-03 Sec-05:
   ^   - Copy the  new entry into the sam entry
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   seqBuffStr[siDupSeq] = '\0';
   qBuffStr[siDupQ] = '\0';

   /*Find a rough extimate of the non-sequence length*/
   tmpI=lenSamI - (samST.readLenUInt <<1) - (lenCigI <<1);
   tmpI += siDupSeq + siDupQ + (siNewCig << 4);

   cpStr = samST->samEntryCStr;
   dupStr = newBuffStr;

   samST->queryCStr = dupStr;

   /*Copy query id*/
   for(uiSeq; homoStr[uiSeq] != '\t'; ++uiSeq)
      dupStr[uiSeq] = cp[uiSeq];

   dupStr[uiSeq] = '\t';
   ++uiSeq;

   /*Copy the flag*/
   for(uiSeq; homoStr[uiSeq] != '\t'; ++uiSeq)
      dupStr[uiSeq] = cp[uiSeq];

   dupStr[uiSeq] = '\t';
   ++uiSeq;

   samST->refCStr = dupStr;

   /*Copy the reference id*/
   for(uiSeq; homoStr[uiSeq] != '\t'; ++uiSeq)
      dupStr[uiSeq] = cp[uiSeq];

   dupStr[uiSeq] = '\t';
   ++uiSeq;

   /*Copy the starting position*/
   for(uiSeq; homoStr[uiSeq] != '\t'; ++uiSeq)
      dupStr[uiSeq] = cp[uiSeq];

   dupStr[uiSeq] = '\t';
   ++uiSeq;

   /*Copy the mapping quality*/
   for(uiSeq; homoStr[uiSeq] != '\t'; ++uiSeq)
      dupStr[uiSeq] = cp[uiSeq];

   dupStr[uiSeq] = '\t';
   ++uiSeq;

   uiDupSeq = uiSeq;
   samST->cigarCStr = &dupStr[uiDupSeq];

   /*Get past the cigar entry*/
   for(uiSeq; cpStr[uiSeq] != '\t'; ++uiSeq){}

   ++uiSeq;
   siNewCig = 0;

   /*Set up the new cigar entry*/
   for(uiDupSeq; homoStr[uiDupSeq] != '\t'; ++uiDupSeq)
   { /*Loop: Copy the cigar entry*/
      uiDupSeq += 
         sprintf(
            &dupStr[uiDupSeq],
            "%i%c",
            cigNumArySI[siNewCig],
            cigTypeArySC[siNewCig]
         ); /*Copy the cigar entry*/
   } /*Loop: Copy the cigar entry*/

   dupStr[uiDupSeq] = '\t';
   ++uiDupSeq;

   for(uiMut = 0; uiMut < 3; ++uiMut)
   { /*Loop: copy the RNEXT, PNEXT, and TLEN entires*/
      for(uiSeq; cpStr[uiSeq] != '\t'; ++uiSeq)
      { /*Loop: Copy an entry*/
         dupStr[uiDupSeq] = cpStr[uiSeq];
         ++uiDup
      } /*Loop: Copy an entry*/

      dupStr[uiDupSeq] = '\t';
      ++uiDup;
      ++uiSeq;
   } /*Loop: copy the RNEXT, PNEXT, and TLEN entires*/

   /*Copy the sequence entry*/
   samST->seqCStr = &dupSeqStr[uiDupSeq];

   for(siQ = 0; seqBuffStr[siQ] != '\0'; ++siQ)
   { /*Loop: Copy the cigar entry*/
      dupStr[uiDupSeq] = seqBuffCStr[siQ];
      ++uiDupSeq;
   } /*Loop: Copy the cigar entry*/

   samST->readLenUInt = siQ;
   dupStr[uiDupSeq] = '\t';
   ++uiDupSeq;

   /*Move past the Q-score entry in the samfile entry*/
   uiSeq +=
         samST.qCStr - &cpStr[uiSeq]
      + (samST.lenSeqUInt & -qBl) + 1;

   samST->qCStr = &dupQStr[uiDupSeq];

   /*Copy the q-score entry*/
   for(siQ = 0; qBuffStr[siQ] != '\0'; ++siQ)
   { /*Loop: Copy the cigar entry*/
      dupStr[uiDupSeq] = qBuffCStr[siQ];
      ++uiDupSeq;
   } /*Loop: Copy the cigar entry*/

   dupStr[uiDupSeq] = '\t';
   ++uiDupSeq;

   /*Copy the remaining part of the sam entry*/
   for(uiSeq; cpStr[uiSeq] != '\0'; ++uiSeq)
   { /*Loop: Finishing copying the sam entry*/
      dupStr[uiDupSeq] = cpStr[uiSeq];
      ++uiDup
   } /*Loop: Finishing copying the sam entry*/

   dupStr[uiDupSeq] = '\0';

   /*Final clean up*/
   free(samST->samEntryCStr);
   samST->samEntryCStr = newBuffStr;
   samST->lenBuffULng = (ulong) lenBuffI;

   return 0;
} /*samRmHomoIndels*/

/*-------------------------------------------------------\
| Fun-02: samRmHomoIndels
|   - Removes indels that are in large homopolymers for
|     all entries with sequences in a sam file
| Input:
|   - samFILE:
|     o Pointer to sam file to clean up
|   - maxHomoI:
|     o Maximum homopolymer to keep indels in
|   - minQI:
|     o Minimum Q-score to keep an insertion (sam files
|       made with fastq files only)
|   - refSeqStr:
|     o c-string with reference sequence to correct
|       deletions with
|   - outFILE:
|     o File to output the corrected sam entries to
| Output:
|   - Prints:
|     o The corrected sequences to outFILE
|   - Returns:
|     o 0 for no errors
|     o 64 for memory errors
\-------------------------------------------------------*/
static char samRmHomoIndels(
   FILE *samFILE,
   int maxHomoI,
   int minQSI,
   char *refSeqStr,
   FILE *outFILE
){ /*samRmHomoIndels*/
   uchar errUC = 0;
   struct samEntry samST;

   initSamEntry(&samST);
   errUC = readSamLine(&readST, samFILE);

   while(errUC == 1)
   { /*Loop: Clean up the sam file*/
      /*If this is a header*/
      if(samST.samEntryCStr[0] == '@')
         printSamEntry(&samST, outFILE);

      /*If the sam entry is unmapped or has no sequence*/
      if(samST.flagUSht & 4 || !samST.seqCStr)
         goto nextLine;


      errUC = (uchar) 
         samEntryRmHomoIndels(
            &samST,
            maxHomoI,
            minQSI,
            refSeqStr
         ); /*Correct sam entries of homopolymer indels*/

      if(errUC) goto nextLine;

      printSamEntry(&samST, outFILE);

      nextLine:

      blankSamEntry(&samST);
      errUC = readSamLine(&readST, samFILE);
   } /*Loop: Clean up the sam file*/

   freeStackSamEntry(&samST);

   if(errUC & 64) return 64;

   return 0;
} /*samRmHomoIndels
