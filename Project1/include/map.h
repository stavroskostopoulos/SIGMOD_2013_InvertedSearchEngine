#pragma once

#include"common_types.h"
#include"entry.h"
#include"bst.h"
#include"hash_functions.h"

struct map_node{
    Bst bst; 
};

struct map{
    MapNode array;
    int size;
    int capacity;
    HashFunction hash_function;
    EntryList entry_list;
};

// Creating a Map
Map map_create();

// Destroying a Map
void map_destroy(Map map);

// Inserting a word at a Map
bool map_insert(Map map,String word);

// Rehashing depending on Load Factor
void map_rehash(Map map);

// Find whether given word exists at a Map
bool map_find(Map map,String word);

// Remove given word if it exists at a Map
bool map_remove(Map map,String word);
