/*########################################################
# Name: ulCpStr
#   - Copies contents of string 1 to string two using
#     unsigned longs (poor mans vectors)
#   - These functions are slower than strcpy and strlen,
#     but they allow deliminators to be used.
########################################################*/

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
|  o header:
|    - Included libraries, definitions and preprocessor
|      checks
|  o fun-01: ulCpStr
|    - Copies cpStr into dupStr using unsigned longs
|  o fun-02: ulCpMakeDelim
|    - Makes an unsigned long delimintor from a character
|      deliminator for use in cpStrDelim
|  o fun-03: ulIfDelim
|    - Detect if the input deliminator is in the input
|      unsigned long
|    - This is a bit slow (9 op), 8 if delimUL can be
|      deterimined at compile time. This is less efficent
|      on 32 and 16 bit cpus (no longer borderline).
|  o fun-06: ulVectCpStrDelim TODO:
|    - ulCpStrDelim, but can be compiled with vectors
|  o fun-06: cCpStr
|   - Copies cpStr into dupStr using characters
|  o fun-07: cCpStrDelim
|    - Copies cpStr into dupStr until delimC is found
|  o fun-08: cLenStr
|    - Finds the length of a string using characters
|  o fun-0?: cStrEql
|    - Checks to see if two strings are equal
\~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*-------------------------------------------------------\
| Header:
|   - Included libraries, definitions and preprocessor
|     checks
\-------------------------------------------------------*/

#ifndef UNSINGED_LONG_COPY_STRING_H
#define UNSINGED_LONG_COPY_STRING_H

#define defBitsPerChar 8
#define defNullDelim 0

#define defCharPerUL sizeof(unsigned long)
#define defModByUL (sizeof(unsigned long )- 1)
   /*The mod values are 7 (longs), 3 (ints), and 1
   `  (shorts), so, 1 - number of bytes will give me the
   `  mod value
   */

/*This should be compliled in.
`  This will only work for up to 64 bit numbers, after
`  that it will fail. However, it is very unlikely a
`  long will by 128 bits.
*/
#define defShiftByUL (\
     (sizeof(unsigned long) >> 1)\
   - (sizeof(unsigned long) >> 3)\
)
/*Logic:
`   - sizeof(unsigned long) >> 1:
`     o This gets the number of bytes to shift to get
`       an 32 bit or 16 bit number to one bytes. Basically
`       it is divide by 2
`   - (sizeof(unsigned long) >> 3)
`     o This adds in 1 if I have eight bytes. It is needed
`       as the 64 bit correction step
`   - (sizeof(long) >> 1) - (sizeof(long) >> 3)
`     o Gives me the number of bits I need to shift to
`       get the number of characters in an long
`   - (sizeof(long) >> 3) << (sizeof(long) >> 4)
`     o This is not used, but would correct for a 128 bit
`       number. Each time I increase the bit size I need
`       to add a correction in
*/

/* This will work for any number, but might be slower if
`    it is not compiled in. It also needs to be called
`    like a function
*/
/*
#define defShiftByUL()({\
   unsigned int tmpNumUI = 0;\
   unsigned int retUI = 0;\
   unsigned int shiftUI = 0;\
   \
   retUI = (sizeof(unsigned long) > 0xF) << 2;\
   tmpNumUI = sizeof(unsigned long) >> retUI;\
   \
   shiftUI = (tmpNumUI > 0x3) << 1;\
   tmpNumUI >>= shiftUI;\
   retUI |= shiftUI;\
   \
   retUI | (tmpNumUI >> 1);\
})
*/

