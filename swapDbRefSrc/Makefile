CC=cc
PREFIX=/usr/local/bin
CFLAGS=\
   -O3 \
   -std=c89 \
   -Wall \
   -static

DEBUGCFLAGS=\
   -O0 \
   -std=c89 \
   -ggdb \
   -Wall \
   -static

MACCFLAGS=\
   -O3

source=\
   ../generalLib/samEntryStruct.c \
   ../generalLib/drug_str_ary.c \
   ../generalLib/amrStruct.c \
   refSwap.c \
   swapDbRef.c

all:
	$(CC) $(CFLAGS) -o swapDbRef $(source)

mac:
	$(CC) $(MACCFLAGS) -o swapDbRef $(source)

check:
	$(CC) $(DEBUGCFLAGS) -o bug-swapDbRef $(source)

bug:
	$(CC) $(DEBUGCFLAGS) -o bug-swapDbRef $(source)
	gdb -x bug-cmds.txt bug-swapDbRef;
