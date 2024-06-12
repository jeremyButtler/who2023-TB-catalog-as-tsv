# Goal:

This is not very great, but it is here to give you an
  idea on how this code functions and how to navigate
  the code.

# The first function:

The first step in this code is to read in the 2023
  catalog. This is done by the read\_2023\_WhoAmrTsv
  function (fun-05 cnvt\_whoToTbAmr.h). This function
  reads in the genome indice tab (tab 2) using functions
  in genIndiceStruct.c/h. Once read in the genome indices
  are sorted by variant ID using an shell sort. This
  allows quick look up times later.

The next step in read\_2023\_whoAmrTsv is to get the
  number of lines in the master tab (tab 1). After that
  there is some memory allocation and then I re-read in
  the catalog. I then check to see if the antibiotic is
  an new antibiotic. If so, I update the users input.

I then extract my target information I can get from tab 1
  until I get to the grade. I then use the grade to decide
  if I should keep the entry or not. After that I extract
  the remaining entries, and use the "comment" and
  "additional grading" entries to get if a AMR is high
  resistance, low resistance, has an additive affect,
  or if it confirs any cross resistance.

I then use a binary search to find the matching entry in
  the genome indice etneries. If there is no matching
  entry I check to see if the user is keeping these
  entries. If so I check to see if it is an LoF or
  deletion entry and set the entire gene flag.

For entries with an matching entry in the genome indices
  tab I check to see if the next genome entry is an match.
  If it is, I then copy the extracted information into
  the next AMR and then copy the sequence entries and
  genome coordinates from the genome indice entries. After
  this I repeat this step until I have copied all the
  information from the genome indices entries for all
  copies.

The input for read\_2023\_whoAmrTsv are

  - C-string with path to the master tab (tab 1)
  - C-string with path to the genome indices tab (tab 2)
  - Pointer to an unsigned long to hold the number of AMRs
  - Pointer to an c-string to hold the antibiotic names
    - use the free function to free this.
  - Pointer to an integer to hold the number of
    antibiotics
  - Boolean: 1 to keep non-resitance AMRs (grade 3 to 5)
  - Boolean: 1 to keep entire gene deletions
  - Pointer to an character to hold the error message
    - Set to def\_amrST\_memError for memory errors
    - Set to def\_amrST\_invalidFILE for file errors

The return value is an array amrStruct (see st-01 in
  amrStruct.h). It needs to be freeded with
  freeAmrStructArray(&amrStruct, number\_of\_AMRs).

# The second function:

The second function you will need to call is
  who\_parse\_varID (fun-06 cnvt\_whoToTbAmr.c). This
  functions uses the amrStruct from read\_2023\_whoAmrTsv
  and gets the amino acid information from the variant id.
  It also allocates memory for the expected amino acid
  sequence size and fills in the first and last amino
  acids for large deletions and duplications. The
  insertions are also filled in, since they are in the
  variant ID.

The input for who\_parse\_varID is 

- the amrStruct structure array from read\_2023\_whoAmrTsv
- The number of amrStructs in the amrStruct array

This function returns 0 for success and
  def\_amrST\_memError for memory error. It will never
  free the amrStruct array.

# The third function:

The third functio is whoAddCodonPos (fun-07
  cnvt\_whoToTbAmr.c). This functions takes in the
  amrStruct array with processed variant ids from
  who\_parse\_varId and then adds in the genome
  coordinates, the reference position of the first codon,
  and the reading frame. It will also fill in the missing
  parts of the amino acid sequences for large deletions
  and duplications.

The first thing it does is find the length of the input
  gene table and then reads it in (the gene id and
  reference id are ignored). It then sorts the starting
  position, ending position, and reading frame direction
  by the starting postion.

After this the reference sequence is read in and newlines,
  spaces, and tabs are removed. This assumes there is only
  one sequence in the reference file.

The next step is to move past the entires without
  postions, since these apply to the entire genome.

After that checks are done to see if the entrie excedes
  a genes limit. If it does, then the variant id of the
  prevoius sequence is checked to see if it is the same.
  If not, then the index for the gene starting array,
  ending array, and direction is incurmented by one. The
  gene direction and starting, and ending positons are
  then copied and for entires with amino acid sequences,
  I move to the next entiry.

