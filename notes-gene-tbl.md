I found the coordinates for each gene using minimap2.

```
minimap2 NC000962.fa NC000962-genes.fa > genes.paf;
```

I then removed all alignments that were not primary
  alignments using awk.

```
awk '
      BEGIN{
         FS=OFS="\t";
         printf "gene\treference\tdirection\tfirstBase";
         printf "\tlastBase\n";
      } # BEGIN

      { # MAIN
         if($13 == "tp:A:P") print $1, $6, $5, $8, $9;
         # $1 is the gene name
         # $6 is the reference name
         # $5 is gene direction (+ = foward, - = reverese)
         # $8 is the first reference base
         # $9 is the last reference base
      }; # MAIN
    ' \
    < genes.paf \
  > gene-tbl.tsv;

rm genes.paf; # No longer need
```
