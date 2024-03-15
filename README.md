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

# Update log

March 15, 2024 I caught some variants with amino acid
  mutations that exceded the gene lengths. These have
  had the amino acids removed (variant id unchanged).

March 14, 2024 I found that a few codons were off in my
  codon table. This will only effect deletions and
  duplications larger than one amino acid. This has
  been fixed.

March 06, 2024. I fixed an error with my deletion entries
  that resulted in the reference only ever having 2 amino
  acids. I would recommend re-downloading the database.
  
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
  - "NA" is unknown (Sorry about the previous mistake)
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
- There could be future columns added, so long as there
  always added at the end. So, always look for the last
  known column by looking for both newlines '\n' and tabs
  '\t' (in C you can do `*char < 31`).
  
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
  will need a C complier). I have not tested on Windows
  or Mac.

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
  folder and the whoToTbAmr\_src folder. This organization
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

### With my program

No one has reported any yet. Probably because no one is
  really using this except me.

### From the catalog (not fixable or hard to fix)

First off I should say that 2023 catalog from the WHO is a
  very vauble reasource and I am gratefull to the WHO for
  putting the 2023 catalog together. Also, these are bugs
  that would be easily missed, expecially when there are
  141000 indicies. So, I can understand why they were
  missed. I am just posting them here so you are aware of
  them and the decisions I made in dealing with them.

Though to be honest, I am really anoyed at the amount of
  work I had to do to convert it and many of the decisions
  the WHO made in its orginization. But, on the bright
  side, it did give me a chance to build a resource that
  others might appricate.

Again thanks WHO for taking the time to make this.

#### Missing variants ids (Only four having resistance)

In there 2023 catalog there are some variant id's labeled
  gene\_deletion. These variants are not in the genome
  indices sheet, despite a few having a reference to the
  genome indices sheet. I have no way to fix this.

**Decision**: Variants are ignored; not in converted files

   - List of missing AMR variants:
   - fgd1_deletion
   - ethA_deletion
   - katG_deletion
   - pncA_deletion

#### Duplicate entries? (Rare?)

The gene\_lof variant id's in the 2023 catalog have
  nothing to distinguish them in the genome indices tab.
  Some genes have more than one LoF entry, so there could
  be duplicate AMR entries (no idea). I have no easy way
  of catching this, because I need to sort my indicies by
  variant id for quick look up times.

**Decision**: I ingoring these possible duplicates and
  pray they cause no problems

##### Table 1; LoFs that are AMRs

|  Variant   | Repeats | No. unique | No. dup |
|:-----------|:-------:|:----------:|:-------:|
| Rv0678_LoF |    2    |     604    |   604   |
| Rv2983_LoF |    1    |    1007    |    0    |
| ddn_LoF    |    1    |     689    |    0    |
| ethA_LoF   |    1    |    1943    |    0    |
| fbiA_LoF   |    1    |    1645    |    0    |
| fbiB_LoF   |    1    |    2115    |    0    |
| fbiC_LoF   |    1    |    4130    |    0    | 
| fgd1_LoF   |    1    |    1487    |    0    |
| gid_LoF    |    1    |     756    |    0    |
| katG_LoF   |    1    |    3041    |    0    |
| pepQ_LoF   |    2    |    1742    |  1742   |
| pncA_LoF   |    1    |     780    |    0    |
| tlyA_LoF   |    1    |    1195    |    0    |

Table: Showing the total number of duplicates likely in
  my conversion of the how database. No. is number.
  Repeats is the number of times a variatn id was
  duplicated. This table is for grade 1 and 2 variants
  (known antibiotic resitance only).

Total known duplicates in my converted database: 2346. I
  have no idea if there are duplicate non-LoF entries.

Code used to build the table:


```
awk \
    '
      BEGIN{FS=OFS="\t";};

      {
         if($4 ~ /[Ll][Oo][Ff]$/ && $106 ~ /^[12]/)
            print $4;
     };
    ' WHO-2023-TB-tab1-master.tsv |
  sort |
  uniq -c |
  awk '{print $2, $1;};'
```

```
awk \
    '
      BEGIN{FS=OFS="\t";};

      {
         if($4 ~ /[Ll][Oo][Ff]$/ && $106 ~ /^[12]/)
            print $4;
     };
    ' WHO-2023-TB-tab1-master.tsv |
  sort |
  uniq -c |
  awk '{print $2;};' \
  > tmp.tsv;

grep -f tmp.tsv WHO-2023-TB-tab2-indices.tsv |
  awk '{print $1}' |
  sort |
  uniq -c;

rm tmp.tsv
```

