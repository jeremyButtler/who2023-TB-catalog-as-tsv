#include "samEntryStruct.h"

#include <stdlib.h>
#include <stdio.h>

/*These have no .c files*/
#include "dataTypeShortHand.h"
#include "base10StrToNum.h"
#include "ulCpStr.h"
#include "numToStr.h"

/*-------------------------------------------------------\
| Fun-03: initSamEntry
|  - Initializes a samEntry structure for use. This 
|    function should only ever be called once per
|    structure or after freeSamEntryStack has been used.
|    Use blankSamEntry to reset the values in a samEntry
|    struct after initialization.
| Input:
|  - samSTPtr:
|    o pointer to samEntry struct to initialize
| Output:
|  - Modifies:
|    o samEntry to have buffers and all values set to 0
|  - Returns:
|    o 0 for success
|    o 1 for memory error
\-------------------------------------------------------*/
unsigned char initSamEntry(struct samEntry *samSTPtr){
    uchar errUC = 0;
    
    (samSTPtr)->seqStr = calloc(1024, sizeof(char));
    errUC |= (!(samSTPtr)->seqStr);
    (samSTPtr)->lenSeqBuffUI = 1024 & ((int) errUC - 1);
    
    (samSTPtr)->qStr = calloc(1024, sizeof(char));
    errUC |= (!(samSTPtr)->qStr);
    (samSTPtr)->lenQBuffUI = 1024 & ((int) errUC - 1);
    
    (samSTPtr)->cigTypeStr = calloc(256, sizeof(char));
    errUC |= (!(samSTPtr)->cigTypeStr);
    (samSTPtr)->lenCigBuffUI = 256 & ((int) errUC - 1);
    
    (samSTPtr)->cigValAryI = calloc(256, sizeof(int));
    errUC |= (!(samSTPtr)->cigValAryI);
    
    (samSTPtr)->extraStr = calloc(256, sizeof(char));
    errUC |= (!(samSTPtr)->extraStr);
    (samSTPtr)->lenExtraBuffUI = 256 & ((int) errUC - 1);
    
    /*Handle no memory errors*/
    if(!errUC) blankSamEntry((samSTPtr));
    
    return errUC;
} /*initSamEntry*/

/*-------------------------------------------------------\
| Fun-06: freeSamEntryStack
| Use:
|  - Frees all variables in samEntry, but not samEntry
| Input:
|  - samSTPtr
|    o Pointer to samEntry struct to free the interal
|      memory of
| Output:
|  - Frees:
|    o allocated memory in samSTPtr
\-------------------------------------------------------*/
void freeSamEntryStack(struct samEntry *samSTPtr){
    blankSamEntry((samSTPtr));
    
    free((samSTPtr)->seqStr);
    (samSTPtr)->seqStr = 0;
    (samSTPtr)->lenSeqBuffUI = 0;
    
    free((samSTPtr)->qStr);
    (samSTPtr)->qStr = 0;
    (samSTPtr)->lenQBuffUI = 0;
    
    free((samSTPtr)->cigTypeStr);
    (samSTPtr)->cigTypeStr = 0;
    (samSTPtr)->lenCigBuffUI = 0;
    
    free((samSTPtr)->cigValAryI);
    (samSTPtr)->cigValAryI = 0;
    
    free((samSTPtr)->extraStr);
    (samSTPtr)->extraStr = 0;
    (samSTPtr)->lenExtraBuffUI = 0;
} /*freeSamEntryStack*/

/*-------------------------------------------------------\
| Fun-04: freeSamEntry
|  - Frees a samEntry struct
| Input:
|  - samSTPtr
|    o Pointer to Sam entry struct to free
| Output:
|  - Frees:
|    o samSTPtr and its variables with allocated memory
\-------------------------------------------------------*/
void freeSamEntry(struct samEntry **samSTPtr){
    freeSamEntryStack((*samSTPtr));
    free((*samSTPtr));
    (*samSTPtr) = 0;
} /*freeSamEntry*/

/*-------------------------------------------------------\
| Fun-05: makeSamEntry
|  - Makes an heap allocated samEntry structure
| Input:
| Output:
|  - Returns:
|    o Pointer to a samEntry structure
|    o 0 if had an memory error
\-------------------------------------------------------*/
struct samEntry * makeSamEntry(){
  struct samEntry *samST=malloc(sizeof(struct samEntry));
  uchar errUC = 0;
  
  if(samST)
  { /*If: I did not have a memory error*/
     errUC = initSamEntry(samST);
     if(errUC) freeSamEntry(&samST);
  } /*If: I did not have a memory error*/
  
