/*########################################################
# Name: cnvt_whoToTbAmr
#   - Holds functions to convert the 2023 WHO database to
#     tbAmr format.
########################################################*/

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
' SOF: Start Of File
'   o header:
'     - Included libraries
'   o .c fun-01: amrIsRes
'     - Dectects if an amr was classified as resistant or
'       is unknow/not resitance
'   o .c fun-02: amrSTAddSingleAa
'       - Add a single amino acid variant to an amrStruct
'   o fun-03: read_2021_WhoAmrCsv
'       - Gets the amr data from the Who TB antibiotice
'         resistance 2021 catalog (genome indicie tab
'         saved as a csv).
'   o .c fun-04: checkCrossRes
'       - Check if there is cross resitance (2023 catalog)
'   o fun-05: read_2023_WhoAmrTsv
'       - Reads in the two tabs (as separate tsv's) and
'         converts them to an amrStructs array
'   o fun-06: who2023ParsVar
'       - Parse the variant idea from the WHO 2023 TB
'         catalog to update amino acid mutations.
'   o fun-07: whoAddCodonPos
'       - Adds the amino acid sequences for deletions and
'         large duplications, reading frame orientation
'         (forward/reverse) to the, and the first
'         reference base in the codon to an amrStruct that
'         has been processed with who_parse_VarID.
'   o license:
'     - Licensing for this code (public domain / mit)
\~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*-------------------------------------------------------\
| Header:
|   - Included libraries
\-------------------------------------------------------*/

/*I have no idea if this would run on plan9, but I figure
`   I can at least add a token gesture in
*/
#ifdef PLAN9
   #include <u.h>
   #include <libc.h>
#else
   #include <stdlib.h>
#endif

#include "cnvt_whoToTbAmr.h"

#include <stdio.h>
#include <string.h>

#include "amrStruct.h"
#include "genIndiceStruct.h"
#include "drug_str_ary.h"

