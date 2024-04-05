/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
' SOF: Start Of File
'   o header:
'     - Included libraries and function headers
'   o main:
'     - Main function (TODO need to add documentation)
'   o fun-01: cnvtWhoToTbAmrInput
'     - Get input (TODO need to add documentation)
'   o license:
'     - Licensing for this code (public domain / mit)
\~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*-------------------------------------------------------\
| Header:
|   - Included libraries and function headers
|   o header Sec-01:
|     - Included libraries
|   o header Sec-02:
|     - function headers
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
#include <string.h>

#include "amrStruct.h"
#include "../generalLib/samEntryStruct.h"
#include "genIndiceStruct.h"
#include "cnvt_whoToTbAmr.h"

/*All of these are just .h files*/
#include "../generalLib/ulCpStr.h"
#include "../generalLib/base10StrToNum.h"
#include "../generalLib/dataTypeShortHand.h"

/*Hidden includes
`  - .c + .h files
`    o #include "drug_str_ary.h"
`  - .h only files
`    o #include "../generalLib/gen-shellSort.h"
`    o #include "../generalLib/genMath.h"
`    o #include "../generalLib/codonTbl.h"
*/

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
^ Header Sec-02:
^   - function headers
\<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

char * cnvtWhoToTbAmrInput(
   int lenArgsI,
   char *argsStrAry[],
   char **whoFirstTabStr,
   char **whoSecTabStr,
   char **refStr,
   char **coordsStr,
   char **outStr,
   char *allAmrsBl,
   char *rmGeneDelBl,
   char *who2023Bl
);

