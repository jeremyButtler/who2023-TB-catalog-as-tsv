#include "cnvt_whoToTbAmr.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "amrStruct.h"
#include "../generalLib/samEntryStruct.h"
#include "genIndiceStruct.h"
#include "drug_str_ary.h"

/*Are just .h files*/
#include "../generalLib/dataTypeShortHand.h"
#include "../generalLib/genMath.h"
#include "../generalLib/codonTbl.h"
#include "../generalLib/ulCpStr.h"
#include "../generalLib/base10StrToNum.h"

/*-------------------------------------------------------\
| Fun-01: amrIsRes
|   - Dectects if an amr was classified as resistant or
|     is unknow/not resitance
| Input:
|   - amrC:
|     - Character with grade assigned by the WHO
| Output:
|   - Returns:
|     o 1 if was a grade 1/2 (resistance)
|     o 0 if was not a grade 1/2
\-------------------------------------------------------*/
#define amrIsRes(amrC)(\
   ((lineStr[uiIter] > 48) & (lineStr[uiIter] < 51))\
)
  /*Logic:
  `  - lineStr[uiIter] < 51
  `    o 1 if is a number < 3 or is a special symbol, or
  `      an invisible character
  '    o limits to special symbols and 0, 1, 2
  '      This should remove NA's, and grade 3, 4, and 5
  '  - lineStr[uiIter] > 48
  `    o 1 if is a number > 0 or a character + extra
  `    o 0 if not 1
  `  - (lineStr[uiIter] >) 48 & (lineStr[uiIter] < 51)
  `    o 1 if number is 1 or 2
  `    o 0 if character is not 1 or 2
  */     

/*-------------------------------------------------------\
| Fun-15: amrSTAddSingleAa
|   - Add a single amino acid variant to an amrStruct
| Input:
|   - amrSTPtr:
|     - Pointer to an amrStruct structure to add the
|       amino acid variant to
|   - refAaC:
|     - Reference amino acid
|   - amrAaC:
|     - Mutant (amr) amino acid
| Output:
|   - Modifies:
|     o refAaStr in amrSTPtr to pointer to a buffer with
|       the refAaC amino acid
|     o lenRefAaUI to be 2 (aa and null)
|     o amrAaStr in amrSTPtr to pointer to a buffer with
|       the amrAaC amino acid
|     o lenAmrAaUI to be 2 (aa and null)
|   - Returns
|     o 0 for success
|     o def_amrST_memError for a memory error
\-------------------------------------------------------*/
#define amrSTAddSingleAa(amrSTPtr, refAaC, amrAaC)({\
   char errC = 0;\
   \
   /*Save the reference amino acid*/\
   (amrSTPtr)->lenRefAaUI = 1;\
   (amrSTPtr)->refAaStr = malloc(2 * sizeof(char));\
   \
   if((amrSTPtr)->refAaStr)\
   { /*If: There were no memory errors*/\
      (amrSTPtr)->refAaStr[0] = (refAaC);\
      (amrSTPtr)->refAaStr[1] = '\0';\
      \
      /*Save the amr amino acid*/\
      (amrSTPtr)->lenAmrAaUI = 1;\
      (amrSTPtr)->amrAaStr = malloc(2 * sizeof(char));\
      \
      if((amrSTPtr)->amrAaStr)\
      { /*If: I had no memory errors*/\
         (amrSTPtr)->amrAaStr[0] = (amrAaC);\
         (amrSTPtr)->amrAaStr[1] = '\0';\
      } /*If: I had no memory errors*/\
      \
      else errC = def_amrST_memError;\
   } /*If: There were no memory errors*/\
   \
   else errC = def_amrST_memError;\
   \
   errC;\
})