/*Are just .h files*/
#include "../generalLib/dataTypeShortHand.h"
#include "../generalLib/genMath.h"
#include "../generalLib/codonTbl.h"
#include "../generalLib/ulCpStr.h"
#include "../generalLib/base10StrToNum.h"
#include "../generalLib/gen-shellSort.h"

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
| Fun-02: amrSTAddSingleAa
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
| Fun-03: read_2021_WhoAmrCsv
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
   ^ Fun-03 Sec-02:
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
   ^ Fun-03 Sec-03:
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
   ^ Fun-03 Sec-04:
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
     * Fun-03 Sec-04 Sub-02:
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
     * Fun-03 Sec-04 Sub-03:
     *   - Check if it is reverse complement
     \***************************************************/

     for(uiIter=uiIter; lineStr[uiIter] !=','; ++uiIter){}
    
     amrST[*numAmrUL].dirFlag =
       -(lineStr[uiIter -1] =='c') & def_amrST_revCompDir;

     ++uiIter;
     
     /***************************************************\
     * Fun-03 Sec-04 Sub-04:
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
     * Fun-03 Sec-04 Sub-05:
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
     * Fun-03 Sec-04 Sub-06:
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
     * Fun-03 Sec-04 Sub-07:
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
     * Fun-03 Sec-04 Sub-08:
     *   - Ignore the amino acid entry
     \***************************************************/

     /*I was getting positioning errors with this, so it
     `   is easier to add this in with a sam file
     */
     while(lineStr[uiIter] !=',') ++uiIter;
     ++uiIter; /*Get off the ','*/

     /***************************************************\
     * Fun-03 Sec-04 Sub-09:
     *   - Get to the first antibiotic column
     \***************************************************/

     for(uiIter=uiIter; lineStr[uiIter] !=','; ++uiIter){}
     ++uiIter;

     for(uiIter=uiIter; lineStr[uiIter] !=','; ++uiIter){}
     ++uiIter;

     /***************************************************\
     * Fun-03 Sec-04 Sub-10:
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
     * Fun-03 Sec-04 Sub-11:
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
     * Fun-03 Sec-04 Sub-12:
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
     * Fun-03 Sec-04 Sub-13:
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
     * Fun-03 Sec-04 Sub-14:
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
     * Fun-03 Sec-04 Sub-15:
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
     * Fun-03 Sec-04 Sub-16:
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
| Fun-04: checkCrossRes
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
| Fun-05: read_2023_WhoAmrTsv
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
|   - keepNonResBl:
|     o Keep the grade 3, 4, and 5 entries (not an AMR
|       mutation)
|   - rmFullGeneVarBl, 
|     o 1: Ignore all variants that apply to the full gene
|     o 0: Keep all variants
|   - errC:
|     o Holds the error type
| Output:
|   - Modifies:
|     o numAmrUL to hold the number of used amrStructs in
|       the returned amrStruct structure array
|     o drugAryStr to hold the name of each antibiotic
|     o numDrugsI to hold the number of drugs in
|       drugAryStr
|     o errC:
|       - 0 for no errors
|       - def_amrST_memError for memory errors
|       - def_amrST_invalidFILE for file errors
|   - Returns:
|     o An array of amrStruct structures that have the
|       resitant (grade 1 and 2) mutations
\-------------------------------------------------------*/
struct amrStruct * read_2023_WhoAmrTsv(
   char *whoMasterStr,  /*path to Master tab tsv*/
   char *whoIndiceStr,  /*path to genome indicie tab*/
   unsigned long *numAmrUL, /*Number of amrs kept*/
   char **drugAryStr,   /*Holds antibiotics*/
   int *numDrugsI,      /*Number of drugs in drugAryStr*/
   char keepNonResBl,   /*1: to keep everything*/
   char rmFullGeneVarBl, /*1: to ignore entire gene*/
   char *errC           /*Reports errors*/
){ /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
   ' Fun-05 TOC: read_2023_WhoAmrTsv
    '   o fun-05 sec-01:
    '     - Variable declerations
    '   o fun-05 sec-02:
    '     - Get the genome coordinates
    '   o fun-05 sec-03:
    '     - Get the master file length
    '   o fun-05 sec-04:
    '     - Set up the buffers
    '   o fun-05 sec-05:
    '     - Read in the file
    '   o fun-05 sec-06:
    '     - Clean up
   \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
    ^ Fun-05 Sec-01:
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

    char cpAmrBl = 0;   /*1: more than 1 indice for amr*/
    int indexI = 0;
    struct genIndice *indiceAryST = 0;

    struct amrStruct *amrST = 0;

    FILE *whoFILE = 0;

    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
    ^ Fun-05 Sec-02:
    ^   - Get the genome coordinates
    \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    *errC = 0;

    indiceAryST =
       read_who2023_indiceTabTsv(
          whoIndiceStr,
          &lenPosAryUI
       ); /*Get the genome coordiantes*/

    if(indiceAryST == 0)
    { /*If: I had a memory error*/
       *errC = def_amrST_memError;
       return 0; /*Memory error*/
    } /*If: I had a memory error*/

    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
    ^ Fun-05 Sec-03:
    ^   - Get the master file length
    \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    whoFILE = fopen(whoMasterStr, "r");

    if(whoFILE == 0)
    { /*If: I could not open the master file*/
       freeGeneIndiceAry(&indiceAryST, lenPosAryUI);
       *errC = def_amrST_invalidFILE;
       return 0;
    } /*If: I could not open the master file*/

    buffStr = malloc(lenBuffUI * sizeof(char));

    buffStr[lenBuffUI - 2] = '\0';

    while(fgets(buffStr, lenBuffUI, whoFILE))
    { /*Loop: Read in each line of the file*/

       /*fgets does not always fill the buffer full, so
       '   I can not use shortcuts to check lengths
       */
       tmpStr = buffStr;
       tmpStr += ulEndStrLine(tmpStr);

       /*Make sure my buffer can read in the full line*/
       while(*tmpStr != '\n')
       { /*Loop: Read in the complete line*/
           free(buffStr);
           buffStr=malloc((lenBuffUI <<1) * sizeof(char));

           if(buffStr == 0)
           { /*If: I had a memory error*/
              *errC = def_amrST_invalidFILE;

              fclose(whoFILE);
              freeGeneIndiceAry(&indiceAryST,lenPosAryUI);

              return 0;
           } /*If: I had a memory error*/

           tmpStr = buffStr + lenBuffUI;

           /*Get line and check for EOF*/
           if(! fgets(tmpStr, lenBuffUI, whoFILE)) break;
           
           tmpStr += ulEndStrLine(tmpStr);
           lenBuffUI <<= 1;
       } /*Loop: Read in the complete line*/

       ++lenFileUI;
    } /*Loop: Read in each line of the file*/

    if(lenFileUI < 3)
    { /*If: I had a memory error*/
       fclose(whoFILE);
       free(buffStr);
       freeGeneIndiceAry(&indiceAryST, lenPosAryUI);
       *errC = def_amrST_memError;
       return 0; /*Memory error*/
    } /*If: I had a memory error*/

    fseek(whoFILE, 0, SEEK_SET); /*Start of file*/

    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
    ^ Fun-05 Sec-04:
    ^   - Set up the buffers
    \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    *numDrugsI = 0;
    *drugAryStr = mallocDrugAryStr(16);

    if(*drugAryStr == 0)
    { /*If: I had a memory error*/
       fclose(whoFILE);
       free(buffStr);

       freeGeneIndiceAry(&indiceAryST, lenPosAryUI);
       *errC = def_amrST_memError;

       return 0;
    } /*If: I had a memory error*/

    drugAryLimitI = 16;

    /*This will be massively oversized for just resitant
    `   genes, but it works
    ` For all AMRs I need to add in lenFileUI becuase
    `   there are more AMRs the in lenPosAryUI or
    `   lenFileUI alone. This is quite annoying. I found
    `   that even just adding lenPosAryUI + lenFileUI was
    `   not enough. So, I needed to multiply lenFileUI by
    `   2. There are around 203910 entries total.
    */
    amrST =
       calloc(
          lenPosAryUI + (lenFileUI << 1),
          sizeof(struct amrStruct));

    if(amrST == 0)
    { /*If: I had a memory error*/
       fclose(whoFILE);
       free(buffStr);

       free(*drugAryStr);
       *drugAryStr = 0;

       freeGeneIndiceAry(&indiceAryST, lenPosAryUI);
       *errC = def_amrST_memError;
       return 0; /*Memory error*/
    } /*If: I had a memory error*/

    *numAmrUL = 0;

    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
    ^ Fun-05 Sec-05:
    ^   - Read in the file
    ^   o fun-05 sec-05 sub-01:
    ^     - get past header, start loop, get antibiotic
    ^   o fun-05 sec-05 sub-02:
    ^     - get the gene name
    ^   o fun-05 sec-05 sub-03:
    ^     - Move past the mutation column
    ^   o fun-05 sec-05 sub-04:
    ^     - Move past the mutation column
    ^   o fun-05 sec-05 sub-05:
    ^     - Move past the mutation column
    ^   o fun-05 sec-05 sub-06:
    ^     - Get the effect column
    ^   o fun-05 sec-05 sub-07:
    ^     - Get the genomic coordinate if present.
    ^       This will be updated twice if there is an
    ^       genome corrdiante in the indicies tab, but
    ^       also ensures all positions with coordinates
    ^       are found
    ^   o fun-05 sec-05 sub-08:
    ^     - Move to the next set of targets to extract
    ^   o fun-05 sec-05 sub-09:
    ^     - Check if this provides resitance
    ^   o fun-05 sec-05 sub-10:
    ^     - Get the comment entry
    ^   o fun-05 sec-05 sub-11:
    ^     - Check for cross resistance (additional grade)
    ^   o fun-05 sec-05 sub-12:
    ^     - Deal with genomic coordiantes
    ^   o fun-05 sec-05 sub-13:
    ^     - Move to the next amr entry
    \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    /****************************************************\
    * Fun-05 Sec-05 Sub-01:
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
       * Fun-05 Sec-05 Sub-02:
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
       * Fun-05 Sec-05 Sub-03:
       *   - Move past the mutation column
       \*************************************************/

       while(buffStr[uiPos] != '\t') ++uiPos;
       ++uiPos; /*get off the tab*/

       /*************************************************\
       * Fun-05 Sec-05 Sub-04:
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
       * Fun-05 Sec-05 Sub-05:
       *   - Read past the teir column
       \*************************************************/

       while(buffStr[uiPos] != '\t') ++uiPos;
       ++uiPos; /*get off the tab*/

       /*************************************************\
       * Fun-05 Sec-05 Sub-06:
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
       * Fun-05 Sec-05 Sub-07:
       *   - Get the genomic coordinate if present.
       *     This will be updated twice if there is an
       *     genome corrdiante in the indicies tab, but
       *     also ensures all positions with coordinates
       *     are found
       \*************************************************/

       if(buffStr[uiPos] > 47 && buffStr[uiPos] < 58)
       { /*If: There is an genomic coordinate here*/
          tmpStr =
             base10StrToUI(
                &buffStr[uiPos],
                amrST[*numAmrUL].refPosUI
             );

          /*Convert the coordinate to index 0*/
          --amrST[*numAmrUL].refPosUI;

          uiPos += tmpStr - &buffStr[uiPos];
       } /*If: There is an genomic coordinate here*/

       /*Else: It is an see genomic coordinates*/

       while(buffStr[uiPos++] != '\t') ;

       /*************************************************\
       * Fun-05 Sec-05 Sub-08:
       *   - Move to the next set of targets to extract
       \*************************************************/

       for(iTab = 7; iTab < 105; ++iTab)
       { /*Loop: Get past columns I am ignoring*/
          while(buffStr[uiPos] != '\t') ++uiPos;
          ++uiPos; /*Get off the tab*/
       } /*Loop: Get past columns I am ignoring*/

       /*************************************************\
       * Fun-05 Sec-05 Sub-09:
       *   - Check if this provides resitance
       \*************************************************/

       if(   (! keepNonResBl)
          && buffStr[uiPos] != '1'
          && buffStr[uiPos] != '2'
       ){ /*If: their is no antibiotic resitance*/
          freeAmrStructStack(&amrST[*numAmrUL]);
          continue;
       } /*If: their is no antibiotic resitance*/

       /*Store the grade*/
       amrST[*numAmrUL].gradeC = buffStr[uiPos] - 48;

       /*Move to the next entry*/
       while(buffStr[uiPos] != '\t') ++uiPos;
       ++uiPos; /*get off the tab*/

       /*************************************************\
       * Fun-05 Sec-05 Sub-10:
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
       * Fun-05 Sec-05 Sub-11:
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
       * Fun-05 Sec-05 Sub-12:
       *   - Deal with genomic coordiantes
       *   o fun-05 sec-05 sun-12 cat-01:
       *     - Find the matching gene indice(s)
       *   o fun-05 sec-05 sun-12 cat-02:
       *     - Check if I have another amr/start loop
       *   o fun-05 sec-05 sun-12 cat-03:
       *     - In next amr, copy the gene id
       *   o fun-05 sec-05 sun-12 cat-04:
       *     - In next amr, copy variant id
       *   o fun-05 sec-05 sun-12 cat-05:
       *     - In next amr, copy effect entry
       *   o fun-05 sec-05 sun-12 cat-06:
       *     - In next amr, copy the comment entry
       *   o fun-05 sec-05 sun-12 cat-07:
       *     - Get reference postion
       *   o fun-05 sec-05 sun-12 cat-08:
       *     - Copy the reference sequence
       *   o fun-05 sec-05 sun-12 cat-09:
       *     - Copy the amr sequence
       *   o fun-05 sec-05 sun-12 cat-10:
       *     - Check the amr mutation type
       *   o fun-05 sec-05 sun-12 cat-11:
       *     - Move to the next gene indice/dup amr
       \*************************************************/

       /*++++++++++++++++++++++++++++++++++++++++++++++++\
       + Fun-05 Sec-05 Sub-12 Cat-01:
       +   - Find the matching gene indice(s)
       \++++++++++++++++++++++++++++++++++++++++++++++++*/

       indexI =
          findGenIndiceVariant(
             indiceAryST,
             amrST[*numAmrUL].varIdStr,
             (int) lenPosAryUI
          ); /*Find the variant's gene indice*/

       if(indexI < 0)
       { /*If: There is no genome coordinates*/
          if(rmFullGeneVarBl & (indexI < 0))
          { /*If: discarding no genome coord entries*/
             fprintf(
                 stderr,
                 "%s not found in indice sheet\n",
                 amrST[*numAmrUL].varIdStr
             ); /*Let the user know it is missing*/

             freeAmrStructStack(&amrST[*numAmrUL]);
          } /*If: discarding no genome coord entries*/

          /*I will deal assign genome coordanites at the
          `   end of fun-07
          */
          else
          { /*Else: I am keeping these entries*/
             uiPos = amrST[*numAmrUL].lenVarIdUI;
             tmpStr = amrST[*numAmrUL].varIdStr;

             if(   (tmpStr[uiPos - 3] & (~32)) == 'L'
                && (tmpStr[uiPos - 2] & (~32)) == 'O'
                && (tmpStr[uiPos - 1] & (~32)) == 'F'
             ){ /*If: This is an loss of function*/
                amrST[*numAmrUL].mutTypeStr[0] = 'l';
                amrST[*numAmrUL].mutTypeStr[1] = 'o';
                amrST[*numAmrUL].mutTypeStr[2] = 'f';

                amrST[*numAmrUL].wholeGeneFlag =
                   def_geneFrameshift_amrST;

                amrST[*numAmrUL].frameshiftBl = 1;
             } /*If: This is an loss of function*/

             else if(
                   (tmpStr[uiPos - 8] & (~32)) == 'D'
                && (tmpStr[uiPos - 7] & (~32)) == 'E'
                && (tmpStr[uiPos - 6] & (~32)) == 'L'
                && (tmpStr[uiPos - 5] & (~32)) == 'E'
                && (tmpStr[uiPos - 4] & (~32)) == 'T'
                && (tmpStr[uiPos - 3] & (~32)) == 'I'
                && (tmpStr[uiPos - 2] & (~32)) == 'O'
                && (tmpStr[uiPos - 1] & (~32)) == 'N'
             ){ /*If: This is an loss of function*/
                amrST[*numAmrUL].mutTypeStr[0] = 'd';
                amrST[*numAmrUL].mutTypeStr[1] = 'e';
                amrST[*numAmrUL].mutTypeStr[2] = 'l';

                amrST[*numAmrUL].wholeGeneFlag =
                   def_geneDel_amrST;
             } /*If: This is an loss of function*/

             /*No idea what type of event it is*/
             else amrST[*numAmrUL].unknownBl = 1;

             /*I need to mark the sequence as blank*/
             amrST[*numAmrUL].refSeqStr =
                malloc(2 * sizeof(char));
             
             if(! amrST[*numAmrUL].refSeqStr)
                goto memoryError;

             amrST[*numAmrUL].refSeqStr[0] = '0';
             amrST[*numAmrUL].refSeqStr[1] = '\0';

             amrST[*numAmrUL].amrSeqStr =
                malloc(2 * sizeof(char));
             
             if(! amrST[*numAmrUL].amrSeqStr)
                goto memoryError;

             amrST[*numAmrUL].amrSeqStr[0] = '0';
             amrST[*numAmrUL].amrSeqStr[1] = '\0';

             ++(*numAmrUL);
          } /*Else: I am keeping these entries*/

          continue;
       } /*If: There is no genome coordinates*/

       /*++++++++++++++++++++++++++++++++++++++++++++++++\
       + Fun-05 Sec-05 Sub-12 Cat-02:
       +   - Check if I have another amr/start loop
       \++++++++++++++++++++++++++++++++++++++++++++++++*/

      cpAmrBl = 1;

       while(cpAmrBl)
       { /*Loop: Copy needed coordiantes*/

          /*No other gene Index entries*/
          if(indexI + 1 >= lenPosAryUI)
          { /*If: I am at the end of the genome indicies*/
             cpAmrBl = 0;
             goto cpRef_2023Catalog_sec05_sub12_cat07;
          } /*If: I am at the end of the genome indicies*/

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

             amrST[*numAmrUL + 1].gradeC =
                amrST[*numAmrUL].gradeC;

             /*++++++++++++++++++++++++++++++++++++++++++\
             + Fun-05 Sec-05 Sub-12 Cat-03:
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
             + Fun-05 Sec-05 Sub-12 Cat-04:
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
             + Fun-05 Sec-05 Sub-12 Cat-05:
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
             + Fun-05 Sec-05 Sub-12 Cat-06:
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
          + Fun-05 Sec-05 Sub-12 Cat-07:
          +   - Copy the reference position
          \+++++++++++++++++++++++++++++++++++++++++++++*/

           cpRef_2023Catalog_sec05_sub12_cat07:;

           amrST[*numAmrUL].refPosUI =
             indiceAryST[indexI].posUI;

          /*+++++++++++++++++++++++++++++++++++++++++++++\
          + Fun-05 Sec-05 Sub-12 Cat-08:
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
          + Fun-05 Sec-05 Sub-12 Cat-09:
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
          + Fun-05 Sec-05 Sub-12 Cat-10:
          +   - Check the amr mutation type
          \+++++++++++++++++++++++++++++++++++++++++++++*/

          uiPos = amrST[*numAmrUL].lenVarIdUI;
          tmpStr = amrST[*numAmrUL].varIdStr;

          if(   (tmpStr[uiPos - 3] & (~32)) == 'L'
             && (tmpStr[uiPos - 2] & (~32)) == 'O'
             && (tmpStr[uiPos - 1] & (~32)) == 'F'
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
          + Fun-05 Sec-05 Sub-12 Cat-11:
          +   - Move to the next gene indice/dup amr
          \+++++++++++++++++++++++++++++++++++++++++++++*/

          *numAmrUL += cpAmrBl;
          ++indexI;
       } /*Loop: Copy needed coordiantes*/

       /*************************************************\
       * Fun-05 Sec-05 Sub-13:
       *   - Move to the next amr entry
       \*************************************************/

       ++(*numAmrUL);
    } /*Loop: Read in the file*/
    
    /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
    ^ Fun-05 Sec-06:
    ^   - Clean up
    \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

    fclose(whoFILE);
    sortAmrStructArray(amrST, 0, *numAmrUL - 1);
    freeGeneIndiceAry(&indiceAryST, lenPosAryUI);

    ++(*numDrugsI); /*Convert to index 1*/

    return amrST;

    memoryError:;

    fclose(whoFILE);
    free(buffStr);

    *errC = def_amrST_memError;
    freeGeneIndiceAry(&indiceAryST, lenPosAryUI);
    freeAmrStructArray(&amrST, *numAmrUL);

    if(*drugAryStr) free(*drugAryStr);
    *drugAryStr = 0;

    return 0;
} /*read_2023_WhoAmrTsv*/

