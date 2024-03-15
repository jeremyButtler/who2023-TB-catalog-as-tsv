/*########################################################
# Name: samEntryStruct
#  - Holds structer to hold a sam file entry. This also
#    includes the functions needed to support this
#    structure.
########################################################*/

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
' SOF: Start Of File
'  o header:
'    - header guards and definitions
'  o st-01 samEntry:
'    - Holds a single samfile entry
'  o fun-01 blankSamEntry:
'    - Sets all non-alloacted variables in samEntryST to 0
'  o fun-02 initSamEntry:
'    - Initalize a samEntry struct to 0's
'  o fun-03 freeSamEntryStack:
'    - Frees heap allocations in a stack allocated
'      samEntry struct
'  o fun-04 freeSamEntry:
'    - Frees a samEntry structer (and sets to null)
'  o fun-05: makeSamEntry
'    - Makes an heap allocated samEntry structure
'  o fun-08: cpQScore (.c file only)
'    - Copies Q-scores from a string into a samEntry
'      structure
'  o fun-09: readSamLine
'    - Reads in a single line from a sam file
'  o fun-10: pSamEntry
'    - Prints the sam file entry to a file. This does not
'      print any extra stats that were found.
'  o fun-11: pSamEntryAsFastq
'    - Prints the sam entry as a fastq entry to a fastq
'      file
'  o fun-12: pSamEntryAsFasta
'    - Prints the sam entry as a fasta entry to a fasta
'      file
'  o fun-13: pSamEntryStats
'    - Prints out the stats in a samEntry struct to a file
'  o note-01:
'     - Notes about the sam file format from the sam file
'       pdf
\~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*-------------------------------------------------------\
| Header:
|   - Header gaurds and definitions
\-------------------------------------------------------*/

#ifndef SAMENTRYSTRUCT_H
#define SAMENTRYSTRUCT_H

#define defQAdjust 33 /*offest to get q-score of 0*/
#define defMaxQScore 94 /*highest possible Q-score*/

/*-------------------------------------------------------\
| ST-01: samEntry
|  - Holds a single samfile entry
\-------------------------------------------------------*/
typedef struct samEntry
{ /*samEntry*/
    /*Buffers for storing strings*/
    char qryIdStr[128];/*Holds the query id/name*/
    unsigned char lenQryIdUC;  /*Length of query id/name*/

    char refIdStr[128];/*Holds the reference id/name*/
    unsigned char lenRefIdUC;  /*Length of ref id/name*/

    char *cigTypeStr;  /*Holds the cigar type entry*/
    int *cigValAryI;   /*Holds the cigar number*/
    unsigned int lenCigUI;     /*Length of cigar entry*/
    unsigned int lenCigBuffUI;/*# bytes malloc to cigStr*/

    char rNextStr[128];/*Holds the rNext entry*/
    unsigned char lenRNextUC;  /*Length of rNextStr*/

    /*The sequence and Q-scores entries have lengths
    ` stored in readLenUI
    */
    char *seqStr;      /*Holds the sequence entry*/
    unsigned int lenSeqBuffUI;/*bytes malloc seqStr/qStr*/

    char *qStr;        /*Holds the q-score entry*/
    unsigned int lenQBuffUI; /*number Q-score entries*/

    char *extraStr;    /*Extra entries (columns 12-end)*/
    unsigned int lenExtraUI;   /*Length of extraStr*/
    unsigned int lenExtraBuffUI;/*# bytes malloc to qStr*/

    /*Flags/single numeric values in the sam entry*/
    unsigned char mapqUC;      /*Holds mapping quality*/
    unsigned short flagUS;     /*Holds the flag*/
    int pNextI;        /*PNEXT postion of mate/next read*/
    int tLenI;         /*TLEN observed template length*/

    /*Stats*/

    /*The Q-scores; includes the softmasked regions*/
    float medianQF;    /*holds median read Q-score*/
    float meanQF;      /*holds mean read Q-score*/
 
    unsigned int refStartUI;  /*First reference base*/
    unsigned int refEndUI;    /*Last reference base*/
    unsigned int readLenUI;   /*Holds read length*/
    unsigned int alnReadLenUI;/*Number ref basea aligned*/

    unsigned int numMatchUI;  /*Holds number of matches*/
    unsigned int numSnpUI;    /*Holds number of mismatches*/
    unsigned int numInsUI;    /*Holds number of insertions*/
    unsigned int numDelUI;    /*number of deletions*/
    unsigned int numMaskUI;   /*number soft masked bases*/

    /*These variables are used in finding the Q-scores*/
    unsigned int qHistUI[defMaxQScore];/*medianHistogram*/
    unsigned long sumQUL;             /*Total for mean Q*/
}samEntry;

