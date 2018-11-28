all:tracer global_data sampling

CFLAGS=-g -O0 -Wall

tracer:tracer.o rbtree.o Ip_Network.o tracer.o

global_data:	rbtree.o Ip_Network.o global_data.o

sampling:	rbtree.o Ip_Network.o sampling.o

rbtree.o:rbtree.h rbtree.c

Ip_Network.o:Ip_Network.h Ip_Network.c

tracer.o:tracer.c

global_data.o:global_data.c

sampling.o:sampling.c

.PHONY:clean

clean:
	rm *.o test parse_timeline
