# Use:

This repository holds a tsv, single file of the who 2023
  TB catalog (see who-tb-2023-catalog-tbAmr-format.tsv).
  If I did this right, then this file should not require
  any complicated steps to parse. However, it does have a
  lot of columns as an result.

First off it looks like the crew behind TBProfiler have
  gotten there parsing script working. I am not sure how
  long this has been up, looks like it has been up for
  a while and I just never noticed it. I would recommend
  using their script before this repository. It will be
  much more better designed.
  [https://github.com/jodyphelan/who-catalogue-parsing](
   https://github.com/jodyphelan/who-catalogue-parsing
  ). 

I have also included my converter in this repository if
  you want to use it. However, this would only be useful
  if the WHO updated there 2023 catalog or kept the same
  format for their next catalog.

The current catalog in here has only the resistant (grade
  1 and 2) entries and excludes entire gene deletions.
  However, you can get all entries from the catalog for
  using the `-all-amrs ` flag. It is around 33 megabytes
  and takes around 2.11 seconds and 134 megabyes of
  RAM (using gnu time (`/usr/bin/time -f "%e\t%M\t%P"`)).
  For the entire gene deletions entries, just run the
  converter on default settings.

I should say that 2023 catalog from the WHO is a very
  valuable resource. I am grateful to the WHO for
  putting the 2023 catalog together. Also, I am very
  grateful for the WHO in taking time to review this
  repository and give me feedback. They gave me several
  points that helped in improving the output.

# Update log

April 12, 2024

- Fixed an memory leak that would cause crashes on more
  memory limted systems
- Fixed an issue were gene\_LoF entries reference position
  would be the start of the gene
- Fixed an error were I was adding two index one values to
  get an index two for the gene ending positions in the
  gene-tbl.tsv

April 05, 2024:

- I have set this up so that the entire gene deletions
  copy the sequence for the entire gene + the base before
  the gene.
- Changed entireGene to be 2 for an gene deletion event,
  1 for an LoF anywere in the genome, and 0 for nothing.

April 02, 2024:

- I fixed several preivous issues, but also
  changed how the converter runs. I replaced the sam file
  with a set of genome coordinates for each gene
  (see gene-tbl.tsv) and the reference genome.
- I set the converter to keep entire deletion entires.
  Their sequence entries are marked with 0's and they have
  an new column, entireGene (Fourth after endAntibiotics)
  set to 1 (2021 catalog prints 0). You can ommit these
  entries with the `-no-whole-gene-dels` flag.
  - The reference sequence is 0
  - The AMR sequence is 0
  - The start position for deletions are now the first
    mapped reference base (gene end or gene start) 
- I added in the primary grade (2021 catalog prints 0
  (no grade))
- I added in an unkownEntry (Fith after endAntibiotics) to
  be for entries I can not identify. There is nothing
  there currently.
- Non-frameshift sequences outside of gene ranges are
  now converted to aa sequences. I do not think this
  applies to any entries, since most of these entries are
  frameshifts.
- Coordinates are now processed from both the genome
  indicies tab and the master tab. So, if one is missing
  it is caught.
- I removed the 2021 catalog files. These can still be
  generated if you wish to get them.

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

The who-tb-2023-catalog-tbAmr-format.tsv is the converted
  2023 (second edition) TB catalog.

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
  - lof is assigned if it is in the variant id
- Column six
  - 1 if there is a frame shift
  - 0 if there is no frame shift
- Column seven
  - Has the reference sequence
- Column eight
  - Has the AMR sequence
- Column nine has the position of the first reference base
  in the codon, or NA if there is no codon, or if it could
  not be found
- Column ten has the number of the first codon in the gene
  or NA if not present
- Column eleven has the number of the last codon in the
  gene or NA if not present
- Column twelve
  - has the reference amino acid sequence or
  - 0 if it is not present
    - I am not using NA, because both N and A are amino
      acid codes
- Column thirteen has the AMR amino acid sequence or 0
  - has the AMR amino acid sequence or
  - 0 if it is not present
    - I am not using NA, because both N and A are amino
      acid codes
- Column fourteen has the starting position of the
  reference gene.
- Column fifteen has the ending position of the reference
  gene.
- Column sixteen
  - 1 if the AMR is an high resistance
  - 0 if not an high resistant AMR
- Column seventeen
  - 1 if the AMR is an low resistance
  - 0 if the AMR is not low resistance (high/mid)
- Column eighteen
  - 1 if multiple low resitistance AMR's in the same gene
    can result in high resistance
  - 0 if not AMR effect is not additive
- Column nineteen
  - has a name of a gene that is is needed to be function
    to have the mutation confer antibiotic resistance
  - NA if there is no needed gene
- The next columns (until the endAntibiotics column) are
  specific antibiotics that a AMR confers resistance to.
  - 0 is no known resistance
  - 1 is resistance
  - 2 is cross-resistance
  - 3 is the entry appears in both the resistant and
    cross-resistance entries
