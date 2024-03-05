/*########################################################
# Name: trimSam
# Use:
#  - trims soft mask regions off all alignments with
#    sequences in a sam file. Aligments without sequences
#    are ignored & not printed out.
# Libraries:
#   - "samEntryStruct.h"
#   o "base10StrToNum.h"
#   o "dataTypeShortHand.h"
#   o "ulCpStr"
#   o "numToStr.h"
# C Standard Libraries:
#   o <stdlib.h>
#   o <stdio.h>
#   o <limits.h>
########################################################*/

#ifndef TRIMSAM_H
#define TRIMSAM_H

#include "samEntryStruct.h"

#if UINT_MAX >= 0xffffffffffffffff
   #define defShiftIntToGetChar 3 /*Not realisistic*/

#elif UINT_MAX >= 0xffffffff
   #define defShiftIntToGetChar 2

#elif ULONG_MAX >= 0xffff
   #define defShiftIntToGetChar 1

#else
   #error This is an 8 bit system
#endif


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
| trimSam SOF: Start Of Functions
|  - fun-01 trimSamEntry:
|    o Trim soft mask regions off end of sam entry
|  - fun-02 trimSamReads:
|    o Trims soft mask regions for all reads with a
|      sequence in a sam file
\~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*-------------------------------------------------------\
| Name: trimSamEntry (Fun-01:)
| Use:
|  - Trims off the soft masked regions of a sam entry
| Input:
|  - samST:
|    o samEntry struct with sam entry to trim
| Output:
|  - Returns:
|    o 0 if suceeded
|    o 2 if header (invalid and ignored)
|    o 4 if an unmapped read (no reference)
|    o 8 if no sequence line
|  - Modifies:
|    o Trims cigar, sequence, & q-score entries in samST.
\-------------------------------------------------------*/
static uchar trimSamEntry(
    struct samEntry *samST   /*has sam line to trim*/
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
    for(iSeq = iSeq; qCpStr[iSeq] != '\0'; ++iSeq)
    { /*Loop: adjust for the timmed ending Q-scores*/
       qUC = (uchar) samST->qStr[iSeq] - defQAdjust;
       --(samST->qHistUI[qUC]);
       samST->sumQUL -= qUC;
    } /*Loop: adjust for the timmed ending Q-scores*/

    /*Make sure they are c-strings*/
    seqDupStr[samST->readLenUI] = '\0';
    qDupStr[samST->readLenUI] = '\0';

    samST->meanQF=(float) samST->sumQUL /samST->readLenUI;
    samEntryQHistToMed(samST);

    return 0;
} /*trimSamEntry*/

/*-------------------------------------------------------\
| Name: trimSamReads (Fun-02:)
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
#define trimSamReads(\
    samFILE,               /*Sam file to trim*/\
    outFILE,               /*File to store output*/\
    keepUnmappedReadsBl    /*1: keep unmapped reads*/\
)({/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
   ' Fun-02 TOC: trimSamReads
   '  - Goes though sam file and calls trimSamEntry for
   '    each entry
   \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/\
    \
    char *buffStr = 0;\
    ulong lenBuffUL = 0;\
    uchar errUC = 0;  /*Tells if memory error*/\
    struct samEntry samST;\
    \
    initSamEntry(&samST);\
    \
    errUC =\
       readSamLine(&samST,&buffStr,&lenBuffUL,samFILE);\
    \
    while(!errUC)\
    { /*While there are lines in sam file to convert*/\
        \
        if(*(samST.extraStr) == '@')\
        { /*If was a header*/\
            pSamEntry(&samST,buffStr,lenBuffUL,outFILE);\
            \
            errUC =\
               readSamLine(\
                  &samST,\
                  &buffStr,\
                  &lenBuffUL,\
                  samFILE\
               );\
            \
            continue; /*header line, move to next line*/\
        } /*If was a header*/\
        \
        /*Convert & print out sam file entry*/\
        errUC = trimSamEntry(&samST);\
        \
        /*Print out the converted entry*/\
        if(!(errUC >> 2))\
           {pSamEntry(&samST,buffStr,lenBuffUL,outFILE);}\
        \
        else if(errUC & 4 && keepUnmappedReadsBl)\
           {pSamEntry(&samST,buffStr,lenBuffUL,outFILE);}\
            /*Else if printing umapped reads*/\
        \
        errUC =\
           readSamLine(\
              &samST,\
              &buffStr,\
              &lenBuffUL,\
              samFILE\
           );\
    } /*While there are lines in sam file to convert*/\
    \
    freeSamEntryStack(&samST);\
    errUC & ~1;\
}) /*trimSamReads*/

#endif
