/*########################################################
# Name: amrStruct
#   - Holds functions to support the amrStruct (also has)
#     used in tbAmr. This includes processing the tbAmr
#     format, sorting the structures, looking up
#     structures and memory managment
########################################################*/

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
' SOF: Start Of File
'   o header:
'     - Definations
'   o .h st01: amrStruct
'     - Holds the information for a single amr mutation
'       that was extracted from the WHO catalog
'   o .h fun01: blankAmrStruct
'     - Sets all non-pointer values in amrStructPtr to 0
'   o .h fun02: initAmrStruct
'     - Sets all values, including pointers in the
'       amrStruct structure to 0
'   o fun03: freeAmrStructStack
'     - Frees the geneIdStr, refSeqStr, and amrSeqStr
'       arrays and sets all values to 0 in the input
'       amrStruct 
'   o fun04: freeAmrStruct
'     - Frees an heap allocated amrStruct structure
'   o fun05: freeAmrStructArray
'     - Frees an heap allocated array of amrStruct
'       structures
'   o .h fun06: swapAmrStructs
'     - Swaps the values in two amrStruct structures
'   o fun07: sortAmrStructArray
'     - Sort on an amrStruct array structures by reference
'       coordiante (uses shell sort)
'   o fun08: geneIdSortAmrSTAry
'     - Sort on an amrStruct array structures by the
'       gene names (ids) (uses shell short)
'   o fun09: findNearestAmr
'      - Finds the nearest amr at or after the input query
'   o fun10: pAmrDB
'     - Print out the amr database used
'   o fun11: readTbAmrTbl
'     - Gets data from a tbAmr tsv file output from pAmrDB
'       (fun09)
'   o license:
'     - Licensing for this code (public domain / mit)
\~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*-------------------------------------------------------\
| Header:
|   - Included libraries and definitions
\-------------------------------------------------------*/

#ifndef AMR_STRUCT_H
#define AMR_STRUCT_H

/*This is the number of longs to hold the resistance
`   flags. Each long holds 64 drugs on a 64 bit system
`   or 32 drugs on an 32 bit system
*/
#define def_amrST_maxDrugs 5

/*Reading frame direction*/
#define def_amrST_forwardDir 0
#define def_amrST_revCompDir 1
#define def_amrST_unkownDir 2

/*Whole gene event flags*/
#define def_geneFrameshift_amrST 1
#define def_geneDel_amrST 2

/*This is the flags for each resistance type when printed
`   out
*/
#define def_amrST_resFlag 1
#define def_amrST_crossResFlag 2
#define def_amrST_crossToResShift 1
   /*for converting cross restence to one bit*/
#define def_amrST_maxFlags (sizeof(unsigned long) << 3)

/*Error values*/
#define def_amrST_memError 64
#define def_amrST_invalidFILE 2
#define def_amrST_missingIndice 4

