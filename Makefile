SRCDIR=./src
BINDIR=./bin
BISONDIR=./bison
FLEXDIR=./flex

CC=gcc
CFLAGS= -g -Wall

BISON=bison
BFLAGS= --output=$(SRCDIR)/parser.c --defines=$(SRCDIR)/parser.h

FLEX=flex
FFLAGS= --outfile=$(SRCDIR)/lexer.c

make:
	$(CC) $(CFLAGS) -c -o $(BINDIR)/parser.o $(SRCDIR)/parser.c;
	$(CC) $(CFLAGS) -c -o $(BINDIR)/lexer.o $(SRCDIR)/lexer.c;
	$(CC) $(CFLAGS) -c -o $(BINDIR)/burp2.o $(SRCDIR)/burp2.c;
	$(CC) $(CFLAGS) -c -o $(BINDIR)/robot.o $(SRCDIR)/robot.c;
	$(CC) $(CFLAGS) -c -o $(BINDIR)/arena.o $(SRCDIR)/arena.c;
	$(CC) $(CFLAGS) -c -o $(BINDIR)/main.o $(SRCDIR)/main.c;
	$(CC) $(CFLAGS) -o main $(BINDIR)/*.o -lm -lncurses

bison:
	$(BISON) $(BFLAGS) $(BISONDIR)/parser.y;

flex:
	rm $(SRCDIR)/lexer.c;
	$(FLEX) $(FFLAGS) $(FLEXDIR)/lexer.l;

clean:
	rm -rf $(BINDIR)/*.o main
