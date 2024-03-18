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

First off it looks like the crew behind TBProfiler have
  gotten there parsing script working. I am not sure how
  long this has been up, looks like it has been up for
  a while and I just never noticed it. I would recommend
  using their script before this repository. It will be
  much more better designed.
  [https://github.com/jodyphelan/who-catalogue-parsing](
   https://github.com/jodyphelan/who-catalogue-parsing
  ). 

I should say that 2023 catalog from the WHO is a very
  valuable resource. I am grateful to the WHO for
  putting the 2023 catalog together. Also, I am very
  grateful for the WHO in taking time to review this
  repository and give me feedback. It looks like there
  were several points of which I need to improve.

There are several decisions I made that may not match up
  with the WHO catalog. These will likely be issues down
  stream, so again use TBProfilers work before mine. I
  have these issues listed at the bottom of this README.

# Update log

March 18, 2024: I realized I had some misunderstands about
  the catalog and I am updating the README to reflect
  this and I am taking back some of what I said.

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

This has been modified because the WHO gently corrected
  me on several of my mistakes. I am very grateful for the
  time the WHO took to do this.

First I am ignoring the "gene\_deletion" entries. Looking
  back at page 91 of the WHO catalog it appears that these
  refer to entire gene deletions. I will have to think how
  to handle these, because these often would be missed in
  amplicons.

For any aa sequences that goes outside of the gene
  regions, I am treating as strict patterns. The WHOs
  program found these to still contribute to resistance,
  so I am not likely handling these in the wrong way. I
  would have to think hard to find a way to fix this
  without breaking my other program. So, I need to add
  more thought here.

Next it appears that the variant IDs use regular
  expressions in them in the form of \*?. This is a
  mistake of Mine I will have to update in my program.
  For know though, this does not seem to be a problem,
  since I am focusing on the resistant entries only.

Next I need to make sure I grab the genome coordinates
  from both the catalog and then update it with the genome
  indices tab if there is an update. I assumed all entries
  in the master tab would be in the genome indices tab.

It also appears that the LoF entries are general and can
  refer to any frame shift. I should add in a separate
  column with a 1 or 0 marking if a frame shift applies to
  any region of the genome.

I will probably not get to this right away, but I will get
  back to this once I get some of my other programs more
  set up.
