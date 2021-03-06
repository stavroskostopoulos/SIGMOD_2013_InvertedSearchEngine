#include"../../include/acutest.h"
#include"../../include/bst.h"
#include"../../include/map.h"
#include"../../include/linked_list.h"
#include"../../include/methods.h"

static String create_random_string(){
    // Create alphabet
    char chars[]="abcdefghijklmnopqrstuvwxyz";
    // Lenght of alphabet
    int len=sizeof(chars)-1;
    // Find a random lenght for the string
    int random_string_len=rand()%len+1;
    // Allocate memory for the string
    String string=malloc(random_string_len+1);
    for(int i=0;i<random_string_len;i++){
        // Find random letter
        int random=rand()%len;
        // Insert at position i the random letter
        string[i]=chars[random];
    }
    // NULL terminated
    string[random_string_len]='\0';
    // Return the string
    return string;
}

void test_create(void){
    Map map=map_create();
    TEST_ASSERT(map!=NULL);
    TEST_ASSERT(map->array!=NULL);
    TEST_ASSERT(map->capacity==53);
    TEST_ASSERT(map->hash_function==hash_string);
    TEST_ASSERT(map->size==0);
    for(int i=0;i<map->capacity;i++){
        TEST_ASSERT(map->array[i].bst!=NULL);
    }
    map_destroy(map);
}

void test_insert(void){
    Map map=map_create();
    String* strings=malloc(sizeof(*strings)*10000);
    for(int i=0;i<10000;i++){
        strings[i]=create_random_string();
        int size=map->size;
        bool inserted=map_insert(map,strings[i]);
        if(inserted==true){
            TEST_ASSERT(map->size==size+1);
        }
        else{
            TEST_ASSERT(map->size==size);
        }
    }
    for(int i=0;i<10000;i++){
        free(strings[i]);
    }
    free(strings);
    map_destroy(map);
}

void test_find(void){
    Map map=map_create();
    String* strings=malloc(sizeof(*strings)*10000);
    LinkedList list=linked_list_create();
    for(int i=0;i<10000;i++){
        strings[i]=create_random_string();
        map_insert(map,strings[i]);
        linked_list_insert_end(list,strings[i]);
    }
    for(LinkedListNode node=list->head;node!=NULL;node=node->next){
        bool exists=map_find(map,node->string);
        TEST_ASSERT(exists==true);
    }
    map_destroy(map);
    linked_list_destroy(list);
    for(int i=0;i<10000;i++){
        free(strings[i]);
    }
    free(strings);
}

void test_remove(void){
    Map map=map_create();
    LinkedList list=linked_list_create();
    String* strings=malloc(sizeof(*strings)*10000);
    for(int i=0;i<10000;i++){
        strings[i]=create_random_string();
        map_insert(map,strings[i]);
        linked_list_insert_end(list,strings[i]);
    }
    for(LinkedListNode node=list->head;node!=NULL;node=node->next){
        int size=map->size;
        bool removed=map_remove(map,node->string);
        TEST_ASSERT(removed==true);
        TEST_ASSERT(map->size==size-1);
    }
    map_destroy(map);
    linked_list_destroy(list);
    for(int i=0;i<10000;i++){
        free(strings[i]);
    }
    free(strings);
}


TEST_LIST = {
    {"map_create",test_create},
    {"map_insert",test_insert},
    {"map_find",test_find},
    {"map_remove",test_remove},
    {NULL,NULL}
};