/*-------------------------------------------------------\
| Fun-16: read_2021_WhoAmrCsv
|   - Gets the amr data from the Who TB antibiotice
|     resistance 2021 catalog (genome indicie tab saved as
|     a csv).
| Input:
|   - whoCsvStr:
|     o C-string with the path to the csv file for the
|       WHO's genome indience tab of the TB antibiotic
|       catalog spread sheet
|   - numArmUL:
|     o Modified to hold the number of catalog entriex
|   - drugAryStr:
|     o C-string that will hold the name of each drug
|   - numDrugsI:
|     o Will have the number of druges in drugAryStr
| Output:
|   - Modifies:
|     o numArmUL to hold the number of extracted amr
|       mutations.
|     o drugAryStr to hold the name of each antibiotic
|     o numDrugsI to hold the number of drugs in
|       drugAryStr
|   - Returns:
|     o An array of amrStruct's (free with
|       freeAmrStructArray)
\-------------------------------------------------------*/
struct amrStruct * read_2021_WhoAmrCsv(
   char *whoCsvStr,
   ulong *numAmrUL,
   char **drugAryStr,
   int *numDrugsI
){
   ushort lenBuffUS = 2048;
   char buffStr[lenBuffUS]; /*buffer*/
   char *lineStr = buffStr; /*Holds a single line*/
   char *tmpStr = 0;

   uint  maxLineLenUI = 0;     /*max line length*/
   uint  lenLineUI = 0;        /*Length of one line*/
   ulong numLinesUL = 0;
   ulong charReadInUL = 0;

   uint uiIter = 0;        /*Iterator for loops*/
   uint uiColIter = 0;     /*For looping through columns*/
   uint uiAmr = 0;

   uint lenSeqUI = 0;
   uint lenIdUI = 0;

   struct amrStruct *amrST = 0;
   FILE *whoFILE = fopen(whoCsvStr, "r");

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-16 Sec-02:
   ^  - Find the number of lines in the who file
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   if(! whoFILE) return 0;

   /*Read in the first section of the file*/
   charReadInUL =
      fread(lineStr, sizeof(char), lenBuffUS, whoFILE);

   while(charReadInUL)
   { /*Loop: Get the number of lines in the file*/
      lenLineUI = 0;

      for(uiIter = 0; uiIter < charReadInUL; ++uiIter)
      { /*Loop: Count number of new lines & characters*/
         ++lenLineUI;         
         numLinesUL += lineStr[uiIter] == '\n';
      } /*Loop: Count number of new lines & characters*/

      maxLineLenUI = noBranchMax(maxLineLenUI, lenLineUI);

      charReadInUL =
         fread(lineStr,sizeof(char),lenBuffUS,whoFILE);
   } /*Loop: Get the number of lines in the file*/

   if(numLinesUL < 2) return 0;

   --numLinesUL; /*Account for the header*/
   fseek(whoFILE, 0, SEEK_SET);
   maxLineLenUI += 2;

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-16 Sec-03:
   ^  - Set up the antibiotic table
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   *drugAryStr = mallocDrugAryStr(14);

   if(*drugAryStr == 0) return 0;

   *numDrugsI = 14;

   /*The who has changed there format, so this is safe
   ` with hardcoding
   */
   cpDrugToDrugAry(*drugAryStr, "RIF", 0, '\t');
   cpDrugToDrugAry(*drugAryStr, "INH", 1, '\t');
   cpDrugToDrugAry(*drugAryStr, "EMB", 2, '\t');
   cpDrugToDrugAry(*drugAryStr, "PZA", 3, '\t');
   cpDrugToDrugAry(*drugAryStr, "LEV", 4, '\t');
   cpDrugToDrugAry(*drugAryStr, "MXF", 5, '\t');
   cpDrugToDrugAry(*drugAryStr, "BDQ", 6, '\t');
   cpDrugToDrugAry(*drugAryStr, "LZD", 7, '\t');
   cpDrugToDrugAry(*drugAryStr, "CFZ", 8, '\t');
   cpDrugToDrugAry(*drugAryStr, "AMI", 9, '\t');
   cpDrugToDrugAry(*drugAryStr, "STM", 10, '\t');
   cpDrugToDrugAry(*drugAryStr, "ETH", 11, '\t');
   cpDrugToDrugAry(*drugAryStr, "KAN", 12, '\t');
   cpDrugToDrugAry(*drugAryStr, "CAP", 13, '\t');

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-16 Sec-04:
   ^  - Extract the AMR mutations
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   lineStr = malloc(maxLineLenUI * sizeof(char));

   if(lineStr == 0)
   { /*If I failed to make a buffer*/
      free(*drugAryStr);
      *drugAryStr = 0;
      return 0;
   } /*If I failed to make a buffer*/

   amrST = calloc(numLinesUL, sizeof(struct amrStruct));
     /*There is some waste in memory usage, but this
     ` allows me to work with an array
     */

   if(amrST == 0)
   { /*If: I had a memroy error*/
      free(*drugAryStr);
      *drugAryStr = 0;

      free(lineStr);
      lineStr = 0;

      return 0;
   } /*If: I had a memroy error*/

   *numAmrUL = 0;

   /*tmpStr is to avoid complents from ubuntus compiler*/
   tmpStr = fgets(lineStr, maxLineLenUI, whoFILE);

   while(fgets(lineStr, maxLineLenUI, whoFILE))
   { /*Loop: Read in all lines in the WHO's amr file*/

     /***************************************************\
     * Fun-16 Sec-04 Sub-02:
     *   - Find the length of the gene name
     \***************************************************/

     for(uiIter = 0; lineStr[uiIter] != ','; ++uiIter){}
     
     lenIdUI = uiIter; /*+1 for '\0'*/

     amrST[*numAmrUL].geneIdStr =
        malloc((lenIdUI + 1) * sizeof(char));

     if(!amrST[*numAmrUL].geneIdStr) goto variantMemErr;
     
     for(uiIter = 0; lineStr[uiIter] != ','; ++uiIter)
       amrST[*numAmrUL].geneIdStr[uiIter]=lineStr[uiIter];

     amrST[*numAmrUL].geneIdStr[uiIter] = '\0';
     amrST[*numAmrUL].lenGeneIdUI = uiIter;

     ++uiIter;

     /***************************************************\
     * Fun-16 Sec-04 Sub-03:
     *   - Check if it is reverse complement
     \***************************************************/

     for(uiIter=uiIter; lineStr[uiIter] !=','; ++uiIter){}
    
     amrST[*numAmrUL].dirFlag =
       -(lineStr[uiIter -1] =='c') & def_amrST_revCompDir;

     ++uiIter;
     
     /***************************************************\
     * Fun-16 Sec-04 Sub-04:
     *   - Move past the codon position 
     \***************************************************/
     
     /*Get off variant column*/
     while(lineStr[uiIter] != ',') ++uiIter;
     ++uiIter;

     /*Go past the codon number; I will get this stuff
     `   from the sam file. This will avoid errors about
     `   the codon position
     */
     while(lineStr[uiIter] != ',') ++uiIter;
     ++uiIter;

     /***************************************************\
     * Fun-16 Sec-04 Sub-05:
     *   - Get the reference coordinate of the mutation
     \***************************************************/

     tmpStr = &lineStr[uiIter];

     /*Get around multi entries; they used commas*/
     for(tmpStr = tmpStr; *tmpStr < 48; ++tmpStr){}

     tmpStr =
        base10StrToUI(tmpStr, amrST[*numAmrUL].refPosUI);

     /*Convert index 1 to index 0*/
     --(amrST[*numAmrUL].refPosUI);

     /*The next entry is the sequence, so no numbers.
     ` Numbers denote a multi entry
     */
     if(lineStr[uiIter] == '"')
     { /*If: this had multiple positions*/
        for(
           uiIter = 1 + tmpStr - lineStr;
           lineStr[uiIter] != '"';
           ++uiIter
        ){}

        uiIter += 2;
     } /*If: this had multiple positions*/

     else
     { /*Else: This entry only had a single position*/
        for(tmpStr = tmpStr; *tmpStr != ','; ++tmpStr){};
        uiIter = 1 + tmpStr - lineStr;
     } /*Else: This entry only had a single position*/

     /***************************************************\
     * Fun-16 Sec-04 Sub-06:
     *   - Copy the reference sequence
     \***************************************************/

     /*Get the length of the reference and mutant
     ` sequence; My goal is to store both sequences in the
     ` same buffer
     */
     for(
        lenSeqUI = uiIter;
        lineStr[lenSeqUI] != ',';
        ++lenSeqUI
     ){} /*Loop: get length of reference sequence*/

     amrST[*numAmrUL].refSeqStr =
        malloc((lenSeqUI + 1) * sizeof(char));

     if(!amrST[*numAmrUL].refSeqStr) goto variantMemErr;


     while(uiIter < lenSeqUI)
     { /*Loop: Copy the reference sequence*/
       amrST[*numAmrUL].refSeqStr[uiIter]=lineStr[uiIter];
       ++uiIter;
     } /*Loop: Copy the reference sequence*/

     amrST[*numAmrUL].refSeqStr[uiIter] = '\0';
     amrST[*numAmrUL].lenRefSeqUI = uiIter;

     ++uiIter; /*Get off ','*/

     /***************************************************\
     * Fun-16 Sec-04 Sub-07:
     *   - Copy the mutant sequence
     \***************************************************/

     for(
        lenSeqUI = uiIter;
        lineStr[lenSeqUI] != ',';
        ++lenSeqUI
     ){} /*Loop: get length of the mutant sequence*/

     amrST[*numAmrUL].amrSeqStr =
        malloc((lenSeqUI + 1) * sizeof(char));

     if(!amrST[*numAmrUL].amrSeqStr) goto variantMemErr;

     while(uiIter < lenSeqUI)
     { /*Loop: Copy the amrerence sequence*/
       amrST[*numAmrUL].amrSeqStr[uiIter]=lineStr[uiIter];
       ++uiIter;
     } /*Loop: Copy the amrerence sequence*/

     amrST[*numAmrUL].amrSeqStr[uiIter] = '\0';
     amrST[*numAmrUL].lenAmrSeqUI = uiIter;

     ++uiIter; /*Get off ','*/

     /***************************************************\
     * Fun-16 Sec-04 Sub-08:
     *   - Ignore the amino acid entry
     \***************************************************/

     /*I was getting positioning errors with this, so it
     `   is easier to add this in with a sam file
     */
     while(lineStr[uiIter] !=',') ++uiIter;
     ++uiIter; /*Get off the ','*/

     /***************************************************\
     * Fun-16 Sec-04 Sub-09:
     *   - Get to the first antibiotic column
     \***************************************************/

     for(uiIter=uiIter; lineStr[uiIter] !=','; ++uiIter){}
     ++uiIter;

     for(uiIter=uiIter; lineStr[uiIter] !=','; ++uiIter){}
     ++uiIter;

     /***************************************************\
     * Fun-16 Sec-04 Sub-10:
     *   - Get the antibiotics
     \***************************************************/

     for(uiAmr = 0; uiAmr < 15; ++uiAmr)
     { /*Loop: Get the rating of all AMRs*/
        /*Move to the next AMR confidence column*/
        for(uiColIter = 0; uiColIter < 3; ++uiColIter)
        { /*Loop: Move to the genome_index column*/
           for(
              uiIter = uiIter;
              lineStr[uiIter] != ',';
              ++uiIter
            ){}

           ++uiIter;
        } /*Loop: Move to the genome_index column*/

        amrST[*numAmrUL].amrFlagsUL[0] |=
           (amrIsRes(lineStr[uiIter]) << uiAmr);
     } /*Loop: Get the rating of all AMRs*/

     if(amrST[*numAmrUL].amrFlagsUL[0] == 0)
     { /*If: this mutant has no antibiotic resitance*/
        freeAmrStructStack(&amrST[*numAmrUL]);
        continue;
     } /*If: this mutant has no antibiotic resitance*/

     /***************************************************\
     * Fun-16 Sec-04 Sub-11:
     *   - Get the final decided position
     \***************************************************/

     /*Get of the last AMR entry*/
     for(uiIter=uiIter;lineStr[uiIter]!=',';++uiIter){}
     ++uiIter; /*Get of the final coma*/

     /*Get off the reference entry*/
     for(uiIter=uiIter;lineStr[uiIter]!=',';++uiIter){}
     ++uiIter;

     tmpStr =
        base10StrToUI(
           &lineStr[uiIter],
           amrST[*numAmrUL].refPosUI
        );

     /*Convert index 1 to index 0*/
     --(amrST[*numAmrUL].refPosUI);

     for(tmpStr = tmpStr; *tmpStr != ','; ++tmpStr){};
     uiIter = 1 + tmpStr - lineStr;

     /***************************************************\
     * Fun-16 Sec-04 Sub-12:
     *   - Get the final decided reference sequence
     \***************************************************/

     lenSeqUI = 0;

     for(uiIter=uiIter; lineStr[uiIter] !=','; ++uiIter)
     { /*Loop: Copy the final reference decision*/
        amrST[*numAmrUL].refSeqStr[lenSeqUI] =
            lineStr[uiIter];

        ++lenSeqUI;
     } /*Loop: Copy the final reference decision*/
      
     amrST[*numAmrUL].refSeqStr[lenSeqUI] = '\0';
     amrST[*numAmrUL].lenRefSeqUI = lenSeqUI;
     ++uiIter; /*Get off the ','*/

     /***************************************************\
     * Fun-16 Sec-04 Sub-13:
     *   - Get the final decided mutant sequence
     \***************************************************/

     lenSeqUI = 0;

     for(uiIter=uiIter; lineStr[uiIter] !=','; ++uiIter)
     { /*Loop: Copy the final mutant decision*/
        amrST[*numAmrUL].amrSeqStr[lenSeqUI] =
            lineStr[uiIter];

        ++lenSeqUI;
     } /*Loop: Copy the final mutant decision*/

     amrST[*numAmrUL].amrSeqStr[lenSeqUI] = '\0';
     amrST[*numAmrUL].lenAmrSeqUI = lenSeqUI;

     /***************************************************\
     * Fun-16 Sec-04 Sub-14:
     *   - Get the variant id
     \***************************************************/

     for(uiColIter = 0; uiColIter < 6; ++uiColIter)
     { /*Loop: To the variant column*/
        while(lineStr[uiIter] != ',') ++uiIter;
        ++uiIter;
     } /*Loop: To the variant column*/

     if(lineStr[uiIter] == ',' || lineStr[uiIter] < 33)
     { /*If: this is a blank cell*/
       amrST[*numAmrUL].lenVarIdUI = 2;
       amrST[*numAmrUL].varIdStr=malloc(3 * sizeof(char));

       if(!amrST[*numAmrUL].varIdStr){goto variantMemErr;}

       amrST[*numAmrUL].varIdStr[0] = 'N';
       amrST[*numAmrUL].varIdStr[1] = 'A';
       amrST[*numAmrUL].varIdStr[2] = '\0';
     } /*If: this is a blank cell*/

     for(
        uiColIter = uiIter;
        lineStr[uiColIter] != ',';
        ++uiColIter
     ) if(lineStr[uiColIter] == '\n') break;

     uiColIter -= uiIter;
     amrST[*numAmrUL].lenVarIdUI = uiColIter;
     ++uiColIter;

     amrST[*numAmrUL].varIdStr =
        malloc(uiColIter * sizeof(char));

     if(!amrST[*numAmrUL].varIdStr)
     { /*If: I had a memroy error*/
        variantMemErr:;

        free(*drugAryStr);
        *drugAryStr = 0;

        freeAmrStructArray(&amrST, *numAmrUL);
        free(lineStr);

        return 0;
     } /*If: I had a memroy error*/

     for(
        uiColIter = 0;
        uiColIter < amrST[*numAmrUL].lenVarIdUI;
        ++uiColIter
     ) { /*Loop: Copy the variant id*/
        if(lineStr[uiIter] == '\n') break;

        amrST[*numAmrUL].varIdStr[uiColIter] =
           lineStr[uiIter];

        ++uiIter;
     } /*Loop: Copy the variant id*/

     amrST[*numAmrUL].varIdStr[uiColIter] = '\0';

     /***************************************************\
     * Fun-16 Sec-04 Sub-15:
     *   - Set the variant type
     \***************************************************/

     if(  amrST[*numAmrUL].lenAmrSeqUI
        < amrST[*numAmrUL].lenRefSeqUI
     ){ /*If: this was a deletion*/
        amrST[*numAmrUL].mutTypeStr[0] = 'd';
        amrST[*numAmrUL].mutTypeStr[1] = 'e';
        amrST[*numAmrUL].mutTypeStr[2] = 'l';
     } /*If: this was a deletion*/

     else if(
          amrST[*numAmrUL].lenAmrSeqUI
        > amrST[*numAmrUL].lenRefSeqUI
     ){ /*Else If: this was an insertion*/
        amrST[*numAmrUL].mutTypeStr[0] = 'i';
        amrST[*numAmrUL].mutTypeStr[1] = 'n';
        amrST[*numAmrUL].mutTypeStr[2] = 's';
     } /*Else If: this was an insertion*/

     else
     { /*Else: Assuming snp*/
        amrST[*numAmrUL].mutTypeStr[0] = 's';
        amrST[*numAmrUL].mutTypeStr[1] = 'n';
        amrST[*numAmrUL].mutTypeStr[2] = 'p';
     } /*Else: Assuming snp*/

     amrST[*numAmrUL].mutTypeStr[3] = '\0';

     /***************************************************\
     * Fun-16 Sec-04 Sub-16:
     *   - Move to next amr
     \***************************************************/

     ++(*numAmrUL);
     initAmrStruct(&(amrST[*numAmrUL]));
   } /*Loop: Read in all lines in the WHO's amr file*/
  
   sortAmrStructArray(amrST, 0, *numAmrUL - 1);
   free(lineStr);
   return amrST;
} /*read_2021_WhoAmrCsv*/

