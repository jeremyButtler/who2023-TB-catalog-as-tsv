/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
' SOF: Start Of File
'   o header:
'     - Included libraries and definitions
'   o st-01: amrStruct
'     - Holds the information for a single amr mutation
'       that was extracted from the WHO catalog
'   o fun-01: amrIsRes
'     - Dectects if an amr was classified as resistant or
'       is unknow/not resitance
'   o fun-02: blankAmrStruct
'     - Sets all non-pointer values in amrStructPtr to 0
'   o fun-03: initAmrStruct
'     - Sets all values, including pointers in the
'       amrStruct structure to 0
'   o fun-04: freeAmrStructStack
'     - Frees the geneIdStr, refSeqStr, and amrSeqStr
'       arrays and sets all values to 0 in the input
'       amrStruct 
'   o fun-05: freeAmrStruct
'     - Frees an heap allocated amrStruct structure
'   o fun-06: freeAmrStructArray
'     - Frees an heap allocated array of amrStruct
'       structures
'   o fun-07: swapAmrStructs
'     - Swaps the values in two amrStruct structures
'   o fun-08: sortAmrStructArray
'     - Sort on an amrStruct array structures by reference
'       coordiante (uses shell sort)
'   o fun-09: findNearestAmr
'      - Finds the nearest amr at or after the input query
'   o fun-10: mallocDrugAryStr
'     - Returns a pointer to allocated memory for
'       drugAryStr
'   o fun-11: reallocDrugAryStr
'     - Returns a pointer to rellocated memory for
'       drugAryStr
'   o fun-12: cpDrugToDrugAry
'      - Copies an antibiotic to a drug array
'   o fun-13: getDrugFromDrugAry
'     - Gets the pointer to a drug c-string in a drugAry
'   o fun-14: findDrug_in_drugAryStr
'     - Finds an antibiotic in a drugAray c-string
'   o fun-15: read_2021_WhoAmrCsv
'     - Gets the amr data from the Who 2021 TB antibiotice
'       resistance catalog (genome indicie tab saved as
'       a csv).
'   o fun-17: checkCrossRes
'     - Check if there is cross resitance (2023 catalog)
'   o fun-18: pCrossRes
'     - Print out cross resitance (report not database)
'   o fun-19: read_2023_WhoAmrTsv
'     - Reads in the two tabs (as separate tsv's) and
'        converts them to an amrStructs array
'   o fun-19: amrSTAddSingleAa
'     - Add a single amino acid variant to an amrStruct
'   o fun-20: who2023ParsVar
'     - Parse the variant idea from the WHO 2023 TB
'       catalog to update amino acid mutations.
\~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*-------------------------------------------------------\
| Header:
|   - Included libraries and definitions
\-------------------------------------------------------*/

#ifndef AMR_STRUCT_H
#define AMR_STRUCT_H

#define def_amrST_maxDrugs 5
#define def_amrST_forwardDir 0
#define def_amrST_revCompDir 1
#define def_amrST_unkownDir 2
#define def_amrST_del 1
#define def_amrST_del_and_ins 2

/*This is for my database printing/read*/
#define def_amrST_resFlag 1
#define def_amrST_crossResFlag 2
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
   char aaDelBl;    /*1: means there is a deletion*/
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
| Fun-02: blankAmrStruct
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
   (amrStructPtr)->aaDelBl = 0;\
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
| Fun-03: initAmrStruct
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
void freeAmrStructStack(struct amrStruct *amrStructPtr);

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
void freeAmrStruct(struct amrStruct **amrStructPtr);

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
   unsigned long numElmUL       /*number of elements*/
);

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
   unsigned int startUI,
   unsigned int endUI
);

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
   unsigned int qryUI,
   int numAmrI
);

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
   unsigned long numAmrUL,
   char *drugStrAry,
   int numDrugsI,
   char *outStr
);

#endif