- endAntibiotics column marks the end of the antibiotic
  entries. It is filled with '\*'s
- First column after endAntibiotics
  - has the WHO's recorded effects for the AMR
  - NA if no effect
- Second column after endAntibiotics
  - has the comment that the WHO had for the AMR
  - NA if there is no comment
  - This was the last column for the March 18, 2024
    version
- Third column after endAntibiotics (grade)
  - Has the WHOs grade for the primary antibiotic
  - 0 if there is no grade
- Fourth column after endAntibiotics (entireGene)
  - 1 if is this is an LoF entry (frameshift anywere in
    the genome).
  - 2 is for an entire gene deletion
  - 0 if the mutation only applies to a single position
- fith column after end antibiotics (unkownEntry)
  - 1 if I could not figure this entry out
  - 0 if the entry was catagorized
- There could be future columns added, so long as there
  always added at the end. So, always look for the last
  known column by looking for both newlines '\n' and tabs
  '\t' (in C you can do `*char < 31`).
  
## The other files (other tsv's, the csv, and sam)

The WHO-2023-TB-tab1-master.tsv is the master tab (tab
  one) from the WHO's 2023 catalog. It has had the extra
  headers removed and is ready for processing. May not be
  up to date.

The WHO-2023-TB-tab2-indices.tsv is the genome indices
  tab (tab two) from the WHO's 2023 catalog. It is ready
  for processing. May not be up to date.

The NC000962.fa file is the reference genome used with the
  WHO's 2023 catalog.

The gene-tbl.tsv is the table of gene coordinates. The
  help message for the converter will tell you how to
  make this.

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

To buld this database:

```
whoToTbAmr -no-whole-gene-dels -ref NC000962.fa -coords gene-tbl.tsv -tabone-who-2023-tsv WHO-2023-TB-tab1-master.tsv -tabtwo-who-2023-tsv WHO-2023-TB-tab2-indices.tsv -out catalog.tsv
```

To build an resitance database with entire gene deletions

```
whoToTbAmr -no-whole-gene-dels -ref NC000962.fa -coords gene-tbl.tsv -tabone-who-2023-tsv WHO-2023-TB-tab1-master.tsv -tabtwo-who-2023-tsv WHO-2023-TB-tab2-indices.tsv -out catalog.tsv
```

To get every entry in the WHO catalog

```
whoToTbAmr -all-amrs -ref NC000962.fa -coords gene-tbl.tsv -tabone-who-2023-tsv WHO-2023-TB-tab1-master.tsv -tabtwo-who-2023-tsv WHO-2023-TB-tab2-indices.tsv -out catalog.tsv
```

To get every entry that is not an entire gene deletion in
  the WHO catalog

```
whoToTbAmr -all-amrs -no-whole-gene-dels -ref NC000962.fa -coords gene-tbl.tsv -tabone-who-2023-tsv WHO-2023-TB-tab1-master.tsv -tabtwo-who-2023-tsv WHO-2023-TB-tab2-indices.tsv -out catalog.tsv
```

## To build the gene table for `-coords`

This is just a table of gene coordinates in the TB genome.

Do not use paf files from minimap2. For some odd reason
  the coordinates disagree with the sam file by a one or
  two bases.

```
minimap2 \
    -a \
    NC000962.fa \
    genes.fa |
  awk '
      BEGIN{FS=OFS="\t";};

      { # MAIN
         if($0 ~ /^@/) next; # Comment entry
         if($2 == 16) dirSC = "-"; # is reverse complement
         else dirSC = "+";         # is an foward gene

         sub(/M/, "", $6); #Remove matches; becomes length
         --$6; # Acount for index 1
               # Avoids adding two index 1 values

         geneEndSI = $4 + $6;
            # $4 +$6 = gene_start + gene_length = gene_end

         if(geneEndSI < $4)
            print $1, $3, dirSC, geneEndSI, $4;
         else
            print $1, $3, dirSC, $4, geneEndSI;
            # $1 = gene name
            # $3 = reference name
            # dirSC = direction (+ or -)
            # $4 = starting position (index 1)
      } # MAIN
' > gene-tbl.tsv
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
  I am working on. The generalLib folder has some uneeded
  files.

There are some files that are not used in generalLib. This
  is because this is part of an larger project and I am to
  lazy to remove the extra files. Feel free to use them if
  you are intrested in one.

# Bugs/errors in output:

## Comments

This code is done as well as I could get it. However,
  there will likely still be problems. So, there may be
  bugs or errors I missed.

I am using the database in my own code, so please report
  errors in the files or bugs in the code.

## Known bugs:

### With my program

I am not doing an AA translation for frame shifts beyond
  the variant id. However, This could be done. I just do
  not see a need for this.
