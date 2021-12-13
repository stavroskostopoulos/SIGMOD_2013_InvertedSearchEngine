#pragma once

#include"common_types.h"
#include"word.h"
#include"bst.h"
#include"map.h"
#include"dictionary.h"
#include"hash_functions.h"
#include"core.h"

///////////////////////////////////////////////////////////////////////////////////////////////

// Keeps all information related to an active query
struct Query
{
	QueryID query_id;
	char str[MAX_QUERY_LENGTH];
	MatchType match_type;
	unsigned int match_dist;
};

///////////////////////////////////////////////////////////////////////////////////////////////


struct core{
    Map document;
    Dictionary exact_queries; //A hashtable that stores the queries with exact match type( buckets are entry lists)
    Dictionary edit_queries; //A hashtable that stores the queries with edit match type( buckets are entry lists)
    Dictionary hamming_queries; //A hashtable that stores the queries with hamming match type( buckets are entry lists)
    Query active_queries;
    uint active_queries_number;
};

// Global struct to store all the needed structs
Core core_create();

// Destroy the global struct
void core_destroy(Core core);

// Parse a .txt file
// No duplicate words or special characters allowed
int deduplication(FILE* document,Map map);

// Tokenize a String and don't allow duplicates to be inserted at the Map
int dedup(String string, Core core, QueryID id, MatchType match_type);

// Check whether a word already exists at a Map
bool exact_matching(Map map,String word);
