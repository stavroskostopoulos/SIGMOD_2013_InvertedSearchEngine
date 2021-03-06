#include "../../include/bkt.h"

// Distance Tools
unsigned int minOfTwo(int x, int y) {
    return x > y ? y : x;
}

unsigned int minOfThree(int x, int y, int z) {
    return minOfTwo(minOfTwo(x, y), z);
}

//Distance Metrics 
unsigned int HammingDistance(String a, String b){

    int i, min, max, result;

    result=0;

    (strlen(a) < strlen(b)) ? (min=strlen(a), max=strlen(b)) : (min=strlen(b), max=strlen(a));

    for(i=0 ; i<min ; i++){
        if(a[i]!=b[i]) result++;
    }

    return result+max-min;

}

unsigned int EditDistance(String str1, String str2){
    if (!strlen(str1)) return strlen(str2);
    if (!strlen(str2)) return strlen(str1);
    
    if (str1[0] == str2[0]) return EditDistance(&str1[1], &str2[1]);
    
    return (1 + minOfThree(
                    EditDistance(&str1[1], str2),
                    EditDistance(str1, &str2[1]),
                    EditDistance(&str1[1], &str2[1])
                )
            );
}



index_node_ptr create_index_node(const String word){

    index_node_ptr newnode = (index_node_ptr)malloc(sizeof(index_node));
    
    newnode->word = strdup(word);
    newnode->parent_distance = 0;
    newnode->children_number = 0;
    newnode->children = NULL;

    return newnode;
}


void build_entry_index(const EntryList el, Metric type, Index_ptr ix){

    if(get_first(el)==NULL){
        perror("Entry list was empty!\nNo index was created!\n");
        return;
    }

    // ix = (Index_ptr)malloc(sizeof(bkindex));

    //Create the root of the BK tree
    Entry temp = get_first(el);

    ix->root = create_index_node(temp->word);
    


    //Now we should traverse through the entry list and build the entry index
    Entry next_entry = NULL;
    next_entry = get_next(el, temp);
    
    while(next_entry!=NULL){
        add_index_node(ix->root, create_index_node(next_entry->word), type);
        next_entry = get_next(el, next_entry);        
    }


    return;
}


void add_index_node(index_node_ptr parent, index_node_ptr newnode, Metric type){
    
    unsigned int distance;

    if(type==0) distance = HammingDistance(parent->word, newnode->word); // Hamming Distance
    else distance = EditDistance(parent->word, newnode->word);  // Edit Distance

    //Iterate through children o this bk tree node and check whether there exists a kid with the same metric distance 
    //with the word we want to insert
    int i;
    // printf("\nPARENT %s %d\n", parent->word,parent->parent_distance);
    // printf("NEWNODE %s %d\n\n", newnode->word, parent->children_number);

    for(i=0 ; i<parent->children_number ; i++){
    
        // printf("forloop %d asdsad\n\n", i);
        
        //if there is a children node with such distance 
        if(parent->children[i]->parent_distance == distance){
            //recursive function call for this node
            add_index_node(parent->children[i], newnode, type);
            return;
        }
    }
    
    //if no node with such metric distance was found:
    //add new node to parent

    //create a new slot for the new node
    parent->children = (index_node_ptr*)realloc(parent->children, (parent->children_number + 1)*sizeof(index_node_ptr));
    parent->children[parent->children_number] = newnode;
    
    //update children numebr of parent
    parent->children_number++;
    
    
    //update the parent distance of the new node we are adding
    newnode->parent_distance = distance;
    
    
}

void destroy_index_nodes(index_node_ptr node){
    int i;

    for( i=0 ; i<node->children_number ; i++ ){
        destroy_index_nodes(node->children[i]);
    }

    if(node->children != NULL) free(node->children);
    
    free(node->word);
    free(node);

}

void destroy_entry_index(Index_ptr ix){
    destroy_index_nodes(ix->root);
    free(ix);
}

void recursive_search(const String w, index_node_ptr node, int threshold, EntryList result, Metric type){

    //The search algorithm contains 3 steps:

    //1)

    //String cand_word = pop_word_list(candidates);

    unsigned int distance;

    if(type==0) distance = HammingDistance(node->word, w); // Hamming Distance
    else distance = EditDistance(node->word, w);  // Edit Distance

    //If the distance is LE to the threshold then add it to result
    if(distance<=threshold){
        add_entry(result, create_entry(node->word));
    }
    
    //free(cand_word);

    //Now we should add all the children of our current node that fullfil the [d-n, d+n] requirement
    int i;

    for( i=0 ; i<node->children_number ; i++ ){
        if((node->children[i]->parent_distance >= (distance-threshold)) && (node->children[i]->parent_distance <= (distance+threshold))){
            //add_word_to_list(node->children[i]->word, candidates);
            recursive_search(w,node->children[i], threshold, result, type);
        }
    }

    
}

void lookup_entry_index(const String w, Index_ptr ix, int threshold, EntryList result, Metric type){
    recursive_search(w,ix->root, threshold, result, type);
}