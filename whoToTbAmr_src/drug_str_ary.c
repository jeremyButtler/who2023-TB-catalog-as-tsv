#include "drug_str_ary.h"
#include <stdlib.h>

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
char * mallocDrugAryStr(unsigned int numStringsUI){
   return
      malloc(numStringsUI * defMaxDrugLen * sizeof(char));
} /*mallocDrugAryStr*/

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
){ /*reallocDrugAryStr*/
   char * tmpMacStr = 0;

   tmpMacStr =
      realloc(
         (*drugAryStr),
         (numStrUI) * defMaxDrugLen * sizeof(char)
      );
   
   if(!tmpMacStr)
   { /*If: I had a memory error*/
      free((*drugAryStr));
      (*drugAryStr) = 0;
   } /*If: I had a memory error*/
   
   else (*drugAryStr) = tmpMacStr;
} /*reallocDrugAryStr*/

/*-------------------------------------------------------\
| Fun-12: reallocDrugAryStrNoFree
|   - Returns a pointer to rellocated memory for
|     drugAryStr, but does not free drugStrAry when there
|     is a memory error
| Input:
|   - drugAryStr:
|     o Pionter to drugAryStr to reallocate memory for
|   - numStringsUI:
|     o Number of strings to make
| Output:
|   - Modifies:
|     o drugAryStr to be resized
|     o frees and sets drugAryStr to 0 if memory error
|   - Returns:
|     o 0 for no problems
|     o 1 for memory errors
\-------------------------------------------------------*/
char reallocDrugAryStrNoFree(
   char **drugAryStr,
   unsigned int numStrUI
){ /*reallocDrugAryStrNoFree*/
   char * tmpMacStr = 0;

   tmpMacStr =
      realloc(
         (*drugAryStr),
         (numStrUI) * defMaxDrugLen * sizeof(char)
      );
   
   if(!tmpMacStr) return 1;
   
  return 0;
} /*reallocDrugAryStrNoFree*/

