#include"../../include/query_map.h"
#include"../../include/methods.h"

#define MAX_QUERY_MAP_LOAD_FACTOR 0.9

int prime_query_sizes[] = {53, 97, 193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317, 196613, 393241,
	786433, 1572869, 3145739, 6291469, 12582917, 25165843, 50331653, 100663319, 201326611, 402653189, 805306457, 1610612741};

QueryMap query_map_create(){
    // Creating an Active Query Set
    QueryMap query_map=malloc(sizeof(*query_map));

    // Set size to 0
    query_map->size=0;
    // Set starting capacity to the first prime of the array
    query_map->capacity=prime_query_sizes[0];
    // Set Hash Function
    query_map->hash_function=hash_string;

    // Allocate memory for the array
    query_map->array=malloc(sizeof(*query_map->array)*query_map->capacity);
    // On each Node
    for(int i=0;i<query_map->capacity;i++){
        // Create a List
        query_map->array[i].query_list=query_list_create();
    }

    return query_map;
}

QueryMap query_map_copy_create(QueryMap old){
    // Creating an Active Query Set
    QueryMap query_map=malloc(sizeof(*query_map));

    // Set size to 0
    query_map->size=0;
    // Set starting capacity to the first prime of the array
    query_map->capacity=old->capacity;
    // Set Hash Function
    query_map->hash_function=hash_string;

    // Allocate memory for the array
    query_map->array=malloc(sizeof(*query_map->array)*query_map->capacity);
    // On each Node
    for(int i=0;i<query_map->capacity;i++){
        // Create a List
        query_map->array[i].query_list=query_list_create();
    }

    //copy its contents
    copy_query_map(old, query_map);

    return query_map;
}

void copy_query_map(QueryMap old, QueryMap new){

    for(int i=0;i<old->capacity;i++){
        if(old->array[i].query_list->size>0){
            for(QueryListNode node=old->array[i].query_list->head;node!=NULL;node=node->next){
                Query copy_query = query_create(node->query->query_id, node->query->match_type, node->query->match_dist);
                passWords_to_query(copy_query, node->query);
                query_list_insert_raw(new->array[i].query_list, copy_query);
            }
        }
    }

}

void query_map_destroy(QueryMap query_map){
    // On each Node
    for(int i=0;i<query_map->capacity;i++){
        // Destroy the list
        query_list_destroy(query_map->array[i].query_list);
    }
    // Free the array
    free(query_map->array);
    // Free the Map
    free(query_map);
}

void query_map_insert(QueryMap query_map,QueryID query_id,String query_str,MatchType match_type,uint match_dist){
    // Create the Query
    Query query=query_create(query_id,match_type,match_dist);
    // Cleanup of each word and insert it at its word array
    query_cleanup(query,query_str);

    // Create the QueryID as a String
    String string_id=malloc(10);
    sprintf(string_id,"%d",query->query_id);

    // Find Hash Position
    ulong pos=query_map->hash_function(string_id)%query_map->capacity;
    // Find Node
    QueryMapNode node=&query_map->array[pos];

    // Insert the Query at the list
    query_list_insert_tail(node->query_list,query);

    // Increase the size
    query_map->size++;
    // Free the id
    free(string_id);

    // Find Load Factor
    float load_factor=(float)query_map->size/query_map->capacity;
    if(load_factor>MAX_QUERY_MAP_LOAD_FACTOR){
        // Rehash if it got increased
        query_map_rehash(query_map);
    }
}

void query_map_rehash(QueryMap query_map){
    // Find old array
    QueryMapNode old_array=query_map->array;
    // Find old capacity
    int old_capacity=query_map->capacity;

    int primes=sizeof(prime_query_sizes)/sizeof(int);

    // Find the next prime, if it exists
    for(int i=0;i<primes;i++){
        if(prime_query_sizes[i]>old_capacity){
            query_map->capacity=prime_query_sizes[i];
            break;
        }
    }
    // Or continue by doubling up the capacity, if all primes are used
    if(query_map->capacity==old_capacity){
        query_map->capacity*=2;
    }

    // Allocate memory for the array
    query_map->array=malloc(sizeof(*query_map->array)*query_map->capacity);
    // Set size to 0
    query_map->size=0;

    // On each Node
    for(int i=0;i<query_map->capacity;i++){
        // Create a new List
        query_map->array[i].query_list=query_list_create();
    }

    // Insert everything to the new Query Map
    for(int i=0;i<old_capacity;i++){
        QueryList query_list=old_array[i].query_list;
        if(query_list->size>0){
            for(QueryListNode node=query_list->head;node!=NULL;node=node->next){
                // Allocate memory for the query_str
                String str=malloc(MAX_QUERY_LENGTH);
                memset(str,0,MAX_QUERY_LENGTH);
                // For each of Node's Query's words
                for(int j=0;j<node->query->query_words_num;j++){
                    // Copy the word to the query_str
                    strcat(str,node->query->words[j]);
                    // And a blank space
                    strcat(str," ");
                }
                // Insert at the QueryMap
                query_map_insert(query_map,node->query->query_id,str,node->query->match_type,node->query->match_dist);
                // Free query_str
                free(str);
            }
        }
        // Free the old list
        query_list_destroy(query_list);
    }
    // Free the old array
    free(old_array);
}

void query_map_remove(QueryMap query_map,uint id){
    // Create QueryID as a String
    String string_id=malloc(10);
    sprintf(string_id,"%d",id);
    // Find Hash Position
    ulong pos=query_map->hash_function(string_id)%query_map->capacity;
    // Find Node of Hash Position
    QueryMapNode node=&query_map->array[pos];
    // Query List of the Node
    QueryList query_list=node->query_list;
    // Remove the id
    bool removed=query_list_detach(query_list,id);
    if(removed==true){
        query_map->size--;
    }
    // Free the String
    free(string_id);
}

Query query_map_find(QueryMap query_map,uint id){
    // Create QueryID as a String
    String string_id=malloc(10);
    sprintf(string_id,"%d",id);
    // Find Hash Position
    ulong pos=query_map->hash_function(string_id)%query_map->capacity;
    // Find Node of Hash Position
    QueryMapNode node=&query_map->array[pos];
    // Query List of the Node
    QueryList query_list=node->query_list;
    // Traverse the List and Find the Query with given id
    for(QueryListNode lnode=query_list->head;lnode!=NULL;lnode=lnode->next){
        if(lnode->query->query_id==id){
            free(string_id);
            return lnode->query;
        }
    }
    free(string_id);
    return NULL;
}
