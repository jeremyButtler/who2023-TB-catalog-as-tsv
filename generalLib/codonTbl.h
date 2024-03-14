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

#define t_code_codon_tbl 0
#define c_code_codon_tbl 1
#define a_code_codon_tbl 2
#define g_code_codon_tbl 3
#define n_code_codon_tbl 4

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

      a_code_codon_tbl, /*A (65)*/
      n_code_codon_tbl, /*B, (C/G/T) treat as N*/
      c_code_codon_tbl, /*C*/
      n_code_codon_tbl, /*D (AGT), treat as N*/
      8, 8,
      g_code_codon_tbl, /*G*/
      n_code_codon_tbl, /*H (ACT) treat as N*/
      8, 8,
      n_code_codon_tbl, /*K (GT), treat as N*/
      8,
      n_code_codon_tbl, /*M (AC), treat as N*/
      n_code_codon_tbl, /*N (AGCT)*/
      8, 8, 8,
      n_code_codon_tbl, /*R (AG), treat as N*/
      n_code_codon_tbl, /*S (CG), treat as N*/
      t_code_codon_tbl, /*T*/
      t_code_codon_tbl, /*U*/
      n_code_codon_tbl, /*V (ACG), treat as N*/
      8,
      n_code_codon_tbl, /*X, as an N, is for amino acids*/
      8, 8,

      /*Special characters after upercase letters*/
      8, 8, 8, 8, 8, 8,

      /*lower case letters*/
      a_code_codon_tbl, /*a (65)*/
      n_code_codon_tbl, /*b, (c/g/t) treat as n*/
      c_code_codon_tbl, /*c*/
      n_code_codon_tbl, /*d (agt), treat as n*/
      8, 8,
      g_code_codon_tbl, /*g*/
      n_code_codon_tbl, /*h (act) treat as n*/
      8, 8,
      n_code_codon_tbl, /*k (gt), treat as n*/
      8,
      n_code_codon_tbl, /*m (ac), treat as n*/
      n_code_codon_tbl, /*n (agct)*/
      8, 8, 8,
      n_code_codon_tbl, /*r (ag), treat as n*/
      n_code_codon_tbl, /*s (cg), treat as n*/
      t_code_codon_tbl, /*t*/
      t_code_codon_tbl, /*u*/
      n_code_codon_tbl, /*v (acg), treat as n*/
      8,
      n_code_codon_tbl, /*x, as an n, is for amino acids*/
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

      t_code_codon_tbl, /*A (complement is T) (65)*/
      n_code_codon_tbl, /*B, (C/G/T) treat as N*/
      g_code_codon_tbl, /*C; complement is G*/
      n_code_codon_tbl, /*D (AGT), treat as N*/
      8, 8,
      c_code_codon_tbl, /*G; complement is C*/
      n_code_codon_tbl, /*H (ACT) treat as N*/
      8, 8,
      n_code_codon_tbl, /*K (GT), treat as N*/
      8,
      n_code_codon_tbl, /*M (AC), treat as N*/
      n_code_codon_tbl, /*N (AGCT)*/
      8, 8, 8,
      n_code_codon_tbl, /*R (AG), treat as N*/
      n_code_codon_tbl, /*S (CG), treat as N*/
      a_code_codon_tbl, /*T; complement is A*/
      a_code_codon_tbl, /*U; complement is A*/
      n_code_codon_tbl, /*V (ACG), treat as N*/
      8,
      n_code_codon_tbl, /*X, as an N, for amino acids*/
      8, 8,

      /*Special characters after upercase letters*/
      8, 8, 8, 8, 8, 8,

      /*lower case letters*/
      t_code_codon_tbl, /*a; complement is T (65)*/
      n_code_codon_tbl, /*b, (c/g/t) treat as n*/
      g_code_codon_tbl, /*c; complement is G*/
      n_code_codon_tbl, /*d (agt), treat as n*/
      8, 8,
      c_code_codon_tbl, /*g; complement is C*/
      n_code_codon_tbl, /*h (act) treat as n*/
      8, 8,
      n_code_codon_tbl, /*k (gt), treat as n*/
      8,
      n_code_codon_tbl, /*m (ac), treat as n*/
      n_code_codon_tbl, /*n (agct)*/
      8, 8, 8,
      n_code_codon_tbl, /*r (ag), treat as n*/
      n_code_codon_tbl, /*s (cg), treat as n*/
      a_code_codon_tbl, /*t; complement is A*/
      a_code_codon_tbl, /*u; complement is A*/
      n_code_codon_tbl, /*v (acg), treat as n*/
      8,
      n_code_codon_tbl, /*x, treat as n, for amino acids*/
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
         {'f', 'f', 'l', 'l', 'x'}, /*2nd element is T*/
         {'s', 's', 's', 's', 's'}, /*2nd element is C*/
         {'y', 'y', '*', '*', 'x'}, /*2nd element is A*/
         {'c', 'c', '*', 'w', 'x'}, /*2nd element is G*/
         {'x', 'x', 'x', 'x', 'x'}  /*2nd element is N*/
      }, /*First element is an T*/

      { /*First element is an C*/
         {'l', 'l', 'l', 'l', 'l'}, /*2nd element is T*/
         {'p', 'p', 'p', 'p', 'p'}, /*2nd element is C*/
         {'h', 'h', 'q', 'q', 'x'}, /*2nd element is A*/
         {'r', 'r', 'r', 'r', 'r'}, /*2nd element is G*/
         {'x', 'x', 'x', 'x', 'x'}  /*2nd element is N*/
      }, /*First element is an C*/

      { /*First element is an A*/
         {'i', 'i', 'i', 'm', 'x'}, /*2nd element is T*/
         {'t', 't', 't', 't', 't'}, /*2nd element is C*/
         {'n', 'n', 'k', 'k', 'x'}, /*2nd element is A*/
         {'s', 's', 'r', 'r', 'x'}, /*2nd element is G*/
         {'x', 'x', 'x', 'x', 'x'}  /*2nd element is N*/
      }, /*First element is an A*/

      { /*First element is an G*/
         {'v', 'v', 'v', 'v', 'v'}, /*2nd element is T*/
         {'a', 'a', 'a', 'a', 'a'}, /*2nd element is C*/
         {'d', 'd', 'e', 'e', 'x'}, /*2nd element is A*/
         {'g', 'g', 'g', 'g', 'g'}, /*2nd element is G*/
         {'x', 'x', 'x', 'x', 'x'}  /*2nd element is N*/
      }, /*First element is an G*/

      { /*First element is an N*/
         {'x', 'x', 'x', 'x', 'x'}, /*2nd element is T*/
         {'x', 'x', 'x', 'x', 'x'}, /*2nd element is C*/
         {'x', 'x', 'x', 'x', 'x'}, /*2nd element is A*/
         {'x', 'x', 'x', 'x', 'x'}, /*2nd element is G*/
         {'x', 'x', 'x', 'x', 'x'}  /*2nd element is N*/
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

/*Table
     T         C        A        G
  +--------+--------+--------+--------+
  | TTT  F | TCT  S | TAT  Y | TGT  C |
T | TTC  F | TCC  S | TAC  Y | TGC  C | T
  | TTA  L | TCA  S | TAA  * | TGA  * |
  | TTG  L | TCG  S | TAG  * | TGG  W |
  +--------+--------+--------+--------+
  | CTT  L | CCT  P | CAT  H | CGT  R |
C | CTC  L | CCC  P | CAC  H | CGC  R | C
  | CTA  L | CCA  P | CAA  Q | CGA  R |
  | CTG  L | CCG  P | CAG  Q | CGG  R |
  +--------+--------+--------+--------+
  | ATT  I | ACT  T | AAT  N | AGT  S |
A | ATC  I | ACC  T | AAC  N | AGC  S | A
  | ATA  I | ACA  T | AAA  K | AGA  R |
  | ATG  M | ACG  T | AAG  K | AGG  R |
  +--------+--------+--------+--------+
  | GTT  V | GCT  A | GAT  D | GGT  G |
G | GTC  V | GCC  A | GAC  D | GGC  G | G
  | GTA  V | GCA  A | GAA  E | GGA  G |
  | GTG  V | GCG  A | GAG  E | GGG  G |
  +--------+--------+--------+--------+
*/