  return samST;
} /*makeSamEntry*/

/*-------------------------------------------------------\
| Fun-07: samEntryFindQScores
|   - Gets the median and mean Q-scores from a samEntry
|     Structure.
| Input:
|   - samSTPTr:
|     o Pointer to samEntry struct to find the median and
|       mean q-scores for
| Output:
|   - Modifies:
|     o samSTPtr->medianQF to have the median Q-score
|     o samSTPtr->meanQF to have the mean Q-score
|     o samSTPtr->qHistUI to have a histogram of Q-scores
|     o samSTPtr->sumQUL to have the sum of all Q-scores
\-------------------------------------------------------*/
void samEntryFindQScores(struct samEntry *samSTPtr){
    ulong qAdjustUL = ulCpMakeDelim(defQAdjust);
    ulong qScoresUL = 0;
    ulong *qPtrUL = (ulong *) (samSTPtr)->qStr;

    uchar *scoreAryUC = 0;
    int iQScore = 0;
    int iChar = 0;
    
    /*Find the number of Q-score characters in buffer*/
    for(
       iQScore = 0;
       iQScore < ((samSTPtr)->readLenUI >> defShiftByUL);
       ++iQScore
    ) { /*Loop: Update the q-score historgram and sum*/
       qScoresUL = qPtrUL[iQScore] - qAdjustUL;
       scoreAryUC = (uchar *) qScoresUL;
       
       for(iChar = 0; iChar < defCharPerUL; ++iChar)
       { /*Loop: Get the q-score entries*/
         ++(samSTPtr)->qHistUI[(uchar) scoreAryUC[iChar]];
         (samSTPtr)->sumQUL += (uchar) scoreAryUC[iChar];
       } /*Loop: Get the q-score entries*/
    } /*Loop: Update the q-score historgram and sum*/
    
    iQScore = (samSTPtr)->readLenUI;
    scoreAryUC = (uchar *) (samSTPtr)->qStr;
    
    for(
       iQScore -= ((samSTPtr)->readLenUI & defModByUL);
       iQScore < (samSTPtr)->readLenUI;
       ++iQScore
    ) { /*Loop: Copy the Q-score entries*/
       (samSTPtr)->qStr[iQScore] = scoreAryUC[iQScore];
        iChar = (uchar) scoreAryUC[iQScore] - defQAdjust;
       
       ++(samSTPtr)->qHistUI[(uint) iChar];
       (samSTPtr)->sumQUL += iChar;
    } /*Loop: Copy the Q-score entries*/
    
    (samSTPtr)->meanQF =
       (float) (samSTPtr)->sumQUL /(samSTPtr)->readLenUI;
    
    samEntryQHistToMed((samSTPtr));
} /*samEntryFindQScores*/

int cpQScores(struct samEntry *samSTPtr, char *cpQStr){
  char *tmpStr = 0;
  int iQ = 0;
  int iChar = 0;
  ulong qAdjustUL = ulCpMakeDelim(defQAdjust);
  ulong *cpPtrUL = (ulong *) (cpQStr);
  ulong *dupPtrUL = (ulong *) samSTPtr->qStr;
  ulong qScoreUL = 0;
  
  for(
     iQ = 0;
     iQ < ((samSTPtr)->readLenUI >> defShiftByUL);
     ++iQ
  ) { /*Loop: Copy the Q-score entries*/
     dupPtrUL[iQ] = cpPtrUL[iQ];
     qScoreUL = dupPtrUL[iQ] - qAdjustUL;
     tmpStr = (char *) &qScoreUL;
     
     for(
        iChar = 0;
        iChar < defCharPerUL;
        ++iChar
     ) { /*Loop: Get the q-score entries*/
        ++(samSTPtr)->qHistUI[ (uchar) tmpStr[iChar] ];
        (samSTPtr)->sumQUL += (uchar) tmpStr[iChar];
     } /*Loop: Get the q-score entries*/
  } /*Loop: Copy the Q-score entries*/
  
  /**************************************************
  * Fun-03 Sec-13 Sub-03:
  *   - Finish copying the end of the Q-score entry
  \*************************************************/
  
  iQ = (samSTPtr)->readLenUI;
  
  for(
     iQ -= ((samSTPtr)->readLenUI & defModByUL);
     iQ < (samSTPtr)->readLenUI;
     ++iQ
  ) { /*Loop: Copy the Q-score entries*/
     (samSTPtr)->qStr[iQ] = (cpQStr)[iQ];
     qScoreUL = (uchar) (cpQStr)[iQ] - defQAdjust;

     ++(samSTPtr)->qHistUI[qScoreUL];
     (samSTPtr)->sumQUL += qScoreUL;
  } /*Loop: Copy the Q-score entries*/
  
  (samSTPtr)->qStr[iQ] = '\0';
  
  /**************************************************
  * Fun-03 Sec-13 Sub-04:
  *   - Find the mean and median Q-scores
  \*************************************************/
  
  (samSTPtr)->meanQF =
     (float)(samSTPtr)->sumQUL/(samSTPtr)->readLenUI;
  
  samEntryQHistToMed((samSTPtr));
  return iQ;
} /*cpQScores*/