/*-------------------------------------------------------\
| Fun-17: checkCrossRes
|   - Check if there is cross resitance (2023 catalog)
| Input:
|   - crossStr:
|     o C-string with cross resitance to check
|   - amrSTPtr:
|     o Pionter to amrStruct structure to add corss
|       resitance to
| Output:
|   - Modifies:
|     o crosRefFlagsUL in amrSTPtr to hold the cross
|       resitance results
|     o drugAryStr if needs to be resized
|       - 0/free if had a memory error
|       - not 0/resized if everything went ok
|   - Returns:
|     o 0 for success
|     o 1 for not an amr
|     o def_amrST_memError for memory errors
\-------------------------------------------------------*/
#define checkCrossRes(\
   crossStr,\
   amrSTPtr,\
   drugAryStr,\
   numDrugs,\
   maxDrugs    /*Max number of strings drugAry can hold*/\
)({\
   int iMacShift = 0;\
   char errC = 0;\
   \
   if((numDrugs) >= (maxDrugs))\
   { /*If: I need to resize the array*/\
      reallocDrugAryStr(&(drugAryStr), (maxDrugs) << 1);\
      if(! (drugAryStr)) errC = def_amrST_memError;\
   } /*If: I need to resize the array*/\
   \
   if(errC == def_amrST_memError) ;\
   else if(*(crossStr) < 32) errC = 1;\
   \
   else if(!cStrEql("FQ X-R\t", (crossStr), '\t'))\
   { /*Else If: this is fluoroquine*/\
      iMacShift =\
        findDrug_in_drugAryStr(\
           "fluoroquine",\
           (drugAryStr),\
           (numDrugs) + 1,\
           '\0'\
        );\
      \
      if(iMacShift < 0)\
      { /*If: there is no fluoroquine*/\
         ++(numDrugs);\
         iMacShift = (numDrugs);\
         \
         cpDrugToDrugAry(\
            (drugAryStr),\
            "fluoroquine",\
            (numDrugs),\
            '\0'\
         ); /*Copy the new drug name in*/\
      } /*If: there is no fluoroquine*/\
      \
      (amrSTPtr)->crossResFlagsUL[0] |=\
         (1UL << (iMacShift));\
   } /*Else If: this is fluoroquine*/\
   \
   else if(!cStrEql("BDQ-CFZ X-R\t", (crossStr), '\t'))\
   { /*Else If: there was bedaquiline/clofazimine*/\
      iMacShift =\
        findDrug_in_drugAryStr(\
           "bedaquiline",\
           (drugAryStr),\
           (numDrugs) + 1,\
           '\0'\
        );\
      \
      if(iMacShift < 0)\
      { /*If: there is no bedaquiline*/\
         ++(numDrugs);\
         iMacShift = (numDrugs);\
         \
         cpDrugToDrugAry(\
            (drugAryStr),\
            "bedaquiline",\
            (numDrugs),\
            '\0'\
         ); /*Copy the new drug name in*/\
      } /*If: there is no bedaquiline*/\
      \
      (amrSTPtr)->crossResFlagsUL[0] |=\
         (1UL << (iMacShift));\
      \
      iMacShift =\
        findDrug_in_drugAryStr(\
           "clofazimine",\
           (drugAryStr),\
           (numDrugs + 1),\
           '\0'\
        );\
      \
      if(iMacShift < 0)\
      { /*If: there is no clorfazimine*/\
         ++(numDrugs);\
         iMacShift = (numDrugs);\
         \
         cpDrugToDrugAry(\
            (drugAryStr),\
            "clofazimine",\
            (numDrugs),\
            '\0'\
         ); /*Copy the new drug name in*/\
      } /*If: there is no clorfazimine*/\
      \
      (amrSTPtr)->crossResFlagsUL[0] |=\
         (1UL << (iMacShift));\
   } /*Else If: there was bedaquiline/clorfazimine*/\
   \
   else if(!cStrEql("INH-ETO X-R\t", (crossStr), '\t'))\
   { /*Else If: there was isoniazid-ethionamide*/\
      iMacShift =\
        findDrug_in_drugAryStr(\
           "isoniazid",\
           (drugAryStr),\
           (numDrugs) + 1,\
            '\0'\
        );\
      \
      if(iMacShift < 0)\
      { /*If: there is no isoniazid*/\
         ++(numDrugs);\
         iMacShift = (numDrugs);\
         \
         cpDrugToDrugAry(\
            (drugAryStr),\
            "isoniazid",\
            (numDrugs),\
            '\0'\
         ); /*Copy the new drug name in*/\
      } /*If: there is no isoniazid*/\
      \
      (amrSTPtr)->crossResFlagsUL[0] |=\
         (1UL << (iMacShift));\
      \
      iMacShift =\
        findDrug_in_drugAryStr(\
           "ethionamide",\
           (drugAryStr),\
           (numDrugs) + 1,\
            '\0'\
        );\
      \
      if(iMacShift < 0)\
      { /*If: there is no ethiondamide*/\
         ++(numDrugs);\
         iMacShift = (numDrugs);\
         \
         cpDrugToDrugAry(\
            (drugAryStr),\
            "ethionamide",\
            (numDrugs),\
            '\0'\
         ); /*Copy the new drug name in*/\
      } /*If: there is no ethiondamide*/\
      \
      (amrSTPtr)->crossResFlagsUL[0] |=\
         (1UL << (iMacShift));\
   } /*Else If: there was isoniazid-ethionamide*/\
   \
   else if( !\
      cStrEql(\
         "Confers DLM-PMD cross-resistance\t",\
         (crossStr),\
         '\t'\
      )\
   ){ /*Else If: is delamanid/(penicillin myceial dreg)*/\
      iMacShift =\
        findDrug_in_drugAryStr(\
           "delamanid",\
           (drugAryStr),\
           (numDrugs) + 1,\
           '\0'\
        );\
      \
      if(iMacShift < 0)\
      { /*If: there is no delamind*/\
         ++(numDrugs);\
         iMacShift = (numDrugs);\
         \
         cpDrugToDrugAry(\
            (drugAryStr),\
            "delamanid",\
            (numDrugs),\
            '\0'\
         ); /*Copy the new drug name in*/\
      } /*If: there is no delamind*/\
      \
      (amrSTPtr)->crossResFlagsUL[0] |=\
         (1UL << (iMacShift));\
      \
      iMacShift =\
        findDrug_in_drugAryStr(\
           "penicillinMyceialDreg",\
           (drugAryStr),\
           (numDrugs) + 1,\
            '\0'\
        );\
      \
      if(iMacShift < 0)\
      { /*If: there is no penicillin myceial dreg*/\
         ++(numDrugs);\
         iMacShift = (numDrugs);\
         \
         cpDrugToDrugAry(\
            (drugAryStr),\
            "penicillinMyceialDreg",\
           (numDrugs),\
            '\0'\
         ); /*Copy the new drug name in*/\
      } /*If: there is no penicillin myceial dreg*/\
      \
      (amrSTPtr)->crossResFlagsUL[0] |=\
         (1UL << (iMacShift));\
   } /*Else If: is delamanid/(penicillin myceial dreg)*/\
   \
   errC;\
}) /*checkCrossRes*/

