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

March 16, 2024 No bugs, I am just adding some more data
  to the README. I also got a response back from the WHO
  and they pointed my to their using the 2023 catalog pdf.
  I had read this before, but having worked a bit with the
  catalog gave me a bit better understanding of some of
  the decisions they made.

March 15, 2024 I caught some variants with amino acid
  mutations that go over the gene lengths. These have
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
   - fgd1\_deletion
   - ethA\_deletion
   - katG\_deletion
   - pncA\_deletion

See table four for a list of all variants missing a genome
  indices entry.

From my issue with the WHO I was pointed to there using
  this database pdf. I did not understand this enough
  when I first read it, but it appears that this was
  done on purpose or was at least known. I am not sure
  why.

#### Duplicate entries? (Rare?)

The gene\_lof variant id's in the 2023 catalog have
  nothing to distinguish them in the genome indices tab.
  Some genes have more than one LoF entry, so there could
  be duplicate AMR entries (no idea). I have no easy way
  of catching this, because I need to sort my indicies by
  variant id for quick look up times.

**Decision**: I ingoring these possible duplicates and
  pray they cause no problems

From my issue with the WHO I was pointed to there using
  this database pdf. I did not understand this enough
  when I first read it, but it appears that this was
  done on purpose to allow multiple grades to be matched
  up (I am guessing grade 1 and 2 to be mixed). I have no
  idea why they did this and it does not make much sense
  to me.

##### Table 1; LoFs that are AMRs

|   Variant   | Repeats | No. unique | No. dup |
|:------------|:-------:|:----------:|:-------:|
| Rv0678\_LoF |    2    |     604    |   604   |
| Rv2983\_LoF |    1    |    1007    |    0    |
| ddn\_LoF    |    1    |     689    |    0    |
| ethA\_LoF   |    1    |    1943    |    0    |
| fbiA\_LoF   |    1    |    1645    |    0    |
| fbiB\_LoF   |    1    |    2115    |    0    |
| fbiC\_LoF   |    1    |    4130    |    0    | 
| fgd1\_LoF   |    1    |    1487    |    0    |
| gid\_LoF    |    1    |     756    |    0    |
| katG\_LoF   |    1    |    3041    |    0    |
| pepQ\_LoF   |    2    |    1742    |  1742   |
| pncA\_LoF   |    1    |     780    |    0    |
| tlyA\_LoF   |    1    |    1195    |    0    |

Table 1:
  Showing the total number of duplicates likely in
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

Table 2:
  Total number of LoF entries in the 2023 catalog
    that could be duplicated (more than one LoF per gene)
    These include the grade 3, 4, and 5 (no antibiotic
    resistance). No. is short for number. NA means the
    variant id could not be found in the genome indice
    tab.
  
Total duplicates (excluding unique genes): 18835. It is
  a bit worring that some non-resitance LoF entries are
  missing from the genome indice page. At least one
  Rv2477c entry has a see genome indice tab entry. See
  table four for a list of all missing enteries. There are
  not that many (156 out of ~40000).
  

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
  mutations that go over the genes range. These caused
  some problems with my tbAmr, a program I am working on.
  I tried to fix this issue, but ran into problems of the
  fixes failing. Basically, after 6 to 8 hours I decided
  it was time to move on.

**Decision**: I removed the amino acid information from
   these entries (variant ids are not changed), but the
   extra information I added is removed.

##### Table 3:

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

Table 3:
  Table showing the variant ids that hadd an extra
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

#### Variants missing coordinates:

I was just curious how many entries had an see genome
  coordinate entry, but were missing an genome coordinate.
  From my previous tables (2 and 1), I know only four of
  these entries were grade 1 or 2 (AMRs; see table 1).
  Otherwise my conversion function would have printed
  them out as errors.

##### Table 4:

Total missing ids: 156