/*-------------------------------------------------------\
| Fun-01: ulCpStr
|   - Copies cpStr into dupStr using unsigned longs
| Input:
|   - dupStr:
|     o Pointer to string to copy cpStr into
|   - cpStr:
|     o Pointer to string to copy
|   - lenUI:
|     o length of cpStr
| Output:
|   - Modifies:
|     o  dupStr to hold lenUI characters from cpStr
\-------------------------------------------------------*/
#define ulCpStr(dupStr, cpStr, lenUI){\
   unsigned long *macCpUL = (ulong *) (cpStr);\
   unsigned long *macDupUL = (ulong *) (dupStr);\
   unsigned int uiCharMac = 0;\
   \
   for(\
      uiCharMac= 0;\
      uiCharMac < ((lenUI) >> defShiftByUL);\
      ++uiCharMac\
   ) macDupUL[uiCharMac] = macCpUL[uiCharMac];\
   \
   for(\
      uiCharMac = ((lenUI) - ((lenUI) & defModByUL));\
      uiCharMac < (lenUI);\
     ++uiCharMac\
   ) (dupStr)[uiCharMac] = (cpStr)[uiCharMac];\
   \
   (dupStr)[lenUI] = '\0';\
   /*Faster than doing *val1++ = *val2++*/\
} /*ulCpStr*/

/*-------------------------------------------------------\
| Fun-02: ulCpMakeDelim
|   - Makes an unsigned long delimintor from a character
|     deliminator for use in cpStrDelim
| Input:
|   - delimC:
|     o Deliminator to use to end a string copy
| Output:
|   - Modifies:
|     o  dupStr to hold the characters from cpStr
\-------------------------------------------------------*/
#define ulCpMakeDelim(delimC)({\
   int iRepMac = 0;\
   unsigned long macDelimUL = (ulong) (delimC);\
   \
   for(\
      iRepMac = sizeof(unsigned long) << 2;\
      iRepMac > 0;\
      iRepMac >>= 1\
   ) macDelimUL |= (macDelimUL << iRepMac);\
   \
   macDelimUL;\
}) /*ulMakeDelim*/
   /*Logic:
   `   - sizeof(unsigned long) << 2:
   `     o Gets me to the half way point in bits. So
   `       8 (64 bit) becomes 32, 4 (32 bit) becomes 16,
   `       an 2 (16 bit) becomes 8
   `   - iRepMac << 1:
   `     o Divides the shift range by 2 each round, so
   `       that each round fills in a different part of
   `       the long
   `   - macDelimUL |= (macDelimUL << iRepMac);
   `     o Moves the deliminator up (shifts) to an
   `       unfilled position.
   */

/*-------------------------------------------------------\
| Fun-03: ulIfDelim
|   - Detect if the input deliminator is in the input
|     unsigned long
| Variations:
|   - SAFE_DELIM uses 9 to 8 OP for 64 bit, 7 to 6 OP for
|     for 32 bit, and 5 to 4 OP for 16 bit. The amount
|     depends on if the deliminator is hardcoded or not.
|   - ifndef SAFE_DELIM takes 5 op no matter what your
|     OS type is. You could reduce it down by removing the
|     (addUL & 1) + addUL step, but this does cause some
|     issues with values that are one off the actual. Tab
|     is safe, since it is odd, but not comma or space.
| Input:
|   - strUL:
|     o Unsigned long to check for the delimintor in
|   - delimUL:
|     o Unsinged long with delimintor (use ulMakeDelim
|       to get this value)
| Output:
|   - Returns:
|     o  unsigned long > 0; strUL has no deliminator. The
|        position of the delimintor can be found with
|        log2(returned ulong) >> 8. The 1 is shifted to
|        the left of the character.
|     o  unsigned long == 0; strUL has deliminator
\-------------------------------------------------------*/
#define ulIfDelim(strUL, delimUL)({\
   unsigned long macAddUL = \
     ( (ulong) (strUL) ) ^ (delimUL);\
   \
     ( ((macAddUL &1) + macAddUL) - ulCpMakeDelim(0x01) )\
   &\
     ulCpMakeDelim(0x80);\
})
   /*Logic:
   ` - addUL = strUL ^ delimUL:
   `   o Converts all values matching delimUL to 0, and
   `     everything else to > 0. Saves result to addUL
   ` - (addUL & 1) + addUL
   `   o Convets odd numbers to even numbers. This gets
   `     means there is no 1;
   `   o This is needed to ensure -1 will not cause an
   `     overflow when 1 is next to an deliminator. The
   `     & 1 ensures that -1 will alwas remove the sign
   `     bit, if one character was one off.
   ` - ((addUL & 1) + addUL) - defOne
   `   o subtracts one from every character. This results
   `     in an overflow for only 0, while every other
   `     value is >= 1, but can go to 0 if next number
   `     overflowed. This sets the value 0 numbers to -1
   ` - (( (addUL & 1) + addUL) - defOne) & defHiBit
   `   o This keeps only the overflow bit (sign bit). This
   `     means that every non-deliminator value becomes
   `     0.
   */
   
