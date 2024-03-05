/*#########################################################
# Name: hiLow
# Use:
#  - Holds look up tables for codons
# Libraries:
# C Standard Libraries:
# License:
#  - dual: CC0 or MIT; choose the license you prefer
#########################################################*/

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
' hiLowPathLookUpTbls TOT: Table Of Tables
'  - Table Of Contents for look up tables
'  o tbl-01 baseToCodeLkTbl:
'    - Table to convert bases to codes used in the codon
'      table.
'  o tbl-02 compBaseToCodeLkTbl:
'    - Table to convert bases to its complement base and
'      then the code used in the codon table.
'  o tbl-03 codonLkTbl:
'    - Table to convert three bases to codons
'  o fun-01: aaThreeLetterToChar
'    - Converts a three letter amino acid idenity to its
'      single letter amino acid identity
\~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef CODON_TABLE_H
#define CODON_TABLE_H

/*--------------------------------------------------------\
| Tbl-01 baseToCodeLkTbl:
|  - Table to convert bases to codes used in the codon
|    table.
\--------------------------------------------------------*/
static char baseToCodeLkTbl[] =
   {  /*baseToCodeLkTbl*/
      /*White space/invisible charactes block*/
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,

      /*symbol/number block*/
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 

      2, /*A (65)*/
      4, /*B, (C/G/T) treat as N*/
      1, /*C*/
      4, /*D (AGT), treat as N*/
      8, 8,
      3, /*G*/
      4, /*H (ACT) treat as N*/
      8, 8,
      4, /*K (GT), treat as N*/
      8,
      4, /*M (AC), treat as N*/
      4, /*N (AGCT)*/
      8, 8, 8,
      4, /*R (AG), treat as N*/
      4, /*S (CG), treat as N*/
      0, /*T*/
      0, /*U*/
      4, /*V (ACG), treat as N*/
      8,
      4, /*X, I treat as an N, but is for amino acids*/
      8, 8,

      /*Special characters after upercase letters*/
      8, 8, 8, 8, 8, 8,

      /*lower case letters*/
      2, /*a (65)*/
      4, /*b, (c/g/t) treat as n*/
      1, /*c*/
      4, /*d (agt), treat as n*/
      8, 8,
      3, /*g*/
      4, /*h (act) treat as n*/
      8, 8,
      4, /*k (gt), treat as n*/
      8,
      4, /*m (ac), treat as n*/
      4, /*n (agct)*/
      8, 8, 8,
      4, /*r (ag), treat as n*/
      4, /*s (cg), treat as n*/
      0, /*t*/
      0, /*u*/
      4, /*v (acg), treat as n*/
      8,
      4, /*x, i treat as an n, but is for amino acids*/
      8, 8,

      /*Special characters after lowercase letters*/
      8, 8, 8, 8, 8, 8,
   }; /*baseToCodeLkTbl*/


/*--------------------------------------------------------\
| Tbl-02 compBaseToCodeLkTbl:
|  - Table to convert bases to its complement base and
|    then the code used in the codon table.
\--------------------------------------------------------*/
static char compBaseToCodeLkTbl[] =
   {  /*baseToCodeLkTbl*/
      /*White space/invisible charactes block*/
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,

      /*symbol/number block*/
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 

      2, /*A (65)*/
      4, /*B, (C/G/T) treat as N*/
      1, /*C*/
      4, /*D (AGT), treat as N*/
      8, 8,
      1, /*G*/
      4, /*H (ACT) treat as N*/
      8, 8,
      4, /*K (GT), treat as N*/
      8,
      4, /*M (AC), treat as N*/
      4, /*N (AGCT)*/
      8, 8, 8,
      4, /*R (AG), treat as N*/
      4, /*S (CG), treat as N*/
      2, /*T*/
      2, /*U*/
      4, /*V (ACG), treat as N*/
      8,
      4, /*X, I treat as an N, but is for amino acids*/
      8, 8,

      /*Special characters after upercase letters*/
      8, 8, 8, 8, 8, 8,

      /*lower case letters*/
      2, /*a (65)*/
      4, /*b, (c/g/t) treat as n*/
      1, /*c*/
      4, /*d (agt), treat as n*/
      8, 8,
      1, /*g*/
      4, /*h (act) treat as n*/
      8, 8,
      4, /*k (gt), treat as n*/
      8,
      4, /*m (ac), treat as n*/
      4, /*n (agct)*/
      8, 8, 8,
      4, /*r (ag), treat as n*/
      4, /*s (cg), treat as n*/
      2, /*t*/
      2, /*u*/
      4, /*v (acg), treat as n*/
      8,
      4, /*x, i treat as an n, but is for amino acids*/
      8, 8,

      /*Special characters after lowercase letters*/
      8, 8, 8, 8, 8, 8,
   }; /*baseToCodeLkTbl*/

