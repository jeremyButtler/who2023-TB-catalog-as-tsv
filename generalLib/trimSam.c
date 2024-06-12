/*########################################################
# Name: trimSam
#   - Holds functions for trimming softmasked regoins at
#     the start and end of sequences in an sam file
########################################################*/

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
' SOF: Start Of File
'   o header:
'     - Included libraries and defined variables
'   o fun-01 trimSamEntry:
'     o Trim soft mask regions off end of sam entry
'   o fun-02: trimByCoords
'     - Trim an sam file entry by coordinates
'   o fun-03 trimSamReads:
'     o Trims soft mask regions for all reads with a
'       sequence in a sam file
'   o license:
'     - Licensing for this code (public domain / mit)
\~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*-------------------------------------------------------\
| Header:
|   - Included libraries and defined variables
|   o header Sec-01:
|     - Included libraries
|   o header Sec-02:
\-------------------------------------------------------*/

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
^ Header Sec-01:
^   - Included libraries
\<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

#ifdef PLAN9
   #include <u.h>
   #include <libc.h>
#else
   #include <stdlib.h>
#endif

#include <stdio.h>

#include "samEntryStruct.h"

/*Has no .c files*/
#include "ulCpStr.h"
#include "dataTypeShortHand.h"

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\
! Hidden depenencies
!   o .h #include "../generalLib/base10StrToNum.h"
!   o .h #include "../generalLib/numToStr.h"
\%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
^ Header Sec-02:
^   - Defined varialbes
\<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

/*This .c version is slightly slower, but it works*/
/*This will work so long as the integer is beneath
`   128 bits, which is non-realistic value.
*/
#define defShiftIntToGetChar (\
     (sizeof(unsigned int) >> 1)\
   - (sizeof(unsigned int) >> 3)\
)

/*Logic:
`   - sizeof(unsigned int) >> 1:
`     o This gets the number of bytes to shift to get
`       an 32 bit or 16 bit number to one bytes. Basically
`       it is divide by 2
`   - (sizeof(unsigned int) >> 3)
`     o This adds in 1 if I have eight bytes. It failes
`       for 
`   - (sizeof(int) >> 1) - (sizeof(int) >> 3)
`     o Gives me the number of bits I need to shift to
`       get the number of characters in an int
`   - (sizeof(int) >> 3) << (sizeof(int) >> 4)
`     o This is not used, but would correct for a 128 bit
`       number. Each time I increase the bit size I need
`       to add a correction in
*/

