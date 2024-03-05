#ifndef CONVERT_WHO_TO_TBAMR_DATABASE_H
#define CONVERT_WHO_TO_TBAMR_DATABASE_H

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
   unsigned long *numAmrUL,
   char **drugAryStr,
   int *numDrugsI
);

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
   unsigned long *numAmrUL,     /*Number of amrs kept*/
   char **drugAryStr,   /*Holds antibiotics*/
   int *numDrugsI,      /*Number of drugs in drugAryStr*/
   unsigned char *errUC         /*Reports errors*/
);

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
);


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
   unsigned long *lenBuffUL /*Length of buffer*/
);

#endif