/*-------------------------------------------------------\
| Fun-08: readSamLine
|  - Reads in a single line from a sam file
| Input:
|  - samSTPtr:
|    o Pionter to samEntry structure to store the sam file
|      line in. This structure should be initialized
|  - buffStr:
|    o Buffer to read the sam file line temporarly into.
|      This is resized if needed. You can input NULL to
|      create a new buffer.
|  - lenBuffUL:
|    o Length of buffStr (updated if buffStr is resized)
|  - samVoidFILE:
|    o Sam file to read a line from. The void is so that
|      I can use samFILE in the function.
| Output:
|  - Modifies:
|    o samSTPtr to have the next line
|    o samFILE to be on the next line
|    o buffStr to hold a sam file line (resized if needed)
|    o lenBuffUL to hold the resized length of buffStr
|  - Returns:
|    o 0 for success
|    o 1 for EOF (End Of File)
|    o 64 for memory errors
\-------------------------------------------------------*/
char readSamLine(
   struct samEntry *samSTPtr,
   char **buffStr,
   ulong *lenBuffUL,
   void *samVoidFILE
){ /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
   ' Fun-08 TOC: readSamLine
   '   - Reads a single line from a sam file into samSTPtr
   '   o fun-08 Sec-01:
   '     - Variable declerations
   '   o fun-08 Sec-02:
   '     - Read in one line from the sam file
   '   o fun-08 Sec-03:
   '     - Get the query id from the buffer
   '   o fun-08 Sec-04:
   '     - Get the flag
   '   o fun-08 Sec-05:
   '     - REad in the reference name/id
   '   o fun-08 Sec-06:
   '     - Get the reference position
   '   o fun-08 Sec-07:
   '     - Get the mapping quality
   '   o fun-08 Sec-08:
   '     - Get the cigar entry
   '   o fun-08 Sec-09:
   '     - Get the RNEXT entry
   '   o fun-08 Sec-10:
   '     - Get the PNEXT entry
   '   o fun-08 Sec-11:
   '     - Get the TLEN entry
   '   o fun-08 Sec-12:
   '     - Get the sequence entry
   '   o fun-08 Sec-13:
   '     - Get the Q-score entry
   '   o fun-08 Sec-14:
   '     - Copy the extra entry; after strict sam entries
   \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-08 Sec-01:
   ^   - Variable declerations
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   ushort extraBuffUS = 4096;

   ulong tabUL = ulCpMakeDelim('\t');
   ulong newLineUL = ulCpMakeDelim('\n');

   ulong oldLenUL = 0;
   char *tmpStr = 0;
   char *iterStr = 0;

   FILE *samFILE = (FILE *) samVoidFILE;

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-08 Sec-02:
   ^   - Read in one line from the sam file
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   blankSamEntry(samSTPtr);

   if(*buffStr == 0)
   { /*If: I need to create a buffer*/
      *lenBuffUL = 0;
      *buffStr = malloc(extraBuffUS * sizeof(char));

      if(*buffStr == 0) return 64;

      *lenBuffUL = extraBuffUS;
   } /*If: I need to create a buffer*/

   /*My marker to make sure I read in a full line*/
   (*buffStr)[*lenBuffUL - 3] = '\0';

   tmpStr = fgets(*buffStr, *lenBuffUL, samFILE);

   if(!tmpStr) return 1; /*End of file*/

   /*This is not the most efficent system for memory, but
   ` it makes the code less complex and easier to deal
   ` with
   */
   while((*buffStr)[*lenBuffUL - 3] != '\0')
   { /*Loop: Read in the full sam entry line*/
      oldLenUL = *lenBuffUL;
      *lenBuffUL += extraBuffUS;
      tmpStr =realloc(*buffStr,*lenBuffUL * sizeof(char));

      if(tmpStr == 0) return 64;

      *buffStr = tmpStr;
      (*buffStr)[*lenBuffUL - 3] = '\0';
      tmpStr = *buffStr + oldLenUL - 1;

      /*Make sure on the first null; the other method can
      ` mess up
      */
      while(*(tmpStr - 1) == '\0') --tmpStr;

        /*-1 to account for extraBuffUS being index 1*/

      tmpStr = fgets(tmpStr, extraBuffUS, samFILE);
      if(!tmpStr) break; /*End of file*/

   } /*Loop: Read in the full sam entry line*/

   iterStr = *buffStr;

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-08 Sec-03:
   ^   - Get the query id from the buffer
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   /*This is a comment*/
   if(iterStr[0] == '@') goto extraEntry;
 
   /*Query id, single byte copy may be better here,
    ` hard to tell
   */
   samSTPtr->lenQryIdUC = (uchar)
      cCpStrDelim(
         samSTPtr->qryIdStr,
         iterStr,
         '\t'
      ); /*Copy the query id/name*/

   iterStr += samSTPtr->lenQryIdUC + 1; /*+1 get off tab*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-08 Sec-04:
   ^   - Get the flag for the alignment
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   iterStr =
      base10StrToUS(
         iterStr,
         samSTPtr->flagUS
      ); /*Get the flag for the alignment*/

   ++iterStr; /*Get off the tab*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-08 Sec-05:
   ^   - REad in the reference name/id
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   /*Not clear wich version is faster here*/
   samSTPtr->lenRefIdUC = (uchar)
      ulCpStrDelim(
         samSTPtr->refIdStr,
         iterStr,
         tabUL,
         '\t'
      ); /*Copy the reference id/name*/
   
   iterStr += samSTPtr->lenRefIdUC + 1; /*+1 get off tab*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-08 Sec-06:
   ^   - Get the reference position
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   iterStr =
      base10StrToUI(
         iterStr,
         samSTPtr->refStartUI
      ); /*Get the starting base in the reference*/

    /*Convert the starting positionto index 0*/
   samSTPtr->refStartUI -= (samSTPtr->refStartUI > 0);

   ++iterStr; /*Get off the tab*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-08 Sec-07:
   ^   - Get the mapping quality
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   iterStr =
      base10StrToUC(
         iterStr,
         samSTPtr->mapqUC
      ); /*Get the mapping quality of the alignment*/

   ++iterStr; /*Get off the tab*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-08 Sec-08:
   ^   - Get the cigar entry
   ^   o fun-08 sec-08 sub-01:
   ^     - Check if there is a cigar entry
   ^   o fun-08 sec-08 sub-02:
   ^     - Read in the cigar entry
   ^   o fun-08 sec-08 sub-03:
   ^     - Count number of matchs/snps/dels/inss/masks in
   ^       the cigar entry
   ^   o fun-08 sec-08 sub-04:
   ^     - Get the read lengths from the cigar entries
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   /*****************************************************\
   * Fun-08 Sec-08 Sub-01:
   *   - Check if there is a cigar entry
   \*****************************************************/

   if(iterStr[0] == '*')
   { /*If: the cigar entry was not present*/
      samSTPtr->cigTypeStr[0] = '*';
      samSTPtr->cigValAryI[0] = 0;
      iterStr += 2;
      (samSTPtr)->lenCigUI = 1;

      goto rNextEntry;
   } /*If: the cigar entry was not present*/

   /*****************************************************\
   * Fun-08 Sec-08 Sub-02:
   *   - Read in the cigar entry
   \*****************************************************/
   
   while(iterStr[0] > 32)
   { /*Loop: Read in the cigar entry*/

      /*Using -1 to account for the null I will add at the
      `end
      */
      if((samSTPtr)->lenCigUI >= samSTPtr->lenCigBuffUI-1)
      { /*If: I need to increase the cigar buff size*/
         samSTPtr->lenCigBuffUI <<= 1;

         tmpStr =
            realloc(
               samSTPtr->cigTypeStr,
               samSTPtr->lenCigBuffUI * sizeof(char)
            ); /*Resize the type cigar buffer*/

          if(tmpStr == 0) return 64;
          samSTPtr->cigTypeStr = tmpStr;

         tmpStr = (char *)
            realloc(
               samSTPtr->cigValAryI,
               samSTPtr->lenCigBuffUI * sizeof(int)
            ); /*Resize the value cigar buffer*/

          if(tmpStr == 0) return 64;
          samSTPtr->cigValAryI = (int *) tmpStr;
      } /*If: I need to increase the cigar buff size*/

      /*Record the cigar entry*/
      iterStr =
         base10StrToSI(
            iterStr,
            samSTPtr->cigValAryI[samSTPtr->lenCigUI]
          ); /*Get the number of bases for this type*/

      samSTPtr->cigTypeStr[samSTPtr->lenCigUI]=iterStr[0];

      /**************************************************\
      * Fun-08 Sec-08 Sub-03:
      *   - Count number of matchs/snps/dels/inss/masks in
      *     the cigar entry
      \**************************************************/

      switch(iterStr[0])
      { /*Switch: Check the cigar entry type*/
         case '=':
         /*Case: This was a match*/
             samSTPtr->numMatchUI +=
                samSTPtr->cigValAryI[samSTPtr->lenCigUI];

             break;
         /*Case: This was a match*/

         case 'M':
         /*Case: This is an snp or match*/
             samSTPtr->numMatchUI +=
                samSTPtr->cigValAryI[samSTPtr->lenCigUI];

             break;
         /*Case: This is an snp or match*/

         case 'X':
         /*Case: This is an snp*/
             samSTPtr->numSnpUI +=
                samSTPtr->cigValAryI[samSTPtr->lenCigUI];

             break;
         /*Case: This is an snp*/

         case 'I':
         /*Case: This is an insertion*/
             samSTPtr->numInsUI +=
                samSTPtr->cigValAryI[samSTPtr->lenCigUI];

             break;
         /*Case: This is an insertion*/

         case 'D':
         /*Case: This is an deletion*/
             samSTPtr->numDelUI +=
                samSTPtr->cigValAryI[samSTPtr->lenCigUI];

             break;
         /*Case: This is an deletion*/

         case 'S':
         /*Case: This is an softmasked region*/
             samSTPtr->numMaskUI +=
                samSTPtr->cigValAryI[samSTPtr->lenCigUI];

             break;
         /*Case: This is an softmasked region*/
      } /*Switch: Check the cigar entry type*/

      ++iterStr; /*Get off the tab*/
      ++(samSTPtr)->lenCigUI;
   } /*Loop: Read in the cigar entry*/

   /*****************************************************\
   * Fun-08 Sec-08 Sub-04:
   *   - Get the read lengths from the cigar entries
   \*****************************************************/

    samSTPtr->cigValAryI[samSTPtr->lenCigUI] = 0;
    samSTPtr->cigTypeStr[samSTPtr->lenCigUI] = '\0';

   samSTPtr->readLenUI =
        samSTPtr->numMatchUI
      + samSTPtr->numSnpUI
      + samSTPtr->numInsUI
      + samSTPtr->numMaskUI;

   samSTPtr->alnReadLenUI =
        samSTPtr->numMatchUI
      + samSTPtr->numSnpUI
      + samSTPtr->numDelUI;

   samSTPtr->refEndUI = samSTPtr->refStartUI;
   samSTPtr->refEndUI += samSTPtr->alnReadLenUI;
   samSTPtr->refEndUI -= (samSTPtr->alnReadLenUI > 0);
      /*-1 from (alnReadLen > 0) converts to index 0*/

   ++iterStr; /*Get off the tab*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-08 Sec-09:
   ^   - Get the RNEXT entry
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   rNextEntry:

   /*Not sure which is better here*/
   samSTPtr->lenRNextUC = (uchar)
      ulCpStrDelim(
         samSTPtr->rNextStr,
         iterStr,
         tabUL,
         '\t'
      ); /*Copy the query id/name*/

   iterStr += samSTPtr->lenRNextUC + 1; /*+1 get off tab*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-08 Sec-10:
   ^   - Get the PNEXT entry
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   iterStr =
      base10StrToSI(
         iterStr,
         samSTPtr->pNextI
       ); /*Get the number of bases for this type*/

   samSTPtr->pNextI -= (samSTPtr->pNextI > 0);

   ++iterStr; /*Get off the tab*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-08 Sec-11:
   ^   - Get the TLEN entry
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   iterStr =
      base10StrToSI(
         iterStr,
         samSTPtr->tLenI
       ); /*Get the number of bases for this type*/

   ++iterStr; /*Get off the tab*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-08 Sec-12:
   ^   - Get the sequence entry
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   if(samSTPtr->readLenUI == 0 && iterStr[0] != '*')
      samSTPtr->readLenUI= ulLenStr(iterStr, tabUL, '\t');

   else if(iterStr[0] == '*')
   { /*Else If: There  is no sequence entry*/
      samSTPtr->seqStr[0] = '*';
      samSTPtr->seqStr[1] = '\0';
      iterStr += 2;

      goto noQEntry;
   } /*Else If: There  is no sequence entry*/

   if(samSTPtr->readLenUI > samSTPtr->lenSeqBuffUI)
   { /*If: I need to resize sequence & Q-score buffers*/
      free(samSTPtr->seqStr);
      samSTPtr->seqStr = 0;

      samSTPtr->seqStr =
         malloc((samSTPtr->readLenUI + 1) * sizeof(char));

      if(!samSTPtr->seqStr) return 64;

      free(samSTPtr->qStr);
      samSTPtr->qStr = 0;

      samSTPtr->qStr =
         malloc((samSTPtr->readLenUI + 1) * sizeof(char));

      if(!samSTPtr->qStr) return 64;

      samSTPtr->lenSeqBuffUI = samSTPtr->readLenUI;
      samSTPtr->lenQBuffUI = samSTPtr->readLenUI;
   } /*If: I need to resize sequence & Q-score buffers*/

   ulCpStr(samSTPtr->seqStr,iterStr,samSTPtr->readLenUI);
   iterStr += samSTPtr->readLenUI + 1; /*+1 gets off tab*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-08 Sec-13:
   ^   - Get the Q-score entry
   ^   o fun-03 sec-13 sub-01:
   ^     - Check if I have a Q-score entry
   ^   o fun-03 sec-13 sub-02:
   ^     - Copy the q-score entry in blocks of 8
   ^   o fun-03 sec-13 sub-03:
   ^     - Finish copying the end of the Q-score entry
   ^   o fun-03 sec-13 sub-04:
   ^     - Find the mean and median Q-scores
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   /*****************************************************\
   * Fun-03 Sec-13 Sub-01:
   *   - Check if I have a Q-score entry
   \*****************************************************/

   if(iterStr[0] == '*' && iterStr[1] == '\t')
   { /*If: there is no Q-score entry*/
      noQEntry:

      samSTPtr->qStr[0] = '*';
      samSTPtr->qStr[1] = '\0';
      iterStr += 2;
   } /*If: there is no Q-score entry*/

   else iterStr += cpQScores(samSTPtr, iterStr) + 1;

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-08 Sec-14:
   ^   - Copy the extra entry; after strict sam entries
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
   
   extraEntry:

   /*not sure if char or ul copy better here*/
   samSTPtr->lenExtraUI= ulLenStr(iterStr,newLineUL,'\n');

   if(samSTPtr->lenExtraUI > samSTPtr->lenExtraBuffUI)
   { /*If: I need to resize the buffer*/
      free(samSTPtr->extraStr);
      samSTPtr->extraStr = 0;

      samSTPtr->extraStr =
         malloc((samSTPtr->lenExtraUI +1)* sizeof(char));
      
      if(samSTPtr->extraStr == 0) return 64;
   } /*If: I need to resize the buffer*/

   ulCpStr(
      samSTPtr->extraStr,
      iterStr,
      samSTPtr->lenExtraUI
   ); /*Copy the extra entry*/

   return 0;
} /*readSamLine*/

