#pragma once

#include"common_types.h"
#include"word.h"
#include"bst.h"
#include"map.h"
#include"dictionary.h"
#include"hash_functions.h"

// Parse a .txt file
// No duplicate words or special characters allowed
int deduplication(FILE* document,Map map);