/*--------------------------------------------------------\
| Tbl-03 codonLkTbl:
|  - Table to convert three bases to codons
\--------------------------------------------------------*/
static char codonLkTbl[5][5][5] =
   {  /*codonLkTbl*/
      { /*First element is an T*/
         {'f', 'l', 'f', 'l', 'x'}, /*2nd element is an T*/
         {'s', 's', 's', 's', 's'}, /*2nd element is an C*/
         {'y', 'y', '*', '*', 'x'}, /*2nd element is an A*/
         {'c', 'c', '*', 'w', 'x'}, /*2nd element is an G*/
         {'x', 'x', 'x', 'x', 'x'}  /*2nd element is an N*/
      }, /*First element is an T*/

      { /*First element is an C*/
         {'l', 'l', 'l', 'l', 'l'}, /*2nd element is an T*/
         {'p', 'p', 'p', 'p', 'p'}, /*2nd element is an C*/
         {'h', 'h', 'q', 'q', 'x'}, /*2nd element is an A*/
         {'r', 'r', 'r', 'r', 'r'}, /*2nd element is an G*/
         {'x', 'x', 'x', 'x', 'x'}  /*2nd element is an N*/
      }, /*First element is an C*/

      { /*First element is an A*/
         {'i', 'i', 'i', 'm', 'x'}, /*2nd element is an T*/
         {'t', 't', 't', 't', 't'}, /*2nd element is an C*/
         {'n', 'n', 'k', 'k', 'x'}, /*2nd element is an A*/
         {'s', 's', 'r', 'r', 'x'}, /*2nd element is an G*/
         {'x', 'x', 'x', 'x', 'x'}  /*2nd element is an N*/
      }, /*First element is an A*/

      { /*First element is an G*/
         {'v', 'v', 'v', 'v', 'v'}, /*2nd element is an T*/
         {'a', 'a', 'a', 'a', 'a'}, /*2nd element is an C*/
         {'d', 'd', 'e', 'e', 'x'}, /*2nd element is an A*/
         {'g', 'g', 'g', 'g', 'g'}, /*2nd element is an G*/
         {'x', 'x', 'x', 'x', 'x'}  /*2nd element is an N*/
      }, /*First element is an C*/

      { /*First element is an N*/
         {'x', 'x', 'x', 'x', 'x'}, /*2nd element is an T*/
         {'x', 'x', 'x', 'x', 'x'}, /*2nd element is an C*/
         {'x', 'x', 'x', 'x', 'x'}, /*2nd element is an A*/
         {'x', 'x', 'x', 'x', 'x'}, /*2nd element is an G*/
         {'x', 'x', 'x', 'x', 'x'}  /*2nd element is an N*/
      }, /*First element is an N*/
   }; /*codonLkTbl*/