/*-------------------------------------------------------\
| Fun-06: who2023ParsVar
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
   ' Fun-06 TOC: who2023ParsVar
   '   - Parse the variant idea from the WHO 2023 TB
   '     catalog to update amino acid mutations.
   '   o fun-06 sec-01:
   '     - Variable declerations
   '   o fun-06 sec-02:
   '     - Process the variant id for amino acid entries
   '   o fun-06 sec-03:
   '     - clean up and exit
   \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-06 Sec-01: who2023ParsVar
   ^   - Variable declerations
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   char firstAaC = 0;
   char secAaC = 0;
   char *varStr = 0;
   char *cpStr = 0;
   int iIndex = 0;

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-06 Sec-02: who2023ParsVar
   ^   - Process the variant id for amino acid entries
   ^   o fun-06 sec-02 sub-01:
   ^     - Start loop (all AMR(s)) and check if I have an
   ^       amino acid variant to process
   ^   o fun-06 sec-02 sub-02:
   ^     - Get the first amino acid from the variant id
   ^   o fun-06 sec-02 sub-03:
   ^     - Get the codon number of the first amio acid
   ^   o fun-06 sec-02 sub-04:
   ^     - Get the second amino acid/mutation type for
   ^       variants that are not deletions/insertions
   ^   o fun-06 sec-02 sub-05:
   ^     - Handle deletions/insertions
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   /*****************************************************\
   * Fun-06 Sec-02 Sub-01: who2023ParsVar
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
        { /*If: this is not an aa mutation*/

           /*Check if this is an full gene LoF*/
           if(
                 (*(varStr - 0) & ~32) == 'F'
              && (*(varStr - 1) & ~32) == 'O'
              && (*(varStr - 2) & ~32) == 'L'
              && (*(varStr - 3)) == '_'
           ){ /*If: is an gene loss of function entry*/
               amrST[iIndex].wholeGeneFlag |=
                  def_geneFrameshift_amrST;
           } /*If: is an gene loss of function entry*/

           continue; /*This not an aa mutation*/
        } /*If: this is not an aa mutation*/
     } /*If: this is not a 2023 catalog entry*/

     varStr += 2;
        /*+2 is to get off the '.p'*/
     
     /***************************************************\
     * Fun-06 Sec-02 Sub-02: who2023ParsVar
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
     * Fun-06 Sec-02 Sub-03: who2023ParsVar
     *   - Get the codon number of the first amio acid
     \***************************************************/

     varStr =
        base10StrToUI(varStr, amrST[iIndex].codonNumUI);

     /***************************************************\
     * Fun-06 Sec-02 Sub-04: who2023ParsVar
     *   - Get the second amino acid/mutation type for
     *     variants that are not deletions/insertions
     \***************************************************/

     if(varStr[0] == '?')
     { /*If: this could be any aa change*/
        secAaC = '?';

        amrST[iIndex].endCodonNumUI =
           amrST[iIndex].codonNumUI;

        if(amrSTAddSingleAa(&amrST[iIndex],firstAaC,'?'))
           return def_amrST_memError;

        continue;
     } /*If: this could be any aa change*/
     
     else if(varStr[0] == '*')
     { /*If: this could be any aa change*/
        secAaC = '*';

        amrST[iIndex].endCodonNumUI =
           amrST[iIndex].codonNumUI;

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

        amrST[iIndex].endCodonNumUI =
           amrST[iIndex].codonNumUI;

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

        amrST[iIndex].endCodonNumUI =
           amrST[iIndex].codonNumUI;

        continue;
     } /*Else If: This is a one aa duplication*/

     else if(varStr[0] != '_')
     { /*Else If: for 2023 this is a single change*/

        /*Check if this is a lost stop codon*/
        if(!cStrEql("ext*?\0", varStr, '\0')) secAaC='?';

        /*Check if this is a single letter*/
        else if(varStr[1] == '\0') secAaC = varStr[0];

        /*This is a three letter code*/
        else secAaC = aaThreeLetterToChar(varStr);

        if(amrSTAddSingleAa(
             &amrST[iIndex],
             firstAaC,
             secAaC
        )) return def_amrST_memError;
 
        amrST[iIndex].endCodonNumUI =
           amrST[iIndex].codonNumUI;

        continue;
     } /*Else If: for 2023 this is a single change*/

     /***************************************************\
     * Fun-06 Sec-02 Sub-05: who2023ParsVar
     *   - Handle deletions/insertions
     *   o fun-06 sec-02 sub-05 cat-01:
     *     - Find the amino acid base at the end of the
     *       deletion/insertion
     *   o fun-06 sec-02 sub-05 cat-02:
     *     - Find the position of the last amino acid
     *     - insertion only case; add start/end aa to ref
     *   o fun-06 sec-02 sub-05 cat-03:
     *     - ins case; add start/end aa to amrSeqStr
     *   o fun-06 sec-02 sub-05 cat-04:
     *     - ins case; Add in the inserted aa sequence
     *   o fun-06 sec-02 sub-05 cat-05:
     *     - Deletion;  add first and last aa to refAaStr
     *   o fun-06 sec-02 sub-05 cat-06:
     *     - Del only; add mutate aa sequence to amrAaStr
     *   o fun-06 sec-02 sub-05 cat-07:
     *     - Del + ins; Use insertion step to update amino
     *       acid sequence in amrAaStr
     *       (goto fun-06 sec-02 sub-05 cat-04)
     \***************************************************/

     /*++++++++++++++++++++++++++++++++++++++++++++++++++\
     + Fun-06 Sec-02 Sub-05 Cat-01: who2023ParsVar
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
     + Fun-06 Sec-02 Sub-05 Cat-02: who2023ParsVar
     +   - Find the position of the last amino acid
     \++++++++++++++++++++++++++++++++++++++++++++++++++*/

     /*This adds some extra checks, but also makes sure
     `   the end codon number is set
     */
     parseDel:;

     /*Last codon in deletion/insertion*/
     cpStr =
       base10StrToUI(varStr,amrST[iIndex].endCodonNumUI);

     if(cpStr == varStr)
     { /*If: There was no end codon number*/
        amrST[iIndex].endCodonNumUI =
           amrST[iIndex].codonNumUI;
     } /*If: There was no end codon number*/

     varStr = cpStr;

     /*++++++++++++++++++++++++++++++++++++++++++++++++++\
     + Fun-06 Sec-02 Sub-05 Cat-03: who2023ParsVar
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
        + Fun-06 Sec-02 Sub-05 Cat-04: who2023ParsVar
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
        + Fun-06 Sec-02 Sub-05 Cat-05: who2023ParsVar
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
     * Fun-06 Sec-02 Sub-06: who2023ParsVar
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
     + Fun-06 Sec-02 Sub-05 Cat-06: who2023ParsVar
     +   - Deletion; add first and last aa to refAaStr
     \++++++++++++++++++++++++++++++++++++++++++++++++++*/

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
     + Fun-06 Sec-02 Sub-05 Cat-07: who2023ParsVar
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
     + Fun-06 Sec-02 Sub-05 Cat-08: who2023ParsVar
     +   - Del + ins; Use insertion step to update amino
     +     acid sequence in amrAaStr
     +     (goto fun-06 sec-02 sub-05 cat-04)
     \++++++++++++++++++++++++++++++++++++++++++++++++++*/

     /*Get off the ins*/
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
   ^ Fun-06 Sec-03: who_parse_VarId
   ^   - clean up and exit
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   return 0;
} /*who_parse_VarID*/