/*-------------------------------------------------------\
| Fun-04: ulCpStrDelim
|   - Copies cpStr into dupStr until delimC is found
| Input:
|   - dupStr:
|     o Pointer to string to copy cpStr into
|   - cpStr:
|     o Pointer to string to copy
|   - delimUL:
|     o Deliminatro to end at (as long). Use makeULDelim
|       to build this deliminator
| Output:
|   - Modifies:
|     o  dupStr to hold the characters from cpStr
| Note:
|   - This will likely not be very good at copying short
|     strings.
\-------------------------------------------------------*/
#define ulCpStrDelim(dupStr, cpStr, delimUL, delimC)({\
   unsigned long *macCpUL = (ulong *) (cpStr);\
   unsigned long *macDupUL = (ulong *) (dupStr);\
   char *dupMacStr = 0;\
   char *cpMacStr = 0;\
   unsigned long macAtDelimUL = 0;\
   unsigned int uiCharMac = 0;\
   \
   macAtDelimUL =ulIfDelim(*macCpUL, (delimUL));\
   \
   while(!macAtDelimUL)\
   { /*Loop: Copy cpStr to dupStr*/\
      *macDupUL++ = *macCpUL++;\
      macAtDelimUL = ulIfDelim(*macCpUL, (delimUL));\
   } /*Loop: Copy cpStr to dupStr*/\
   \
   cpMacStr = (char *) macCpUL;\
   dupMacStr = (char *) macDupUL;\
   /*Loop: Till macAtDelimUL says there is no longer
   ` a deliminator. The last character in the string
   ' is at the end
   */\
   while(*cpMacStr != (delimC))\
      *dupMacStr++ = *cpMacStr++;\
   \
   uiCharMac = dupMacStr - (dupStr);\
   (dupStr)[uiCharMac] = '\0';\
   uiCharMac; /*the number of characters copied*/\
}) /*ulCpStrDelim*/

/*-------------------------------------------------------\
| Fun-05: ulLenStr
|   - Finds the length of a string using unsigned longs
| Input:
|   - inStr:
|     o C-string or string with deliminator to find length
|       of
|   - delimUL:
|     o Deliminator marking the end of the string
| Output:
|   - Returns:
|     o Number of characters in the string
\-------------------------------------------------------*/
#define ulLenStr(inStr, delimUL, delimC)({\
   unsigned long *macPtrUL = (ulong *) (inStr);\
   unsigned long macAtDelimUL = 0;\
   unsigned int uiLenStrMac = 0;\
   \
   macAtDelimUL = ulIfDelim(*macPtrUL, (delimUL));\
   \
   while(!macAtDelimUL)\
   { /*Loop: Copy cpStr to dupStr*/\
      uiLenStrMac += 8;\
      ++macPtrUL;\
      macAtDelimUL = ulIfDelim(*macPtrUL, (delimUL));\
   } /*Loop: Copy cpStr to dupStr*/\
   \
   while((inStr)[uiLenStrMac] !=(delimC)) ++uiLenStrMac;\
   \
   uiLenStrMac;\
}) /*ulCpStrDelim*/


/*These are the single byte copy functions*/