/*-------------------------------------------------------\
| ST-01: amrStruct
|   - Holds the information for a single amr mutation
|     that was extracted from the WHO catalog
\-------------------------------------------------------*/
typedef struct amrStruct{
   unsigned int refPosUI;  /*Start position on reference*/
   unsigned int codonPosUI;/*Codon start position on ref*/
   unsigned int codonNumUI;/*Codon number*/
   unsigned int endCodonNumUI; /*end of an indel*/

   /*This might allow my custom database to be a bit more
   ` usefull to others. It will at least help with mapping
   ` AMR's in my graphAmpDepth.r script
   */
   unsigned int geneFirstRefUI; /*1st ref base in gene*/
   unsigned int geneLastRefUI;  /*Last ref base in gene*/

   /*Amino acid sequence stuff*/
   char *refAaStr;    /*What the reference amino acid is*/
   unsigned int lenRefAaUI; /*Length of reference aa seq*/

   char *amrAaStr;    /*What the mutant amino acid is*/
   unsigned int lenAmrAaUI; /*Length of reference aa seq*/

   char frameshiftBl; /*1: is a frame shift*/
   char wholeGeneFlag;/*1: means an whole gene event*/
   char unknownBl;    /*1: means I have no idea what is*/
   char aaMultiDupBl; /*1: means range duplicated*/

   /*General info/ mutation*/
   char *geneIdStr; /*Gene name*/
   unsigned int lenGeneIdUI;/*Length of gene id*/

   char *refSeqStr; /*Reference sequence at refPosUI*/
   unsigned int lenRefSeqUI;/*Length of reference sequence*/

   char *amrSeqStr; /*AMR sequence at refPosUI*/
   unsigned int lenAmrSeqUI;/*Length of amr sequence*/

   char *varIdStr;  /*Variant nomincalture/id*/
   unsigned int lenVarIdUI;

   char *effectStr; /*WHO comment on what mutation did*/
   unsigned int lenEffectUI;/*Length of effect*/

   char *commentStr; /*WHO comment (has epistatic)*/
   unsigned int lenCommentUI; /*Length of comment*/

   char gradeC;    /*Grade of resitance*/
   char mutTypeStr[4]; /*Mutation type*/
   char dirFlag;   /*-1: gene reverse complement, else 0*/
      /*This is marked in the gene_locus column as a c
      ' at the end
      */
   
   /*Flags for antibiotic resistance*/
   unsigned long amrFlagsUL[def_amrST_maxDrugs];
   unsigned long crossResFlagsUL[def_amrST_maxDrugs];
      /*5 items should allow for ~ 150 antibiotics on a
      `  32 bit system. ~ 300 on a 64 bit
      */

   unsigned int numSupReadsUI; /*Number of read support*/
   unsigned int numMapReadsUI; /*Number of reads mapping*/

   /*Flags/variables for special who comments*/
   char highResBl; /*1: high resitance primary*/
   char lowResBl;  /*1: low resitance primary*/
   char additiveResBl; /*1: low resitance is addivitve*/
   char *needsGeneStr; /*Gene needed for resitance*/
   unsigned int lenNeedsGeneUI; /*length of needsGeneStr*/
}amrStruct;

/*-------------------------------------------------------\
| Fun01: blankAmrStruct
|   - Sets all non-pointer values in amrStructPtr to 0
| Input:
|   - ampStructPtr:
|     - Pointer to amrStruct to blank
| Output:
|   - Modifies
|     o All non-pointer values in amrStructPtr to be 0
\-------------------------------------------------------*/
#define blankAmrStruct(amrStructPtr){\
   int iAmrMac = 0;\
   \
   (amrStructPtr)->refPosUI = 0;\
   (amrStructPtr)->codonPosUI = 0;\
   (amrStructPtr)->codonNumUI = 0;\
   (amrStructPtr)->endCodonNumUI = 0;\
   \
   (amrStructPtr)->geneFirstRefUI = 0;\
   (amrStructPtr)->geneLastRefUI = 0;\
   \
   (amrStructPtr)->frameshiftBl = 0;\
   (amrStructPtr)->wholeGeneFlag = 0;\
   (amrStructPtr)->unknownBl = 0;\
   (amrStructPtr)->aaMultiDupBl = 0;\
   \
   (amrStructPtr)->dirFlag = def_amrST_unkownDir;\
   (amrStructPtr)->mutTypeStr[0] = '\0';\
   \
   (amrStructPtr)->gradeC = 0;\
   (amrStructPtr)->numSupReadsUI = 0;\
   (amrStructPtr)->numMapReadsUI = 0;\
   \
   (amrStructPtr)->highResBl = 0;\
   (amrStructPtr)->lowResBl = 0;\
   (amrStructPtr)->additiveResBl = 0;\
   \
   for(\
      iAmrMac = 0;\
      iAmrMac < def_amrST_maxDrugs;\
      ++iAmrMac\
   ){ /*Loop: Blank the antibiotic flags*/\
      (amrStructPtr)->amrFlagsUL[iAmrMac] = 0;\
      (amrStructPtr)->crossResFlagsUL[iAmrMac] = 0;\
   } /*Loop: Blank the antibiotic flags*/\
} /*blankAmrStruct*/

