# Use:

Converts gene coordinates in an tbAmr database from one
  Mycrobacterium *tuberculosis* reference to another
  reference. This only does an strict conversion, so it
  does not check to see if reading frames are present, if
  reading frames are not shifted, or if genes are shifted
  around. It assumes the numeric positions are always
  correct.

Only use full genomes that are close to H37Rv. I am not
  sure what an partial or distant genome would do.

# License:

This is under an dual license. The first license is public
  domain, however, not all companies/countries like public
  domain. In those cases or any other case were public
  domain is inconvenient this is under the MIT license.

# Install:

## Linux (static):

```
make
sudo make install
```

## Mac (or non-static):

```
make mac
sudo make install
```

# Usage:

swapDbRef uses an sam file (the cigar) that has the new
  reference mapped to the old reference used for the
  database. The cigar is used to correct the coordinates.

```
minimap2 -a oldRef.fasta newRef.fasta > guide.sam
swapDbRef -amr amr-database.tsv -sam guide.sam > newDb.tsv
```
