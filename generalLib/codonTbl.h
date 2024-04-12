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
'  o fun-01: codonToAA
'    - Converts an codon (three bases) to its amino acid
'  o fun-02: revCompCodonToAA
'    - Reverses and complements the input codon. This then
'      converts the reverse complement codon into an amino
'      acid
'  o fun-03: bacteriaStartCode_check
'    - Checks to see in input bases (converted with codon
'      look up tables) are an bacterial start codon
'  o fun-04: bacteriaStart_check
'    - Checks to is if an codon is bacterial start codon
'  o fun-05: bacteriaReverseStart_check
'    - Reverse complements codon and then checks to see if
'      codont is codon is an bacterial start codon
'  o fun-06: aaThreeLetterToChar
'    - Converts a three letter amino acid idenity to its
'      single letter amino acid identity
'   o license:
'     - Licensing for this code (public domain / mit)
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
static unsigned char baseToCodeLkTbl[] =
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
static unsigned char compBaseToCodeLkTbl[] =
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
| Fun-01: codonToAA
|   - Converts an codon (three bases) to its amino acid
| Input:
|   - firstBaseC:
|     o First base in the codon
|   - secBaseC:
|     o Second base in the codon
|   - thirdBaseC:
|     o Last base in the codon
| Output:
|   - Returns:
|     o The amino acid of the input codon
\-------------------------------------------------------*/
#define \
codonToAA(\
   firstBaseC,\
   secBaseC,\
   thirdBaseC\
)(\
   codonLkTbl[\
      baseToCodeLkTbl[(unsigned char) (firstBaseC)]\
   ][\
      baseToCodeLkTbl[(unsigned char) (secBaseC)]\
   ][\
      baseToCodeLkTbl[(unsigned char) (thirdBaseC)]\
   ]\
) /*codonToAA*/

/*-------------------------------------------------------\
| Fun-02: revCompCodonToAA
|   - Reverses and complements the input codon. This Then
|     converts the reverse complement codon into an amino
|     acid
| Input:
|   - firstBaseC:
|     o First base in the foward codon, but third (last)
|       base in the reverse complement codon
|   - secBaseC:
|     o Second base in forward codon, but second (middle)
|       base in the reverse complement codon
|   - thirdBaseC:
|     o Last base in forward codon, but first base in the
|       reverse complement codon
| Output:
|   - Returns:
|     o The reverse complement amino acid of the input
|       codon
\-------------------------------------------------------*/
#define \
revCompCodonToAA(\
   firstBaseC,/*First base at position (third in codon)*/\
   secBaseC,  /*Second base at ref position (mid codon)*/\
   thirdBaseC /*Third base ref position (first codon)*/\
)(\
   codonLkTbl[\
      compBaseToCodeLkTbl[(unsigned char) (thirdBaseC)]\
   ][\
      compBaseToCodeLkTbl[(unsigned char) (secBaseC)]\
   ][\
      compBaseToCodeLkTbl[(unsigned char) (firstBaseC)]\
   ]\
) /*revCompCodonToAA*/\

