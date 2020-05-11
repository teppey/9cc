CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

sobacc: $(OBJS)
	$(CC) -o sobacc $(OBJS) $(LDFLAGS)

$(OBJS): sobacc.h

test: sobacc
	./test.sh

clean:
	rm -f sobacc *.o *~ tmp* testfunc/*.o

.PHONY: test clean
