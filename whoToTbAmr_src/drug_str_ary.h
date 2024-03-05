#ifndef DRUG_STRING_ARRAY_H
#define DRUG_STRING_ARRAY_H

#define defMaxDrugLen 32

/*-------------------------------------------------------\
| Fun-10: mallocDrugAryStr
|   - Returns a pointer to allocated memory for drugAryStr
| Input:
|   - numStringsUI:
|     o Number of strings to make
| Output:
|   - Returns:
|     o An pointer to the new arrray
|     o 0 for memory error
\-------------------------------------------------------*/
char * mallocDrugAryStr(unsigned int numStringsUI);

/*-------------------------------------------------------\
| Fun-11: reallocDrugAryStr
|   - Returns a pointer to rellocated memory for
|     drugAryStr
| Input:
|   - drugAryStr:
|     o Pionter to drugAryStr to reallocate memory for
|   - numStringsUI:
|     o Number of strings to make
| Output:
|   - Modifies:
|     o drugAryStr to be resized
|     o frees and sets drugAryStr to 0 if memory error
\-------------------------------------------------------*/
void reallocDrugAryStr(
   char **drugAryStr,
   unsigned int numStrUI
);

/*-------------------------------------------------------\
| Fun-12: cpDrugToDrugAry
|   - Copies an antibiotic to a drug array
| Input:
|   - drugAryStr:
|     o Drug array to copy the antibiotic name to
|   - drugStr:
|     o Drug to copy
|   - indexUI:
|     o Index to copy drug to
|   - delimC:
|     o Deliminator to stop copy at
| Output:
|   - Modifies:
|     o dubAryStr to hold the new drug name
|   - Returns:
|     o The length of the copied string
\-------------------------------------------------------*/
#define cpDrugToDrugAry(\
   drugAryStr,\
   drugStr,\
   indexUI,\
   delimC\
)({\
   char *dupMacStr =\
       (drugAryStr) + (defMaxDrugLen * (indexUI));\
   \
   int iMacChar = 0;\
   \
   for(iMacChar=0; iMacChar < defMaxDrugLen; ++iMacChar)\
   { /*Loop: Copy over the drug names*/\
      if((drugStr)[iMacChar] == (delimC)) break;\
      dupMacStr[iMacChar] = (drugStr)[iMacChar] | 32;\
   } /*Loop: Copy over the drug names*/\
   \
   dupMacStr[iMacChar] = '\0';\
   iMacChar;\
}) /*cpDrugToDrugAry*/

/*-------------------------------------------------------\
| Fun-13: getDrugFromDrugAry
|   - Gets the pointer to a drug c-string in a drugAry
| Input:
|   - drugAryStr:
|     o Drug array to get the antibiotic name pointer
|   - indexUI:
|     o Index to grab
| Output:
|   - Returns:
|     o An pointer to the correct position
|       resitant (grade 1 and 2) mutations
\-------------------------------------------------------*/
#define getDrugFromDrugAry(drugAryStr, indexUI)(\
   (char *) (drugAryStr) + ((indexUI) * defMaxDrugLen)\
) /*getDrugFromDrugAry*/

/*-------------------------------------------------------\
| Fun-14: findDrug_in_drugAryStr()
|   - Finds an antibiotic in a drugAray c-string
| Input:
|   - qryStr:
|     o Drug to search for in drugAryStr
|   - drugAryStr:
|     o Drug array (c-string; treated as char[][32]) to
|       get the antibiotic name pointer
|   - numDrugsI:
|     o Number of antibiotics in drugAryStr
| Output:
|   - Returns:
|     o index of the drug if qryStr is in drugAryStr
|     o -1 if the drug is not in drugAry
| Note:
|    - Do to the way this is procesed, I can not use a
|      fast search method like a binary search, since
|      (there is no garuntee of order).
\-------------------------------------------------------*/
#define findDrug_in_drugAryStr(\
   qryStr,     /*Drug to search for*/\
   drugAryStr, /*Array of drugs to search*/\
   numDrugsI,  /*Number of drugs in array*/\
   delimC      /*Deliminator to stop at*/\
)({ /*findDrug_in_durgAryStr*/\
   int iIndexMac = 0;\
   char *drugMacStr = 0;\
   char *qryMacStr = (qryStr);\
   \
   for(iIndexMac=0; iIndexMac < (numDrugsI); ++iIndexMac)\
   { /*Loop: find qryStr in drugAryStr*/\
      drugMacStr =\
         getDrugFromDrugAry((drugAryStr), iIndexMac);\
      \
      while((*qryMacStr | 32) == (*drugMacStr | 32))\
      { /*Loop: Check if this is the same string*/\
         if((*drugMacStr=='\0') | (*qryMacStr==(delimC)))\
            break;\
         ++qryMacStr;\
         ++drugMacStr;\
      } /*Loop: Check if this is the same string*/\
      \
      if((*drugMacStr=='\0') & (*qryMacStr==(delimC)))\
         break;\
      \
      qryMacStr = (qryStr);\
   } /*Loop: find qryStr in drugAryStr*/\
   \
   iIndexMac | ( (int) -(iIndexMac >= (numDrugsI)) );\
      /*Logic:
      `   - iIndexMac >= numDrugsI
      `     o 1 when I did not find the query
      `     o 0 when I found the query
      `   - -(iIndexMac >= numDrugsI)
      `     o Changes 1 to -1 (all bits set) and 0 to 0
      `       (no bits set)
      `   - iIndexMac | -(iIndexMac >= numDrugsI)
      `     o Converts iIndexMac to -1 when the query was
      `       not found [-(iIndexMac >= numDragsI) is -1]
      `     o Does nothing to iIndexMac when the query was
      `       found [-(iIndexMac >= numDragsI) is 0]
      */\
}) /*findDrug_in_drugAryStr*/

#endif
