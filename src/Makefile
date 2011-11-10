CC			= gcc
CFLAGS		= -O3 -Wall -I/usr/local/include -I../include
DFLAGS		= -g  -I/usr/local/include -I../include
DEST		=
LDFLAGS		=
LIBS		= ../lib/libutil.a
OBJS		= lexer.o parser.o type.o env.o id.o show.o main.o compile.o \
			  primitives.o expr.o sexpr.o typing.o nexpr.o knormal.o alpha.o \
			  beta.o flatten.o cexpr.o closure.o asm.o virtual.o regalloc.o emit.o
PROGRAM		= a.out
LEXER		= lexer.c
PARSER		= parser.c


all:		$(PROGRAM)

$(PROGRAM):	$(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) $(LIBS) -o $(PROGRAM)

debug:	CFLAGS = $(DFLAGS)
debug:	$(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) $(LIBS) -o $(PROGRAM)

parser:
	$(MAKE) $(LEXER)
	$(MAKE) $(PARSER)

$(LEXER):	lexer.l
	flex -o $(LEXER) lexer.l

$(PARSER):	parser.y
	bison -d -o parser.c parser.y


clean:
	rm -f *.o *~ $(PROGRAM)