/*-------------------------------------------------------\
| Fun-03: bacteriaStartCode_check
|   - Checks to see in input bases (converted with codon
|     look up tables) are an bacterial start codon
| Input:
|   - firstBaseC:
|     o first base to check; it needs to be the output of
|       an base to code look up table
|   - secBaseC:
|     o second base to check; it needs to be the output
|       of an base to code look up table
|   - thirdBaseC:
|     o third base to check; it needs to be the output
|       of an base to code look up table
| Output:
|   - Returns:
|     o 1 if this is an bacterial start codon
|     o 0 if this is not an bacterial start codon
\-------------------------------------------------------*/
#define \
bacteriaStartCode_check(\
   firstBaseC,\
   secBaseC,\
   thirdBaseC\
)(\
     ( /*Check if first base is g, t or a*/\
        ( /*Check if the first bsae is an g or an t*/\
            ((firstBaseC) == g_code_codon_tbl)\
          | ((firstBaseC) == t_code_codon_tbl)\
        ) /*Check if the first bsae is an g or an t*/\
      \
      | ((firstBaseC) == a_code_codon_tbl)\
     ) /*Check if first base is g, t or a*/\
     \
     & ((secBaseC) == t_code_codon_tbl)\
     & ((thirdBaseC) == g_code_codon_tbl)\
     \
     /*Logic:
     `   - x = firstBaseC == g_code_codon_tbl:
     `     o 1 if the first base is a G
     `     o 0 if not a g
     `   - x |= (firstBaseC == t_code_codon_tbl):
     `     o 1 if the first base is an "G" or "T"
     `     o 0 if not an "G" or "T"
     `   - x |= (firstBaseC == a_code_codon_tbl):
     `     o 1 if the first base is an "G", "T", or "A"
     `     o 0 if not an "G", "T", or "A"
     `     o This covers the first codon for all posible
     `       start codons (ATG, GTG, and TTG)
     `   - The second and thrid base comparisions clear
     `     the bit (set to 0) if I do not have an
     `     TTG, GTG, or ATG codon
     */\
) /*bacteriaStartCode_check*/

/*-------------------------------------------------------\
| Fun-04: bacteriaStart_check
|   - Checks to is if an input codon is an bacterial start
|     codon
| Input:
|   - firstBaseC:
|     o first base in codon
|   - secBaseC:
|     o second base in codon
|   - thirdBaseC:
|     o third base in codon
| Output:
|   - Returns:
|     o 1 if this is an bacterial start codon
|     o 0 if this is not an bacterial start codon
\-------------------------------------------------------*/
#define \
bacteriaStart_check(\
   firstBaseC,\
   secBaseC,\
   thirdBaseC\
)(\
     ( /*Check if first base is g, t or a*/\
        ( /*Check if the first bsae is an g or an t*/\
            (     baseToCodeLkTbl[\
                     (unsigned char) (firstBaseC)\
                  ]\
               == g_code_codon_tbl\
            )\
          | (     baseToCodeLkTbl[\
                     (unsigned char) (firstBaseC)\
                  ]\
               == t_code_codon_tbl\
            )\
        ) /*Check if the first bsae is an g or an t*/\
      \
      | (     baseToCodeLkTbl[\
                 (unsigned char) (firstBaseC)\
              ]\
           == a_code_codon_tbl\
        )\
     ) /*Check if first base is g, t or a*/\
     \
     /*Check if the second base is an t*/\
     & (    baseToCodeLkTbl[\
               (unsigned char) (secBaseC)\
            ]\
         == t_code_codon_tbl\
       )\
     \
     /*Check if the third base is an t*/\
     & (    baseToCodeLkTbl[\
               (unsigned char) (thirdBaseC)\
            ]\
         == g_code_codon_tbl\
       )\
     \
     /*Logic:
     `   - x = firstBaseC == g_code_codon_tbl:
     `     o 1 if the first base is a G
     `     o 0 if not a g
     `   - x |= (firstBaseC == t_code_codon_tbl):
     `     o 1 if the first base is an "G" or "T"
     `     o 0 if not an "G" or "T"
     `   - x |= (firstBaseC == a_code_codon_tbl):
     `     o 1 if the first base is an "G", "T", or "A"
     `     o 0 if not an "G", "T", or "A"
     `     o This covers the first codon for all posible
     `       start codons (ATG, GTG, and TTG)
     `   - The second and thrid base comparisions clear
     `     the bit (set to 0) if I do not have an
     `     TTG, GTG, or ATG codon
     */\
) /*bacteriaStart_check*/

