CC = gcc

HFILES = 
CFILES = aesdsocket.c

SRCS = ${HFILES} ${CFILES}
OBJS = ${CFILES:.c=.o}

CFLAGS += -Wall -Werror
LDFLAGS += 

all: aesdsocket

clean:
	-rm -f *.o *.d
	-rm -f aesdsocket

aesdsocket: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o aesdsocket
