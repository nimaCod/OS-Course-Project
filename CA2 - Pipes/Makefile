CC = g++ -std=c++17

all: bills building billing_office app

logger.o: logger.cpp 
	$(CC) -c logger.cpp

serialize.o: serialize.cpp logger.h
	$(CC) -c serialize.cpp

bills.o: bills.cpp serialize.h logger.h
	$(CC) -c bills.cpp 

bills: bills.o serialize.o logger.o
	$(CC) -o bills bills.o serialize.o logger.o

building.o: building.cpp serialize.h logger.h
	$(CC) -c building.cpp

building:  building.o serialize.o logger.o
	$(CC) -o building building.o serialize.o logger.o

billing_office.o: billing_office.cpp serialize.h logger.h
	$(CC) -c billing_office.cpp  

billing_office:  billing_office.o serialize.o logger.o
	$(CC) -o billing_office billing_office.o serialize.o logger.o

app.o: app.cpp serialize.o logger.o
	$(CC) -c app.cpp

app: app.o serialize.o logger.o
	$(CC) -o app app.o serialize.o logger.o

# types.o: types.c
#         $(CC)  -c types.c

# cli.o: cli.c types.h
#         $(CC)  -c cli.c

# net.o: net.c cli.h types.h
#         $(CC)  -c net.c

# #supplier.o: supplier.c net.h cli.h types.h
# #       $(CC)  -c supplier.c

# list.o: list.c cli.h types.h
        # $(CC)  -c list.c

# customer.o: customer.c net.h cli.h types.h list.h
        # $(CC)  -c customer.c

# customer: customer.o  net.o cli.o types.o list.o
#         $(CC)  -o customer customer.o  net.o cli.o types.o list.o

# resturant.o: resturant.c net.h cli.h types.h list.h
#         $(CC)  -c resturant.c

# resturant: resturant.o  net.o cli.o types.o list.o
#         $(CC)  -o resturant resturant.o  net.o cli.o types.o list.o

clean:
		rm -f *.o bills building billing_office app full_log.txt