|          Variant           |  Count  |
|:---------------------------|:-------:|
| PPE35\_LoF                 |    1    |
| PPE35\_deletion            |    1    |
| Rv0010c\_LoF               |    1    |
| Rv0010c\_p.Ter142Cysext\*? |    1    |
| Rv0565c\_LoF               |    1    |
| Rv0565c\_p.Ter487Trpext\*? |    1    |
| Rv0678\_p.Ter166Argext\*?  |    2    |
| Rv1129c\_LoF               |    4    |
| Rv1258c\_LoF               |    3    |
| Rv1258c\_deletion          |    3    |
| Rv1258c\_p.Ter420Glyext\*? |    3    |
| Rv1979c\_LoF               |    2    |
| Rv1979c\_deletion          |    2    |
| Rv2477c\_LoF               |    7    |
| Rv2477c\_p.Ter559Glnext\*? |    6    |
| Rv2680\_LoF                |    1    |
| Rv2681\_LoF                |    1    |
| Rv2752c\_LoF               |    5    |
| Rv2752c\_deletion          |    5    |
| Rv2752c\_p.Ter559Glnext\*? |    3    |
| Rv2752c\_p.Ter559Tyrext\*? |    5    |
| Rv2752c\_p.Ter559ext\*?    |    2    |
| Rv2983\_p.Ter215Argext\*?  |    2    |
| Rv3083\_LoF                |    1    |
| Rv3083\_deletion           |    1    |
| Rv3236c\_LoF               |    1    |
| Rv3236c\_deletion          |    1    |
| Rv3236c\_p.Ter386Tyrext\*? |    1    |
| aftB\_LoF                  |    1    |
| aftB\_p.Ter628Argext\*?    |    1    |
| aftB\_p.Ter628Serext\*?    |    1    |
| aftB\_p.Ter628ext\*?       |    1    |
| ahpC\_LoF                  |    1    |
| ahpC\_deletion             |    1    |
| ahpC\_p.Ter196ext\*?       |    1    |
| bacA\_LoF                  |    4    |
| bacA\_deletion             |    4    |
| dnaA\_LoF                  |    1    |
| dnaA\_p.Ter508Gluext\*?    |    1    |
| dnaA\_p.Ter508Tyrext\*?    |    1    |
| eis\_deletion              |    2    |
| embA\_LoF                  |    1    |
| embR\_LoF                  |    1    |
| embR\_deletion             |    1    |
| embR\_p.Ter389Serext\*?    |    1    |
| ethA\_deletion             |    1    |
| ethR\_LoF                  |    1    |
| fbiC\_p.Ter857Trpext\*?    |    2    |
| fgd1\_deletion             |    2    |
| gid\_p.Ter225Cysext\*?     |    1    |
| glpK\_LoF                  |    6    |
| glpK\_deletion             |    6    |
| gyrA\_p.Ter839ext\*?       |    2    |
| gyrB\_LoF                  |    1    |
| hadA\_LoF                  |    1    |
| katG\_deletion             |    1    |
| katG\_p.Ter741Argext\*?    |    1    |
| katG\_p.Ter741Trpext\*?    |    1    |
| lpqB\_LoF                  |    1    |
| mmpS5\_LoF                 |    2    |
| mshA\_LoF                  |    2    |
| mshA\_deletion             |    2    |
| mshA\_p.Ter481ext\*?       |    2    |
| mtrB\_LoF                  |    2    |
| ndh\_LoF                   |    3    |
| ndh\_deletion              |    3    |
| panD\_LoF                  |    1    |
| pncA\_deletion             |    1    |
| pncA\_p.Ter187Argext\*?    |    1    |
| pncA\_p.Ter187Glyext\*?    |    1    |
| pncA\_p.Ter187Trpext\*?    |    1    |
| rpsA\_LoF                  |    1    |
| rpsA\_p.Ter482ext\*?       |    1    |
| rpsL\_LoF                  |    1    |
| sigE\_LoF                  |    1    |
| tlyA\_p.Ter269Trpext\*?    |    1    |
| tsnR\_LoF                  |    1    |
| whiB6\_LoF                 |    3    |
| whiB6\_deletion            |    3    |
| whiB6\_p.Ter117Trpext\*?   |    2    |
| whiB7\_LoF                 |    3    |

Table 4:
  Number of variants that have an 
    "See genome coordinates" entry, but have no actual
    genome coordinates entry.

##### Code used to build table 4

```
awk '
       BEGIN{OFS=FS="\t"};
       {if($7 ~ /Genomic_coor/) print $4;};
    ' \
    < WHO-2023-TB-tab1-master.tsv \
  > tmp.tsv;

awk \
    '{print $1;}' \
    < WHO-2023-TB-tab2-indices.tsv \
  > tmp-filt.tsv;

# Get the count for variants missing locations
grep -v -f tmp-filt.tsv tmp.tsv | wc -l

# Get the table of variants missing ids
grep \
    -v \
    -f tmp-filt.tsv tmp.tsv |
  sort |
  uniq -c |
  awk '
         BEGIN{
            printf "|          Variant           |";
            printf "  Count  |\n";
            printf "|:---------------------------|";
            printf ":-------:|\n";
         }; # BEGIN (print header)

         { # MAIN add formatting
            # Escape some markdown reserved characters
            gsub(/_/, "\\_", $2);
            gsub(/\*/, "\\*", $2);

            # Add a left pad to the variant id
            padI = 26 - length($2);
               # I  happen to know the longest variant id
               #   that was printed out was 26 characters.
               # This will need to be changed if there are
               #   longer variant ids in the missed
               #   entries in the future.

            for(iPad=0; iPad < padI; ++iPad) $2 = $2 " ";

            # Here I am adding a center padding to the
            # number
            if($1 < 10) $1 = "   " $1 "   ";
            else if($1 < 100) $1 = "  " $1 "   ";
            else if($1 < 1000) $1 = "  " $1 "  ";
            else if($1 < 10000) $1 = " " $1 "  " ;
            else if($1 < 100000) $1 = " " $1 " ";
            else if($1 < 1000000) $1 = $1 " ";
            else $1 = $1;

            print "| " $2 " | " $1 " |";
         }; # Main
      ' \
  > variantsMissingIndices.md

rm tmp.tsv;
rm tmp-filt.tsv;
```