/*-------------------------------------------------------\
| Fun02: initAmrStruct
|   - Sets all values, including pointers in amrStructPtr
|     to 0
| Input:
|   - ampStructPtr:
|     - Pointer to amrStruct to initialize
| Output:
|   - Modifies
|     o All values in amrStructPtr to be 0
\-------------------------------------------------------*/
#define initAmrStruct(amrStructPtr){\
   blankAmrStruct((amrStructPtr));\
   \
   (amrStructPtr)->geneIdStr = 0;\
   (amrStructPtr)->lenGeneIdUI = 0;\
   \
   (amrStructPtr)->refSeqStr = 0;\
   (amrStructPtr)->lenRefSeqUI = 0;\
    \
   (amrStructPtr)->amrSeqStr = 0;\
   (amrStructPtr)->lenAmrSeqUI = 0;\
   \
   (amrStructPtr)->varIdStr = 0;\
   (amrStructPtr)->lenVarIdUI = 0;\
   \
   (amrStructPtr)->effectStr = 0;\
   (amrStructPtr)->lenEffectUI = 0;\
   \
   (amrStructPtr)->commentStr = 0;\
   (amrStructPtr)->lenCommentUI = 0;\
   \
   (amrStructPtr)->refAaStr = 0;\
   (amrStructPtr)->lenRefAaUI = 0;\
   \
   (amrStructPtr)->amrAaStr = 0;\
   (amrStructPtr)->lenAmrAaUI = 0;\
   \
   (amrStructPtr)->needsGeneStr = 0;\
   (amrStructPtr)->lenNeedsGeneUI = 0;\
} /*initAmrStruct*/

/*-------------------------------------------------------\
| Fun03: freeAmrStructStack
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
void freeAmrStructStack(struct amrStruct *amrStructPtr);

/*-------------------------------------------------------\
| Fun04: freeAmrStruct
|   - Frees an heap allocated amrStruct structure
| Input:
|   - ampStructPtr:
|     - Pointer to amrStruct to free
| Output:
|   - Frees:
|     o amrStructPtr and the variables inside amrStructPtr
\-------------------------------------------------------*/
void freeAmrStruct(struct amrStruct **amrStructPtr);

/*-------------------------------------------------------\
| Fun05: freeAmrStructArray
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
   unsigned long numElmUL       /*number of elements*/
);