/*-------------------------------------------------------\
| Fun-06: cCpStr
|   - Copies cpStr into dupStr using characters
| Input:
|   - dupStr:
|     o Pointer to string to copy cpStr into
|   - cpStr:
|     o Pointer to string to copy
|   - lenUI:
|     o length of cpStr
| Output:
|   - Modifies:
|     o  dupStr to hold lenUI characters from cpStr
\-------------------------------------------------------*/
#define cCpStr(dupStr, cpStr, lenUI){\
   unsigned int uiIterMac = 0;\
   for(uiIterMac = 0; uiIterMac < lenUI; ++uiIterMac)\
      (dupStr)[uiIterMac] = (cpStr)[uiIterMac];\
   (dupStr)[uiIterMac] = '\0';\
}

/*-------------------------------------------------------\
| Fun-07: cCpStrDelim
|   - Copies cpStr into dupStr until delimC is found
| Input:
|   - dupStr:
|     o Pointer to string to copy cpStr into
|   - cpStr:
|     o Pointer to string to copy
|   - delimUL:
|     o Deliminatro to end at (as long). Use makeULDelim
|       to build this deliminator
| Output:
|   - Modifies:
|     o  dupStr to hold the characters from cpStr
\-------------------------------------------------------*/
#define cCpStrDelim(dupStr, cpStr, delimC)({\
   char *dupMacStr = (dupStr);\
   char *cpMacStr = (cpStr);\
   \
   while(*(cpMacStr) != (delimC))\
      *dupMacStr++ = *cpMacStr++;\
   \
   *dupMacStr = '\0';\
   dupMacStr - (dupStr);\
})
/*The *dupStr++ = *cpStr++; is faster than incurmenting
` individually or by number
*/

/*-------------------------------------------------------\
| Fun-08: cLenStr
|   - Finds the length of a string using characters
| Input:
|   - inStr:
|     o C-string or string with deliminator to find length
|       of
|   - delimUL:
|     o Deliminator marking the end of the string
| Output:
|   - Returns:
|     o Number of characters in the string
\-------------------------------------------------------*/
#define cLenStr(inStr, delimC)({\
   char *inMacStr = (inStr);\
   while(*inMacStr != (delimC)) ++inMacStr;\
   inMacStr - (inStr);\
})

/*-------------------------------------------------------\
| Fun-0?: cStrEql
|   - Checks to see if two strings are equal
| Input:
|   - qryStr:
|     o The query c-string to compare against the
|       reference
|   - refStr:
|     o The reference c-string to compare against the
|       query
| Output:
|   - Returns:
|     o < 0 for qry less than reference
|     o 0 for qry is same as the reference
|     o >0 for qry is greater than the reference
\-------------------------------------------------------*/
#define cStrEql(qryStr, refStr, delimC)({\
   int iCntMac = 0;\
   \
   /*This is faster than incurmenting qryMacStr and
   ` refMacStr separately. Doing *val++ == *val2++ does
   ` not work here.
   */\
   while((qryStr)[iCntMac] == (refStr)[iCntMac])\
   { /*Looop*/\
      if((qryStr)[iCntMac] == (delimC)) break;\
      ++iCntMac;\
   } /*Looop*/\
   \
   (qryStr)[iCntMac] - (refStr)[iCntMac];\
   \
   /*this was removed because it caused different behavior
   ` them strcmp
   */\
   /*(-(*(qryStr) !=(delimC))) &(*(qryStr) -*(refStr));*/\
   /*Logic (for old code):
   `  - qryStr != delimC:
   `    o 0 if I have the delimintaor (match)
   `    o 1 if I do not have the delimintaor (no match)
   `  - -(qryStr != delimC):
   `    o Converts 1 to -1 (all bits set) and 0 to 0, no
   `      bits set
   `  - qryStr - refStr
   `    o 0 if the stings were equal
   `    o < 0 if qryStr is less than refStr
   `    o > 0 if qryStr is greater than refStr
   `  - (-(qryStr != delimC)) & (qryStr - refStr)
   `    o is (-1 & number) = number, if qryStr was not on
   `      the deliminator
   `    o is (0 & number) = 0, if qryStr was on the
   `      deliminator
   */\
})

#endif
