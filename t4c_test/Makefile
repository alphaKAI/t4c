CC      = clang
AR			= ar
CFLAGS  = -g -MMD -MP
LDFLAGS = -lcrypto -lresolv -lcurl -lpcre
STATIC_LIBS = ./libs/t4c/t4c.a
INCLUDE = -I./include -I./libs/t4c/include
SRC_DIR = ./src
OBJ_DIR = ./build
SOURCES = $(shell ls $(SRC_DIR)/*.c) 
OBJS    = $(subst $(SRC_DIR),$(OBJ_DIR), $(SOURCES:.c=.o))
TARGET  = test_shared test_static
DEPENDS = $(OBJS:.o=.d)

all: $(TARGET)

t4c_static: libs/t4c/Makefile
	cd libs/t4c; make static

t4c_shared: libs/t4c/Makefile
	cd libs/t4c; make shared

test_static: $(OBJS) t4c_static
	echo "[make - test_static]"
	$(CC) $(CFLAGS) -o test_static $(OBJS) $(STATIC_LIBS) $(LDFLAGS)

test_shared: $(OBJS) t4c_shared
	echo "[make - test_shared]"
	$(CC) $(CFLAGS) -o test_shared $(OBJS) -L./libs/t4c -lt4c

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c 
	@if [ ! -d $(OBJ_DIR) ]; \
			then echo "mkdir -p $(OBJ_DIR)"; mkdir -p $(OBJ_DIR); \
      fi
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $< 

clean:
	$(RM) $(OBJS) $(TARGET) $(DEPENDS)
	rm -f libt4c.so test_static test_shared
	rm -rf build

-include $(DEPENDS)

.PHONY: all clean