/*-------------------------------------------------------\
| Fun-01: blankSamEntry
| Use:
|  - Sets all values to 0, or for c-strings to '\0'
| Input:
|  - samSTPtr:
|    - Pointer to samEntry structure to blank
| Output:
|  - Modifies:
|    o Sets every variable to 0
|    o the first character in every c-string to be '\0'
\-------------------------------------------------------*/
#define blankSamEntry(samSTPtr){\
    int iIter = 0;\
    \
    (samSTPtr)->qryIdStr[0] = '\0';/*query id/name*/\
    (samSTPtr)->lenQryIdUC = 0;    /*Length of query id*/\
    \
    (samSTPtr)->refIdStr[0] = '\0';/*reference id/name*/\
    (samSTPtr)->lenRefIdUC = 0;  /*Length; reference id*/\
    \
    (samSTPtr)->lenCigUI = 0;     /*Length; cigar entry*/\
    \
    for(iIter=0; iIter <(samSTPtr)->lenCigBuffUI;++iIter)\
    { /*Loop: Clear all cigar entries*/\
       (samSTPtr)->cigTypeStr[iIter] = '\0';\
       (samSTPtr)->cigValAryI[iIter] = 0;\
    } /*Loop: Clear all cigar entries*/\
    \
    (samSTPtr)->rNextStr[0] = '\0'; /*rNext entry*/\
    (samSTPtr)->lenRNextUC = 0;     /*Length of rNext*/\
    \
    (samSTPtr)->seqStr[0] = '\0';   /*sequence entry*/\
    (samSTPtr)->qStr[0] = '\0';     /*q-score entry*/\
    \
    (samSTPtr)->extraStr[0] = '\0';    /*Extra entries*/\
    (samSTPtr)->lenExtraUI = 0;\
    \
    /*Flags/single numeric values in the sam entry*/\
    (samSTPtr)->mapqUC = 0;      /*mapping quality*/\
    (samSTPtr)->flagUS = 0;      /*flag in samEntryCStr*/\
    (samSTPtr)->pNextI = 0;      /*PNEXT postion*/\
    (samSTPtr)->tLenI = 0;       /*TLEN*/\
    \
    /*Stats*/\
    \
    /*The Q-scores excluded the softmasked regions*/\
    (samSTPtr)->medianQF = 0;    /*median read Q-score*/\
    (samSTPtr)->meanQF = 0;      /*mean read Q-score*/\
    \
    (samSTPtr)->refStartUI = 0;  /*First reference base*/\
    (samSTPtr)->refEndUI = 0;    /*Last reference base*/\
    (samSTPtr)->readLenUI = 0;   /*Holds read length*/\
    (samSTPtr)->alnReadLenUI = 0;/*Aligned length*/\
    \
    (samSTPtr)->numMatchUI = 0;  /*number of matches*/\
    (samSTPtr)->numSnpUI = 0;    /*number of mismatches*/\
    (samSTPtr)->numInsUI = 0;    /*number of insertions*/\
    (samSTPtr)->numDelUI = 0;    /*number of deletions*/\
    (samSTPtr)->numMaskUI = 0;   /*number soft masked*/\
    \
    /*These variables are used in finding the Q-scores*/\
    for(iIter = 0; iIter < defMaxQScore; ++iIter)\
       (samSTPtr)->qHistUI[iIter] = 0;\
    \
    (samSTPtr)->sumQUL = 0;\
} /*blankSamEntry*/

/*-------------------------------------------------------\
| Fun-02: initSamEntry
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
unsigned char initSamEntry(struct samEntry *samSTPtr);

/*-------------------------------------------------------\
| Fun-03: freeSamEntryStack
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
void freeSamEntryStack(struct samEntry *samSTPtr);

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
void freeSamEntry(struct samEntry **samSTPtr);

/*-------------------------------------------------------\
| Fun-05: makeSamEntry
|  - Makes an heap allocated samEntry structure
| Input:
| Output:
|  - Returns:
|    o Pointer to a samEntry structure
|    o 0 if had an memory error
\-------------------------------------------------------*/
struct samEntry * makeSamEntry();

