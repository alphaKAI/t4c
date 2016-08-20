all: sds-test

sds-test: sds.c sds.h testhelp.h
	$(CC) -o sds-test sds.c -Wall -std=c99 -pedantic -O2 -DSDS_TEST_MAIN
	@echo ">>> Type ./sds-test to run the sds.c unit tests."

static: sds.c sds.h
	$(CC) -c sds.c
	$(AR) rv sds.a sds.o

clean:
	rm -f sds-test
