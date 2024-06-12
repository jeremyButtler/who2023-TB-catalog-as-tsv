CC=cc
PREFIX=/usr/local/bin
CFLAGS= -static -O3 -std=c89
MACCFLAGS= -O3
DEBUGFLAGS= -static -O0 -ggdb -Wall


all:
	make CC="$(CC)" CFLAGS="$(CFLAGS)" -C whoToTbAmr_src
	make CC="$(CC)" CFLAGS="$(CFLAGS)" -C swapDbRefSrc

mac:
	make mac CC="$(CC)" MACCFLAGS="$(MACCFLAGS)" -C whoToTbAmr_src
	make mac CC="$(CC)" MACCFLAGS="$(MACCFLAGS)" -C swapDbRefSrc

# Not really worth installing, but just in case
install: 
	mv whoToTbAmr_src whoToTbAmr $(PREFIX);
	mv swapDbRef whoToTbAmr $(PREFIX);
	chmod a+x $(PREFIX)/whoToTbAmr;
	chmod a+x $(PREFIX)/swapDbRef;
