CC      = clang
AR			= ar
CFLAGS  = -g -MMD -MP
LDFLAGS = -L/usr/local/opt/openssl/lib -lcrypto -lresolv -lcurl -lpcre
LIBS    =
INCLUDE = -I./include -I./libs
SRC_DIR = ./src
OBJ_DIR = ./build
SOURCES = $(shell ls $(SRC_DIR)/*.c) 
OBJS    = $(subst $(SRC_DIR),$(OBJ_DIR), $(SOURCES:.c=.o))
TARGET  = static shared test
SHARED_FLAGS = -shared 
DEPENDS = $(OBJS:.o=.d)

all: $(TARGET)

static: $(OBJS) $(LIBS)
	echo "[make - static]"
	$(AR) rv t4c.a $^

shared: $(OBJS) $(LIBS)
	echo "[make - shared]"
	$(CC) $(SHARED_FLAGS) -o libt4c.so $(OBJS) $(LDFLAGS)

test: t4c_test/Makefile
	echo "[make - test(static)]"
	cd t4c_test; make test_static

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c 
	@if [ ! -d $(OBJ_DIR) ]; \
			then echo "mkdir -p $(OBJ_DIR)"; mkdir -p $(OBJ_DIR); \
      fi
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $< 

clean:
	$(RM) $(OBJS) $(TARGET) $(DEPENDS)
	rm -f t4c.a libt4c.so 
	cd t4c_test; make clean
	cd libs/sds; rm -f *.o *.a *.so

-include $(DEPENDS)

.PHONY: all clean
