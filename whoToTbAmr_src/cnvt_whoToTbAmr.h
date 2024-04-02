/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
' SOF: Start Of File
'   o header:
'     - guards for the header file (definsive coding)
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
* Header:
*   - guards for the header file (definsive coding)
\-------------------------------------------------------*/

#ifndef CONVERT_WHO_TO_TBAMR_DATABASE_H
#define CONVERT_WHO_TO_TBAMR_DATABASE_H

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
   unsigned long *numAmrUL,
   char **drugAryStr,
   int *numDrugsI
);

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
);

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
);

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
);

#endif

/*=======================================================\
: License:
: 
: This code is under the unlicense (public domain).
:   However, for cases were the public domain is not
:   suitable, such as countries that do not respect the
:   public domain or were working with the public domain
:   is inconveint / not possible, this code is under the
:   MIT license
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