/*-------------------------------------------------------\
| Fun-19: read_2023_WhoAmrTsv
|   - Reads in the two tabs (as separate tsv's) and
|     converts them to an amrStructs array
| Input:
|   - whoMasterStr:
|     o C-string with the path to the tsv file with 
|       tab one (master) of the 2023 who TB catalog
|   - whoIndiceFILE:
|     o C-string with the path to the tsv with tab two
|       (genome indices) of the 2023 who catalog
|   - numAmrUL:
|     o Modifed to hold the number of kept amr genes
|   - drugStr:
|     o Modified to hold the antibiotic names. Each name
|       is at most 31 characters long. To get to the next
|       name add 32.
|   - numDrugsI:
|     o Mdofied to hold the number of drugs in drugStr
|   - errUC:
|     o Holds the error type
| Output:
|   - Modifies:
|     o numAmrUL to hold the number of used amrStructs in
|       the returned amrStruct structure array
|     o drugAryStr to hold the name of each antibiotic
|     o numDrugsI to hold the number of drugs in
|       drugAryStr
|   - Returns:
|     o An array of amrStruct structures that have the
|       resitant (grade 1 and 2) mutations
\-------------------------------------------------------*/
struct amrStruct * read_2023_WhoAmrTsv(
   char *whoMasterStr,  /*path to Master tab tsv*/
   char *whoIndiceStr,  /*path to genome indicie tab*/
   ulong *numAmrUL,     /*Number of amrs kept*/
   char **drugAryStr,   /*Holds antibiotics*/
   int *numDrugsI,      /*Number of drugs in drugAryStr*/
   uchar *errUC         /*Reports errors*/
){ /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
   ' Fun-19 TOC: read_2023_WhoAmrTsv
    '   o fun-19 sec-01:
    '     - Variable declerations
    '   o fun-19 sec-02:
    '     - Get the genome coordinates
    '   o fun-19 sec-03:
    '     - Get the master file length
    '   o fun-19 sec-04:
    '     - Set up the buffers
    '   o fun-19 sec-05:
    '     - Read in the file
    '   o fun-19 sec-06:
    '     - Clean up
   \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
    ^ Fun-19 Sec-01:
    ^   - Variable declerations
    \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    /*The master file is sorted by the drug name, there
    ' are less than 20 drugs, so I should be ok
    */
    char *buffStr = 0;
    uint lenBuffUI = 4096;

    int iTab = 0;
    uint uiPos = 0;

    uint lenFileUI = 0;
    uint lenPosAryUI = 0;
    char *tmpStr = 0;

    int iDrug = 0;         /*Antbiotic on*/
    int drugAryLimitI = 0; /*Number strings in drugAry*/
    char *drugOnStr = 0;     /*Index of drug working on*/

    char cpAmrBl = 0;   /*1: more than 1 indice for amr*/
    int indexI = 0;
    struct genIndice *indiceAryST = 0;

    struct amrStruct *amrST = 0;

    FILE *whoFILE = 0;

    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
    ^ Fun-19 Sec-02:
    ^   - Get the genome coordinates
    \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    indiceAryST =
       read_who2023_indiceTabTsv(
          whoIndiceStr,
          &lenPosAryUI
       ); /*Get the genome coordiantes*/

    if(indiceAryST == 0)
    { /*If: I had a memory error*/
       *errUC = def_amrST_memError;
       return 0; /*Memory error*/
    } /*If: I had a memory error*/

    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
    ^ Fun-19 Sec-03:
    ^   - Get the master file length
    \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    whoFILE = fopen(whoMasterStr, "r");

    if(whoFILE == 0)
    { /*If: I could not open the master file*/
       freeGeneIndiceAry(&indiceAryST, lenPosAryUI);
       *errUC = def_amrST_invalidFILE;
       return 0;
    } /*If: I could not open the master file*/

    buffStr = malloc(lenBuffUI * sizeof(char));

    buffStr[lenBuffUI - 2] = '\0';

    while(fgets(buffStr, lenBuffUI, whoFILE))
    { /*Loop: Read in each line of the file*/

       /*Make sure my buffer can read in the full line*/
       while(buffStr[lenBuffUI - 2] != '\0')
       { /*Loop: Read in the complete line*/
           free(buffStr);
           buffStr=malloc((lenBuffUI <<1) * sizeof(char));

           if(buffStr == 0)
           { /*If: I had a memory error*/
              *errUC = def_amrST_invalidFILE;

              fclose(whoFILE);
              freeGeneIndiceAry(&indiceAryST,lenPosAryUI);

              return 0;
           } /*If: I had a memory error*/

           tmpStr = buffStr + lenBuffUI;
           buffStr[(lenBuffUI << 1) - 2] = '\0';

           fgets(tmpStr, lenBuffUI, whoFILE);
           lenBuffUI <<= 1;
       } /*Loop: Read in the complete line*/

       buffStr[lenBuffUI - 2] = '\0';
       ++lenFileUI;
    } /*Loop: Read in each line of the file*/

    if(lenFileUI < 3)
    { /*If: I had a memory error*/
       fclose(whoFILE);
       free(buffStr);
       freeGeneIndiceAry(&indiceAryST, lenPosAryUI);
       *errUC = def_amrST_memError;
       return 0; /*Memory error*/
    } /*If: I had a memory error*/

    fseek(whoFILE, 0, SEEK_SET); /*Start of file*/

    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
    ^ Fun-19 Sec-04:
    ^   - Set up the buffers
    \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    *numDrugsI = 0;
    *drugAryStr = mallocDrugAryStr(16);

    if(*drugAryStr == 0)
    { /*If: I had a memory error*/
       fclose(whoFILE);
       free(buffStr);

       freeGeneIndiceAry(&indiceAryST, lenPosAryUI);
       *errUC = def_amrST_memError;

       return 0;
    } /*If: I had a memory error*/

    drugAryLimitI = 16;

    /*This will be massively oversized, but it works*/
    amrST = calloc(lenPosAryUI, sizeof(struct amrStruct));

    if(amrST == 0)
    { /*If: I had a memory error*/
       fclose(whoFILE);
       free(buffStr);

       free(*drugAryStr);
       *drugAryStr = 0;

       freeGeneIndiceAry(&indiceAryST, lenPosAryUI);
       *errUC = def_amrST_memError;
       return 0; /*Memory error*/
    } /*If: I had a memory error*/

    *numAmrUL = 0;

    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
    ^ Fun-19 Sec-05:
    ^   - Read in the file
    ^   o fun-19 sec-05 sub-01:
    ^     - get past header, start loop, get antibiotic
    ^   o fun-19 sec-05 sub-02:
    ^     - get the gene name
    ^   o fun-19 sec-05 sub-03:
    ^     - Move past the mutation column
    ^   o fun-19 sec-05 sub-04:
    ^     - Move past the mutation column
    ^   o fun-19 sec-05 sub-05:
    ^     - Move past the mutation column
    ^   o fun-19 sec-05 sub-06:
    ^     - Get the effect column
    ^   o fun-19 sec-05 sub-07:
    ^     - Move past the genome coordinate. I will get
    ^       this form the genIndice array I made at the
    ^       end
    ^   o fun-19 sec-05 sub-08:
    ^     - Move to the next set of targets to extract
    ^   o fun-19 sec-05 sub-09:
    ^     - Check if this provides resitance
    ^   o fun-19 sec-05 sub-10:
    ^     - Get the comment entry
    ^   o fun-19 sec-05 sub-11:
    ^     - Check for cross resistance (additional grade)
    ^   o fun-19 sec-05 sub-12:
    ^     - Deal with genomic coordiantes
    ^   o fun-19 sec-05 sub-13:
    ^     - Move to the next amr entry
    \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    /****************************************************\
    * Fun-19 Sec-05 Sub-01:
    *   - get past header, start loop, get antibiotic
    \****************************************************/

    /*Move past the headers*/
    tmpStr = fgets(buffStr, lenBuffUI, whoFILE);
    tmpStr = fgets(buffStr, lenBuffUI, whoFILE);

    *numDrugsI = 0;
    uiPos = 0;

    goto firstTime;

    while(fgets(buffStr, lenBuffUI, whoFILE))
    { /*Loop: Read in the file*/
       uiPos = 0;

       iDrug = 
          findDrug_in_drugAryStr(
             buffStr,
             *drugAryStr,
             (*numDrugsI + 1),
             '\t'
          );

       if(iDrug < 0)
       { /*If: I am on a new antibiotic*/ 
         ++(*numDrugsI);

         firstTime:;

         if(*numDrugsI >= drugAryLimitI)
         { /*If: I need to resize the drug list*/
            reallocDrugAryStr(drugAryStr, *numDrugsI +10);

            if(!(*drugAryStr)) goto memoryError;

            drugAryLimitI += 10;
         } /*If: I need to resize the drug list*/

         cpDrugToDrugAry(
             *drugAryStr,
              buffStr,
              *numDrugsI,
              '\t'
         ); /*Copy the drug name*/
       } /*If: I am on a new antibiotic*/ 

       amrST[*numAmrUL].amrFlagsUL[0] =
          ((ulong) 1 << (iDrug));

       while(buffStr[uiPos] > 31) ++uiPos;

       ++uiPos; /*Get off the tab*/

       /*************************************************\
       * Fun-19 Sec-05 Sub-02:
       *   - get the gene name
       \*************************************************/

       amrST[*numAmrUL].lenGeneIdUI = uiPos;

       while(buffStr[amrST[*numAmrUL].lenGeneIdUI] !='\t')
          ++(amrST[*numAmrUL].lenGeneIdUI);
       
       amrST[*numAmrUL].lenGeneIdUI -= uiPos;

       amrST[*numAmrUL].geneIdStr =
          malloc(
                (amrST[*numAmrUL].lenGeneIdUI + 1)
              * sizeof(char)
          ); /*Make memory for the gene id*/

       if(!amrST[*numAmrUL].geneIdStr) goto memoryError;
       
       cCpStr(
          amrST[*numAmrUL].geneIdStr,
          &buffStr[uiPos],
          amrST[*numAmrUL].lenGeneIdUI
       );

       uiPos += amrST[*numAmrUL].lenGeneIdUI + 1;

       /*************************************************\
       * Fun-19 Sec-05 Sub-03:
       *   - Move past the mutation column
       \*************************************************/

       while(buffStr[uiPos] != '\t') ++uiPos;
       ++uiPos; /*get off the tab*/

       /*************************************************\
       * Fun-19 Sec-05 Sub-04:
       *   - Read in the variant column
       \*************************************************/

       amrST[*numAmrUL].lenVarIdUI = uiPos;

       while(buffStr[amrST[*numAmrUL].lenVarIdUI] != '\t')
          ++(amrST[*numAmrUL].lenVarIdUI);
       
       amrST[*numAmrUL].lenVarIdUI -= uiPos;

       amrST[*numAmrUL].varIdStr =
          malloc(
                (amrST[*numAmrUL].lenVarIdUI + 1)
              * sizeof(char)
          ); /*Make memory for the var id*/

       if(!amrST[*numAmrUL].varIdStr) goto memoryError;

       cCpStr(
          amrST[*numAmrUL].varIdStr,
          &buffStr[uiPos],
          amrST[*numAmrUL].lenVarIdUI
       );

       uiPos += amrST[*numAmrUL].lenVarIdUI + 1;

       /*************************************************\
       * Fun-19 Sec-05 Sub-05:
       *   - Read past the teir column
       \*************************************************/

       while(buffStr[uiPos] != '\t') ++uiPos;
       ++uiPos; /*get off the tab*/

       /*************************************************\
       * Fun-19 Sec-05 Sub-06:
       *   - Get the effect column
       \*************************************************/

       amrST[*numAmrUL].lenEffectUI = uiPos;

       while(buffStr[amrST[*numAmrUL].lenEffectUI] !='\t')
          ++(amrST[*numAmrUL].lenEffectUI);
       
       amrST[*numAmrUL].lenEffectUI -= uiPos;

       if(amrST[*numAmrUL].lenEffectUI == 0)
          goto skipEffect;

       amrST[*numAmrUL].effectStr =
          malloc(
                (amrST[*numAmrUL].lenEffectUI + 1)
              * sizeof(char)
          ); /*Make memory for the var id*/

       if(!amrST[*numAmrUL].effectStr) goto memoryError;
       
       cCpStr(
          amrST[*numAmrUL].effectStr,
          &buffStr[uiPos],
          amrST[*numAmrUL].lenEffectUI
       );

       /*Convert spaces to dashes*/
       tmpStr = amrST[*numAmrUL].effectStr;

       while(buffStr[uiPos] > 31)
       { /*Loop: Convert spaces to dashes*/
          *tmpStr += (-(*tmpStr == ' ') & 13);
          ++tmpStr;
          ++uiPos;
       } /*Loop: Convert spaces to dashes*/

       ++uiPos; /*Get off the tab*/

       skipEffect:;

       /*************************************************\
       * Fun-19 Sec-05 Sub-07:
       *   - Move past the genome coordinate. I will get
       *     this form the genIndice array I made at the
       *     end
       \*************************************************/

       while(buffStr[uiPos] != '\t') ++uiPos;
       ++uiPos; /*get off the tab*/

       /*************************************************\
       * Fun-19 Sec-05 Sub-08:
       *   - Move to the next set of targets to extract
       \*************************************************/

       for(iTab = 7; iTab < 105; ++iTab)
       { /*Loop: Get past columns I am ignoring*/
          while(buffStr[uiPos] != '\t') ++uiPos;
          ++uiPos; /*Get off the tab*/
       } /*Loop: Get past columns I am ignoring*/

       /*************************************************\
       * Fun-19 Sec-05 Sub-09:
       *   - Check if this provides resitance
       \*************************************************/

       if(buffStr[uiPos] !='1' && buffStr[uiPos] !='2')
       { /*If: their is no antibiotic resitance*/
          freeAmrStructStack(&amrST[*numAmrUL]);
          continue;
       } /*If: their is no antibiotic resitance*/

       /*Store the grade*/
       amrST[*numAmrUL].gradeC = buffStr[uiPos] - 48;

       /*Move to the next entry*/
       while(buffStr[uiPos] != '\t') ++uiPos;
       ++uiPos; /*get off the tab*/

       /*************************************************\
       * Fun-19 Sec-05 Sub-10:
       *   - Get the comment entry
       \*************************************************/

       amrST[*numAmrUL].lenCommentUI = uiPos;

       while(buffStr[amrST[*numAmrUL].lenCommentUI]!='\t')
          ++(amrST[*numAmrUL].lenCommentUI);

       amrST[*numAmrUL].lenCommentUI -= uiPos;
       
       if(amrST[*numAmrUL].lenCommentUI < 1)
          goto skipComment;


       if( !
          cStrEql(
            "Confers DLM-PMD cross-resistance\t",
            &buffStr[uiPos],
            '\t'
          ) /*See if this is a cross resistance comment*/
       ){/*If: this is an antibiotic note*/
          checkCrossRes(
             &buffStr[uiPos],
             &amrST[*numAmrUL],
             *drugAryStr,
             (*numDrugsI),
             drugAryLimitI
          );

          if(! *drugAryStr) goto  memoryError;
       }/*If: this is an antibiotic note*/

       amrST[*numAmrUL].commentStr =
          malloc(
                (amrST[*numAmrUL].lenCommentUI + 3)
              * sizeof(char)
          ); /*Make memory for the var id*/

       if(!amrST[*numAmrUL].commentStr) goto memoryError;

       ulCpStr(
          amrST[*numAmrUL].commentStr,
          &buffStr[uiPos],
          amrST[*numAmrUL].lenCommentUI
       );

       /*Convert spaces to dashes*/
       tmpStr = amrST[*numAmrUL].commentStr;

       while(buffStr[uiPos] > 31)
       { /*Loop: Convert spaces (32) to dashes*/
          *tmpStr += (-(*tmpStr == 32) & 13);
          ++tmpStr;
          ++uiPos;
       } /*Loop: Convert spaces (32) to dashes*/

       tmpStr = amrST[*numAmrUL].commentStr;

       /*Process the possible elements in the comment*/
       if(strstr(tmpStr, "High-level-resistance"))
          amrST[*numAmrUL].highResBl = 1;

       if(strstr(tmpStr, "Low-level-resistance"))
          amrST[*numAmrUL].lowResBl = 1;

       if(strstr(
            tmpStr,
            "multiple,-genetically-linked-low-level-resistance-mutations-are-additive-and-confer-high-level-resistance"
          ) /*Check if combine to get higher resistance*/
       ) amrST[*numAmrUL].additiveResBl = 1;

       tmpStr =
          strstr(
             tmpStr,
             "Can-only-confer-resistance-if-genetically-linked-to-a-functional"
       ); /*Needs gene X; 2023 catalog, only 1 gene*/

       if(tmpStr)
       { /*If: there is a gene dependency*/
          tmpStr += 65;

          if(*tmpStr < 32) goto noNeededGene;

          amrST[*numAmrUL].lenNeedsGeneUI =
             cLenStr(tmpStr, '\0');

          amrST[*numAmrUL].needsGeneStr =
             malloc(
                  (amrST[*numAmrUL].lenNeedsGeneUI + 1)
                * sizeof(char)
             ); /*Allocate memory for copying*/

          if(! amrST[*numAmrUL].needsGeneStr)
             goto memoryError;

          cCpStr(
             amrST[*numAmrUL].needsGeneStr,
             tmpStr,
             amrST[*numAmrUL].lenNeedsGeneUI
          ); /*Copy the gene/genes*/
       } /*If: there is a gene dependency*/

       else
       { /*Else: there is no gene entry*/
          noNeededGene:;

          amrST[*numAmrUL].lenNeedsGeneUI = 2;

          amrST[*numAmrUL].needsGeneStr =
             malloc(3 * sizeof(char));

          if(! amrST[*numAmrUL].needsGeneStr)
             goto memoryError;
         
          amrST[*numAmrUL].needsGeneStr[0] = 'N';
          amrST[*numAmrUL].needsGeneStr[1] = 'A';
          amrST[*numAmrUL].needsGeneStr[2] = '\0';
       } /*Else: there is no gene entry*/


       ++uiPos; /*Get off the tab*/

       skipComment:;

       /*************************************************\
       * Fun-19 Sec-05 Sun-11:
       *   - Check for cross resistance (additional grade)
       \*************************************************/

       for(iTab = 0; iTab < 4; ++iTab)
       { /*Loop: Get past columns I am ignoring*/
          while(buffStr[uiPos] != '\t') ++uiPos;
          ++uiPos; /*Get off the tab*/
       } /*Loop: Get past columns I am ignoring*/

       checkCrossRes(
          &buffStr[uiPos],
          &amrST[*numAmrUL],
          *drugAryStr,
          (*numDrugsI),
          drugAryLimitI
       );

       if(! *drugAryStr) goto  memoryError;

       /*************************************************\
       * Fun-19 Sec-05 Sun-12:
       *   - Deal with genomic coordiantes
       *   o fun-19 sec-05 sun-12 cat-01:
       *     - Find the matching gene indice(s)
       *   o fun-19 sec-05 sun-12 cat-02:
       *     - Check if I have another amr/start loop
       *   o fun-19 sec-05 sun-12 cat-03:
       *     - In next amr, copy the gene id
       *   o fun-19 sec-05 sun-12 cat-04:
       *     - In next amr, copy variant id
       *   o fun-19 sec-05 sun-12 cat-05:
       *     - In next amr, copy effect entry
       *   o fun-19 sec-05 sun-12 cat-06:
       *     - In next amr, copy the comment entry
       *   o fun-19 sec-05 sun-12 cat-07:
       *     - Get reference postion
       *   o fun-19 sec-05 sun-12 cat-08:
       *     - Copy the reference sequence
       *   o fun-19 sec-05 sun-12 cat-09:
       *     - Copy the amr sequence
       *   o fun-19 sec-05 sun-12 cat-10:
       *     - Check the amr mutation type
       *   o fun-19 sec-05 sun-12 cat-11:
       *     - Move to the next gene indice/dup amr
       \*************************************************/

       /*++++++++++++++++++++++++++++++++++++++++++++++++\
       + Fun-19 Sec-05 Sun-12 Cat-01:
       +   - Find the matching gene indice(s)
       \++++++++++++++++++++++++++++++++++++++++++++++++*/

       indexI =
          findGenIndiceVariant(
             indiceAryST,
             amrST[*numAmrUL].varIdStr,
             (int) lenPosAryUI
          ); /*Find the variant's gene indice*/

       if(indexI < 0)
       { /*If: I could not find the indice*/
          fprintf(
              stderr,
              "%s not found in indice sheet\n",
              amrST[*numAmrUL].varIdStr
          ); /*Let the user know it is missing*/

          freeAmrStructStack(&amrST[*numAmrUL]);
          continue;
       } /*If: I could not find the indice*/

       /*++++++++++++++++++++++++++++++++++++++++++++++++\
       + Fun-19 Sec-05 Sun-12 Cat-02:
       +   - Check if I have another amr/start loop
       \++++++++++++++++++++++++++++++++++++++++++++++++*/

      cpAmrBl = 1;

       while(cpAmrBl)
       { /*Loop: Copy needed coordiantes*/
          cpAmrBl = !
             cStrEql(
                indiceAryST[indexI + 1].varIdStr,
                amrST[*numAmrUL].varIdStr,
                '\0'
             ); /*Compare strings*/

          if(cpAmrBl)
          { /*If: I have multiple amrs for this entry*/
             initAmrStruct(&amrST[*numAmrUL + 1]);

             amrST[*numAmrUL + 1].crossResFlagsUL[0] = 
                amrST[*numAmrUL].crossResFlagsUL[0];

             amrST[*numAmrUL + 1].amrFlagsUL[0] = 
                amrST[*numAmrUL].amrFlagsUL[0];

             /*++++++++++++++++++++++++++++++++++++++++++\
             + Fun-19 Sec-05 Sun-12 Cat-03:
             +   - In next amr, copy the gene id
             \++++++++++++++++++++++++++++++++++++++++++*/

             amrST[*numAmrUL + 1].geneIdStr = 
               malloc(
                   (amrST[*numAmrUL].lenGeneIdUI + 1)
                 * sizeof(char)
               ); /*allocate memory for the sequence*/

             if(!amrST[*numAmrUL + 1].geneIdStr)
                goto memoryError;

             amrST[*numAmrUL + 1].lenGeneIdUI = 
                amrST[*numAmrUL].lenGeneIdUI;

             cCpStr(
                amrST[*numAmrUL + 1].geneIdStr,
                amrST[*numAmrUL].geneIdStr,
                amrST[*numAmrUL].lenGeneIdUI
             ); /*Copy the reference sequence*/

             /*++++++++++++++++++++++++++++++++++++++++++\
             + Fun-19 Sec-05 Sun-12 Cat-04:
             +   - In next amr, copy variant id
             \++++++++++++++++++++++++++++++++++++++++++*/

             amrST[*numAmrUL + 1].varIdStr = 
               malloc(
                   (amrST[*numAmrUL].lenVarIdUI + 1)
                 * sizeof(char)
               ); /*allocate memory for the sequence*/

             if(!amrST[*numAmrUL + 1].varIdStr)
                goto memoryError;

             amrST[*numAmrUL + 1].lenVarIdUI = 
                amrST[*numAmrUL].lenVarIdUI;

             cCpStr(
                amrST[*numAmrUL + 1].varIdStr,
                amrST[*numAmrUL].varIdStr,
                amrST[*numAmrUL].lenVarIdUI
             ); /*Copy the reference sequence*/

             /*++++++++++++++++++++++++++++++++++++++++++\
             + Fun-19 Sec-05 Sun-12 Cat-05:
             +   - In next amr, copy effect entry
             \++++++++++++++++++++++++++++++++++++++++++*/

             amrST[*numAmrUL + 1].effectStr = 
               malloc(
                   (amrST[*numAmrUL].lenEffectUI + 1)
                 * sizeof(char)
               ); /*allocate memory for the sequence*/

             if(!amrST[*numAmrUL + 1].effectStr)
                goto memoryError;

             amrST[*numAmrUL + 1].lenEffectUI = 
                amrST[*numAmrUL].lenEffectUI;

             cCpStr(
                amrST[*numAmrUL + 1].effectStr,
                amrST[*numAmrUL].effectStr,
                amrST[*numAmrUL].lenEffectUI
             ); /*Copy the reference sequence*/

             /*++++++++++++++++++++++++++++++++++++++++++\
             + Fun-19 Sec-05 Sun-12 Cat-06:
             +   - In next amr, copy the comment entry
             \++++++++++++++++++++++++++++++++++++++++++*/

             amrST[*numAmrUL + 1].lenCommentUI = 
                amrST[*numAmrUL].lenCommentUI;

             amrST[*numAmrUL + 1].commentStr = 
               malloc(
                   (amrST[*numAmrUL].lenCommentUI + 1)
                 * sizeof(char)
               ); /*allocate memory for the sequence*/

             if(!amrST[*numAmrUL + 1].commentStr)
                goto memoryError;

             cCpStr(
                amrST[*numAmrUL + 1].commentStr,
                amrST[*numAmrUL].commentStr,
                amrST[*numAmrUL].lenCommentUI
             ); /*Copy the reference sequence*/

             /*Copy the booleans for the comment*/
             amrST[*numAmrUL + 1].highResBl = 
                amrST[*numAmrUL].highResBl;

             amrST[*numAmrUL + 1].lowResBl = 
                amrST[*numAmrUL].lowResBl;

             amrST[*numAmrUL + 1].additiveResBl = 
                amrST[*numAmrUL].additiveResBl;

             /*Deal with the needed gene entry*/
             amrST[*numAmrUL + 1].lenNeedsGeneUI = 
                amrST[*numAmrUL].lenNeedsGeneUI;

             amrST[*numAmrUL + 1].needsGeneStr = 
               malloc(
                   (amrST[*numAmrUL].lenNeedsGeneUI + 1)
                 * sizeof(char)
               ); /*allocate memory for the sequence*/

             if(!amrST[*numAmrUL + 1].needsGeneStr)
                goto memoryError;

             cCpStr(
                amrST[*numAmrUL + 1].needsGeneStr,
                amrST[*numAmrUL].needsGeneStr,
                amrST[*numAmrUL].lenNeedsGeneUI
             ); /*Copy the reference sequence*/
          } /*If: I have multiple amrs for this entry*/

          /*+++++++++++++++++++++++++++++++++++++++++++++\
          + Fun-19 Sec-05 Sun-12 Cat-07:
          +   - Copy the reference position
          \+++++++++++++++++++++++++++++++++++++++++++++*/

           amrST[*numAmrUL].refPosUI =
             indiceAryST[indexI].posUI;

          /*+++++++++++++++++++++++++++++++++++++++++++++\
          + Fun-19 Sec-05 Sun-12 Cat-08:
          +   - Copy the reference sequence
          \+++++++++++++++++++++++++++++++++++++++++++++*/

          amrST[*numAmrUL].refSeqStr = 
            malloc(
                (indiceAryST[indexI].lenRefSeqUI + 1)
              * sizeof(char)
            ); /*allocate memory for the sequence*/

          if(!amrST[*numAmrUL].refSeqStr)
             goto memoryError;

          amrST[*numAmrUL].lenRefSeqUI = 
             indiceAryST[indexI].lenRefSeqUI;

          cCpStr(
             amrST[*numAmrUL].refSeqStr,
             indiceAryST[indexI].refSeqStr,
             amrST[*numAmrUL].lenRefSeqUI
          ); /*Copy the reference sequence*/

          /*+++++++++++++++++++++++++++++++++++++++++++++\
          + Fun-19 Sec-05 Sun-12 Cat-09:
          +   - Copy the amr sequence
          \+++++++++++++++++++++++++++++++++++++++++++++*/

          amrST[*numAmrUL].amrSeqStr = 
            malloc(
                (indiceAryST[indexI].lenAmrSeqUI + 1)
              * sizeof(char)
            ); /*allocate memory for the sequence*/

          if(!amrST[*numAmrUL].amrSeqStr)
             goto memoryError;

          amrST[*numAmrUL].lenAmrSeqUI = 
             indiceAryST[indexI].lenAmrSeqUI;

          cCpStr(
             amrST[*numAmrUL].amrSeqStr,
             indiceAryST[indexI].amrSeqStr,
             amrST[*numAmrUL].lenAmrSeqUI
          ); /*Copy the amrerence sequence*/

          /*+++++++++++++++++++++++++++++++++++++++++++++\
          + Fun-19 Sec-05 Sun-12 Cat-10:
          +   - Check the amr mutation type
          \+++++++++++++++++++++++++++++++++++++++++++++*/

          uiPos = amrST[*numAmrUL].lenVarIdUI;

          if(     (amrST[*numAmrUL].varIdStr[uiPos-1]&~32)
                == 'L'
             &&
                  (amrST[*numAmrUL].varIdStr[uiPos-2]&~32)
                == 'O'
             &&
                  (amrST[*numAmrUL].varIdStr[uiPos-3]&~32)
                == 'F'
          ){ /*If: this was a loss of function mutation*/
             amrST[*numAmrUL].mutTypeStr[0] = 'l';
             amrST[*numAmrUL].mutTypeStr[1] = 'o';
             amrST[*numAmrUL].mutTypeStr[2] = 'f';
          } /*If: this was a loss of function mutation*/

          else if(
               amrST[*numAmrUL].lenRefSeqUI
             > amrST[*numAmrUL].lenAmrSeqUI
          ){ /*Else If: I have a deletion*/
             amrST[*numAmrUL].mutTypeStr[0] = 'd';
             amrST[*numAmrUL].mutTypeStr[1] = 'e';
             amrST[*numAmrUL].mutTypeStr[2] = 'l';
          } /*Else If: I have a deletion*/

          else if(
               amrST[*numAmrUL].lenRefSeqUI
             < amrST[*numAmrUL].lenAmrSeqUI
          ){ /*Else If: I have an insertion*/
             amrST[*numAmrUL].mutTypeStr[0] = 'i';
             amrST[*numAmrUL].mutTypeStr[1] = 'n';
             amrST[*numAmrUL].mutTypeStr[2] = 's';
          } /*Else If: I have a deletion*/

          else
          { /*Else: I have an snp*/
             amrST[*numAmrUL].mutTypeStr[0] = 's';
             amrST[*numAmrUL].mutTypeStr[1] = 'n';
             amrST[*numAmrUL].mutTypeStr[2] = 'p';
          } /*Else: I have an snp*/

          amrST[*numAmrUL].mutTypeStr[3] = '\0';

          /*+++++++++++++++++++++++++++++++++++++++++++++\
          + Fun-19 Sec-05 Sun-12 Cat-11:
          +   - Move to the next gene indice/dup amr
          \+++++++++++++++++++++++++++++++++++++++++++++*/

          *numAmrUL += cpAmrBl;
          ++indexI;
       } /*Loop: Copy needed coordiantes*/

       /*************************************************\
       * Fun-19 Sec-05 Sun-13:
       *   - Move to the next amr entry
       \*************************************************/

       ++(*numAmrUL);
    } /*Loop: Read in the file*/
    
    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
    ^ Fun-19 Sec-06:
    ^   - Clean up
    \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    goto noError;

    memoryError:;

    fclose(whoFILE);
    free(buffStr);

    *errUC = def_amrST_memError;
    freeGeneIndiceAry(&indiceAryST, lenPosAryUI);
    freeAmrStructArray(&amrST, *numAmrUL);

    if(*drugAryStr) free(*drugAryStr);
    *drugAryStr = 0;

    return 0;

    noError:;

    fclose(whoFILE);
    sortAmrStructArray(amrST, 0, *numAmrUL - 1);
    freeGeneIndiceAry(&indiceAryST, lenPosAryUI);

    ++(*numDrugsI); /*Convert to index 1*/

    return amrST;
} /*read_2023_WhoAmrTsv*/

