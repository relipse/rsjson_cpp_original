RSJSONFILES = RSJsonParser.h RSJsonBasic.h RSJsonInt.h RSJsonArray.h RSJsonObject.h RSJsonFloat.h RSJsonString.h
OBJS = rsjson_tester.o RSJson.o
CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall -ansi $(DEBUG)

tester: test_rsjson.cpp $(RSJSONFILES)
	$(CC) $(LFLAGS) -DDEBUG_RSJSON test_rsjson.cpp -o rsjson_tester 

rsjson_tester : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o rsjson_tester

rsjson_tester.o : test_rsjson.cpp
	$(CC) $(CFLAGS) test_rsjson.cpp

RSJson.o : RSJsonParser.h RSJsonBasic.h RSJsonInt.h RSJsonArray.h RSJsonObject.h RSJsonFloat.h RSJsonString.h
	$(CC) $(CFLAGS) RSJson.h
uncrustify:
	../uncrustify --replace -c ../defaults.cfg -F *.h
clean:
	\rm *.o *~ rsjson_tester

tar:
	tar cfv rsjson.tar test_rsjson.cpp $(RSJSONFILES) Makefile README
