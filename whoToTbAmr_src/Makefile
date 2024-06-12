CC=cc
PREFIX=/usr/local/bin
CFLAGS= -static -O3 -std=c89
MACCFLAGS= -O3
DEBUGFLAGS= -static -O0 -ggdb -Wall

SOURCE=\
   whoToTbAmr.c\
   cnvt_whoToTbAmr.c\
   genIndiceStruct.c\
   ../generalLib/drug_str_ary.c\
   ../generalLib/amrStruct.c


all:
	$(CC) $(CFLAGS) -o whoToTbAmr $(SOURCE)

debug:
	$(CC) $(DEBUGFLAGS) -o debugWhoToTbAmr $(SOURCE)
	gdb -x bug-cmds.txt debugWhoToTbAmr

mac:
	$(CC) $(MACCFLAGS) -o whoToTbAmr $(SOURCE)

check:
	$(CC) $(DEBUGFLAGS) -o debugWhoToTbAmr $(SOURCE)

# Not really worth installing, but just in case
install: 
	cp whoToTbAmr $(PREFIX);
	chmod a+x $(PREFIX)/whoToTbAmr;