/*-------------------------------------------------------\
| Fun06: swapAmrStructs
|   - Swaps the values in two amrStruct structures
| Input:
|   - firstAmrSTPtr:
|     - first amrStruct to swap values in (not a pointer)
|   - secAmrSTPtr:
|     - second amrStruct to swap values in (not a pointer)
| Output:
|   - Modifies:
|     o fristAmrSTPtr to have the values of secAmrSTPtr
|     o secAmrSTPtr to have the values of frstAmrSTPtr
\-------------------------------------------------------*/
#define swapAmrStructs(firstAmrST, secAmrST){\
   char tmpMacC = 0;\
   char *tmpMacStr = 0;\
   uint tmpMacUI = 0;\
   ulong tmpMacUL = 0;\
   int iAmrMac = 0;\
   \
   \
   tmpMacUI = (firstAmrST).refPosUI;\
   (firstAmrST).refPosUI = (secAmrST).refPosUI;\
   (secAmrST).refPosUI = tmpMacUI;\
   \
   tmpMacUI = (firstAmrST).codonPosUI;\
   (firstAmrST).codonPosUI = (secAmrST).codonPosUI;\
   (secAmrST).codonPosUI = tmpMacUI;\
   \
   tmpMacUI = (firstAmrST).codonNumUI;\
   (firstAmrST).codonNumUI = (secAmrST).codonNumUI;\
   (secAmrST).codonNumUI = tmpMacUI;\
   \
   tmpMacUI = (firstAmrST).endCodonNumUI;\
   (firstAmrST).endCodonNumUI =\
      (secAmrST).endCodonNumUI;\
   (secAmrST).endCodonNumUI = tmpMacUI;\
   \
   tmpMacUI = (firstAmrST).geneFirstRefUI;\
   (firstAmrST).geneFirstRefUI =\
      (secAmrST).geneFirstRefUI;\
   (secAmrST).geneFirstRefUI = tmpMacUI;\
   \
   tmpMacUI = (firstAmrST).geneLastRefUI;\
   (firstAmrST).geneLastRefUI =\
      (secAmrST).geneLastRefUI;\
   (secAmrST).geneLastRefUI = tmpMacUI;\
   \
   tmpMacStr = (firstAmrST).refAaStr;\
   (firstAmrST).refAaStr = (secAmrST).refAaStr;\
   (secAmrST).refAaStr = tmpMacStr;\
   \
   tmpMacUI = (firstAmrST).lenRefAaUI;\
   (firstAmrST).lenRefAaUI = (secAmrST).lenRefAaUI;\
   (secAmrST).lenRefAaUI = tmpMacUI;\
   \
   \
   tmpMacStr = (firstAmrST).amrAaStr;\
   (firstAmrST).amrAaStr = (secAmrST).amrAaStr;\
   (secAmrST).amrAaStr = tmpMacStr;\
   \
   tmpMacUI = (firstAmrST).lenAmrAaUI;\
   (firstAmrST).lenAmrAaUI = (secAmrST).lenAmrAaUI;\
   (secAmrST).lenAmrAaUI = tmpMacUI;\
   \
   \
   tmpMacC = (firstAmrST).aaMultiDupBl;\
   (firstAmrST).aaMultiDupBl = (secAmrST).aaMultiDupBl;\
   (secAmrST).aaMultiDupBl = tmpMacC;\
   \
   tmpMacC = (firstAmrST).frameshiftBl;\
   (firstAmrST).frameshiftBl = (secAmrST).frameshiftBl;\
   (secAmrST).frameshiftBl = tmpMacC;\
   \
   tmpMacC = (firstAmrST).wholeGeneFlag;\
   (firstAmrST).wholeGeneFlag = (secAmrST).wholeGeneFlag;\
   (secAmrST).wholeGeneFlag = tmpMacC;\
   \
   tmpMacC = (firstAmrST).unknownBl;\
   (firstAmrST).unknownBl = (secAmrST).unknownBl;\
   (secAmrST).unknownBl = tmpMacC;\
   \
   \
   tmpMacStr = (firstAmrST).geneIdStr;\
   (firstAmrST).geneIdStr = (secAmrST).geneIdStr;\
   (secAmrST).geneIdStr = tmpMacStr;\
   \
   tmpMacUI = (firstAmrST).lenGeneIdUI;\
   (firstAmrST).lenGeneIdUI=(secAmrST).lenGeneIdUI;\
   (secAmrST).lenGeneIdUI = tmpMacUI;\
   \
   \
   tmpMacStr = (firstAmrST).refSeqStr;\
   (firstAmrST).refSeqStr = (secAmrST).refSeqStr;\
   (secAmrST).refSeqStr = tmpMacStr;\
   \
   tmpMacUI = (firstAmrST).lenRefSeqUI;\
   (firstAmrST).lenRefSeqUI=(secAmrST).lenRefSeqUI;\
   (secAmrST).lenRefSeqUI = tmpMacUI;\
   \
   \
   tmpMacStr = (firstAmrST).amrSeqStr;\
   (firstAmrST).amrSeqStr = (secAmrST).amrSeqStr;\
   (secAmrST).amrSeqStr = tmpMacStr;\
   \
   tmpMacUI = (firstAmrST).lenAmrSeqUI;\
   (firstAmrST).lenAmrSeqUI=(secAmrST).lenAmrSeqUI;\
   (secAmrST).lenAmrSeqUI = tmpMacUI;\
   \
   \
   tmpMacStr = (firstAmrST).varIdStr;\
   (firstAmrST).varIdStr = (secAmrST).varIdStr;\
   (secAmrST).varIdStr = tmpMacStr;\
   \
   tmpMacUI = (firstAmrST).lenVarIdUI;\
   (firstAmrST).lenVarIdUI = (secAmrST).lenVarIdUI;\
   (secAmrST).lenVarIdUI = tmpMacUI;\
   \
   \
   tmpMacStr = (firstAmrST).effectStr;\
   (firstAmrST).effectStr = (secAmrST).effectStr;\
   (secAmrST).effectStr = tmpMacStr;\
   \
   tmpMacUI = (firstAmrST).lenEffectUI;\
   (firstAmrST).lenEffectUI=(secAmrST).lenEffectUI;\
   (secAmrST).lenEffectUI = tmpMacUI;\
   \
   \
   tmpMacStr = (firstAmrST).commentStr;\
   (firstAmrST).commentStr = (secAmrST).commentStr;\
   (secAmrST).commentStr = tmpMacStr;\
   \
   tmpMacUI = (firstAmrST).lenCommentUI;\
   (firstAmrST).lenCommentUI =\
      (secAmrST).lenCommentUI;\
   (secAmrST).lenCommentUI = tmpMacUI;\
   \
   \
   tmpMacC = (firstAmrST).dirFlag;\
   (firstAmrST).dirFlag = (secAmrST).dirFlag;\
   (secAmrST).dirFlag = tmpMacC;\
   \
   tmpMacC = (firstAmrST).gradeC;\
   (firstAmrST).gradeC = (secAmrST).gradeC;\
   (secAmrST).gradeC = tmpMacC;\
   \
   \
   tmpMacUI = (firstAmrST).numSupReadsUI;\
   (firstAmrST).numSupReadsUI =\
       (secAmrST).numSupReadsUI;\
   (secAmrST).numSupReadsUI = tmpMacUI;\
   \
   tmpMacUI = (firstAmrST).numMapReadsUI;\
   (firstAmrST).numMapReadsUI =\
       (secAmrST).numMapReadsUI;\
   (secAmrST).numMapReadsUI = tmpMacUI;\
   \
   \
   tmpMacC = (firstAmrST).mutTypeStr[0];\
   \
   (firstAmrST).mutTypeStr[0]\
      = (secAmrST).mutTypeStr[0];\
   \
   (secAmrST).mutTypeStr[0] = tmpMacC;\
   \
   tmpMacC = (firstAmrST).mutTypeStr[1];\
   \
   (firstAmrST).mutTypeStr[1]\
      = (secAmrST).mutTypeStr[1];\
   \
   (secAmrST).mutTypeStr[1] = tmpMacC;\
   \
   tmpMacC = (firstAmrST).mutTypeStr[2];\
   \
   (firstAmrST).mutTypeStr[2]\
      = (secAmrST).mutTypeStr[2];\
   \
   (secAmrST).mutTypeStr[2] = tmpMacC;\
   \
   \
   tmpMacStr = (firstAmrST).needsGeneStr;\
   (firstAmrST).needsGeneStr =\
      (secAmrST).needsGeneStr;\
   (secAmrST).needsGeneStr = tmpMacStr;\
   \
   tmpMacUI = (firstAmrST).lenNeedsGeneUI;\
   (firstAmrST).lenNeedsGeneUI =\
      (secAmrST).lenNeedsGeneUI;\
   (secAmrST).lenNeedsGeneUI = tmpMacUI;\
   \
   tmpMacC = (firstAmrST).highResBl;\
   (firstAmrST).highResBl = (secAmrST).highResBl;\
   (secAmrST).highResBl = tmpMacC;\
   \
   tmpMacC = (firstAmrST).lowResBl;\
   (firstAmrST).lowResBl = (secAmrST).lowResBl;\
   (secAmrST).lowResBl = tmpMacC;\
   \
   tmpMacC = (firstAmrST).additiveResBl;\
   (firstAmrST).additiveResBl =\
      (secAmrST).additiveResBl;\
   (secAmrST).additiveResBl = tmpMacC;\
   \
   \
   for(\
      iAmrMac = 0;\
      iAmrMac < def_amrST_maxDrugs;\
      ++iAmrMac\
   ){ /*Loop: Blank the antibiotic flags*/\
      tmpMacUL = (firstAmrST).amrFlagsUL[iAmrMac];\
      \
      (firstAmrST).amrFlagsUL[iAmrMac] =\
         (secAmrST).amrFlagsUL[iAmrMac];\
      \
      (secAmrST).amrFlagsUL[iAmrMac] = tmpMacUL;\
      \
      \
      tmpMacUL =(firstAmrST).crossResFlagsUL[iAmrMac];\
      \
      (firstAmrST).crossResFlagsUL[iAmrMac] =\
         (secAmrST).crossResFlagsUL[iAmrMac];\
      \
      (secAmrST).crossResFlagsUL[iAmrMac] = tmpMacUL;\
   } /*Loop: Blank the antibiotic flags*/\
} /*swapAmrStructs*/


/*-------------------------------------------------------\
| Fun07: sortAmrStructArray
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
   unsigned int startUI,
   unsigned int endUI
);

/*-------------------------------------------------------\
| Fun08: geneIdSortAmrSTAry
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
);

/*-------------------------------------------------------\
| Fun09: findNearestAmr
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
   unsigned int qryUI,
   int numAmrI
);

/*-------------------------------------------------------\
| Fun10: pAmrDB
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
   unsigned long numAmrUL,
   char *drugStrAry,
   int numDrugsI,
   char *outStr
);

/*-------------------------------------------------------\
| Fun11: readTbAmrTbl
|   - Gets data from a tbAmr tsv file output from pAmrDB
|     (fun09)
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
);

#endif

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