For amino acid sequences I find the starting codon
  position `gene_start + (codon_number - 1) * 3` for
  foward genes. For reverse genes I
  use `gene_end - (codon_number - 1) * 3`. For genes that
  are filled in I then move on. However, for large
  duplications and deletions, I use the reference sequence
  to get the translated sequence.

After all genes, except the entries without reference
  postions (at 0) are finshed I the sort the amrStruct
  array be variant ID. I then fill in the information for
  the variants missing reference positions using the
  variants for the same gene (gene start, end, and
  direction). The reference postion for these entries are
  set to the first reference base for easier sorting
  later.

I then resort the amrStruct array by reference postion.

Input for whoAddCodonPos:

- amrStruct array modifed by who\_parse\_varId
- number of AMRs (length of amrStruct array)
- The path to the gene table
- The path to the reference file

Returns 0 for success, def\_amrST\_memError for an memory
  error, and def\_amrST\_invalidFILE for an file error.
  This will never free any input.

# The fourth function:

The final function is pAmrDB (fun-10 amrStruct.c), which
  prints out the entries in the amrStruct array to an
  file.

Input fro pAmrDB:

- amrStruct array form whoAddCodonPos
- number of AMRs (length of amrStruct array)
- Pointer to c-string to with the antibiotic names
  - This was filled in by read\_2023\_WhoAmrTsv
- Pointer to an integer to hold the number of
    antibiotics
  - This was filled in by read\_2023\_WhoAmrTsv
- C-string with the file name to output to
  - nothing (null) or '-' is for stdout

# Reading in AMRs:

I need to test this with the new data. It should work.
  Use readTbAmrTbl (fun-11 in amrStruct.c). Check the
  function header to see how it works. This returns an
  array of amrStructs. Just to warn you there may be some
  pointers set to null.

# Style:

My style is very odd.

First I switched ot only include libraries in my .c files
  for this project. This was due to some exposure to
  plan9, my logic is that coding this way only takes some
  pratice and it allows my code to be more easy to port.
  However, I am not sure if this will even compile on
  plan9.

I also use a series of headers to allow my to quickly
  jump around in code. My method is to keep the table
  of content entries in lower case and have the first
  letter of each header in uppercase. That way I can
  type `Fun-01` to go to function 01 quickly. Or type
  `License:` to move to the license quickly. Sometimes
  this method is confusing, sometimes I ingore it and use
  page down, other times it is really usefull.

## Block one file header

When properally documente files will start of with an
  header block that holds an description of the files
  tasks.

```
/*########################################################
# Name: file-name
#   - description
########################################################*/
```

## Block two file table of contents

The next entry is an table of contents for the whole
  file (both for .c and .h)

```
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
' SOF: Start Of File
'   o header:
'     - description of what is in header
'   o fun-01: name
'     - description of functions task
'   o fun-02: name
'     - description of functions task
'   o fun-03: name
'     - description of functions task
'   .
'   .
'   .
'   o fun-xx: name
'     - description of functions task
'   o license:
'     - Licensing for this code (license name(s))
\~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
```

The values inside the SOF are fun (for function/macro),
  st (for structure), and header (for header section).
  Each entry can be preceded by an `.h` for only in the
  `.h` file or `.c` for only in the `.c` file. Older
  variations may not have this. Each entry is lower case
  to allow searching with upper case (ex: `Fun-01` to find
  fuction 01).

## Block three file the real header block

The next entry is my header block

```
/*-------------------------------------------------------\
| Header:
|   - description of what is in header
\-------------------------------------------------------*/
```

This is for the libraries (.c files only), the
  defined/global variables (often .h), and the header
  guards (.h only) (in case someone else uses).

## Block four; function blocks

The next entries are my blocks marking each functio


```
/*-------------------------------------------------------\
| Fun-01: name
|   - Descritpion of what function does
| Input:
|   - variable 1:
|     - description of varible one
|   - variable 2:
|     - description of varible two
|   - variable 3:
|     - description of varible three
|   .
|   .
|   .
|   - variable n:
|     - description of varible n
| Output:
|   - Returns:
|     o return value success
|     o return value failure
\-------------------------------------------------------*/
```

This block starts of with the function number, then the
  name. It then provides and description. Then it lists
  what the input is and what I expect it to look like.
  After this is the output.

Some common output items:

- Returns: Returns something from the function
- Modifies: Modifies input values
- Frees: Frees input values
- Prints: Prints out something

## Extra blocks:

### Function table of contents:

