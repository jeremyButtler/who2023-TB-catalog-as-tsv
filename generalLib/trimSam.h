#ifndef TRIMSAM_H
#define TRIMSAM_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
| trimSam SOF: Start Of Functions
|  - fun-01 trimSamEntry:
|    o Trim soft mask regions off end of sam entry
|  - fun-02 trimSamReads:
|    o Trims soft mask regions for all reads with a
|      sequence in a sam file
\~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*-------------------------------------------------------\
| Name: trimSamEntry (Fun-01:)
| Use:
|  - Trims off the soft masked regions of a sam entry
| Input:
|  - samVoidST:
|    o samEntry struct with sam entry to trim. Named
|      void so I can cast it as a samEntry to samST
| Output:
|  - Returns:
|    o 0 if suceeded
|    o 2 if header (invalid and ignored)
|    o 4 if an unmapped read (no reference)
|    o 8 if no sequence line
|  - Modifies:
|    o Trims cigar, sequence, & q-score entries in samST.
\-------------------------------------------------------*/
unsigned char trimSamEntry(void *samVoidST);

/*-------------------------------------------------------\
| Name: trimSamReads (Fun-02:)
| Use:
|  - Goes though sam file and calls trimSamEntry for each
|    entry
| Input:
|  - samFILE:
|    o Pointer to FILE. Sam file with entries to trim
|  - outFILE:
|    o File to write the trimmed sam file entries to
|  - keepUnmappedReadsBl:
|    o Also print out entries with unmapped reads
| Output:
|  - Returns:
|    o 0 if suceeded
|    o 64 for memory error
|  - Prints:
|    o Trimmed sam entries with sequences to outFILE, but
|      ignores sam entries without sequences
\-------------------------------------------------------*/
unsigned char trimSamReads(
    void *samFILE,              /*Sam file to trim*/
    void *outFILE,              /*File to store output*/
    char keepUnmappedReadsBl   /*1: keep unmapped reads*/
);

#endif