/*-------------------------------------------------------\
| Fun-01: trimSamEntry
| Use:
|  - Trims off the soft masked regions of a sam entry
| Input:
|  - samVoidST:
|    o samEntry struct with sam entry to trim. Named
|      void so I can cast it as a samEntry to samST
| Output:
|  - Returns:
|    o 0 if suceeded
|    o 2 if header (invalid and ignored)
|    o 4 if an unmapped read (no reference)
|    o 8 if no sequence line
|  - Modifies:
|    o Trims cigar, sequence, & q-score entries in samST.
\-------------------------------------------------------*/
unsigned char trimSamEntry(
    void *samVoidST   /*has sam line to trim*/
){ /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
   ' Fun-01 TOC: trimSamEntry
   '  - Trims soft masked regions at start & end of sam
   '    entry
   '  o fun-01 sec-01:
   '    - Variable declerations
   '  o fun-01 sec-02:
   '    - Check if I have something to trim
   '  o fun-01 sec-03:
   '    - Finding trimming postions and trim the cigar
   '  o fun-01 sec-04:
   '    - Trim the sequence and Q-score entry
   \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
    ^ Fun-01 Sec-01:
    ^  - Variable declerations
    \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    /*Number of bases soft masked at start or end*/
    uchar qUC = 0;

    int startPosI = 0;
    int endTrimI = 0;
    uint iCig = 0;
    uint iSeq = 0;

    char *seqCpStr = 0;
    char *seqDupStr = 0;
    ulong *seqCpUL = 0;
    ulong *seqDupUL = 0;

    char *qCpStr = 0;
    char *qDupStr = 0;
    ulong *qCpUL = 0;
    ulong *qDupUL = 0;
        
    struct samEntry *samST= (struct samEntry *) samVoidST;

    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
    ^ Fun-01 Sec-02:
    ^   - Check if I have something to trim
    \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    /*Check if is an header is an header entry*/
    if(samST->extraStr[0] == '@') return 2;

    /*Check if is an unmapped read*/
    if(samST->flagUS & 4) return 4;

    /*Check if is an unmapped read*/
    if(samST->cigTypeStr[0] == '*') return 4;

    /*Check if has a sequence to trim*/
    if(samST->seqStr[0] == '*') return 8;

    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
    ^ Fun-01 Sec-03:
    ^   - Finding trimming postions and trim the cigar
    \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    /*Position my self at the start of the sequence*/
    startPosI = -((int) samST->cigTypeStr[0] == 'S');
    iCig -= startPosI; /*Marks if skipping the start*/

    endTrimI =
       -((int) samST->cigTypeStr[samST->lenCigUI-1]=='S');

    endTrimI &= samST->cigValAryI[samST->lenCigUI - 1];

    samST->lenCigUI += startPosI; /*+0 or + -1*/
    samST->lenCigUI -= (endTrimI > 0);

    startPosI &= samST->cigValAryI[0];
    samST->readLenUI -= (startPosI + endTrimI);

    ulCpStr(
       samST->cigTypeStr,
       (samST->cigTypeStr + iCig),
       samST->lenCigUI
    ); /*Trim the cigar types array*/

    ulCpStr(
       (char *) samST->cigValAryI,
       (char *) (samST->cigValAryI + iCig),
       (samST->lenCigUI << defShiftIntToGetChar)
    ); /*Trim the cigar value (bases per type) array*/

    samST->numMaskUI = 0;

    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
    ^ Fun-01 Sec-04:
    ^   - Trim the sequence and Q-score entry
    ^   o fun-01 sec-04 sub-01:
    ^     - Set up the pionters for trimming (copying)
    ^   o fun-01 sec-04 sub-02:
    ^     - Adjust Q-scores for removing bases from start
    ^   o fun-01 sec-04 sub-03:
    ^     - Copy kept parts of Q-score & sequence entries
    ^   o fun-01 sec-04 sub-04:
    ^     - Adjust for the Q-scores removed at the end
    \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    /****************************************************\
    * Fun-01 Sec-04 Sub-01:
    *   - Set up the pionters for trimming (copying)
    \****************************************************/

    /*The longs are for 8 byte copying, while the c-string
    ' pionters are for the end (when sequence was not a
    ' multiple of 8
    */

    seqCpUL = (ulong *) (samST->seqStr + startPosI);
    seqDupUL = (ulong *) samST->seqStr;
    seqCpStr = samST->seqStr + startPosI;
    seqDupStr = samST->seqStr;

    qCpUL = (ulong *) (samST->qStr + startPosI);
    qDupUL = (ulong *) samST->qStr;
    qCpStr = samST->qStr + startPosI;
    qDupStr = samST->qStr;

    /****************************************************\
    * Fun-01 Sec-04 Sub-02:
    *   - Adjust Q-scores for removing bases from start
    \****************************************************/

    for(iSeq = 0; iSeq < startPosI; ++iSeq)
    { /*Loop: adjust for the timmed starting Q-scores*/
       qUC = (uchar) samST->qStr[iSeq] - defQAdjust;
       --(samST->qHistUI[qUC]);
       samST->sumQUL -= qUC;
    } /*Loop: adjust for the timmed starting Q-scores*/

    /****************************************************\
    * Fun-01 Sec-04 Sub-03:
    *   - Copy kept parts of Q-score & sequence entries
    \****************************************************/

    /*I am not using my copy functions here to leverage dual
    ` dual accumulators, which the cpu should hanlde.
    ` easier.
    */

    /*Copy With longs*/
    for(
       iSeq = 0;
       iSeq < (samST->readLenUI >> defShiftByUL);
       ++iSeq
    ) { /*Loop: Copy sequences and q-score entries*/
       seqDupUL[iSeq] = seqCpUL[iSeq];
       qDupUL[iSeq] = qCpUL[iSeq];
    } /*Loop: Copy sequences and q-score entries*/

    /*Finish coping the kept region using characters*/
    for(
       iSeq =
           samST->readLenUI
         - (samST->readLenUI & defModByUL);
       iSeq < samST->readLenUI;
       ++iSeq
    ) { /*Loop: Copy sequences and q-score entries*/
       seqDupStr[iSeq] = seqCpStr[iSeq];
       qDupStr[iSeq] = qCpStr[iSeq];
    } /*Loop: Copy sequences and q-score entries*/
   
    /****************************************************\
    * Fun-01 Sec-04 Sub-04:
    *   - Adjust for the Q-scores removed at the end
    \****************************************************/

    /*Adjust for having removed q-scores at the end*/
    while(qCpStr[iSeq] != '\0')
    { /*Loop: adjust for the timmed ending Q-scores*/
       qUC = (uchar) samST->qStr[iSeq] - defQAdjust;
       --(samST->qHistUI[qUC]);
       samST->sumQUL -= qUC;
       ++iSeq;
    } /*Loop: adjust for the timmed ending Q-scores*/

    /*Make sure they are c-strings*/
    seqDupStr[samST->readLenUI] = '\0';
    qDupStr[samST->readLenUI] = '\0';

    samST->meanQF=(float) samST->sumQUL /samST->readLenUI;
    samEntryQHistToMed(samST);

    return 0;
} /*trimSamEntry*/