/*-------------------------------------------------------\
| Fun-09: pSamEntry
| Use:
|  - Prints the sam file entry to a file. This does not
|    print any extra stats that were found.
| Input:
|  - samST
|    o Pointer to samEntry struct with sam entry to print
|  - buffStr:
|    o Pointer to c-string buffer to temporarly hold the
|      cigar entry (speeds things up)
|  - lenBuffUL:
|    o Current length of buffer, adjusted if buffStr is
|      expanded
|  - outFILE:
|    o File to print the sam entry to
| Output:
|  - Prints:
|    o Sam file entry in samST to outFILE.
|  - Modifies:
|    o inceases the size of buffStr, if is not 8x the
|      cigar length
|    o Sets lenBuffUL to the new buffStr size when buffStr
|      is resized
|  - Returns:
|    o 0 for no problems
|    o 64 for memory errors
\-------------------------------------------------------*/
char pSamEntry(
   struct samEntry *samSTPtr,
   char **buffStr,
   ulong *lenBuffUL,
   void *outFILE
){
   uint uiCig = 0;
   char *tmpStr = *buffStr;
   char errC = 0;
   
   if(*(lenBuffUL) < ((samSTPtr)->lenCigUI << 3))
   { /*If: I need to add more buffer*/
      free(*buffStr);
      *(lenBuffUL) = (samSTPtr)->lenCigUI << 3;
      *buffStr = malloc(*(lenBuffUL) * sizeof(char));
   } /*If: I need to add more buffer*/
   
   if(! (*buffStr)) errC = 64;/*If I had a memory error*/
   
   else if(
         (samSTPtr)->extraStr[0] == '@'
      && (samSTPtr)->qryIdStr[0] == '\0'
   ) fprintf(
        (FILE *) (outFILE),
        "%s\n",
        (samSTPtr)->extraStr
     ); /*Print out the header (in extra since commnet)*/
   
   else if((samSTPtr)->flagUS & 4)
   { /*Else If: this was an umapped read*/
      
      fprintf(
         (FILE *) (outFILE),
         "%s\t%u\t%s\t0\t0\t*\t*\t0\t0",
         (samSTPtr)->qryIdStr,
         (samSTPtr)->flagUS,
         (samSTPtr)->refIdStr
      );
      
      fprintf(
         (FILE *) (outFILE),
         "\t%s\t%s\t%s\n",
         (samSTPtr)->seqStr,
         (samSTPtr)->qStr,
         (samSTPtr)->extraStr
      ); /*Print out the final entries*/
   } /*Else If: this was an umapped read*/
   
   else
   { /*Else: This is an entry, not a comment*/
      
      fprintf(
         (FILE *) (outFILE),
         "%s\t%u\t%s\t%u\t%u\t",
         (samSTPtr)->qryIdStr,
         (samSTPtr)->flagUS,
         (samSTPtr)->refIdStr,
         (samSTPtr)->refStartUI + 1,
         (samSTPtr)->mapqUC
      );
      
      if((samSTPtr)->cigTypeStr[0] != '*')
      { /*If: I need to print out the cigar entry*/
         for(
            uiCig=0;
            uiCig < (samSTPtr)->lenCigUI;
            ++uiCig
         ){
            tmpStr +=
               numToStr(
                  tmpStr,
                  (samSTPtr)->cigValAryI[uiCig]
               );
            tmpStr[0] = (samSTPtr)->cigTypeStr[uiCig];
            ++tmpStr;
          }
          tmpStr[0] = '\0';
          fprintf((FILE *) (outFILE), *buffStr);
      } /*If: I need to print out the cigar entry*/
      
      else fprintf((outFILE), "*");
      
      fprintf(
         (FILE *) (outFILE),
         "\t%s\t%u\t%u\t%s\t%s\t%s\n",
         (samSTPtr)->rNextStr,
         (samSTPtr)->pNextI + ((samSTPtr)->pNextI > 0),
         (samSTPtr)->tLenI,
         (samSTPtr)->seqStr,
         (samSTPtr)->qStr,
         (samSTPtr)->extraStr
      ); /*Print out the final entries*/
   } /*Else: This is an entry, not a comment*/
   
   return errC;
} /*printSamEntry*/

