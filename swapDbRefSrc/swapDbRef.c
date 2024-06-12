/*########################################################
# Name: swapDbRef
#   - swaps reference coordinates in an tbAmr database
########################################################*/

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
' SOF: Start Of File
'   o header:
'     - included libraries
'   o fun01: phelp_swapDbRef
'     - prints the help message to stdout or stderr
'   o fun02: getInput_swapDbRef
'     - gets user input for swapDbRef
'   o main:
'     - driver function
\~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*-------------------------------------------------------\
| Header:
|   - included libraries
\-------------------------------------------------------*/

#ifdef PLAN9
   #include <u.h>
   #include <libc.h>
#else
   #include <stdlib.h>
#endif

#include "refSwap.h"

#include <stdio.h>

#include "../generalLib/samEntryStruct.h"
#include "../generalLib/amrStruct.h"

/*.h files only*/
#include "../generalLib/dataTypeShortHand.h"
#include "../generalLib/ulCpStr.h"

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\
! Hidden libraries
!   - .c #include "../generalLib/drug_str_ary.h"
!   - .h #include "../generalLib/base10StrToNum.h"
!   - .h #include "../generalLib/numToStr.h"
!   - .h #include "../generalLib/ntToBit.h"
\%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/*-------------------------------------------------------\
| Fun01: phelp_swapDbRef
|   - prints the help message to stdout or stderr
| Input:
|   - errBl:
|     o 1: print help message to stderr
|     o 0: print help message to stdout
| Output:
|   - Prints:
|     o help messate to stdout/stderr
\-------------------------------------------------------*/
void
phelp_swapDbRef(
   signed char errBl
){
   FILE *outFILE = 0;

   if(errBl)
      outFILE = stderr;
   else 
      outFILE = stdout;

   fprintf(
     outFILE,
     "swapDbRef -sam ref.sam -amr amr-database.tsv\n"
   );

   fprintf(
     outFILE,
     "  - Change coordinates in an tbAmr database\n"
   );

   fprintf(
     outFILE,
     "Input:\n"
   );

   fprintf(
     outFILE,
     "  -amr amr-database.tsv: [Required]\n"
   );

   fprintf(
     outFILE,
     "    o amr databse to change coordinates for\n"
   );

   fprintf(
     outFILE,
     "  -sam ref.sam: [stdin]\n"
   );

   fprintf(
     outFILE,
     "    o sam file with new reference mapped to old\n"
   );

   fprintf(
     outFILE,
     "      reference\n"
   );

   fprintf(
     outFILE,
     "Output:\n "
   );

   fprintf(
      outFILE,
      "  - Prints database with converted coordinates\n"
   );

   fprintf(
      outFILE,
      "    to outFILE\n"
   );
} /*phelp_swapDbRef*/

/*-------------------------------------------------------\
| Fun02: getInput_swapDbRef
|   - gets user input for swapDbRef
| Input:
|   - numArgsSI:
|     o number of arguments user input
|   - argsAryStr:
|     o array of c-strings with user input
|   - dbFileStr:
|     o pointer to c-string be set to the dbFile c-string
|   - samFileStr:
|     o pointer to c-string be set to the sam c-string
| Output:
|   - Modifies:
|     o dbFileStr to hold the AMR database to convert
|     o samFileStr to hold the mapping coordinates for
|       the reference to convert
|   - Returns:
|     o 0 for no problems
|     o 1 for printed help message or version number
|     o 2 for invalid input
\-------------------------------------------------------*/
char
getInput_swapDbRef(
   signed int numArgsSI,
   char *argsAryStr[],
   char **dbFileStr,
   char **samFileStr
){

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun02 Sec01:
   ^   - variable declerations
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   sint siArg = 1;

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun02 Sec02:
   ^   - get input
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   if(numArgsSI < 2)
   { /*If: no arguments were input*/
         phelp_swapDbRef(0);
         return 1;
   } /*If: no arguments were input*/

   while(siArg < numArgsSI)
   { /*Loop: get user input*/
      if(! cStrEql("-amr", argsAryStr[siArg], 0))
      { /*If: this is the amr database file*/
         ++siArg;
         *dbFileStr = argsAryStr[siArg];
      } /*If: this is the amr database file*/

      else if(! cStrEql("-sam", argsAryStr[siArg], 0))
      { /*Else If: this is the reference sam file*/
         ++siArg;
         *samFileStr = argsAryStr[siArg];
      } /*Else If: this is the reference sam file*/

      else if(! cStrEql("-h", argsAryStr[siArg], 0))
      { /*Else If: user asked for help*/
         phelp_swapDbRef(0);
         return 1;
      } /*Else If: user asked for help*/

      else if(! cStrEql("--h", argsAryStr[siArg], 0))
      { /*Else If: user asked for help*/
         phelp_swapDbRef(0);
         return 1;
      } /*Else If: user asked for help*/

      else if(! cStrEql("help", argsAryStr[siArg], 0))
      { /*Else If: user asked for help*/
         phelp_swapDbRef(0);
         return 1;
      } /*Else If: user asked for help*/

      else if(! cStrEql("-help", argsAryStr[siArg], 0))
      { /*Else If: user asked for help*/
         phelp_swapDbRef(0);
         return 1;
      } /*Else If: user asked for help*/

      else if(! cStrEql("--help", argsAryStr[siArg], 0))
      { /*Else If: user asked for help*/
         phelp_swapDbRef(0);
         return 1;
      } /*Else If: user asked for help*/

      else
      { /*Else: invalid input*/
         phelp_swapDbRef(1);

         fprintf(
            stderr,
            "%s is not reconginized\n",
           argsAryStr[siArg]
         );

         return 2; /*argument not recognized*/
      } /*Else: invalid input*/

      ++siArg;
   } /*Loop: get user input*/

   return 0;
} /*getInput_swapDbRef*/