/*-------------------------------------------------------\
| Main:
|   - Main fuction to convert the WHO catalog to tbAmr
|     format
\-------------------------------------------------------*/
#ifdef PLAN9
void
#else
int
#endif
main(
   int lenArgsI,       /*Number arguments from user*/
   char *argsStrAry[]  /*Arguments from user*/
){ /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
   ' Main TOC:
   '   o main sec-01:
   '     - Variable declerations
   '   o main sec-02:
   '     - Get and check user input
   '   o main sec-03:
   '     - Read in the AMRs from the catalog
   '   o main sec-04:
   '     - Parse each AMRs variant id
   '   o main sec-05:
   '     - Add in the aa data
   '   o main sec-06:
   '     - Print out the AMRs and clean up
   \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Main Sec-01:
   ^   - Variable declerations
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   char *whoFirstTabStr = 0;
   char *whoSecTabStr = 0;
   char *refStr = 0;
   char *coordsStr = 0;
   char *outStr = "-";

   char allAmrsBl = 0;
   char rmGeneDelBl = 0;
   char who2023Bl = 1; /*2023 catalog entries*/

   /*Buffer for reading sam files*/

   ulong numAmrUL = 0;
   char *drugAryStr = 0;
   int numDrugsI = 0;

   char *errStr = 0;
   uchar errUC = 0;

   FILE *checkFILE = 0;

   struct amrStruct *amrSTAry = 0;

   char *helpStr ="\
      \nwhoToTbAmr\\\
      \n     -sam genes.sam\\\
      \n     -tabone-who-2023-tsv tabone.tsv\\\
      \n     -tabtwo-who-2023-tsv tabtwo.tsv\
      \n   - Converts the who catalog to a tsv file\
      \n Input:\
      \n   -tabone-who-2023-tsv: [Requird]\
      \n     o The master (fist tab) of the who 2023\
      \n       catalog saved as a tsv (tab deliminted)\
      \n       file.\
      \n     o This expects only one header, so if you\
      \n       converted the excel file to a tvs, then\
      \n       you will have to delete the first two rows\
      \n   -tabtwo-who-2023-tsv: [Required]\
      \n     o The genome indice (second tab) of the who\
      \n       2023 catalog saved as a tsv file.\
      \n   -who-2021-csv:\
      \n     o Genome indice tab of the 2021 who catalog\
      \n       saved as a csv. Only use this if you are\
      \n       converting the  2021 catalog instead of\
      \n       the 2023 catalog.\
      \n   -ref: [Required]\
      \n     o Fasta file with the reference (NC000962)\
      \n       sequence\
      \n   -coords: [Required]\
      \n     o Tsv file with the gene coordiantes:\
      \n       - column 1 is the gene name\
      \n       - column 2 is the reference name\
      \n       - column 3 the reading frame direction\
      \n         - \'+\' for an foward gene\
      \n         - \'-\' for an reverse complement gene\
      \n       - column 4 is first reference base in gene\
      \n       - column 5 is last reference base in gene\
      \n     o Command to build tsv file. The -a in\
      \n       minimap2 is to avoid the coordinates being\
      \n       slightly off (about 4 bases). Not sure why\
      \n       this happens in the paf but not sam file\
      \n         minimap2 \\\
      \n              --secondary=no \\\
      \n              -a \\\
      \n              NC000962.3.fa \\\
      \n              genes.fa |\
      \n           awk \\\
      \n              \'BEGIN{FS=OFS=\"\\t\"}; \
      \n               {\
      \n                 if($6 !~ /^[0-9]*M$/) next;\
      \n                 if($2 != 16 && $2 != 0) next;\
      \n                 sub(/M/, \"\", $6);\
      \n                 if($2 == 16) dirC = \"-\";\
      \n                 else dirC = \"+\";\
      \n                 print $1, $3, dirC, $4, $4 + $6;\
      \n               }\' > gene-tbl.tsv\
      \n   -out: [stdout/terminal]\
      \n     o File to output to.\
      \n   -all-amrs: [No]\
      \n     o Print out both resitance and non-resitant\
      \n       entries\
      \n     o disable with -res-amrs\
      \n     o This setting is only for the 2023 catalog\
      \n   -whole-gene-dels: [Yes]\
      \n     o Print out whole genome deletion entries\
      \n     o Disable with -no-whole-gene-dels\
      \n     o This setting is only for the 2023 catalog\
      \n Output:\
      \n    - Prints converted file to -out\
      \n Version: April 05, 2024\
      ";

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Main Sec-02:
   ^   - Get and check user input
   ^   o main sec-02 sub-01:
   ^     - Get user input and check for input errors
   ^   o main sec-02 sub-02:
   ^     - Check if I can open the first tab
   ^   o main sec-02 sub-03:
   ^     - Check if can open second tab of 2023 catalog
   ^   o main sec-02 sub-04:
   ^     - Check if the user provided the reference file
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   /*****************************************************\
   * Main Sec-02 Sub-01:
   *   - Get user input and check for input errors
   \*****************************************************/

   errStr =
      cnvtWhoToTbAmrInput(
         lenArgsI,
         argsStrAry,
         &whoFirstTabStr,
         &whoSecTabStr,
         &refStr,
         &coordsStr,
         &outStr,
         &allAmrsBl,
         &rmGeneDelBl,
         &who2023Bl
      ); /*Get user input*/

   if(errStr)
   { /*If: I had an error*/
      if(  ! strcmp("-h", errStr)
         | ! strcmp("--h", errStr)
         | ! strcmp("-help", errStr)
         | ! strcmp("--help", errStr)
         | ! strcmp("help", errStr)
      ){ /*Pirnt out the help message*/
         fprintf(stdout, "%s\n", helpStr);
         exit(0);
      } /*Pirnt out the help message*/


      if(  ! strcmp("-v", errStr)
         | ! strcmp("--v", errStr)
         | ! strcmp("-version", errStr)
         | ! strcmp("--version", errStr)
         | ! strcmp("version", errStr)
      ){ /*Pirnt out the help message*/
         fprintf(stdout, "March 15, 2024\n");
         exit(0);
      } /*Pirnt out the help message*/

      else
         fprintf(stderr, "%s is not valid\n", errStr);

      exit(-1);
   } /*If: I had an error*/

   /*****************************************************\
   * Main Sec-02 Sub-02:
   *   - Check if I can open the first tab
   \*****************************************************/

   if(! whoFirstTabStr)
   { /*If: No first file was provied*/

      fprintf(
         stderr,
         "No file for who-2021-tsv or"
      );
      fprintf(stderr, " -tabone-who-2023-tsv\n");
      exit(-1);
   } /*If: No first file was provied*/

   checkFILE = fopen(whoFirstTabStr, "r");

   if(! checkFILE)
   { /*If: No first file was provied*/
      fprintf(
         stderr,
         "Could not open -tabone-who-2023-tsv %s\n",
          whoFirstTabStr
      );

      exit(-1);
   } /*If: No first file was provied*/

   fclose(checkFILE);
   checkFILE = 0;

   /*****************************************************\
   * Main Sec-02 Sub-03:
   *   - Check if can open the second tab of 2023 catalog
   \*****************************************************/

   if(who2023Bl)
   { /*If: this is a 2023 catalog entry*/
      if(! whoSecTabStr)
      { /*If: No first file was provied*/
         fprintf(
            stderr,
            "No file for -tabtwo-who-2023-tsv\n"
         );

         exit(-1);
      } /*If: No first file was provied*/

      checkFILE = fopen(whoSecTabStr, "r");

      if(! checkFILE)
      { /*If: I could not open second tab file*/

         fprintf(
            stderr,
            "Could not open -tabtwo-who-2023-tsv %s\n",
             whoSecTabStr
         );
   
         exit(-1);
      } /*If: I could not open second tab file*/

      fclose(checkFILE);
      checkFILE = 0;
   } /*If: this is a 2023 catalog entry*/

   /*****************************************************\
   * Main Sec-02 Sub-04:
   *   - Check if the user provided the reference file
   \*****************************************************/

   if(! coordsStr)
   { /*If: No reference sequence file was input*/
      fprintf(
         stderr,
         "A reference sequence (-ref ref.fa) is needed\n"
      );

      exit(-1);
   } /*If: No reference sequence file was input*/

   checkFILE = fopen(coordsStr, "r");

   if(! checkFILE)
   { /*If: could not open reference sequence file*/
         fprintf(
            stderr,
            "Could not open -ref %s\n",
             coordsStr
         );
   
         exit(-1);
   } /*If: could not open reference sequence file*/

   fclose(checkFILE);
   checkFILE = 0;

   /*****************************************************\
   * Main Sec-02 Sub-05:
   *   - Check if the user provided gene coordinates file
   \*****************************************************/

   if(! refStr)
   { /*If: No reference sequence file was input*/
      fprintf(
         stderr,
         "Gene coordiantes (-coords genes.tsv) are needed"
      );

      fprintf(stderr, "\n");
      exit(-1);
   } /*If: No reference sequence file was input*/

   checkFILE = fopen(refStr, "r");

   if(! checkFILE)
   { /*If: could not open reference sequence file*/
         fprintf(
            stderr,
            "Could not open -coords %s\n",
             refStr
         );
   
         exit(-1);
   } /*If: could not open reference sequence file*/

   fclose(checkFILE);
   checkFILE = 0;

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Main Sec-03:
   ^   - Read in the AMRs from the catalog
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   switch(who2023Bl)
   { /*Switch: checking amr input method*/
      case 0:
         amrSTAry =
            read_2021_WhoAmrCsv(
               whoFirstTabStr,
               &numAmrUL,
               &drugAryStr,
               &numDrugsI
            ); /*get the amr data (2021 catalog*/
         break;

      case 1:
         amrSTAry =
             read_2023_WhoAmrTsv(
                whoFirstTabStr,
                whoSecTabStr,
                &numAmrUL,
                &drugAryStr,
                &numDrugsI,
                allAmrsBl,
                rmGeneDelBl,
                (char *) &errUC
             ); /*Get the amr data (2023 catalog)*/
         break;
   } /*Switch: checking amr input method*/

   if(amrSTAry == 0)
   { /*If: I could not make an amrStruct array*/
      fprintf(stderr, "Mem Err: making AMR array\n");

      freeAmrStructArray(&amrSTAry, numAmrUL);
      free(drugAryStr);

      exit(-1);
   } /*If: I could not make an amrStruct array*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Main Sec-04:
   ^   - Parse each AMRs variant id
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   errUC=(uchar) who_parse_VarID(amrSTAry,(int) numAmrUL);

   if(errUC)
   { /*If: I had a memory error*/
      fprintf(stderr,"Mem Err: processing variant ids\n");
      exit(-1);
   } /*If: I had a memory error*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Main Sec-05:
   ^   - Add in the aa data
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   errUC =
      (uchar)
      whoAddCodonPos(
         amrSTAry,
         (int) numAmrUL,
         coordsStr,
         refStr
      );

   /*I already checked the files, so this is likely an
   `   memory error
   */
   if(errUC)
   { /*If: I had a memory error*/
      fprintf(stderr,"Mem Err: find codon position\n");

      freeAmrStructArray(&amrSTAry, numAmrUL);
      free(drugAryStr);

      exit(-1);
   } /*If: I had a memory error*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Main Sec-06:
   ^   - Print out the AMRs and clean up
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   pAmrDB(amrSTAry,numAmrUL,drugAryStr,numDrugsI,outStr);

   freeAmrStructArray(&amrSTAry, numAmrUL);
   free(drugAryStr);

   exit(0);
} /*Main*/

