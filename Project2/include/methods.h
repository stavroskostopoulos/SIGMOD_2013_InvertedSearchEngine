#pragma once

#include"common_types.h"
#include"word.h"
#include"bst.h"
#include"map.h"
#include"dictionary.h"
#include"hash_functions.h"
#include"core.h"

// Parse a .txt file
// No duplicate words or special characters allowed
int deduplication(FILE* document,Map map);

// Tokenize a String and don't allow duplicates to be inserted at the Map
int dedup(String string,Map map,QueryID id);

// Check whether a word already exists at a Map
bool exact_matching(Map map,String word);