/*-------------------------------------------------------\
| Fun-05: bacteriaReverseStart_check
|   - Reverse complements codon and then checks to see if
|     codont is codon is an bacterial start codon
| Input:
|   - firstBaseC:
|     o first base in sequence, last base in codon
|   - secBaseC:
|     o second base in sequence, second base in codon
|   - thirdBaseC:
|     o third base in sequence, first base in
| Output:
|   - Returns:
|     o 1 if this is an bacterial start codon
|     o 0 if this is not an bacterial start codon
\-------------------------------------------------------*/
#define \
bacteriaReverseStart_check(\
   firstBaseC,\
   secBaseC,\
   thirdBaseC\
)(\
     ( /*Check if first base is g, t or a*/\
        ( /*Check if the first bsae is an g or an t*/\
            (    compBaseToCodeLkTbl[\
                    (unsigned char) (thirdBaseC)\
                 ]\
              == g_code_codon_tbl\
            )\
          | (    compBaseToCodeLkTbl[\
                    (unsigned char) (thirdBaseC)\
                 ]\
              == t_code_codon_tbl\
            )\
        ) /*Check if the first bsae is an g or an t*/\
      \
      | (     compBaseToCodeLkTbl[\
                 (unsigned char) (thirdBaseC)\
              ]\
           == a_code_codon_tbl\
        )\
     ) /*Check if first base is g, t or a*/\
     \
     /*Check if the second base is an t*/\
     & (    compBaseToCodeLkTbl[\
               (unsigned char) (secBaseC)\
            ]\
         == t_code_codon_tbl\
       )\
     \
     /*Check if the third base is an t*/\
     & (    compBaseToCodeLkTbl[\
               (unsigned char) (firstBaseC)\
            ]\
         == g_code_codon_tbl\
       )\
     \
     /*Logic:
     `   - x = firstBaseC == g_code_codon_tbl:
     `     o 1 if the first base is a G
     `     o 0 if not a g
     `   - x |= (firstBaseC == t_code_codon_tbl):
     `     o 1 if the first base is an "G" or "T"
     `     o 0 if not an "G" or "T"
     `   - x |= (firstBaseC == a_code_codon_tbl):
     `     o 1 if the first base is an "G", "T", or "A"
     `     o 0 if not an "G", "T", or "A"
     `     o This covers the first codon for all posible
     `       start codons (ATG, GTG, and TTG)
     `   - The second and thrid base comparisions clear
     `     the bit (set to 0) if I do not have an
     `     TTG, GTG, or ATG codon
     */\
) /*bacteriaReverseStart_check*/

/*-------------------------------------------------------\
| Fun-06: aaThreeLetterToChar
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
  +----------+----------+----------+----------+
  | TTT  F . | TCT  S . | TAT  Y . | TGT  C . | T
T | TTC  F . | TCC  S . | TAC  Y . | TGC  C . | C
  | TTA  L . | TCA  S . | TAA  * . | TGA  * . | A
  | TTG  L 2 | TCG  S . | TAG  * . | TGG  W . | G
  +----------+----------+----------+----------+
  | CTT  L . | CCT  P . | CAT  H . | CGT  R . | T
C | CTC  L . | CCC  P . | CAC  H . | CGC  R . | C
  | CTA  L . | CCA  P . | CAA  Q . | CGA  R . | A
  | CTG  L . | CCG  P . | CAG  Q . | CGG  R . | G
  +----------+----------+----------+----------+
  | ATT  I . | ACT  T . | AAT  N . | AGT  S . | T
A | ATC  I . | ACC  T . | AAC  N . | AGC  S . | C
  | ATA  I . | ACA  T . | AAA  K . | AGA  R . | A
  | ATG  M 1 | ACG  T . | AAG  K . | AGG  R . | G
  +----------+----------+----------+----------+
  | GTT  V . | GCT  A . | GAT  D . | GGT  G . | T
G | GTC  V . | GCC  A . | GAC  D . | GGC  G . | C
  | GTA  V . | GCA  A . | GAA  E . | GGA  G . | A
  | GTG  V 2 | GCG  A . | GAG  E . | GGG  G . | G
  +----------+----------+----------+----------+

  1 = Canonical start codon
  2 = Bacterial non-canconical start codon
  . = Nothing
*/

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
