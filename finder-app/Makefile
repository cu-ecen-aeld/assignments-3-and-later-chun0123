CROSS_COMPILE ?= gcc
writer: writer.o
	$(CROSS_COMPILE) writer.o -o writer
writer.o : writer.c
	${CROSS_COMPILE} -c writer.c

clean:
	rm -rf writer *.o
