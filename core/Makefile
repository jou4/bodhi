CC			= gcc
CFLAGS		= -O3 -Wall -I/usr/local/include
DEST		=
LDFLAGS		=
LIBS		=
OBJS		= core.o
PROGRAM		= libcore.a
AR			= ar


all:		$(PROGRAM)

$(PROGRAM):	$(OBJS)
	$(AR) rv $(PROGRAM) $(OBJS)

clean:
	rm -f *.o *~ $(PROGRAM)