/*-------------------------------------------------------\
| Fun-10: pSamEntryAsFastq
| Use:
|  - Prints the sam entry as a fastq entry to a fastq file
| Input:
|  - samST:
|    o Pointer to samEntry structure with fastq entry to
|      print out
|  - outFILE:
|    o Fastq file to print the new fastq entry to
| Output:
|  - Prints:
|    o fastq entry from samST to outFILE
\-------------------------------------------------------*/
void pSamEntryAsFastq(
   struct samEntry *samSTPtr,
   void *outFILE
){
   if(   (samSTPtr)->seqStr[1] != '\0'
      && (samSTPtr)->qStr[1] != '\0'
   ){ /*If: This entry can be printed out*/
      fprintf(
        (FILE *) (outFILE),
        "@%s ref=%s start=%u len=%u refAlnLen=%u",
        (samSTPtr)->qryIdStr,
        (samSTPtr)->refIdStr,
        (samSTPtr)->refStartUI + 1,
        (samSTPtr)->readLenUI,
        (samSTPtr)->alnReadLenUI
      );
      
      fprintf(
        (FILE *) (outFILE),
        "  flag=%u mapq=%u match=%u snp=%u ins=%u",
        (samSTPtr)->flagUS,
        (samSTPtr)->mapqUC,
        (samSTPtr)->numMatchUI,
        (samSTPtr)->numSnpUI,
        (samSTPtr)->numInsUI
      );
      
      fprintf(
        (FILE *) (outFILE),
        "  del=%u softMasked=%u meanQ=%f medianQ=%f\n",
        (samSTPtr)->numDelUI,
        (samSTPtr)->numMaskUI,
        (samSTPtr)->meanQF,
        (samSTPtr)->medianQF
      );
      fprintf(
        (FILE *) (outFILE),
        "%s\n+\n%s\n",
        (samSTPtr)->seqStr,
        (samSTPtr)->qStr
      );
   } /*If: This entry can be printed out*/
} /*pSamEntryAsFq*/

