```
minimap2 \
    -a \
    freezeTbFiles/TB-NC000962.fa \
    freezeTbFiles/TB-NC000962-genes.fa |
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
' > gene-tab.tsv
```