The table of contents for the fuction, which has an
  description and lists all the sections in the function.
  This is only present when the fuctio is divided into
  sections.

```
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\
' Fun-01 TOC:
'   - repeat description of function
'   o fun-01 sec-01:
'     - description of section one
'   o fun-01 sec-02:
'     - description of section two
'   o fun-01 sec-03:
'     - description of section three
'   .
'   .
'   .
'   o fun-01 sec-xx:
'     - description of section xx
\~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
```

### Sections:

After the mandatory blocks there can also be section
  (sec) blocks which list what they do and any subsections
  they have.

Simple version

```
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
^ Fun-01 Sec-01:
^   - description of section 01
\<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
```

Version with subsections

```
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\
^ Fun-01 Sec-01:
^   - description of section 01
^   o fun-01 sec-01 sub-01:
^     - description of the 1st subsection of section one
^   o fun-01 sec-01 sub-02:
^     - description of the 2nd subsection of section one
^   o fun-01 sec-01 sub-03:
^     - description of the 3rd subsection of section one
^   .
^   .
^   .
^   o fun-01 sec-01 sub-xx:
^     - description of the nth subsection of section one
\<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
```

### Subsections:

The subsection blocks are withing sections and have an
  description about what they do and list any catagories.
  Most of the time these will not have any cataogires and
  will just be an description line.

Simple version


```
/********************************************************\
* Fun-01 Sec-01 Sub-01:
*   - description of section 01 subsection 01
\********************************************************/
```

Version with catagories


```
/********************************************************\
* Fun-01 Sec-01 Sub-01:
*   - description of section 01 subsection 01
*   o fun-01 sec-01 sub-01 cat-01:
*     - description of 1st subsection of section 1 cat 01
*   o fun-01 sec-01 sub-01 cat-02:
*     - description of 2nd subsection of section 1 cat 02
*   o fun-01 sec-01 sub-01 cat-03:
*     - description of 3rd subsection of section 1 cat 03
*   .
*   .
*   .
*   o fun-01 sec-01 sub-01 cat-xx:
*     - description of  nth subsection of section 1 cat xx
\********************************************************/
```

### Catagories

The catagories blocks are the end of the dividing. They
  have a breif description.


```
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++\
+ Fun-01 Sec-01 Sub-01 Cat-01:
+   - description of section 01 subsection 01 Catagory 01
\+++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
```

## License block:

At the end of the file is the license block. This one is
  rather new for me.


```
/*=======================================================\
: License:
: 
: This code is under the unlicense (public domain).
:   However, for cases were the public domain is not
:   suitable, such as countries that do not respect the
:   public domain or were working with the public domain
:   is inconvient / not possible, this code is under the
:   MIT license.
: 
: Public domain:
: 
: This is free and unencumbered software released into the
:   public domain.
: 
: Anyone is free to copy, modify, publish, use, compile,
:   sell, or distribute this software, either in source
:   code form or as a compiled binary, for any purpose,
:   commercial or non-commercial, and by any means.
: 
: In jurisdictions that recognize copyright laws, the
:   author or authors of this software dedicate any and
:   all copyright interest in the software to the public
:   domain. We make this dedication for the benefit of the
:   public at large and to the detriment of our heirs and
:   successors. We intend this dedication to be an overt
:   act of relinquishment in perpetuity of all present and
:   future rights to this software under copyright law.
: 
: THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
:   ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
:   LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
:   FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO
:   EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM,
:   DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
:   CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
:   IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
:   DEALINGS IN THE SOFTWARE.
: 
: For more information, please refer to
:   <https://unlicense.org>
: 
: MIT License:
: 
: Copyright (c) 2024 jeremyButtler
: 
: Permission is hereby granted, free of charge, to any
:   person obtaining a copy of this software and
:   associated documentation files (the "Software"), to
:   deal in the Software without restriction, including
:   without limitation the rights to use, copy, modify,
:   merge, publish, distribute, sublicense, and/or sell
:   copies of the Software, and to permit persons to whom
:   the Software is furnished to do so, subject to the
:   following conditions:
: 
: The above copyright notice and this permission notice
:   shall be included in all copies or substantial
:   portions of the Software.
: 
: THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
:   ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
:   LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
:   FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO
:   EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
:   FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
:   AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
:   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
:   USE OR OTHER DEALINGS IN THE SOFTWARE.
\=======================================================*/
```
