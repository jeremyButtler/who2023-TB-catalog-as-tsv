#include <time.h>
#include "ulCpStr.h"
#include <stdio.h>
#include <string.h>

int main(
){
   ulong delimUL = defNullDelim;
   char delimC = 0;
   char buffStr [2048];
   char *qryStr = "11ca8a7a-52b9-439b-bcd2-d03d9bb5328b11ca8a7a-52b9-439b-bcd2-d03d9bb5328b11ca8a7a-52b9-439b-bcd2-d03d9bb5328b11ca8a7a-52b9-439b-bcd2-d03d9bb5328b11ca8a7a-52b9-439b-bcd2-d03d9bb5328b11ca8a7a-52b9-439b-bcd2-d03d9bb5328b11ca8a7a-52b9-439b-bcd2-d03d9bb5328b11ca8a7a-52b9-439b-bcd2-d03d9bb5328b11ca8a7a-52b9-439b-bcd2-d03d9bb5328b11ca8a7a-52b9-439b-bcd2-d03d9bb5328b11ca8a7a-52b9-439b-bcd2-d03d9bb5328b11ca8a7a-52b9-439b-bcd2-d03d9bb5328b1";
   char *refStr = "11ca8a7a-52b9-439b-bcd2-d03d9bb5328b11ca8a7a-52b9-439b-bcd2-d03d9bb5328b11ca8a7a-52b9-439b-bcd2-d03d9bb5328b11ca8a7a-52b9-439b-bcd2-d03d9bb5328b11ca8a7a-52b9-439b-bcd2-d03d9bb5328b11ca8a7a-52b9-439b-bcd2-d03d9bb5328b11ca8a7a-52b9-439b-bcd2-d03d9bb5328b11ca8a7a-52b9-439b-bcd2-d03d9bb5328b11ca8a7a-52b9-439b-bcd2-d03d9bb5328b11ca8a7a-52b9-439b-bcd2-d03d9bb5328b11ca8a7a-52b9-439b-bcd2-d03d9bb5328b11ca8a7a-52b9-439b-bcd2-d03d9bb5328b1";
   ulong lenQryUL = ulLenStr(qryStr, delimUL, delimC); 

   uint lenStrUI = 0;
   uint iRep = 0;
   uint numRepI = 1000000;
   uint iTrial = 0;

   float startF = 0;
   float endF = 0;

   nextRound:;

   startF = (float) clock()/CLOCKS_PER_SEC;

   for(iRep = 0; iRep < numRepI; ++iRep)
   { /*Loop: run test*/
       ulCpStrDelim(buffStr, qryStr, delimUL,delimC);
       fprintf(stderr, "%s\n", buffStr); 
   } /*Loop: run test*/

   endF = (float) clock()/CLOCKS_PER_SEC;
   printf("ulCpStr: %f\n", endF - startF);

   startF = (float) clock()/CLOCKS_PER_SEC;
   for(iRep = 0; iRep < numRepI; ++iRep)
   { /*Loop: run test*/
       strcpy(buffStr,qryStr);
       fprintf(stderr, "%s\n", buffStr); 
   } /*Loop: run test*/

   endF = (float) clock()/CLOCKS_PER_SEC;
   printf("strcpy: %f\n", endF - startF);

   ++iTrial;

   if(iTrial < 5) goto nextRound;
} /*main*/