/*-------------------------------------------------------\
| Fun-20: who2023ParsVar
|   - Parse the variant idea from the WHO 2023 TB
|     catalog to update amino acid mutations.
| Input:
|   - amrST:
|     - Pointer to an array of amrStruct structures to
|       update and process variants for
|   - numAmrI:
|     - Number of amrStructs in amrST
| Output:
|   - Modifies:
|     o each structure in amrST with amino acid mutations
|       to have the amino acid sequence whe possible. For
|       large deletions/duplications you will need to call
|       whoAddCodonPos
|   - Returns
|     o 0 for success
|     o def_amrST_memError for a memory error
\-------------------------------------------------------*/
char who_parse_VarID(
   struct amrStruct *amrST,/*Has amr variants to update*/
   int numAmrI          /*Number of amrs*/
){ /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
   ' Fun-20 TOC: who2023ParsVar
   '   - Parse the variant idea from the WHO 2023 TB
   '     catalog to update amino acid mutations.
   '   o fun-20 sec-01:
   '     - Variable declerations
   '   o fun-20 sec-02:
   '     - Process the variant id for amino acid entries
   '   o fun-20 sec-03:
   '     - clean up and exit
   \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-20 Sec-01: who2023ParsVar
   ^   - Variable declerations
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   char firstAaC = 0;
   char secAaC = 0;
   char *varStr = 0;
   char *cpStr = 0;
   int iIndex = 0;

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-20 Sec-02: who2023ParsVar
   ^   - Process the variant id for amino acid entries
   ^   o fun-20 sec-02 sub-01:
   ^     - Start loop (all AMR(s)) and check if I have an
   ^       amino acid variant to process
   ^   o fun-20 sec-02 sub-02:
   ^     - Get the first amino acid from the variant id
   ^   o fun-20 sec-02 sub-03:
   ^     - Get the codon number of the first amio acid
   ^   o fun-20 sec-02 sub-04:
   ^     - Get the second amino acid/mutation type for
   ^       variants that are not deletions/insertions
   ^   o fun-20 sec-02 sub-05:
   ^     - Handle deletions/insertions
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   /*****************************************************\
   * Fun-20 Sec-02 Sub-01: who2023ParsVar
   *   - Start loop (all AMR(s)) and check if I have an
   *     amino acid variant to process
   \*****************************************************/

   for(iIndex = 0; iIndex < numAmrI; ++iIndex)
   { /*Loop: Process each variant*/
     varStr = amrST[iIndex].varIdStr;

     if(varStr[1] != '.')
     { /*If: this includes the gene name (2023 catalog)*/
        while((*varStr !='.') & (*varStr > 32)) ++varStr;
        --varStr;
     } /*If: this includes the gene name (2023 catalog)*/

     if(varStr[0] != '.' || (varStr[1] & ~32) != 'P')
     { /*If: this is not a 2023 catalog entry*/
        if((varStr[0] & ~32) != 'P' || varStr[1] != '.')
           continue; /*This not an aa mutation*/
     } /*If: this is not a 2023 catalog entry*/

     varStr += 2;
        /*+2 is to get off the '.p'*/
     
     /***************************************************\
     * Fun-20 Sec-02 Sub-02: who2023ParsVar
     *   - Get the first amino acid from the variant id
     \***************************************************/

     if(*varStr == '*')
     { /*If: This is a stop codon*/
        firstAaC = '*';
        ++varStr;
     } /*If: This is a stop codon*/

     else if(
           (varStr[0] & ~32) == 'T'
        && (varStr[1] & ~32) == 'E'
        && (varStr[2] & ~32) == 'R'
     ){ /*If: this is a stop codon*/
        firstAaC = '*';
        varStr += 3;
     } /*If this is a stop codn*/

     else 
     { /*Else: This is not a stop codon*/
        firstAaC = aaThreeLetterToChar(varStr);
        varStr += 3;
     } /*Else: This is not a stop codon*/

     /***************************************************\
     * Fun-20 Sec-02 Sub-03: who2023ParsVar
     *   - Get the codon number of the first amio acid
     \***************************************************/

     varStr =
        base10StrToUI(varStr, amrST[iIndex].codonNumUI);

     /***************************************************\
     * Fun-20 Sec-02 Sub-04: who2023ParsVar
     *   - Get the second amino acid/mutation type for
     *     variants that are not deletions/insertions
     \***************************************************/

     if(varStr[0] == '?')
     { /*If: this could be any aa change*/
        secAaC = '?';

        if(amrSTAddSingleAa(&amrST[iIndex],firstAaC,'?'))
           return def_amrST_memError;

        continue;
     } /*If: this could be any aa change*/
     
     else if(varStr[0] == '*')
     { /*If: this could be any aa change*/
        secAaC = '*';

        if(amrSTAddSingleAa(&amrST[iIndex],firstAaC,'*'))
           return def_amrST_memError;

        continue;
     } /*If: this could be any aa change*/

     else if(
           (varStr[0] & ~32) == 'F'
        && (varStr[1] & ~32) == 'S' 
     ){ /*Else If: this was a frame shift*/
        amrST[iIndex].frameshiftBl = 1;

        amrST[iIndex].lenRefAaUI = 1;
        amrST[iIndex].refAaStr = malloc(2* sizeof(char));

        if(!amrST[iIndex].refAaStr)
           return def_amrST_memError;

        amrST[iIndex].refAaStr[0] = firstAaC;
        amrST[iIndex].refAaStr[1] = '\0';

        /*No amr amino acid, because it is a frame shift*/
        continue;
     } /*Else If: this was a frame shift*/

     else if(
           (varStr[0] & ~32) == 'D'
        && (varStr[1] & ~32) == 'E'
        && (varStr[2] & ~32) == 'L'
     ) goto parseDel;

     else if(
           (varStr[0] & ~32) == 'D'
        && (varStr[1] & ~32) == 'U'
        && (varStr[2] & ~32) == 'P'
     ){ /*Else If: This is a one aa duplication*/
        amrST[iIndex].lenRefAaUI = 1;

        amrST[iIndex].refAaStr = malloc(2*sizeof(char *));

        if(!amrST[iIndex].refAaStr)
           return def_amrST_memError;

        amrST[iIndex].refAaStr[0] = firstAaC;
        amrST[iIndex].refAaStr[1] = '\0';

        amrST[iIndex].lenAmrAaUI = 2;

        amrST[iIndex].amrAaStr = malloc(3*sizeof(char *));

        if(!amrST[iIndex].amrAaStr)
           return def_amrST_memError;

        amrST[iIndex].amrAaStr[0] = firstAaC;
        amrST[iIndex].amrAaStr[1] = firstAaC;
        amrST[iIndex].amrAaStr[2] = '\0';
        continue;
     } /*Else If: This is a one aa duplication*/

     else if(varStr[0] != '_')
     { /*Else If: for 2023 this is a single change*/
        secAaC = aaThreeLetterToChar(varStr);
        varStr += 3;
 
        /*Check if this is a lost stop codon*/
        if(!cStrEql("ext*?\t", varStr, '\t'))
        { /*If: this is a stop codon change*/
         if(amrSTAddSingleAa(&amrST[iIndex],firstAaC,'?'))
            return def_amrST_memError;
        } /*If: this is a stop codon change*/

        else
           if(amrSTAddSingleAa(
                &amrST[iIndex],
                firstAaC,
                secAaC
           )) return def_amrST_memError;
        /*Else; this is a change in an aa*/

        continue;
     } /*Else If: for 2023 this is a single change*/

     /***************************************************\
     * Fun-20 Sec-02 Sub-05: who2023ParsVar
     *   - Handle deletions/insertions
     *   o fun-20 sec-02 sub-05 cat-01:
     *     - Find the amino acid base at the end of the
     *       deletion/insertion
     *   o fun-20 sec-02 sub-05 cat-02:
     *     - Find the position of the last amino acid
     *     - insertion only case; add start/end aa to ref
     *   o fun-20 sec-02 sub-05 cat-03:
     *     - ins case; add start/end aa to amrSeqStr
     *   o fun-20 sec-02 sub-05 cat-04:
     *     - ins case; Add in the inserted aa sequence
     *   o fun-20 sec-02 sub-05 cat-05:
     *     - Deletion;  add first and last aa to refAaStr
     *   o fun-20 sec-02 sub-05 cat-06:
     *     - Del only; add mutate aa sequence to amrAaStr
     *   o fun-20 sec-02 sub-05 cat-07:
     *     - Del + ins; Use insertion step to update amino
     *       acid sequence in amrAaStr
     *       (goto fun-20 sec-02 sub-05 cat-04)
     \***************************************************/

     /*++++++++++++++++++++++++++++++++++++++++++++++++++\
     + Fun-20 Sec-02 Sub-05 Cat-01: who2023ParsVar
     +   - Find the amino acid base at the end of the
     +     deletion/insertion
     \++++++++++++++++++++++++++++++++++++++++++++++++++*/

     /*Else: this is an indel*/
     ++varStr; /*Get off the '_'*/
     
     /*Not likely to happen, but want to make sure*/
     if(varStr[0] == '*' || varStr[0] == '?')
     { /*If: this could be any aa change*/
        secAaC = varStr[0];
        ++varStr;
     } /*If: this could be any aa change*/

     else
     { /*Else: this has a three letter code*/
        secAaC = aaThreeLetterToChar(varStr);
        varStr += 3;
     } /*Else: this has a three letter code*/

     /*++++++++++++++++++++++++++++++++++++++++++++++++++\
     + Fun-20 Sec-02 Sub-05 Cat-02: who2023ParsVar
     +   - Find the position of the last amino acid
     \++++++++++++++++++++++++++++++++++++++++++++++++++*/

     /*Last codon in deletion/insertion*/
     varStr =
       base10StrToUI(varStr,amrST[iIndex].endCodonNumUI);

     /*++++++++++++++++++++++++++++++++++++++++++++++++++\
     + Fun-20 Sec-02 Sub-05 Cat-03: who2023ParsVar
     +   - insertion only case; add start/end aa to ref
     \++++++++++++++++++++++++++++++++++++++++++++++++++*/

     if(   (varStr[0] & ~32) == 'I'
        && (varStr[1] & ~32) == 'N'
        && (varStr[2] & ~32) == 'S'
     ){ /*If this was a insertion*/
        varStr += 3; /*get off ins*/

        /*Copy the reference amino acids*/
        amrST[iIndex].lenRefAaUI = 2;

        amrST[iIndex].refAaStr =
           malloc(3 * sizeof(char *));

        if(!amrST[iIndex].refAaStr)
           return def_amrST_memError;

        amrST[iIndex].refAaStr[0] = firstAaC;
        amrST[iIndex].refAaStr[1] = secAaC;
        amrST[iIndex].refAaStr[2] = '\0';

        /*Copy the amr amino acid sequence. This will
        ` be an over estimate, but it works
        */

        /*+++++++++++++++++++++++++++++++++++++++++++++++\
        + Fun-20 Sec-02 Sub-05 Cat-04: who2023ParsVar
        +   - ins case; add start/end aa to amrAaStr
        \+++++++++++++++++++++++++++++++++++++++++++++++*/

        cpStr = varStr;

        while(*cpStr != '\0')
        { /*Loop: Get the string length*/
           /*Check if they used a single letter or multi*/
           if(*cpStr == '*' || *cpStr == '?') ++cpStr;
           else cpStr += 3;

           ++amrST[iIndex].lenAmrAaUI;
        } /*Loop: Get the string length*/

        amrST[iIndex].lenAmrAaUI += 2;

        amrST[iIndex].amrAaStr =
           malloc(
            (amrST[iIndex].lenAmrAaUI+1) * sizeof(char *)
           );

        if(!amrST[iIndex].amrAaStr)
           return def_amrST_memError;

        /*Add start/end amino acids to amr aa sequence*/
        amrST[iIndex].amrAaStr[0] = firstAaC;

        amrST[iIndex].amrAaStr[
           amrST[iIndex].lenAmrAaUI - 1
        ] = secAaC;

        amrST[iIndex].amrAaStr[
           amrST[iIndex].lenAmrAaUI
        ] = '\0';

        /*+++++++++++++++++++++++++++++++++++++++++++++++\
        + Fun-20 Sec-02 Sub-05 Cat-05: who2023ParsVar
        +   - ins case; Add in the inserted aa sequence
        \+++++++++++++++++++++++++++++++++++++++++++++++*/

        cpStr = &amrST[iIndex].amrAaStr[1];

        while(*varStr != '\0')
        { /*Copy the aa from varStr*/
           if(*varStr == '*' || *varStr == '?')
              *cpStr++ = *varStr++;
           else
           { /*Else: This is a three letter codon*/
              *cpStr++ = aaThreeLetterToChar(varStr);
              varStr += 3;
           } /*Else: This is a three letter codon*/
        } /*Copy the aa from varStr*/

        continue;
     } /*If this was a insertion*/

     /***************************************************\
     * Fun-20 Sec-02 Sub-06: who2023ParsVar
     *   - Handle large (> 1 aa) duplications
     \***************************************************/

     if(   (varStr[0] & ~32) == 'D'
        && (varStr[1] & ~32) == 'U'
        && (varStr[2] & ~32) == 'P'
     ){ /*If: this was an large (over 1) duplication*/
        amrST[iIndex].aaMultiDupBl = 1;

        amrST[iIndex].lenRefAaUI = 
             amrST[iIndex].endCodonNumUI
           - amrST[iIndex].codonNumUI
           + 1; /*+1 to account for first aa*/

        amrST[iIndex].refAaStr =
           malloc(
                (amrST[iIndex].lenRefAaUI  + 1)
              * sizeof(char *)
           );

        if(!amrST[iIndex].refAaStr)
           return def_amrST_memError;

        amrST[iIndex].refAaStr[0] = firstAaC;

        amrST[iIndex].refAaStr[
           amrST[iIndex].lenRefAaUI - 1
        ] = secAaC;

        amrST[iIndex].refAaStr[
           amrST[iIndex].lenRefAaUI
        ] = '\0';

        /*Set up the amr buffer*/

        amrST[iIndex].lenAmrAaUI = 
           amrST[iIndex].lenRefAaUI << 1;

        amrST[iIndex].amrAaStr =
           malloc(
                (amrST[iIndex].lenAmrAaUI  + 1)
              * sizeof(char *)
           );

        if(!amrST[iIndex].amrAaStr)
           return def_amrST_memError;

        continue; /*will handle aa in sam file step*/
     } /*If: this was an large (over 1) duplication*/

     /*++++++++++++++++++++++++++++++++++++++++++++++++++\
     + Fun-20 Sec-02 Sub-05 Cat-06: who2023ParsVar
     +   - Deletion; add first and last aa to refAaStr
     \++++++++++++++++++++++++++++++++++++++++++++++++++*/

     parseDel:;

     /*Mark that I have a deltion for the sam file*/
     amrST[iIndex].aaDelBl = def_amrST_del_and_ins;

     varStr += 3; /*get off "del"*/

     /*Find the number of amino acids in reference*/
     /*Reference length needs to be found*/
     amrST[iIndex].lenRefAaUI =
          1
        + noBranchAB(
               amrST[iIndex].endCodonNumUI
             - amrST[iIndex].codonNumUI
          ); /*Get the length of deletion*/

     amrST[iIndex].refAaStr =
        malloc(
         (amrST[iIndex].lenRefAaUI+1) * sizeof(char *)
        );

     if(!amrST[iIndex].refAaStr)
        return def_amrST_memError;

     amrST[iIndex].refAaStr[0] = firstAaC;

     if(secAaC != 0)
     { /*If: There this is a range of deletions*/
        amrST[iIndex].refAaStr[
            amrST[iIndex].lenRefAaUI - 1
        ] = secAaC;

        amrST[iIndex].refAaStr[
            amrST[iIndex].lenRefAaUI
        ] = '\0';
     } /*If: There this is a range of deletions*/

     else amrST[iIndex].refAaStr[2] = '\0';

     /*++++++++++++++++++++++++++++++++++++++++++++++++++\
     + Fun-20 Sec-02 Sub-05 Cat-07: who2023ParsVar
     +   - Del only; add mutate aa sequence to amrAaStr
     \++++++++++++++++++++++++++++++++++++++++++++++++++*/

     /*Get the amino acid sequece for the amr*/
     if(*varStr == '\0')
     { /*If: this is just a deletion*/
        /*Copy the amr amino acid sequence*/
        amrST[iIndex].lenAmrAaUI = 1;

        amrST[iIndex].amrAaStr = malloc(2*sizeof(char *));

        if(!amrST[iIndex].amrAaStr)
           return def_amrST_memError;

        amrST[iIndex].amrAaStr[0] = '0';
        amrST[iIndex].amrAaStr[1] = '\0';

        continue;
     } /*If: this is just a deletion*/

     /*++++++++++++++++++++++++++++++++++++++++++++++++++\
     + Fun-20 Sec-02 Sub-05 Cat-08: who2023ParsVar
     +   - Del + ins; Use insertion step to update amino
     +     acid sequence in amrAaStr
     +     (goto fun-20 sec-02 sub-05 cat-04)
     \++++++++++++++++++++++++++++++++++++++++++++++++++*/

     /*Get off the ins*/
     amrST[iIndex].aaDelBl = def_amrST_del_and_ins;
     varStr += 3;
     cpStr = varStr;

     while(*cpStr != '\0')
     { /*Loop: Get the string length*/
        /*Check if they used a single letter or multi*/
        if(*cpStr == '*' || *cpStr == '?') ++cpStr;
        else cpStr += 3;

        ++amrST[iIndex].lenAmrAaUI;
     } /*Loop: Get the string length*/

     amrST[iIndex].amrAaStr =
        malloc(
         (amrST[iIndex].lenAmrAaUI+1) * sizeof(char *)
        );

     if(!amrST[iIndex].amrAaStr)
        return def_amrST_memError;

     amrST[iIndex].amrAaStr[
        amrST[iIndex].lenAmrAaUI
     ] = '\0';

     /*Add the new insertion in*/
     cpStr = &amrST[iIndex].amrAaStr[0];

     while(*varStr != '\0')
     { /*Copy the aa from varStr*/
        if(*varStr == '*' || *varStr == '?')
           *cpStr++ = *varStr++;
        else
        { /*Else: This is a three letter codon*/
           *cpStr++ = aaThreeLetterToChar(varStr);
           varStr += 3;
        } /*Else: This is a three letter codon*/
     } /*Copy the aa from varStr*/
   } /*Loop: Process each variant*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-20 Sec-03: who_parse_VarId
   ^   - clean up and exit
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   return 0;
} /*who_parse_VarID*/

/*-------------------------------------------------------\
| Fun-21: whoAddCodonPos
|   - Adds the amino acid sequences for deletions and
|     large duplications, reading frame orientation
|     (forward/reverse) to the, and the first reference
|     base in the codon to an amrStruct that has been
|     processed with who_parse_VarID.
| Input:
|   - amrST:
|     - Pointer to an array of amrStruct structures to
|       update and process variants for
|   - numAmrI:
|     - Number of amrStructs in amrST
|   - samStr:
|     - C-string with the path to the sam file with the
|       gene mappings to get the reference coordiantes
|       from.
|   - buffStr:
|     - Pointer to c-string to use in reading in each
|       sam entry (resized as needed)
|   - lenBuffUL;
|     - current length of buffStr; updated when buffStr is
|       resized
| Output:
|   - Modifies:
|     o each structure in amrST with amino acid mutations
|       to have the amino acid sequence and starting
|       position of codon on the reference
|     o buffStr to be larger if needed
|     o lenBuffUL if buffStr is resized
|     o samFILE to point to the end of the file
|   - Returns
|     o 0 for success
|     o def_amrST_memError for a memory error
\-------------------------------------------------------*/
char whoAddCodonPos(
   struct amrStruct *amrST,/*Has amr variants to update*/
   int numAmrI,         /*Number of amrs*/
   char *samStr,          /*Path to Sam file with genes*/
   char **buffStr,         /*Buffer for reading sam file*/
   ulong *lenBuffUL         /*Length of buffer*/
){ /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
   ' Fun-21 TOC: whoAddCodonPost
   '   - Adds the amino acid sequences for deletions and
   '     large duplications, reading frame orientation
   '     (forward/reverse) to the, and the first reference
   '     base in the codon to an amrStruct that has been
   '     processed with who_parse_VarID.
   '   o fun-20 sec-01:
   '     - Variable declerations
   '   o fun-20 sec-02:
   '     - Add the sam entry cordiantes and the deletion
   '       amino acids
   '   o fun-21 sec-03:
   '     - Check if had memory error and return error
   '       status
   \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-21 Sec-01: whoAddCodonPos
   ^   - Variable declerations
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   char *cpStr = 0;
   char *dupStr = 0;
   int iIndex = 0;

   /*Specificly for dealing with aa deletions*/
   int iCodon = 0; /*Codon on for copying deletions*/
   uchar oneNtUC = 0;
   uchar twoNtUC = 0;
   uchar threeNtUC = 0;

   char errC = 0; /*Sam file error reporting*/
   struct samEntry samST;

   FILE *samFILE = fopen(samStr, "r");

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-21 Sec-03: whoAddCodonPos
   ^   - Add the sam entry cordiantes and the deletion
   ^     amino acids
   ^   o fun-21 sec-07 sub-01:
   ^     - Read each sam entry and filter out entries I
   ^       can not use
   ^   o fun-21 sec-07 sub-02:
   ^     - Find the codon positions/del aa for each AMR
   ^   o fun-21 sec-07 sub-03:
   ^     - Get the next entry (gene) in the sam file
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    /****************************************************\
    * Fun-21 Sec-03 Sub-01:
    *   - Read each sam entry and filter out entries I
    *     can not use
    \****************************************************/

    if(! samFILE) return def_amrST_invalidFILE;

    initSamEntry(&samST);
    errC = readSamLine(&samST,buffStr,lenBuffUL,samFILE);

    while(!errC)
    { /*Loop: Get the one base in the codon*/
       /*Check if I  havee a header/comment*/
       if(samST.extraStr[0] == '@') goto getNextLine;

       /*Check if this is an umapped gene, secondary
       ` alignment, or an supplemental alignet
       */
       if(samST.flagUS & (4 | 256 | 2048))
          goto getNextLine;

       /*Check if there is a sequence entry*/
       if(samST.seqStr[0] == '*') goto getNextLine;

       /*Check if I had an AMR(s) in this gene*/
       iIndex =
          findNearestAmr(amrST,samST.refStartUI,numAmrI);

       if(iIndex == -1) goto getNextLine;

       if(amrST[iIndex].refPosUI > samST.refEndUI)
          goto getNextLine;

       if(amrST[iIndex].refPosUI < samST.refStartUI)
          goto getNextLine;

       /*************************************************\
       * Fun-21 Sec-03 Sub-02:
       *   - Find the codon positions/del aa for each AMR
       *   o fun-21 sec-03 sub-02 cat-01:
       *     - Loop though all AMR(s) with this gene and
       *       add the gene coordiantes on the reference
       *   o fun-21 sec-03 sub-02 cat-02:
       *     - Handle cases with reverse complemnt genes
       *   o fun-21 sec-03 sub-02 cat-03:
       *     - Get reference amino acid sequence for
       *       AMR(s) in reverse complement genes
       *   o fun-21 sec-03 sub-02 cat-04:
       *     - Handle cases with forward genes
       *   o fun-21 sec-03 sub-02 cat-05:
       *     - Get reference amino acid sequence for
       *       AMR(s) in forward genes
       *   o fun-21 sec-03 sub-02 cat-06:
       *     - Move to the next AMR/restart loop
       \*************************************************/

       /*++++++++++++++++++++++++++++++++++++++++++++++++\
       + Fun-21 Sec-03 Sub-02 Cat-01:
       +   - Loop though all AMR(s) with this gene and add
       +     the gene coordiantes on the reference
       \++++++++++++++++++++++++++++++++++++++++++++++++*/

       while(amrST[iIndex].refPosUI < samST.refEndUI)
       { /*Loop: Update positon for each amr*/
          if(iIndex >= numAmrI) break;

          amrST[iIndex].geneFirstRefUI = samST.refStartUI;
          amrST[iIndex].geneLastRefUI = samST.refEndUI;

          /*+++++++++++++++++++++++++++++++++++++++++++++\
          + Fun-21 Sec-03 Sub-02 Cat-02:
          +   - Handle cases with reverse complemnt genes
          \+++++++++++++++++++++++++++++++++++++++++++++*/

          /*Set up the reading frame direction*/
          if(samST.flagUS & 16)
          { /*If: this is a reverse sequence*/
             amrST[iIndex].dirFlag = def_amrST_revCompDir;

             if(amrST[iIndex].codonNumUI == 0)
             { /*If: this is not an aa mutation*/
                 ++iIndex;
                 continue;
             } /*If: this is not an aa mutation*/

             /*Update the starting position*/
             amrST[iIndex].codonPosUI =
                  samST.refEndUI
                - ((amrST[iIndex].codonNumUI - 1) * 3);

             if(amrST[iIndex].endCodonNumUI == 0)
             { /*If: this is a single aa change*/
                amrST[iIndex].endCodonNumUI =
                   amrST[iIndex].codonNumUI;

                 ++iIndex;
                 continue;
             } /*If: this is a single aa change*/

             if(amrST[iIndex].aaDelBl) goto revGetDupDel;

             else if(amrST[iIndex].aaMultiDupBl)
                goto revGetDupDel;

             else
             { /*Else: this has an updated mutation*/
                 ++iIndex;
                 continue;
             } /*Else: this has an updated mutation*/

             revGetDupDel:;

             /*++++++++++++++++++++++++++++++++++++++++++\
             + Fun-21 Sec-03 Sub-02 Cat-02:
             +   - Get reference amino acid sequence for
             +     AMR(s) in reverse complement genes
             \++++++++++++++++++++++++++++++++++++++++++*/

             /*Set up for getting the reference amino
             `   acids; the + or - 1 is to ignore the
             `   first amino acid in the deletin (I
             `   already have it recorded)
             */
             dupStr = amrST[iIndex].refAaStr + 1;
             cpStr = samST.seqStr + samST.readLenUI;
             cpStr -= ((amrST[iIndex].codonNumUI -1) * 3);
                /*Not using -1, because I already have the
                ` one codon
                */
             for(
                iCodon = amrST[iIndex].codonPosUI + 1;
                iCodon < amrST[iIndex].endCodonNumUI - 1;
                ++iCodon
             ){ /*Loop: Copy the deletion*/
                oneNtUC =
                   (uchar)
                   compBaseToCodeLkTbl[(uchar) *cpStr--];

                twoNtUC =
                   (uchar)
                    compBaseToCodeLkTbl[(uchar) *cpStr--];

                threeNtUC =
                   (uchar)
                   compBaseToCodeLkTbl[(uchar) *cpStr--];

                *dupStr++ =
                  codonLkTbl[oneNtUC][twoNtUC][threeNtUC];
             } /*Loop: Copy the deletion*/
          } /*If: this is a reverse sequence*/

          /*+++++++++++++++++++++++++++++++++++++++++++++\
          + Fun-21 Sec-03 Sub-02 Cat-04:
          +   - Handle cases with forward genes
          \+++++++++++++++++++++++++++++++++++++++++++++*/

          else
          { /*Else: This is a foward sequence*/
             amrST[iIndex].dirFlag = def_amrST_forwardDir;

             if(amrST[iIndex].codonNumUI == 0)
             { /*If: this is not an aa mutation*/
                 ++iIndex;
                 continue;
             } /*If: this is not an aa mutation*/
     
             /*Update the starting position*/
             amrST[iIndex].codonPosUI =
                  samST.refStartUI
                + ((amrST[iIndex].codonNumUI - 1) * 3);

             if(amrST[iIndex].endCodonNumUI == 0)
             { /*If: this is a single aa change*/
                amrST[iIndex].endCodonNumUI =
                   amrST[iIndex].codonNumUI;

                 ++iIndex;
                 continue;
             } /*If: this is a single aa change*/

             if(amrST[iIndex].aaDelBl) goto forGetDupDel;

             else if(amrST[iIndex].aaMultiDupBl)
                goto forGetDupDel;

             else
             { /*Else: this has an updated mutation*/
                 ++iIndex;
                 continue;
             } /*Else: this has an updated mutation*/
             /*Check if I have a deltion*/

             forGetDupDel:;

             /*++++++++++++++++++++++++++++++++++++++++++\
             + Fun-21 Sec-03 Sub-02 Cat-05:
             +   - Get reference amino acid sequence for
             +     AMR(s) in forward genes
             \++++++++++++++++++++++++++++++++++++++++++*/

             /*Set up for getting the reference amino
             `   acids; the + 1 is to ignore the
             `   first amino acid in the deletin (I
             `   already have it recorded)
             */
             dupStr = amrST[iIndex].refAaStr + 1;
             cpStr = samST.seqStr;
             cpStr += ((amrST[iIndex].codonNumUI -1) * 3);
                /*Not using +1, because I already have the
                ` one codon
                */

             for(
                iCodon = amrST[iIndex].codonPosUI + 1;
                iCodon < amrST[iIndex].endCodonNumUI - 1;
                ++iCodon
             ){ /*Loop: Copy the deletion*/
                oneNtUC =
                   (uchar)
                   baseToCodeLkTbl[(uchar) *cpStr++];

                twoNtUC =
                   (uchar)
                    baseToCodeLkTbl[(uchar) *cpStr++];

                threeNtUC =
                   (uchar)
                   baseToCodeLkTbl[(uchar) *cpStr++];

                *dupStr++ =
                  codonLkTbl[oneNtUC][twoNtUC][threeNtUC];
             } /*Loop: Copy the deletion*/

          } /*Else: This is a foward sequence*/

          /*+++++++++++++++++++++++++++++++++++++++++++++\
          + Fun-21 Sec-03 Sub-02 Cat-06:
          +   - Move to the next AMR/restart loop
          \+++++++++++++++++++++++++++++++++++++++++++++*/

          if(amrST[iIndex].aaMultiDupBl)
          { /*If: this is a large duplicate*/
             dupStr = amrST[iIndex].amrAaStr;
             cpStr = amrST[iIndex].refAaStr;

             while(*cpStr) *dupStr++ = *cpStr++;

             cpStr = amrST[iIndex].refAaStr;
             while(*cpStr) *dupStr++ = *cpStr++;

             *dupStr = '\0';
          } /*If: this is a large duplicate*/

          ++iIndex;
       } /*Loop: Update positon for each amr*/

       /*************************************************\
       * Fun-21 Sec-03 Sub-03:
       *   - Get the next entry (gene) in the sam file
       \*************************************************/

       getNextLine:;

       errC=readSamLine(&samST,buffStr,lenBuffUL,samFILE);
    } /*Loop: Get the one base in the codon*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-21 Sec-04: whoAddCodonPos
   ^   - Check if had memory error and return error status
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   fclose(samFILE);
   samFILE = 0;

   if(errC > 1) return def_amrST_memError;

   freeSamEntryStack(&samST);

   return 0; /*No errors*/
} /*whoAddCodonPos*/