/*-------------------------------------------------------\
| Fun-06: samEntryQHistToMed
|   - Gets the median Q-score for an histogram of q-scores
|     in a samStruct
| Input:
|   - samSTPTr:
|     o Pointer to samEntry struct with a Q-score
|       histogram to find the median Q-score for
| Output:
|   - Modifies:
|     o samSTPtr->medianQF to have the median Q-score
\-------------------------------------------------------*/
#define samEntryQHistToMed(samSTPtr){\
    unsigned int numBasesUI = 0;\
    unsigned int midPointUL = (samSTPtr)->readLenUI >> 1;\
    unsigned int uiQ = 0;\
    \
    for(uiQ = 0; uiQ < defMaxQScore; uiQ++)\
    { /*Loop: through Q-score histogram for midpoint*/\
        numBasesUI += (samSTPtr)->qHistUI[uiQ];\
        \
        if(numBasesUI >= midPointUL)\
        { /*if found the midpoint, then find the median*/\
            if(numBasesUI > midPointUL || numBasesUI & 1)\
                (samSTPtr)->medianQF = (float) uiQ;\
            \
            else\
            { /*Else: even & 2 differnt Q's at midpoint*/\
                numBasesUI = uiQ;\
                ++uiQ;\
                \
                while((samSTPtr)->qHistUI[uiQ]==0) ++uiQ;\
                \
                (samSTPtr)->medianQF =\
                    (numBasesUI + uiQ) / ((float) 2);\
            } /*Else: even & 2 differnt Q's at midpoint*/\
            \
            break;\
        } /*if found the midpoint, then find the median*/\
    } /*Loop: through Q-score histogram for midpoint*/\
} /*samEntryQHistToMed*/

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
void samEntryFindQScores(struct samEntry *samSTPtr);

/*I am not making cpQScores available globally*/

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
   unsigned long *lenBuffUL,
   void *samVoidFILE
);

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
   unsigned long *lenBuffUL,
   void *outFILE
);

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
);

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
);

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
);

#endif

/*-------------------------------------------------------\
| Note-01:
|   - Notes about the sam file format from the sam file
|     pdf
\-------------------------------------------------------*/

/*
Sam file table for first 11 columns (all sam files have)
| Col | Field |  Type  |        Brief description              |
|:---:|:-----:|:------:|:-------------------------------------:|
|  1  | QNAME | String |       Query template NAME             |
|  2  | FLAG  |  Int   |          bitwise FLAG                 |
|  3  | RNAME | String |     Reference sequence NAME           |
|  4  |  POS  |  Int   |  1-based leftmost mapping POSition    |
|  5  | MAPQ  |  Int   |          MAPping Quality              |
|  6  | CIGAR | String |            CIGAR string               |
|  7  | RNEXT | String | Reference name of the mate/next read  |
|  8  | PNEXT |  Int   |   Position of the mate/next read      |
|  9  | TLEN  |  Int   |      observed Template LENgth         |
| 10  |  SEQ  | String |          segment SEQuence             |
| 11  | QUAL  | String | ASCII of Phred-scaled base Quality+33 |
*/

/*eqx cigar entry
`   matches are '='
`   mimsatches are 'X'
'   non-eqx merges matches and mismatches into 'M'
`   deletions are 'D'
`   insertions are 'I'
`   soft masks are 'S'
`   Numbers come before entry (=, X, D, I, or S) & show
`     number of times the entry is repeated
`   Everything else is a hard mask (was removed) or
'      removed the bases from the sequence
`   EX: 10S5=1X701 (10 soft masked bases, 5 matches,
`       1 SNP, 701 matches)
*/

/* Sam file flag values tables
| Bit  | FLAG  |                        Description                                 |
|:----:|:-----:|:------------------------------------------------------------------:|
| 1    | 0x1   | template having multiple segments in sequencing                    |
| 2    | 0x2   | each segment properly aligned according to the aligner             |
| 4    | 0x4   | segment unmapped                                                   |
| 8    | 0x8   | next segment in the template unmapped                              |
| 16   | 0x10  | SEQ being reverse complemented                                     |
| 32   | 0x20  | SEQ of the next segment in the template being reverse complemented |
| 64   | 0x40  | the first segment in the template                                  |
| 128  | 0x80  | the last segment in the template                                   |
| 256  | 0x100 | secondary alignment                                                |
| 512  | 0x200 | not passing filters, such as platform/vendor quality controls      |
| 1024 | 0x400 | PCR or optical duplicate                                           |
| 2048 | 0x800 | supplementary alignment                                            |
*/
