# Use:

This repository holds a tsv, single file of the who 2023
  TB catalog (see who-tb-2023-catalog-tbAmr-format.tsv). I
  I did this right, then this file should not require any
  complicated steps to parse. However, it does have a lot
  of columns.

I have also included my converter in this repository if
  you want to use it. However, this would only be useful
  if the WHO updated there 2023 catalog or kept the same
  format for their next catalog, if they ever release a
  next addition.

These converted WHO catalogs have only variants with AMR
  resistance.

# Files

## The tsv file(s)

The who-tb-2021-catalog-tbAmr-format.tsv is the converted
  2021 (first) catalog. The
  who-tb-2023-catalog-tbAmr-format.tsv is the converted
  2023 (second) catalog.

The format is as follows:

- Column one is the gene name
- Column two is the variant id
- Column three is/should be the position of the first base
  in the pattern on the reference.
- Column four is the direction of the gene
  - 'F' is forward
  - 'R' is reverse
  - 'U' is unknown
- Column five is the mutation
  - An deletion (del) is assigned when  the AMR sequence
    is shorter than the reference.
  - An insertion (ins) is assigned when  the AMR sequence
    is longer than the reference.
  - snp is assigned if the AMR and reference sequences are
    the same length
- Column six has a 1 if there is a frame shift, else a 0
- Column seven has the reference sequence
- Column eight has the AMR sequence
- Column nine has the position of the first reference base
  in the codon, or NA if there is no codon, or if it could
  not be found
- Column ten has the number of the first codon in the gene
  or NA if not present
- Column eleven has the number of the last codon in the
  gene or NA if not present
- Column twelve has the reference amino acid sequence or
  0 if it is not present (I am not using NA, because both
  N and A are amino acid codes).
- Column thirteen has the AMR amino acid sequence or 0
  0 if it is not present
- Column fourteen has the starting position of the
  reference gene.
- Column fifteen has the ending position of the reference
  gene.
- Column sixteen has a 1 if the AMR is an high resistance
  AMR, else 0
- Column seventeen has a 1 if the AMR is an low resistance
  AMR, else 0
- Column eighteen has a 1 if multiple AMR's in the same
  gene can result in high resistance, else it is a 0
- Column nineteen is NA, or has a name of a gene that is
  is needed to be function to have the mutation confer
  antibiotic resistance
- The next columns (until the endAntibiotics column) are
  specific antibiotics that a AMR confers resistance to.
  - 0 is no known resistance
  - 1 is resistance
  - 2 is cross-resistance
  - 3 is the entry appears in both the resistant and
    cross-resistance entries
- The endAntibiotics column marks the end of the
  antibiotic entries. It is just filled with '\*'s
- The first column after endAntibiotics has the WHO's
  recorded effects for the AMR or NA
- The second column (currently last) after endAntibiotics
  has the comment (or NA) that the WHO had for the AMR
  
## The other files (other tsv's, the csv, and sam)

The WHO-2021-TB.csv is the saved genome indices tab from
  the WHO's 2021 catalog. It is in the correct format to
  convert.
  
The WHO-2023-TB-tab1-master.tsv is the master tab (tab
  one) from the WHO's 2023 catalog. It has had the extra
  headers removed and is ready for processing.

The WHO-2023-TB-tab2-indices.tsv is the genome indices
  tab (tab two) from the WHO's 2023 catalog. It is ready
  for processing.

The TB-NC000962-gene-maps.sam is a sam file with the genes
  from NC000962 mapped to its self. This is the sam file
  you would run through this program.

# Running the converter

## Building the converter

The converter should be pretty easy to build, except for
  Windows (code should be platform independent, but you
  will need a C complier)

For Linux:

```
make
sudo make install
```

For Mac: This should work

```
make mac
sudo make install
```

## Running the converter

Help message: `whoToTbAmr -h`

Converting the 2023 catalog (using files in this rep):

```
whoToTbAmr -sam TB-NC000962-gene-maps.sam -tabone-who-2023-tsv WHO-2023-TB-tab1-master.tsv -tabtwo-who-2023-tsv WHO-2023-TB-tab2-indices.tsv -out catalog.tsv
```

Converting the 2021 catalog (using files in this rep):

```
whoToTbAmr -sam TB-NC000962-gene-maps.sam -who-2021-csv WHO-2021-TB.csv -out catalog.tsv
```

## The code:

This code is under a dual license of the Unlicense (public
  domain) and MIT. Pick the license that works best for
  you or that works in your country/region.

Keep in mind, that the other files (non-code files) are
  from other people, and thus, are under their licensing
  scheme.

The code is split up into two folders, the generalLib
  folder and the whoToTbAmr_src folder. This organization
  is from this program being part of a larger project that
  I am working on.

I need to go back and document this code properly.

# Bugs/errors in output:

## Comments

This code is done as well as I could get it. However,
  there will likely still be problems. So, there may be
  bugs or errors I missed.

I am using the database in my own code, so please report
  errors in the files or bugs in the code.

## Known bugs:

The gene_lof variant id's in the 2023 catalog have nothing
  to distinguish them in the genome indices tab. Some
  genes have more than one lof entry, so there are likely
  duplicate AMR entries.

In there 2023 catalog there are some variant id's labeled
  gene_deletion. These variants are not in the genome
  indices sheet, despite a few having a reference to the
  genome indices sheet. So, these variants are not in the
  converted tsv files. This one would require the WHO to
  fix it.

   - **List of missing AMR variants**
   - fgd1_deletion
   - ethA_deletion
   - katG_deletion
   - pncA_deletion