/*-------------------------------------------------------\
| Fun-07: whoAddCodonPos
|   - Adds the amino acid sequences for deletions and
|     large duplications, reading frame orientation
|     (forward/reverse) to the, and the first reference
|     base in the codon to an amrStruct that has been
|     processed with who_parse_VarID.
| Input:
|   - amrST:
|     o Pointer to an array of amrStruct structures to
|       update and process variants for
|   - numAmrI:
|     o Number of amrStructs in amrST
|   - geneTblFileStr:
|     o C-string with the path to the gene coordinates
|       file with gene mappings 
|       - column 3: + for foward gene; - for reverse gene
|       - column 4: frist mapped base in reference
|       - column 5: last mapped base in reference
|   - refFileStr:
|     o C-string with path to fasta file with reference
|       sequence (should only have one sequence)
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
|     o def_amrST_invalidFILE for an file error
|     o def_amrST_memError for a memory error
\-------------------------------------------------------*/
char whoAddCodonPos(
   struct amrStruct *amrST,/*Has amr variants to update*/
   int numAmrI,         /*Number of amrs*/
   char *geneTblFileStr,/*Path to gene coordinates table*/
   char *refFileStr     /*Path to rerence file*/
){ /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
   ' Fun-07 TOC: whoAddCodonPost
   '   - Adds the amino acid sequences for deletions and
   '     large duplications, reading frame orientation
   '     (forward/reverse) to the, and the first reference
   '     base in the codon to an amrStruct that has been
   '     processed with who_parse_VarID.
   '   o fun-07 sec-01:
   '     - Variable declerations
   '   o fun-07 sec-02:
   '     - Get the gene coordinates
   '   o fun-07 sec-03:
   '     - Read in the reference sequence
   '   o fun-07 sec-04:
   '     - Add in the gene coordinates and ammino acid
   '   o fun-07 sec-05:
   '     - Add in the whole gene target data
   '   o fun-07 sec-06:
   '     - Clean up and return
   \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-07 Sec-01: whoAddCodonPos
   ^   - Variable declerations
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   ushort lenBuffUS = 2 << 12;
   char buffStr[lenBuffUS];
   ulong bytesInBuffUL = 0;
   int numGenesI = 0;
   int posI = 0;
   uint tmpUI = 0; /*For temporary stuff*/

   char *refSeqHeapStr = 0;
   ulong lenRefUL = 0;

   char *revGene_heapAryBl = 0;
   int *startGene_heapArySI = 0;
   int *endGene_heapArySI = 0;

   char *cpStr = 0;
   char *dupStr = 0;
   char outBoundsBl = 0; /*Is outside of gene bounds*/
   int iIndex = 0;
   /*int iSwap = 0;*/ /*off gene correction attempt*/

   /*Specificly for dealing with aa deletions*/
   int iCodon = 0; /*Codon on for copying deletions*/
   uchar oneNtUC = 0;
   uchar twoNtUC = 0;
   uchar threeNtUC = 0;

   char retErrC = 0; /*Error message to return*/

   FILE *tmpFILE = 0;

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-07 Sec-02:
   ^   - Get the gene coordinates
   ^   o fun-07 sec-02 sub-01:
   ^     - Open the gene coordinates table
   ^   o fun-07 sec-02 sub-02:
   ^     - Find the number of genes in the table
   ^   o fun-07 sec-02 sub-03:
   ^     - Allocate memory for the gene coordinates
   ^   o fun-07 sec-02 sub-04:
   ^     - Get the gene coordinates from the table
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   /*****************************************************\
   * Fun-07 Sec-02 Sub-01:
   *   - Open the gene coordinates table
   \*****************************************************/

   tmpFILE = fopen(geneTblFileStr, "r");

   if(! tmpFILE) goto fileErr_fun07_sec06_sub03;

   buffStr[lenBuffUS - 1] = '\0';

   bytesInBuffUL =
      fread(buffStr, sizeof(char), lenBuffUS -1, tmpFILE);

   posI = 0;

   /*****************************************************\
   * Fun-07 Sec-02 Sub-02:
   *   - Find the number of genes in the table
   \*****************************************************/

   while(bytesInBuffUL)
   { /*Loop: Count the number of genes*/
      while(posI < bytesInBuffUL)
      { /*Loop: Find the number of new lines in buffer*/
         posI += ulEndStrLine(&buffStr[posI]);
         numGenesI += (buffStr[posI] == '\n');
         ++posI;
      } /*Loop: Find the number of new lines in buffer*/

      bytesInBuffUL =
         fread(
            buffStr,
            sizeof(char),
            lenBuffUS -1,
            tmpFILE
         );

      posI = 0;
   } /*Loop: Count the number of genes*/

   /*****************************************************\
   * Fun-07 Sec-02 Sub-03:
   *   - Allocate memory for the gene coordinates
   \*****************************************************/

   revGene_heapAryBl =
      malloc((numGenesI + 1) * sizeof(char));

   if(! revGene_heapAryBl) goto memErr_fun07_sec06_sub02;

   startGene_heapArySI =
      malloc((numGenesI + 1) * sizeof(int));

   if(! startGene_heapArySI)
      goto memErr_fun07_sec06_sub02;

   endGene_heapArySI= malloc((numGenesI+1) * sizeof(int));

   if(! endGene_heapArySI) goto memErr_fun07_sec06_sub02;
   
   /*****************************************************\
   * Fun-07 Sec-02 Sub-04:
   *   - Get the gene coordinates from the table
   \*****************************************************/

   fseek(tmpFILE, 0, SEEK_SET);
   cpStr = fgets(buffStr, lenBuffUS, tmpFILE);
   posI = 0;

   while(fgets(buffStr, lenBuffUS, tmpFILE))
   { /*Loop: Read in the file*/
      /*Get past the gene and reference name*/
      cpStr = buffStr;

      while(*cpStr++ != '\t') ;
      while(*cpStr++ != '\t') ;

      /*Sets to 1 if I have an '-', else it is 0*/
      revGene_heapAryBl[posI] = (*cpStr == '-');

      while(*cpStr++ != '\t') ;

      cpStr =
         base10StrToSI(cpStr, startGene_heapArySI[posI]);

      --startGene_heapArySI[posI]; /*Convert to index 0*/
      ++cpStr; /*Get off the tab*/

      cpStr  =
         base10StrToSI(cpStr, endGene_heapArySI[posI]);

      --endGene_heapArySI[posI]; /*Convert to index 0*/

      ++posI;
   } /*Loop: Read in the file*/

   fclose(tmpFILE);
   tmpFILE = 0;

   threeArySortNumeric(
      startGene_heapArySI,
      endGene_heapArySI,
      revGene_heapAryBl,
      0,
      posI - 1
   ); /*Sort the genes by starting position*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-07 Sec-03:
   ^   - Read in the reference sequence
   ^   o fun-07 sec-02 sub-01:
   ^     - Open the reference sequence file
   ^   o fun-07 sec-02 sub-02:
   ^     - Find the reference sequence length
   ^   o fun-07 sec-02 sub-03:
   ^     - Allocate memory for the reference sequence
   ^   o fun-07 sec-02 sub-04:
   ^     - Get and clean up the reference sequence
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   /*****************************************************\
   * Fun-07 Sec-02 Sub-01:
   *   - Open the reference file
   \*****************************************************/

   tmpFILE = fopen(refFileStr, "r");

   if(! tmpFILE) goto fileErr_fun07_sec06_sub03;

   /*****************************************************\
   * Fun-07 Sec-02 Sub-02:
   *   - Find the length of the reference sequence
   \*****************************************************/
  
   fgets(buffStr, lenBuffUS, tmpFILE);
   bytesInBuffUL = ftell(tmpFILE); /*record header*/
   fseek(tmpFILE, 0, SEEK_END);
   lenRefUL = ftell(tmpFILE);

   lenRefUL -= bytesInBuffUL;/*Rough length of reference*/
   /*This assumes that there is only one sequence in the
   `   file. Memory usage will be bad for multiple.
   */
   
   /*****************************************************\
   * Fun-07 Sec-02 Sub-03:
   *   - Allocate memory for the reference sequence
   \*****************************************************/

   refSeqHeapStr = malloc((lenRefUL + 1) * sizeof(char));
   if(! refSeqHeapStr) goto memErr_fun07_sec06_sub02;

   /*****************************************************\
   * Fun-07 Sec-02 Sub-04:
   *   - Get and clean up the reference sequence
   \*****************************************************/

   /*Go back to the start of the sequence, but skip the
   `   header
   */
   fseek(tmpFILE, bytesInBuffUL, SEEK_SET);

   bytesInBuffUL =
      fread(refSeqHeapStr,sizeof(char),lenRefUL,tmpFILE);

   refSeqHeapStr[lenRefUL + 1] = '\0';

   lenRefUL = 0;
   cpStr = refSeqHeapStr;

   while(*cpStr != '\0')
   { /*Loop: Remove white space*/
      /*This is just a gaurd for multiple sequences*/
      if(*cpStr == '>') break;

      refSeqHeapStr[lenRefUL] = *cpStr;
      lenRefUL += (*cpStr > 32);
      ++cpStr;
   } /*Loop: Remove white space*/

   refSeqHeapStr[lenRefUL] = '\0';

   fclose(tmpFILE);
   tmpFILE = 0;

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-07 Sec-04: whoAddCodonPos
   ^   - Add in the gene coordinates and ammino acid
   ^     sequences 
   ^   o fun-07 sec-04 sub-01:
   ^     - Move past the entire gene effects
   ^   o fun-07 sec-04 sub-02:
   ^     - Find the gene the AMR is on
   ^   o fun-07 sec-04 sub-03:
   ^     - Check if AMR is on multiple genes/find primary
   ^   o fun-07 sec-04 sub-04:
   ^     - Add in the gene starting positions
   ^   o fun-07 sec-04 sub-05:
   ^     - Processing for reverse complement AMR genes
   ^   o fun-07 sec-04 sub-06:
   ^     - Add in large duplication events
   ^   o fun-07 sec-04 sub-07:
   ^     - Clean up allocated memory
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   /*****************************************************\
   * Fun-07 Sec-04 Sub-01:
   *   - Move past the entire gene effects
   \*****************************************************/

    /*Check if this has had a reference position
    `   assigned. 0 likely means it has not had an
    `   reference position assigned
    */
    for(iIndex = 0; iIndex < numAmrI; ++iIndex)
       if(amrST[iIndex].refPosUI) break;

   /*****************************************************\
   * Fun-07 Sec-04 Sub-02:
   *   - Find the gene the AMR is on
   \*****************************************************/

    posI = 0;

    for(iIndex = iIndex; iIndex < numAmrI; ++iIndex)
    { /*Loop: Assign aa sequences to each AMR*/

       /*Find the position of the AMRs gene*/
       while(
            endGene_heapArySI[posI++]
          < amrST[iIndex].refPosUI
       ) if(posI > numGenesI) break;

       --posI; /*I am always one gene off*/

       if(posI >= numGenesI) break; /*Finished*/

      /**************************************************\
      * Fun-07 Sec-04 Sub-03:
      *   - Check if AMR is on multiple genes/find primary
      \**************************************************/

       /*Check I the sequence is inside gene bounds*/
       tmpUI =
            amrST[iIndex].refPosUI
          + amrST[iIndex].lenRefSeqUI;

       if(tmpUI >= endGene_heapArySI[posI])
       { /*If: I have sequences outside of gene bounds*/
          /*See if this AMR is really on the next gene
          `   by comparing it to the last/next AMR
          */
          tmpUI =
             cStrEql(
                amrST[iIndex].geneIdStr,
                amrST[
                    iIndex - (iIndex > 1) + (iIndex < 1)
                  ].geneIdStr,
                '\0'
             ); /*Check if I have the same ids*/

          /*1 if is an different gene*/
          outBoundsBl = !!tmpUI;
          posI += outBoundsBl;
       } /*If: I have sequences outside of gene bounds*/

      /**************************************************\
      * Fun-07 Sec-04 Sub-04:
      *   - Add in the gene starting positions
      \**************************************************/

       /*Make sure the start is the first mapped base*/
       if(  startGene_heapArySI[posI]
          < endGene_heapArySI[posI]
       ){ /*If: the start comes first*/
          amrST[iIndex].geneFirstRefUI =
             startGene_heapArySI[posI];

          amrST[iIndex].geneLastRefUI=
             endGene_heapArySI[posI];
       } /*If: the start comes first*/

       else
       { /*Else: the start comes last*/
         amrST[iIndex].geneLastRefUI =
            startGene_heapArySI[posI];

         amrST[iIndex].geneFirstRefUI =
            endGene_heapArySI[posI];
       } /*Else: the start comes last*/

      /**************************************************\
      * Fun-07 Sec-04 Sub-05:
      *   - Processing for reverse complement AMR genes
      *   o fun-07 sec-04 sub-05 cat-01:
      *     - Add in the direction & check if aa sequence
      *   o fun-07 sec-04 sub-05 cat-02:
      *     - Find codon position & check aa seq length
      *   o fun-07 sec-04 sub-05 cat-03:
      *     - Get reference amino acid sequence for
      *       AMR(s) in reverse complement genes
      \**************************************************/

       /*++++++++++++++++++++++++++++++++++++++++++++++++\
       + Fun-07 Sec-04 Sub-05 Cat-01:
       +   - Add in the direction and check if aa sequence
       \++++++++++++++++++++++++++++++++++++++++++++++++*/

       if(revGene_heapAryBl[posI])
       { /*If: This is an reverse complement gene*/
          amrST[iIndex].dirFlag = def_amrST_revCompDir;

          /*Check if this is an aa mutation*/
          if(amrST[iIndex].codonNumUI == 0) continue;

          /*+++++++++++++++++++++++++++++++++++++++++++++\
          + Fun-07 Sec-04 Sub-05 Cat-02:
          +   - Find codon position & check aa seq length
          \+++++++++++++++++++++++++++++++++++++++++++++*/

          /*Update the starting position*/
          /*This assumes the codon number is correct. I
          `   had one case (pncA_p.Ter187Argext*?) were I
          `   am not sure if this was true. Ref: 2288680,
          `   codon: 187. The output was 2288675, which
          `   is off
          */
          amrST[iIndex].codonPosUI =
               endGene_heapArySI[posI]
             - ((amrST[iIndex].codonNumUI - 1) * 3);
          
          if(amrST[iIndex].endCodonNumUI == 0)
          { /*If: this is a single aa change*/
             amrST[iIndex].endCodonNumUI =
                amrST[iIndex].codonNumUI;

             continue;
          } /*If: this is a single aa change*/

          /*Check to see if more than 1 or 2 aa long*/
          tmpUI =
               amrST[iIndex].endCodonNumUI
             - amrST[iIndex].codonNumUI;

          if(tmpUI <2) continue;/*< 2 aa; already set up*/

          /*+++++++++++++++++++++++++++++++++++++++++++++\
          + Fun-07 Sec-04 Sub-05 Cat-03:
          +   - Get reference amino acid sequence for
          +     AMR(s) in reverse complement genes
          \+++++++++++++++++++++++++++++++++++++++++++++*/

          /*Set up for getting the reference amino
          `   acids; the +1 is to ignore the first amino
          `   acid in the deletion (I already have it)
          */
          dupStr = amrST[iIndex].refAaStr + 1;

          /*-3 to get off the first codon*/
          cpStr =
             refSeqHeapStr + amrST[iIndex].codonPosUI - 3;

          for(
             iCodon = amrST[iIndex].codonNumUI + 1;
             iCodon < amrST[iIndex].endCodonNumUI;
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
       } /*If: This is an reverse complement gene*/

      /**************************************************\
      * Fun-07 Sec-04 Sub-06:
      *   - Processing for forward AMR genes
      *   o fun-07 sec-04 sub-06 cat-01:
      *     - Add in the direction & check if aa sequence
      *   o fun-07 sec-04 sub-06 cat-02:
      *     - Find codon position & check aa seq length
      *   o fun-07 sec-04 sub-06 cat-03:
      *     - Get reference amino acid sequence for
      *       AMR(s) in reverse complement genes
      \**************************************************/

       /*++++++++++++++++++++++++++++++++++++++++++++++++\
       + Fun-07 Sec-04 Sub-06 Cat-01:
       +   - Add in the direction and check if aa sequence
       \++++++++++++++++++++++++++++++++++++++++++++++++*/

       else
       { /*Else: This is an foward gene*/
          amrST[iIndex].dirFlag = def_amrST_forwardDir;

          /*Check fi ther is an aa mutation*/
          if(amrST[iIndex].codonNumUI == 0) continue;

          /*+++++++++++++++++++++++++++++++++++++++++++++\
          + Fun-07 Sec-04 Sub-06 Cat-02:
          +   - Find codon position & check aa seq length
          \+++++++++++++++++++++++++++++++++++++++++++++*/

          /*Update the starting position*/
          /*This assumes the codon number is correct. I
          `   had one case (pncA_p.Ter187Argext*?) were I
          `   am not sure if this was true. Ref: 2288680,
          `   codon: 187. The output was 2288675, which
          `   is off. This was an reverse case
          */
          amrST[iIndex].codonPosUI =
               startGene_heapArySI[posI]
             + ((amrST[iIndex].codonNumUI - 1) * 3);

          if(amrST[iIndex].endCodonNumUI == 0)
          { /*If: this is a single aa change*/
             amrST[iIndex].endCodonNumUI =
                amrST[iIndex].codonNumUI;

             continue;
          } /*If: this is a single aa change*/

          /*Check to see if more than 1 or 2 aa long*/
          tmpUI =
               amrST[iIndex].endCodonNumUI
             - amrST[iIndex].codonNumUI;

          if(tmpUI <2) continue;/*< 2 aa; already set up*/

          /*+++++++++++++++++++++++++++++++++++++++++++++\
          + Fun-07 Sec-04 Sub-06 Cat-03:
          +   - Get reference amino acid sequence for
          +     AMR(s) in reverse complement genes
          \+++++++++++++++++++++++++++++++++++++++++++++*/

          /*Set up for getting the reference amino
          `   acids; the +1 is to ignore the first amino
          `   acid in the deletion (I already have it)
          */
          dupStr = amrST[iIndex].refAaStr + 1;

          /*+3 to get off the first codon*/
          cpStr =
             refSeqHeapStr + amrST[iIndex].codonPosUI + 3;

          for(
             iCodon = amrST[iIndex].codonNumUI + 1;
             iCodon < amrST[iIndex].endCodonNumUI;
             ++iCodon
          ){ /*Loop: Copy the deletion*/
             oneNtUC =
                (uchar) baseToCodeLkTbl[(uchar) *cpStr++];

             twoNtUC =
                (uchar) baseToCodeLkTbl[(uchar) *cpStr++];

             threeNtUC =
                (uchar) baseToCodeLkTbl[(uchar) *cpStr++];

             *dupStr++ =
               codonLkTbl[oneNtUC][twoNtUC][threeNtUC];
          } /*Loop: Copy the deletion*/
       } /*Else: This is an foward gene*/

      /**************************************************\
      * Fun-07 Sec-04 Sub-07:
      *   - Add in large duplication events
      \**************************************************/

       if(amrST[iIndex].aaMultiDupBl)
       { /*If: this is a large duplicate*/
          dupStr = amrST[iIndex].amrAaStr;
          cpStr = amrST[iIndex].refAaStr;

          while(*cpStr) *dupStr++ = *cpStr++;

          cpStr = amrST[iIndex].refAaStr;
          while(*cpStr) *dupStr++ = *cpStr++;

          *dupStr = '\0';
       } /*If: this is a large duplicate*/
    } /*Loop: Assign aa sequences to each AMR*/

   /*****************************************************\
   * Fun-07 Sec-04 Sub-08:
   *   - Clean up the allocated memory
   \*****************************************************/

   free(revGene_heapAryBl);
   free(startGene_heapArySI);
   free(endGene_heapArySI);

   revGene_heapAryBl = 0;
   startGene_heapArySI = 0;
   endGene_heapArySI = 0;

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-07 Sec-05:
   ^   - Add in the whole gene target data
   ^   o fun-07 sec-05 sub-01:
   ^   o fun-07 sec-05 sub-02:
   ^     - Add in the genes sequecne
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   /*****************************************************\
   * Fun-07 Sec-05 Sub-01:
   *   - Set up the gene coordinates for deletions
   \*****************************************************/

   geneIdSortAmrSTAry(amrST, 0, numAmrI - 1);

   for(iIndex = 0; iIndex < numAmrI; ++iIndex)
   { /*Loop: Add gene start & end to entire gene events*/
      if(amrST[iIndex].wholeGeneFlag)
      { /*If: This is an entire gene event*/
         if(   iIndex > 1
            && (! cStrEql(
                 amrST[iIndex].geneIdStr,
                 amrST[iIndex - 1].geneIdStr,
                 '\0'
              ))
         ){ /*If: I the last amr is the same gene*/
            amrST[iIndex].geneFirstRefUI =
               amrST[iIndex - 1].geneFirstRefUI;

            amrST[iIndex].geneLastRefUI =
               amrST[iIndex - 1].geneLastRefUI;

            amrST[iIndex].dirFlag =
               amrST[iIndex - 1].dirFlag;

            if(
                 amrST[iIndex].wholeGeneFlag
               & def_geneDel_amrST
            ){ /*If: Add in coordinate for gene deletion*/
               /*I want to grab the base before the gene*/
               amrST[iIndex].refPosUI =
                  amrST[iIndex - 1].geneFirstRefUI - 1;
            } /*If: Add in coordinate for gene deletion*/
         } /*If: I the last amr is the same gene*/

         else
         { /*Else: The AMR is same as the next gene*/
            amrST[iIndex].geneFirstRefUI =
               amrST[iIndex + 1].geneFirstRefUI - 1;

            amrST[iIndex].geneLastRefUI =
               amrST[iIndex + 1].geneLastRefUI;

            amrST[iIndex].dirFlag =
               amrST[iIndex + 1].dirFlag;

            if(
                 amrST[iIndex].wholeGeneFlag
               & def_geneDel_amrST
            ){ /*If: Add in coordinate for gene deletion*/
               /*I want to grab the base before the gene*/
               amrST[iIndex].refPosUI =
                  amrST[iIndex + 1].geneFirstRefUI - 1;
            } /*If: Add in coordinate for gene deletion*/
         } /*Else: The AMR is same as the next gene*/

         if(!
            (  amrST[iIndex].wholeGeneFlag
             & def_geneDel_amrST
            )
         ) continue; /*If this is not an gene deletion*/

         /***********************************************\
         * Fun-07 Sec-05 Sub-02:
         *   - Add in the genes sequecne
         *   o fun-07 sec-05 sub-02 cat-01:
         *     - copy gene + base before gene to reference
         *   o fun-07 sec-05 sub-02 cat-02:
         *     - copy the base before the gene to the AMR
         \***********************************************/

         /*++++++++++++++++++++++++++++++++++++++++++++++\
         + Fun-07 Sec-05 Sub-02 Cat-01:
         +   - copy gene + base before gene into reference
         \++++++++++++++++++++++++++++++++++++++++++++++*/

         /*Get the length of the deletion*/
         tmpUI =
              amrST[iIndex].geneLastRefUI
            - amrST[iIndex].geneFirstRefUI;

         if(amrST[iIndex].refSeqStr)
            free(amrST[iIndex].refSeqStr);

         amrST[iIndex].refSeqStr = 0;

         ++tmpUI; /*Account for base before gene*/

         amrST[iIndex].refSeqStr =
            malloc((tmpUI + 1) * sizeof(char));

         if(! amrST[iIndex].refSeqStr)
            goto memErr_fun07_sec06_sub02;

         amrST[iIndex].lenRefSeqUI = tmpUI;

         ulCpStr(
            amrST[iIndex].refSeqStr,
            &refSeqHeapStr[ amrST[iIndex].refPosUI ],
            amrST[iIndex].lenRefSeqUI
         ); /*Copy the genes sequence + 1 base*/

         /*++++++++++++++++++++++++++++++++++++++++++++++\
         + Fun-07 Sec-05 Sub-02 Cat-02:
         +   - copy the base before the gene to the AMR
         \++++++++++++++++++++++++++++++++++++++++++++++*/

         if(amrST[iIndex].amrSeqStr)
            free(amrST[iIndex].amrSeqStr);

         amrST[iIndex].amrSeqStr = 0;

         amrST[iIndex].amrSeqStr =
            malloc(2 * sizeof(char));

         if(! amrST[iIndex].amrSeqStr)
            goto memErr_fun07_sec06_sub02;

         amrST[iIndex].amrSeqStr[0] =
            refSeqHeapStr[amrST[iIndex].refPosUI];

         amrST[iIndex].amrSeqStr[1] = '\0';
      } /*If: This is an entire gene event*/
   } /*Loop: Add gene start & end to entire gene events*/

   /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
   ^ Fun-07 Sec-06:
   ^   - Clean up and return
   ^   o fun-07 sec-06 sub-01:
   ^     - Handle clean up/return for no errors
   ^   o fun-07 sec-06 sub-02:
   ^     - Handle memory errors
   ^   o fun-07 sec-06 sub-03:
   ^     - Handle file errors
   ^   o fun-07 sec-06 sub-04:
   ^     - Clean up for errors
   \<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

   /*****************************************************\
   * Fun-07 Sec-06 Sub-01:
   *   - Handle clean up/return for no errors
   \*****************************************************/

   free(refSeqHeapStr);
   refSeqHeapStr = 0;

   /*Resort by starting position*/
   sortAmrStructArray(amrST, 0, numAmrI - 1); 

   return 0; /*No errors*/

   /*****************************************************\
   * Fun-07 Sec-06 Sub-02:
   *   - Handle memory errors
   \*****************************************************/

   memErr_fun07_sec06_sub02:;

   retErrC = def_amrST_memError;

   goto errCleanUp_fun07_sec06_sub04;

   /*****************************************************\
   * Fun-07 Sec-06 Sub-03:
   *   - Handle file errors
   \*****************************************************/

   fileErr_fun07_sec06_sub03:;

   retErrC = def_amrST_invalidFILE;

   goto errCleanUp_fun07_sec06_sub04;

   /*****************************************************\
   * Fun-07 Sec-06 Sub-04:
   *   - Clean up for errors
   \*****************************************************/

   errCleanUp_fun07_sec06_sub04:;

   /*Make sure the AMR array is sorted correctly*/
   sortAmrStructArray(amrST, 0, numAmrI - 1); 

   if(refSeqHeapStr) free(refSeqHeapStr);
   refSeqHeapStr = 0;

   if(revGene_heapAryBl) free(revGene_heapAryBl);
   revGene_heapAryBl = 0;

   if(startGene_heapArySI) free(startGene_heapArySI);
   startGene_heapArySI = 0;

   if(endGene_heapArySI) free(endGene_heapArySI);
   endGene_heapArySI = 0;

   if(tmpFILE) fclose(tmpFILE);
   tmpFILE = 0;

   return retErrC;
} /*whoAddCodonPos*/

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
