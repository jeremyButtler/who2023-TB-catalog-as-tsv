#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "amrStruct.h"
#include "../generalLib/samEntryStruct.h"
#include "genIndiceStruct.h"
#include "cnvt_whoToTbAmr.h"

/*All of these are just .h files*/
#include "../generalLib/ulCpStr.h"
#include "../generalLib/base10StrToNum.h"
#include "../generalLib/dataTypeShortHand.h"

char * cnvtWhoToTbAmrInput(
   int lenArgsI,
   char *argsStrAry[],
   char **whoFirstTabStr,
   char **whoSecTabStr,
   char **samStr,
   char **outStr,
   char *who2023Bl
);

int main(
   int lenArgsI,       /*Number arguments from user*/
   char *argsStrAry[]  /*Arguments from user*/
){ /*Main*/
   char *whoFirstTabStr = 0;
   char *whoSecTabStr = 0;
   char *samStr = 0;
   char *outStr = "-";

   char who2023Bl = 1; /*2023 catalog entries*/

   /*Buffer for reading sam files*/
   char *buffStr = 0;
   ulong lenBuffUL = 0;

   ulong numAmrUL = 0;
   char *drugAryStr = 0;
   int numDrugsI = 0;
   uchar errUC = 0;

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
      \n   -sam: [Required]\
      \n     o A sam file with the reference genes mapped\
      \n       to the reference genome (NC000962). These\
      \n       genes need to be exact matches.\
      \n   -out: [stdout/terminal]\
      \n     o File to output to.\
      \n Output:\
      \n    - Prints converted file to -out\
      ";

   FILE *checkFILE = 0;

   struct amrStruct *amrSTAry = 0;


   buffStr =
      cnvtWhoToTbAmrInput(
         lenArgsI,
         argsStrAry,
         &whoFirstTabStr,
         &whoSecTabStr,
         &samStr,
         &outStr,
         &who2023Bl
      ); /*Get user input*/

   if(buffStr)
   { /*If: I had an error*/
      if(  ! strcmp("-h", buffStr)
         | ! strcmp("--h", buffStr)
         | ! strcmp("-help", buffStr)
         | ! strcmp("--help", buffStr)
         | ! strcmp("help", buffStr)
      ){ /*Pirnt out the help message*/
         fprintf(stdout, "%s\n", helpStr);
         exit(0);
      } /*Pirnt out the help message*/


      if(  ! strcmp("-v", buffStr)
         | ! strcmp("--v", buffStr)
         | ! strcmp("-version", buffStr)
         | ! strcmp("--version", buffStr)
         | ! strcmp("version", buffStr)
      ){ /*Pirnt out the help message*/
         fprintf(stdout, "March 15, 2024\n");
         exit(0);
      } /*Pirnt out the help message*/

      else
         fprintf(stderr, "%s is not valid\n", buffStr);

      exit(-1);
   } /*If: I had an error*/

   if(! whoFirstTabStr)
   { /*If: No first file was provied*/
      fprintf(stderr, "No file one input\n");
      exit(-1);
   } /*If: No first file was provied*/

   checkFILE = fopen(whoFirstTabStr, "r");

   if(! checkFILE)
   { /*If: No first file was provied*/
      fprintf(
         stderr,
         "Could not open %s\n",
          whoFirstTabStr
      );

      exit(-1);
   } /*If: No first file was provied*/

   fclose(checkFILE);
   checkFILE = 0;

   if(who2023Bl)
   { /*If: this is a 2023 catalog entry*/
      if(! whoSecTabStr)
      { /*If: No first file was provied*/
         fprintf(stderr, "No second file input\n");
         exit(-1);
      } /*If: No first file was provied*/

      checkFILE = fopen(whoSecTabStr, "r");

      if(! checkFILE)
      { /*If: No first file was provied*/
         fprintf(
            stderr,
            "Could not open %s\n",
             whoFirstTabStr
         );
   
         exit(-1);
      } /*If: No first file was provied*/

      fclose(checkFILE);
      checkFILE = 0;
   } /*If: this is a 2023 catalog entry*/

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
                &errUC
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

   errUC=(uchar) who_parse_VarID(amrSTAry,(int) numAmrUL);

   if(errUC)
   { /*If: I had a memory error*/
      fprintf(stderr,"Mem Err: processing variant ids\n");
      exit(-1);
   } /*If: I had a memory error*/

   if(samStr != 0) checkFILE = fopen(samStr, "r");

   if(checkFILE)
   { /*If: I have a samfile to process*/
      fclose(checkFILE);
      checkFILE = 0;

      errUC =
         (uchar)
         whoAddCodonPos(
            amrSTAry,
            (int) numAmrUL,
            samStr,
            &buffStr,
            &lenBuffUL
         );

      free(buffStr);
      buffStr = 0;
      lenBuffUL = 0;

      if(errUC)
      { /*If: I had a memory error*/
         fprintf(stderr,"Mem Err: find codon position\n");

         freeAmrStructArray(&amrSTAry, numAmrUL);
         free(drugAryStr);

         exit(-1);
      } /*If: I had a memory error*/
   } /*If: I have a samfile to process*/

   else if(samStr)
   { /*Else If: I had a file error*/
      fprintf(stderr, "Could not open -sam %s\n", samStr);

      freeAmrStructArray(&amrSTAry, numAmrUL);
      free(drugAryStr);

      exit(-1);
   } /*Else If: I had a file error*/

   pAmrDB(amrSTAry,numAmrUL,drugAryStr,numDrugsI,outStr);

   freeAmrStructArray(&amrSTAry, numAmrUL);
   free(drugAryStr);
} /*Main*/


char * cnvtWhoToTbAmrInput(
   int lenArgsI,
   char *argsStrAry[],
   char **whoFirstTabStr,
   char **whoSecTabStr,
   char **samStr,
   char **outStr,
   char *who2023Bl
){
   char *argStr = 0;
   int iArg = 0;

   for(iArg = 1; iArg < lenArgsI; iArg += 2)
   { /*Loop: Get user input*/
      argStr = argsStrAry[iArg];

      if(! strcmp("-sam", argStr))
         *samStr = argsStrAry[iArg + 1];

      else if(! strcmp("-who-2021-csv", argStr))
      { /*Else If: the user input the  2021 who csv file*/
         *whoFirstTabStr = argsStrAry[iArg + 1];
         *who2023Bl = 0;
      } /*Else If: the user input the  2021 who csv file*/

      else if(! strcmp("-tabone-who-2023-tsv", argStr))
      { /*Else If: the user input tab1 of the who 2023*/
         *whoFirstTabStr = argsStrAry[iArg + 1];
         *who2023Bl = 1;
      } /*Else If: the user input tab1 of the who 2023*/

      else if(! strcmp("-tabtwo-who-2023-tsv", argStr))
      { /*Else If: the user input tab1 of the who 2023*/
         *whoSecTabStr = argsStrAry[iArg + 1];
         *who2023Bl = 1;
      } /*Else If: the user input tab1 of the who 2023*/

      else if(! strcmp("-out", argStr))
         *outStr = argsStrAry[iArg + 1];

      else return argStr;
   } /*Loop: Get user input*/

   return 0;
} /*cnvtWhoToTbAmrInput*/