/*-------------------------------------------------------\
| Fun-01: aaThreeLetterToChar
|   - Converts a three letter amino acid idenity to its
|     single letter amino acid identity
| Input:
|   - codonStr:
|     o C-string with the three letter amino acid to
|       convert
| Output:
|   - Returns:
|     o The one letter code for the amino acid
|     o 0 for an invalid code
\-------------------------------------------------------*/
#define aaThreeLetterToChar(codonStr)({\
   char retC = 0;\
   \
   switch((codonStr)[0] & ~32)\
   { /*Switch: Check the first letter*/\
      case 'A':\
      /*Case: The first letter is an A*/\
         switch((codonStr)[2] & ~32)\
         { /*Switch: Check the A's third letter*/\
            case 'A': retC = 'a'; break; /*Ala*/\
            case 'G': retC = 'r'; break; /*Arg*/\
            case 'N': retC = 'n'; break; /*Asn*/\
            case 'P': retC = 'd'; break; /*Asp*/\
         } /*Switch: Check the A's third letter*/\
         \
         break;\
      /*Case: The first letter is an A*/\
      \
      case 'C': retC = 'c'; break; /*Cys*/\
      \
      case 'G':\
      /*Case: The first letter is a G*/\
         switch((codonStr)[2] & ~32)\
         { /*Switch: Check the third letter of G*/\
            case 'N': retC = 'q'; break; /*Gln*/\
            case 'U': retC = 'e'; break; /*Glu*/\
            case 'Y': retC = 'g'; break; /*Gly*/\
         } /*Switch: Check the third letter of G*/\
         \
         break;\
      /*Case: The first letter is a G*/\
      \
      case 'H': retC = 'h'; break; /*His*/\
      case 'I': retC = 'i'; break; /*Ile*/\
      \
      case 'L':\
      /*Case: The first letter is a L*/\
         switch((codonStr)[2] & ~32)\
         { /*Switch: Check the third letter of L*/\
            case 'S': retC = 'k'; break; /*Lys*/\
            case 'U': retC = 'l'; break; /*Leu*/\
         } /*Switch: Check the third letter of L*/\
         \
         break;\
      /*Case: The first letter is a L*/\
      \
      case 'M': retC = 'm'; break; /*Met*/\
      \
      case 'P':\
      /*Case: The first letter is a P*/\
         switch((codonStr)[2] & ~32)\
         { /*Switch: Check the third letter of P*/\
            case 'E': retC = 'f'; break; /*Phe*/\
            case 'O': retC = 'p'; break; /*Pro*/\
         } /*Switch: Check the third letter of P*/\
         \
         break;\
      /*Case: The first letter is a P*/\
      \
      case 'S': retC = 's'; break; /*Ser*/\
      \
      case 'T':\
      /*Case: The first letter is a T*/\
         switch((codonStr)[2] & ~32)\
         { /*Switch: Check the third letter of T*/\
            case 'R': retC = 't'; break; /*Thr*/\
            case 'P': retC = 'w'; break; /*Trp*/\
            case 'Y': retC = 'y'; break; /*Try*/\
         } /*Switch: Check the third letter of T*/\
         \
         break;\
      /*Case: The first letter is a T*/\
      \
      case 'V': retC = 'v'; break; /*Val*/\
      \
      /*This case is for stop and it will look like a
      ` new line. So there is some risk
      */\
      case ('*' & ~32): retC = '*'; break; /*stop*/\
   } /*Switch: Check the first letter*/\
   \
   retC;\
}) /*aaThreeLetterToChar*/

#endif

/*
   Ala A Alanine
   Arg R Arginine
   Asn N Asparagine
   Asp D Aspartic acid
   Cys C Cysteine
   Gln Q Glutamine
   Glu E Glutamic acid
   Gly G Glycine
   His H Histidine
   Ile I Isoleucine
   Lys K Lysine
   Leu L Luecine
   Met M Methionine
   Phe F Phenylalanine
   Pro P Proline
   Ser S Serine
   Thr T Threonine
   Trp W Tryptophan
   Try Y Tyrosine
   Val V Valine
*/