/*-------------------------------------------------------\
| Fun-11: pSamEntryAsFasta
| Use:
|  - Prints the sam entry as a fasta entry to a fasta file
| Input:
|  - samST:
|    o Pointer to samEntry structure with fastq entry to
|      print out
|  - outFILE:
|    o Fasta file to print the new fasta entry to
| Output:
|  - Prints:
|    o fasta entry from samST to outFILE
\-------------------------------------------------------*/
void pSamEntryAsFasta(
   struct samEntry *samSTPtr,
   void *outFILE
){
   if((samSTPtr)->seqStr[1] != '\0')
   { /*If: This entry can be printed out*/
      fprintf(
        (FILE *) (outFILE),
        ">%s ref=%s start=%u len=%u refAlnLen=%u",
        (samSTPtr)->qryIdStr,
        (samSTPtr)->refIdStr,
        (samSTPtr)->refStartUI + 1,
        (samSTPtr)->readLenUI,
        (samSTPtr)->alnReadLenUI
      );
      
      fprintf(
        (FILE *) (outFILE),
        "  flag=%u mapq=%u match=%u snp=%u ins=%u",
        (samSTPtr)->flagUS,
        (samSTPtr)->mapqUC,
        (samSTPtr)->numMatchUI,
        (samSTPtr)->numSnpUI,
        (samSTPtr)->numInsUI
      );
      
      fprintf(
        (FILE *) (outFILE),
        "  del=%u softMasked=%u meanQ=%f medianQ=%f\n",
        (samSTPtr)->numDelUI,
        (samSTPtr)->numMaskUI,
        (samSTPtr)->meanQF,
        (samSTPtr)->medianQF
      );
      fprintf((outFILE), "%s\n", (samSTPtr)->seqStr);
   } /*If: This entry can be printed out*/
} /*pSamEntryAsFq*/