/*-------------------------------------------------------\
| Fun-01: cnvtWhoToTbAmrInput
\-------------------------------------------------------*/
char * cnvtWhoToTbAmrInput(
   int lenArgsI,
   char *argsStrAry[],
   char **whoFirstTabStr,
   char **whoSecTabStr,
   char **refStr,
   char **coordsStr,
   char **outStr,
   char *allAmrsBl,
   char *rmGeneDelBl,
   char *who2023Bl
){
   char *argStr = 0;
   int iArg = 1;

   while(iArg < lenArgsI)
   { /*Loop: Get user input*/
      argStr = argsStrAry[iArg];

      if(! strcmp("-ref", argStr))
      { /*If: The user provided an reference sequence*/
         *refStr = argsStrAry[iArg + 1];
         ++iArg;
      } /*If: The user provided an reference sequence*/

      else if(! strcmp("-coords", argStr))
      { /*Else If: The user provided an coordinates file*/
         *coordsStr = argsStrAry[iArg + 1];
         ++iArg;
      } /*Else If: The user provided an coordinates file*/

      else if(! strcmp("-all-amrs", argStr))
         *allAmrsBl = 1;

      else if(! strcmp("-res-amrs", argStr))
         *allAmrsBl = 0;

      else if(! strcmp("-no-whole-gene-dels", argStr))
         *rmGeneDelBl = 1;

      else if(! strcmp("-whole-gene-dels", argStr))
         *rmGeneDelBl = 0;
      
      else if(! strcmp("-who-2021-csv", argStr))
      { /*Else If: the user input the  2021 who csv file*/
         *whoFirstTabStr = argsStrAry[iArg + 1];
         *who2023Bl = 0;
         ++iArg;
      } /*Else If: the user input the  2021 who csv file*/

      else if(! strcmp("-tabone-who-2023-tsv", argStr))
      { /*Else If: the user input tab1 of the who 2023*/
         *whoFirstTabStr = argsStrAry[iArg + 1];
         *who2023Bl = 1;
         ++iArg;
      } /*Else If: the user input tab1 of the who 2023*/

      else if(! strcmp("-tabtwo-who-2023-tsv", argStr))
      { /*Else If: the user input tab1 of the who 2023*/
         *whoSecTabStr = argsStrAry[iArg + 1];
         *who2023Bl = 1;
         ++iArg;
      } /*Else If: the user input tab1 of the who 2023*/

      else if(! strcmp("-out", argStr))
      { /*Else If: The user provided an output file*/
         *outStr = argsStrAry[iArg + 1];
         ++iArg;
      } /*Else If: The user provided an output file*/

      else return argStr;

      ++iArg;
   } /*Loop: Get user input*/

   return 0;
} /*cnvtWhoToTbAmrInput*/

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