/*-------------------------------------------------------\
| Main:
|   - driver function
\-------------------------------------------------------*/
#ifdef PLAN9
void
#else
int
#endif
main(
   int numArgsSI,
   char *argsAryStr[]
){ /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
   ' Main TOC:
   '   o main sec01:
   '     - variable declerations
   '   o main sec02:
   '     - get user input
   '   o main sec03:
   '     - read in input files
   '   o main sec04:
   '     - convert the amr database coordinates
   '   o main sec05:
   '     - clean up
   \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Main Sec01:
   ^   - variable declerations
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   char *amrDbFileStr = 0;
   char *samFileStr = 0;

   char errSC = 0;

   struct samEntry samStackST;
   char *buffHeapStr = 0;
   ulong lenBuffUL = 0;

   FILE *samFILE = 0;

   struct amrStruct *amrHeapAryST = 0;
   uint numAmrsUI = 0;
   uint numCnvtAmrsUI = 0;

   char *drugHeapAryStr;
   uint numDrugsUI = 0;
   uint maxDrugsUI = 0;

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Main Sec02:
   ^   - get user input
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   initSamEntry(&samStackST);

   errSC =
      getInput_swapDbRef(
         numArgsSI,
         argsAryStr,
         &amrDbFileStr,
         &samFileStr
   );

   if(errSC)
   { /*If: I had an error*/
      --errSC; /*help message is 1, so goes to 0*/
      goto cleanUp_main;
   } /*If: I had an error*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Main Sec03:
   ^   - read in input files
   ^   o main sec05 sub01:
   ^     - read in the amr database
   ^   o main sec05 sub02:
   ^     - read in the sam file with mapped references
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   /*****************************************************\
   * Main Sec05 Sub01:
   *   - read in the amr database
   \*****************************************************/

   amrHeapAryST =
      readTbAmrTbl(
         amrDbFileStr,
         &numAmrsUI,
         &drugHeapAryStr,
         (int *) &numDrugsUI,
         (int *) &maxDrugsUI,
         &errSC
      );

   if(! amrHeapAryST)
   { /*If: I had an error*/
      if(errSC == def_amrST_memError)
         fprintf(
            stderr,
            "mem error reading AMR database (-amr %s)\n",
            amrDbFileStr
         );

      else
         fprintf(
            stderr,
            "could not opem AMR database (-amr %s)\n",
            amrDbFileStr
         );

      errSC = -1;
      goto cleanUp_main;
   } /*If: I had an error*/

   /*****************************************************\
   * Main Sec05 Sub02:
   *   - read in the sam file with mapped references
   *   o main sec05 sub02 cat01:
   *     - open the sam file
   *   o main sec05 sub02 cat02:
   *     - get the reference from the sam file
   \*****************************************************/

   /*++++++++++++++++++++++++++++++++++++++++++++++++++++\
   + Main Sec05 Sub02 Cat01:
   +   - open the sam file
   \++++++++++++++++++++++++++++++++++++++++++++++++++++*/

   if(! samFileStr || samFileStr[0] == '-')
      samFILE = stdin;

   else
      samFILE =
         fopen(
            samFileStr,
            "r"
         );

   if(! samFILE)
   { /*If: I coudl not open the sam file*/
      fprintf(
         stderr,
         "could not open -sam %s\n",
         samFileStr
      );

      errSC = -2;
      goto cleanUp_main;
   } /*If: I coudl not open the sam file*/

   /*++++++++++++++++++++++++++++++++++++++++++++++++++++\
   + Main Sec05 Sub02 Cat02:
   +   - get the reference from the sam file
   \++++++++++++++++++++++++++++++++++++++++++++++++++++*/

   do{ /*Loop: read in reference sequence in sam file*/
      errSC =
        readSamLine(
           &samStackST,
           &buffHeapStr,
           &lenBuffUL,
           samFILE
      );

      if(errSC > 1)
      { /*If: I had an error*/
         fprintf(
            stderr,
            "memory error reading sam file (-sam %s)\n",
            samFileStr
         );

         errSC = -3;
         goto cleanUp_main;
      } /*If: I had an error*/
   } /*Loop: read in reference sequence in sam file*/
   while(*samStackST.extraStr == '@');

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Main Sec04:
   ^   - convert the amr database coordinates
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   numCnvtAmrsUI =
      swapAmrAryRef_refSwap(
         amrHeapAryST,
         numAmrsUI,
         &samStackST
      );

   errSC =
      pAmrDB(
         amrHeapAryST,
         numCnvtAmrsUI,
         drugHeapAryStr,
         numDrugsUI - 1,
         "-"              /*print to stdout*/
      ); /*print the converted database*/

   errSC = 0;

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Main Sec05:
   ^   - clean up
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   cleanUp_main:;

   if(samFILE && samFILE != stdin)
      fclose(samFILE);

   samFILE = 0;

   free(buffHeapStr);
   buffHeapStr = 0;

   free(drugHeapAryStr);
   drugHeapAryStr = 0;

   freeSamEntryStack(&samStackST);

   exit((sint) errSC);
} /*main*/