##### Table 2; All genes with more than one LoF

Number of LoF entries with more than one variant id in the
   who catalog (all grades)

|    Variant   | Repeats | No. unique | No. dup |
|:-------------|:-------:|:----------:|:-------:|
| Rv0678\_LoF  |    2    |    604     |   604   |
| Rv1129c\_LoF |    4    |     NA     |    0    |
| Rv1258c\_LoF |    3    |     NA     |    0    |
| Rv1979c\_LoF |    2    |     NA     |    0    |
| Rv2477c\_LoF |    7    |     NA     |    0    |
| Rv2752c\_LoF |    5    |     NA     |    0    |
| Rv2983\_LoF  |    2    |   1007     |   1007  |
| bacA\_LoF    |    4    |     NA     |    0    |
| eis\_LoF     |    2    |   1578     |   1578  |
| fbiA\_LoF    |    2    |   1645     |   1645  |
| fbiB\_LoF    |    2    |   2115     |   2215  |
| fbiC\_LoF    |    2    |   4130     |   4130  |
| fgd1\_LoF    |    2    |   1487     |   1487  |
| glpK\_LoF    |    6    |     NA     |    0    |
| mmpL5\_LoF   |    2    |   4527     |   4527  |
| mmpS5\_LoF   |    2    |     NA     |    0    |
| mshA\_LoF    |    2    |     NA     |    0    |
| mtrB\_LoF    |    2    |     NA     |    0    |
| ndh\_LoF     |    3    |     NA     |    0    |
| pepQ\_LoF    |    2    |   1742     |   1742  |
| whiB6\_LoF   |    3    |     NA     |    0    |
| whiB7\_LoF   |    3    |     NA     |    0    |

Table: Total number of LoF entries in the 2023 catalog
  that could be duplicated (more than one LoF per gene)
  These include the grade 3, 4, and 5 (no antibiotic
  resistance). No. is short for number. NA means the
  variant id could not be found in the genome indice tab.
  
Total duplicates (excluding unique genes): 18835. It is
  a bit worring that some non-resitance LoF entries are
  missing from the genome indice page. At least one
  Rv2477c entry has a see genome indice tab entry.
  However, I have not checked any other entries.
  

```
awk \
    '{if($4 ~ /[Ll][Oo][Ff]$/) print $4;};' \
    WHO-2023-TB-tab1-master.tsv |
  sort |
  uniq -c |
  awk '{if($1 > 1) print $2, $1;};' 
```

```
awk \
    '{if($4 ~ /[Ll][Oo][Ff]$/) print $4;};' \
    WHO-2023-TB-tab1-master.tsv |
  sort |
  uniq -c |
  awk '{if($1 > 1) print $2;};' \
  > tmp.tsv;

grep -f tmp.tsv WHO-2023-TB-tab2-indices.tsv |
  awk '{print $1}' | sort | uniq -c

rm tmp.tsv
```

#### Amino acid sequences outside of genes (Rare)

In the 2023 catalog there are some AMR amino acid
  mutations that excede the genes boundries. These caused
  some problems with my tbAmr, a program I am working on.
  I tried to fix this issue, but ran into problems of the
  fixes failing. Basically, after 6 to 8 hours I decided
  it was time to move on.

**Decision**: I removed the amino acid information from
   these entries (variant ids are not changed), but the
   extra information I added is removed.

Here is a table of the variant ids I found:

|       Variant id        | Number times |   effect      |
|:------------------------|:------------:|:--------------|
| ethA\_p.His281fs        |       1      | frame shift   |
| ethA\_p.Ile325fs        |       1      | frame shift   |
| ethA\_p.Leu295fs        |       1      | frame shift   |
| ethA\_p.Thr353fs        |       1      | frame shift   |
| ethA\_p.Val489fs        |       1      | frame shift   |
| pncA\_p.Ter187Argext\*? |       1      | removes stop  |
| pncA\_p.Thr61fs         |       1      | frame shift   |
| tlyA\_p.Met1?           |      14      | removes start |

Table: Table showing the variant ids that hadd an extra
       base or extra amino acid sequences outside of
       genes. These would have their aa sequences wiped,
       but variant ids remain the same. Number times is
       the number of times this was an issue. The effect
       is the effect of the mutation.
 
Here is the code I used to get this table


```
awk \
   '{if($2 ~ /p\./ && $10 == 0) print $2;};' \
    who-tb-2023-catalog-tbAmr-format.tsv |
  sort |
  uniq -c 
```