/*-------------------------------------------------------\
| Fun-02: trimByCoords
|   - Trim an sam file entry by coordinates
| Input:
|   - samSTPtr:
|     o Pointer to an sam entry structure with an read to
|       trim
|   - startSI:
|     o Singed integer with the starting coordinate
|   - endSI:
|     o Singed integer with the ending coordinate
|     o < 1 is treated as do not trim
| Output:
|   - Modifies:
|     o samSTPtr to be trimmed
|   - Returns:
|     o 0 for no errors
|     o 1 for coordinates out of range
\-------------------------------------------------------*/
signed char
trimByCoords(
   struct samEntry *samSTPtr,
   signed int startSI,
   signed int endSI
){ /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
   ' Fun-02 TOC:
   '   - Trim an sam file entry by coordinates
   '   o fun-02 sec-01:
   '     - Variable declerations
   '   o fun-02 sec-02:
   '     - Check ranges and set up variables
   '   o fun-02 sec-03:
   '     - Find the starting position
   '   o fun-02 sec-04:
   '     - Find the ending position
   '   o fun-02 sec-05:
   '     - Set up the lengths and ending coordinates
   '   o fun-02 sec-06:
   '     - Trim the sequence, Q-score, and cigar entries
   \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-02 Sec-01:
   ^   - Variable declerations
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   uint seqStartUI = 0;
   sint firstCigSI = 0;

   sint siCig = 0;
   sint cigBaseOnSI = 0;
   sint refPosSI = 0;
   sint seqPosSI = 0;

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-02 Sec-02:
   ^   - Check ranges and set up variables
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   if(samSTPtr->refStartUI > endSI)
       return 1;

   if(samSTPtr->refEndUI < startSI)
       return 1;

   cigBaseOnSI = samSTPtr->cigValAryI[0];
   refPosSI = (sint) samSTPtr->refStartUI;

   /*Setting to zero becuase it is easier to recount*/
   samSTPtr->numMaskUI = 0;
   samSTPtr->numInsUI = 0;
   samSTPtr->numDelUI = 0;
   samSTPtr->numSnpUI = 0;
   samSTPtr->numMatchUI = 0;

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-02 Sec-03:
   ^   - Find the starting position
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   samEntryFindRefPos(
      samSTPtr,
      siCig,
      cigBaseOnSI,
      startSI,
      refPosSI,
      seqPosSI
   );

   samSTPtr->cigValAryI[siCig] = cigBaseOnSI;
   samSTPtr->refStartUI = (uint) refPosSI;

   seqStartUI = (uint) seqPosSI;
   firstCigSI = siCig;

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-02 Sec-04:
   ^   - Find the ending position
   ^   o fun-02 sec-04 sub-01:
   ^     - Start loop and handle soft masking entires
   ^   o fun-02 sec-04 sub-02:
   ^     - Handle insertion entries
   ^   o fun-02 sec-04 sub-03:
   ^     - Handle deletion entries
   ^   o fun-02 sec-04 sub-04:
   ^     - Handle snp entries
   ^   o fun-02 sec-04 sub-05:
   ^     - Move to next cigar entry
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   /*****************************************************\
   * Fun-02 Sec-04 Sub-01:
   *   - Start loop and handle soft masking entires
   \*****************************************************/

   while(refPosSI < endSI)
   { /*Loop: till I am on the target base*/
      switch((samSTPtr)->cigTypeStr[(siCig)])
      { /*Switch: check what the next entry is*/
         case 'S':
         /*Case: Softmasking*/
            seqStartUI += cigBaseOnSI;
            samSTPtr->numMaskUI +=  cigBaseOnSI;

            ++siCig;
            cigBaseOnSI = 0;
            break;
         /*Case: Softmasking*/

         /***********************************************\
         * Fun-02 Sec-04 Sub-02:
         *   - Handle insertion entries
         \***********************************************/

         case 'I':
         /*Case: insertions*/
            seqStartUI += cigBaseOnSI;
            samSTPtr->numInsUI +=  cigBaseOnSI;

            ++siCig;
            cigBaseOnSI = 0;
            break;
         /*Case: insertions*/

         /***********************************************\
         * Fun-02 Sec-04 Sub-03:
         *   - Handle deletion entries
         \***********************************************/

         case 'D':
         /*Case: Deletion*/
            refPosSI += cigBaseOnSI;
            samSTPtr->numDelUI += cigBaseOnSI;

            if(refPosSI <= endSI)
            { /*If: I have not found target position*/
               ++siCig;
               cigBaseOnSI = 0;
            } /*If: I have not found target position*/

            else 
            { /*Else: I overshot the target*/
               /*Find how many bases overshot by*/
               cigBaseOnSI = (refPosSI - endSI);

               /*Make corrections for overshooting*/
               refPosSI -= cigBaseOnSI;
               samSTPtr->numDelUI -= cigBaseOnSI;
            } /*Else: I overshot the target*/

            break;
         /*Case: Deletion*/\

         /***********************************************\
         * Fun-02 Sec-04 Sub-03:
         *   - Handle match entries
         \***********************************************/

         case 'M':\
         case '=':\
         /*Case: match (M or =)*/
            refPosSI += cigBaseOnSI;
            seqPosSI += cigBaseOnSI;
            samSTPtr->numMatchUI += cigBaseOnSI;

            if(refPosSI <= endSI)
            { /*If: I have not found target position*/
               cigBaseOnSI = 0;
               ++siCig;
            } /*If: I have not found target position*/

            else
            { /*Else: I overshot the target*/
               /*Find how many bases overshot by*/
               cigBaseOnSI = (int) (refPosSI - endSI);

               /*Make corrections for overshooting*/
               refPosSI -= cigBaseOnSI;
               seqPosSI -= cigBaseOnSI;
               samSTPtr->numMatchUI -= cigBaseOnSI;
            } /*Else: I overshot the target*/

            break;
         /*Case: match (M or =)*/

         /***********************************************\
         * Fun-02 Sec-04 Sub-04:
         *   - Handle snp entries
         \***********************************************/

         case 'X':\
         /*Case: SNP (X)*/
            refPosSI += cigBaseOnSI;
            seqPosSI += cigBaseOnSI;
            samSTPtr->numSnpUI += cigBaseOnSI;

            if(refPosSI <= endSI)
            { /*If: I have not found target position*/
               cigBaseOnSI = 0;
               ++siCig;
            } /*If: I have not found target position*/

            else
            { /*Else: I overshot the target*/
               /*Find how many bases overshot by*/
               cigBaseOnSI = (int) (refPosSI - endSI);

               /*Make corrections for overshooting*/
               refPosSI -= cigBaseOnSI;
               seqPosSI -= cigBaseOnSI;

               samSTPtr->numSnpUI = cigBaseOnSI;
            } /*Else: I overshot the target*/

            break;
         /*Case: SNP (X)*/
      } /*Switch: check what the next entry is*/

      /**************************************************\
      * Fun-02 Sec-04 Sub-05:
      *   - Move to next cigar entry
      \**************************************************/

      if(siCig >= samSTPtr->lenCigUI)
         break; /*End of the sequence*/

      /*This case will be true most of the time, unless
      `   the start has already been found
      */
      if(cigBaseOnSI == 0)
         cigBaseOnSI = samSTPtr->cigValAryI[siCig];
   } /*Loop: till I am on the target base*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-02 Sec-05:
   ^   - Set up the lengths and ending coordinates
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   samSTPtr->cigValAryI[siCig] = cigBaseOnSI;
   samSTPtr->lenCigUI = siCig - firstCigSI;

   samSTPtr->readLenUI = (uint) (seqPosSI - seqStartUI);
   samSTPtr->refEndUI = refPosSI;
   samSTPtr->alnReadLenUI = samSTPtr->refEndUI;
   samSTPtr->alnReadLenUI -= samSTPtr->refStartUI;

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-02 Sec-06:
   ^   - Trim the sequence, Q-score, and cigar entries
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   ulCpStr(
      samSTPtr->seqStr,
      &(samSTPtr->seqStr[seqStartUI]),
      samSTPtr->readLenUI
   );

   if(samSTPtr->qStr[0] !='*' && samSTPtr->qStr[1] !='\0')
   { /*If: There is no Q-score entry*/
      cpQScores(
         samSTPtr,
         &(samSTPtr->qStr[seqStartUI]),
         1
      ); /*Uses samSTPtr->readLenUI to get length*/
         /*Also finds median and mean Q-scores*/
   } /*If: There is no Q-score entry*/

   ulCpStr(
      samSTPtr->cigTypeStr,
      &(samSTPtr->cigTypeStr[firstCigSI]),
      samSTPtr->lenCigUI
   );

   ulCpStr(
      samSTPtr->cigValAryI,
      &(samSTPtr->cigValAryI[firstCigSI]),
      (samSTPtr->lenCigUI << 2)
   );

   return 0;
} /*trimByCoords*/

