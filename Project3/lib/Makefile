# paths
MODULES = ../modules
INCLUDE = ../include

# compiler
CC = gcc

# Compile options. Το -I<dir> λέει στον compiler να αναζητήσει εκεί include files
CFLAGS = -O3 -fPIC -Wall -Werror -g -I$(INCLUDE)
LDFLAGS = -lpthread -lm

# Αρχεία .o για δημιουργία της βιβλιοθήκης libcore.so
OBJS = $(MODULES)/entries/word.o $(MODULES)/entries/entry.o \
$(MODULES)/trees/bst.o $(MODULES)/trees/bkt.o \
$(MODULES)/lists/word_list.o $(MODULES)/lists/list.o $(MODULES)/lists/fifoqueue.o $(MODULES)/lists/query_list.o \
$(MODULES)/hash/hash_functions.o $(MODULES)/hash/dictionary.o \
$(MODULES)/hash/map.o $(MODULES)/hash/query_map.o $(MODULES)/hash/bloom_filter.o \
$(MODULES)/methods/core.o $(MODULES)/methods/deduplication.o $(MODULES)/methods/global_core.o \
$(MODULES)/methods/query.o $(MODULES)/methods/documents.o $(MODULES)/methods/job_scheduler.o \
$(MODULES)/vector/vector.o $(MODULES)/trees/bk-tree.o


# Η βιβλιοθήκη
LIBRARY = libcore.so

$(LIBRARY): $(OBJS)
	$(CC) $(OBJS) -shared -o $(LIBRARY) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(LIBRARY)