/*-------------------------------------------------------\
| Fun-11: pSamEntryStats
| Use:
|  - Prints out the stats in a samEntry struct to a file
| Input:
|  - samEntryST:
|    o Pointer to samEntry struct to print stats for
|  - pHeadBl:
|    o 1: Print the header for the stats tsv file
|    o 0: Do not print the header
|  - outFILE:
|    o TSV (tab deliminated) file to print stats to
| Output:
|  - Prints:
|    o line with stats from samEntryStruct to file (tsv)
|  - Modifies:
|    o printHeaderChar to be 0 if set to 1
\-------------------------------------------------------*/
void pSamEntryStats(
   struct samEntry *samSTPtr,
   char pHeadBl,
   void *outFILE
){
   if(   (samSTPtr)->extraStr[0] != '@'
      && (samSTPtr)->qryIdStr[0] != '\0'
   ){ /*If: This is not a comment*/
      if((pHeadBl))
      { /*If: I need to print the header*/
        fprintf((FILE *) (outFILE), "Read\tRef\tFlag");
        fprintf((FILE *) (outFILE), "\tMapQ\tRefPos"); 
        fprintf((FILE *) (outFILE), "\tReadLength");
        fprintf((FILE *) (outFILE), "\tRefAlnLength");
        fprintf((FILE *) (outFILE), "\tMatches\tInss");
        fprintf((FILE *) (outFILE), "\tDels\tSnps");
        fprintf((FILE *) (outFILE), "\tMedianQ\tMeanQ\n");

        (pHeadBl) = 0;
      } /*If: I need to print the header*/
      
      fprintf(
        (FILE *) (outFILE),
        "%s\t%s\t%u\t%u\t%u\t%u",
        (samSTPtr)->qryIdStr,
        (samSTPtr)->refIdStr,
        (samSTPtr)->flagUS,
        (samSTPtr)->mapqUC,
        (samSTPtr)->refStartUI + 1,
        (samSTPtr)->readLenUI
      );
      
      fprintf(
        (FILE *) (outFILE),
        "\t%u\t%u\t%u\t%u\t%u\t%f\t%f\n",
        (samSTPtr)->alnReadLenUI,
        (samSTPtr)->numMatchUI,
        (samSTPtr)->numSnpUI,
        (samSTPtr)->numInsUI,
        (samSTPtr)->numDelUI,
        (samSTPtr)->meanQF,
        (samSTPtr)->medianQF
      );
   } /*If: This is not a comment*/
} /*pSamEntryStats*/

