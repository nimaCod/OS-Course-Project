CC = gcc
CFLAGS = -Wall -g

all: resturant customer

types.o: types.c
	$(CC) $(CFLAGS) -c types.c

cli.o: cli.c types.h
	$(CC) $(CFLAGS) -c cli.c

net.o: net.c cli.h types.h
	$(CC) $(CFLAGS) -c net.c

#supplier.o: supplier.c net.h cli.h types.h
#	$(CC) $(CFLAGS) -c supplier.c

list.o: list.c cli.h types.h
	$(CC) $(CFLAGS) -c list.c

customer.o: customer.c net.h cli.h types.h list.h
	$(CC) $(CFLAGS) -c customer.c

customer: customer.o  net.o cli.o types.o list.o
	$(CC) $(CFLAGS) -o customer customer.o  net.o cli.o types.o list.o

resturant.o: resturant.c net.h cli.h types.h list.h
	$(CC) $(CFLAGS) -c resturant.c

resturant: resturant.o  net.o cli.o types.o list.o
	$(CC) $(CFLAGS) -o resturant resturant.o  net.o cli.o types.o list.o

clean:
	rm -f *.o resturant customer