/*-------------------------------------------------------\
| Fun-03: trimSamReads
| Use:
|  - Goes though sam file and calls trimSamEntry for each
|    entry
| Input:
|  - samFILE:
|    o Pointer to FILE. Sam file with entries to trim
|  - outFILE:
|    o File to write the trimmed sam file entries to
|  - keepUnmappedReadsBl:
|    o Also print out entries with unmapped reads
| Output:
|  - Returns:
|    o 0 if suceeded
|    o 64 for memory error
|  - Prints:
|    o Trimmed sam entries with sequences to outFILE, but
|      ignores sam entries without sequences
\-------------------------------------------------------*/
unsigned char trimSamReads(
    void *samFILE,              /*Sam file to trim*/
    void *outFILE,              /*File to store output*/
    char keepUnmappedReadsBl   /*1: keep unmapped reads*/
){/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   ' Fun-03 TOC: trimSamReads
   '  - Goes though sam file and calls trimSamEntry for
   '    each entry
   \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    
    char *buffStr = 0;
    ulong lenBuffUL = 0;
    uchar errUC = 0;  /*Tells if memory error*/
    struct samEntry samST;
    
    initSamEntry(&samST);
    
    errUC =
       readSamLine(
          &samST,
          &buffStr,
          &lenBuffUL,
          (FILE *) samFILE
      );
    
    while(!errUC)
    { /*While there are lines in sam file to convert*/
        
        if(*(samST.extraStr) == '@')
        { /*If was a header*/
            pSamEntry(
               &samST,
               &buffStr,
               &lenBuffUL,
               (FILE *) outFILE
            );
            
            errUC =
               readSamLine(
                  &samST,
                  &buffStr,
                  &lenBuffUL,
                  (FILE *) samFILE
               );
            
            continue; /*header line, move to next line*/
        } /*If was a header*/
        
        /*Convert & print out sam file entry*/
        errUC = trimSamEntry(&samST);
        
        /*Print out the converted entry*/
        if(!(errUC >> 2))
           pSamEntry(
               &samST,
               &buffStr,
               &lenBuffUL,
               (FILE *) outFILE
           );
        
        else if(errUC & 4 && keepUnmappedReadsBl)
           pSamEntry(
               &samST,
               &buffStr,
               &lenBuffUL,
               (FILE *) outFILE
           );
            /*Else if printing umapped reads*/
        
        errUC =
           readSamLine(
              &samST,
              &buffStr,
              &lenBuffUL,
              (FILE *) samFILE
           );
    } /*While there are lines in sam file to convert*/
    
    freeSamEntryStack(&samST);
    return errUC & ~1;
} /*trimSamReads*/

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
