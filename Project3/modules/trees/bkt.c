#include "../../include/bkt.h"


// // Distance Tools
// unsigned int minOfTwo(int x, int y) {
//     return x > y ? y : x;
// }

// unsigned int minOfThree(int x, int y, int z) {
//     return minOfTwo(minOfTwo(x, y), z);
// }

// //Distance Metrics 
// unsigned int HammingDistance(String a, String b){

//     int i, min, max, result;

//     result=0;

//     (strlen(a) < strlen(b)) ? (min=strlen(a), max=strlen(b)) : (min=strlen(b), max=strlen(a));

//     for(i=0 ; i<min ; i++){
//         if(a[i]!=b[i]) result++;
//     }

//     return result+max-min;

// }

// unsigned int EditDistance(String str1, String str2){
//     if (!strlen(str1)) return strlen(str2);
//     if (!strlen(str2)) return strlen(str1);
    
//     if (str1[0] == str2[0]) return EditDistance(&str1[1], &str2[1]);
    
//     return (1 + minOfThree(
//                     EditDistance(&str1[1], str2),
//                     EditDistance(str1, &str2[1]),
//                     EditDistance(&str1[1], &str2[1])
//                 )
//             );
// }

unsigned int EditDistance(String a, int na, String b, int nb)
{
	int oo=0x7FFFFFFF;

    int T[2][MAX_WORD_LENGTH+1];

	int ia, ib;

	int cur=0;
	ia=0;

	for(ib=0;ib<=nb;ib++)
		T[cur][ib]=ib;

	cur=1-cur;

	for(ia=1;ia<=na;ia++)
	{
		for(ib=0;ib<=nb;ib++)
			T[cur][ib]=oo;

		int ib_st=0;
		int ib_en=nb;

		if(ib_st==0)
		{
			ib=0;
			T[cur][ib]=ia;
			ib_st++;
		}

		for(ib=ib_st;ib<=ib_en;ib++)
		{
			int ret=oo;

			int d1=T[1-cur][ib]+1;
			int d2=T[cur][ib-1]+1;
			int d3=T[1-cur][ib-1]; if(a[ia-1]!=b[ib-1]) d3++;

			if(d1<ret) ret=d1;
			if(d2<ret) ret=d2;
			if(d3<ret) ret=d3;

			T[cur][ib]=ret;
		}

		cur=1-cur;
	}

	int ret=T[1-cur][nb];

	return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////

// Computes Hamming distance between a null-terminated string "a" with length "na"
//  and a null-terminated string "b" with length "nb" 
unsigned int HammingDistance(String a, int na, String b, int nb)
{
	int j, oo=0x7FFFFFFF;
	if(na!=nb) return oo;
	
	unsigned int num_mismatches=0;
	for(j=0;j<na;j++) if(a[j]!=b[j]) num_mismatches++;
	
	return num_mismatches;
}


index_node_ptr create_index_node(const String word, List payload){

    index_node_ptr newnode = (index_node_ptr)malloc(sizeof(index_node));
    
    newnode->word = strdup(word);

    newnode->payload = list_create();
    for(ListNode temp=payload->head ; temp!=NULL ; temp=temp->next){
        list_insert_tail(newnode->payload, temp->value);
    }

    newnode->parent_distance = 0;
    newnode->children_number = 0;
    newnode->children = NULL;

    return newnode;
}


void build_entry_index(Core core, const EntryList el, MatchType type, Index_ptr ix){

    if(get_first(el)==NULL){
        perror("Entry list was empty!\nNo index was created!\n");
        return;
    }

    // ix = (Index_ptr)malloc(sizeof(bkindex));

    //Create the root of the BK tree
    Entry temp = get_first(el);

    ix->root = create_index_node(temp->word, temp->payload);
    


    //Now we should traverse through the entry list and build the entry index
    Entry next_entry = NULL;
    next_entry = get_next(el, temp);
    // int i=0;
    while(next_entry!=NULL){
        // printf("for build %d\n", i++);
        add_index_node(core, ix->root, create_index_node(next_entry->word, next_entry->payload), type);
        next_entry = get_next(el, next_entry);        
    }


    return;
}

void build_entry_index_from_dictionary(Core core, Dictionary dictionary, MatchType type, Index_ptr ix){

    int root_flag=0;

    for(int i=0;i<dictionary->capacity;i++){

        if(dictionary->array[i].entry_list->size!=0){
            for(Entry entry=dictionary->array[i].entry_list->head ; entry!=NULL ; entry=entry->next){
                //if this is the first entry we are seeing, then create the root
                if(!(root_flag++)){
                    ix->root = create_index_node(entry->word, entry->payload);
                    continue;   
                }

                add_index_node(core, ix->root, create_index_node(entry->word, entry->payload), type);
                

            }
        }
    }


    if(!root_flag){
        perror("Entry list was empty!\nNo index was created!\n");
        
    }

    return;

}

void fill_hamming_ix_array_from_dictionary(Core core, Index_ptr* array, Dictionary dictionary, MatchType type){

    //MIN_WORD_LENGTH 4 | MAX_WORD_LENGTH 31 so we'll need 28 cells
    for(int i=0 ; i<28 ; i++){
        array[i]=NULL;
    }

    //Iterate through haming dictionary
    for(int i=0;i<dictionary->capacity;i++){
        if(dictionary->array[i].entry_list->size!=0){
            for(Entry entry=dictionary->array[i].entry_list->head ; entry!=NULL ; entry=entry->next){
                //if this is the first entry we are seeing, with thsi word length
                if( array[ strlen(entry->word)-4 ] == NULL ){
                    array[ strlen(entry->word)-4 ] = (Index_ptr)malloc(sizeof(bkindex));
                    array[ strlen(entry->word)-4 ]->root = create_index_node(entry->word, entry->payload);
                    continue;   
                }

                //if there is already a word with such length
                add_index_node(core, array[ strlen(entry->word)-4 ]->root, create_index_node(entry->word, entry->payload), type);
                

            }
        }
    }

    return;

}


void fill_hamming_ix_array(Core core, Index_ptr* array, EntryList el, MatchType type){

    //MIN_WORD_LENGTH 4 | MAX_WORD_LENGTH 31 so we'll need 28 cells
    for(int i=0 ; i<28 ; i++){
        array[i]=NULL;
    }
    if(el->size!=0){
        for(Entry entry=el->head ; entry!=NULL ; entry=entry->next){
                //if this is the first entry we are seeing, with such word length
                if( array[ strlen(entry->word)-4 ] == NULL ){
                    array[ strlen(entry->word)-4 ] = (Index_ptr)malloc(sizeof(bkindex));
                    array[ strlen(entry->word)-4 ]->root = create_index_node(entry->word, entry->payload);
                    continue;   
                }

                //if there is already a word with such length
                add_index_node(core, array[ strlen(entry->word)-4 ]->root, create_index_node(entry->word, entry->payload), type);
                

            }
    }

}



void add_index_node(Core core, index_node_ptr parent, index_node_ptr newnode, MatchType type){
    // printf("add_index\n");
    unsigned int distance;

    // printf("pro dist\n");

    if(type==MT_HAMMING_DIST) distance = HammingDistance(parent->word, strlen(parent->word), newnode->word, strlen(newnode->word)); // Hamming Distance
    else{
        
    // pthread_mutex_lock(&core->job_scheduler->stupidmtx);
    distance = EditDistance(parent->word, strlen(parent->word), newnode->word, strlen(newnode->word));  // Edit Distance
    // pthread_mutex_unlock(&core->job_scheduler->stupidmtx);
    } 
    // printf("after dist\n");
    


    //Iterate through children o this bk tree node and check whether there exists a kid with the same metric distance 
    //with the word we want to insert
    int i;
    // printf("\nPARENT %s %d\n", parent->word,parent->parent_distance);
    // printf("NEWNODE %s %d\n\n", newnode->word, parent->children_number);
    // int j=0;
    for(i=0 ; i<parent->children_number ; i++){
    
        
        //if there is a children node with such distance 
        if(parent->children[i]->parent_distance == distance){
            // printf("palevw %d\n", j++);
            //recursive function call for this node
            add_index_node(core, parent->children[i], newnode, type);
            return;
        }
    }
    // printf("neo length\n ");
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
    list_destroy(node->payload);
    free(node);

}

void destroy_entry_index(Index_ptr ix){
    destroy_index_nodes(ix->root);
    free(ix);
}

void recursive_search(Core core, const String w, index_node_ptr node, int threshold, EntryList result, MatchType type){

    //The search algorithm contains 3 steps:

    //1)

    //String cand_word = pop_word_list(candidates);

    unsigned int distance;


    if(type==MT_HAMMING_DIST) distance = HammingDistance(node->word, strlen(node->word), w, strlen(w)); // Hamming Distance
    else{

    // pthread_mutex_lock(&core->job_scheduler->stupidmtx);
    distance = EditDistance(node->word, strlen(node->word), w, strlen(w));  // Edit Distance
    // pthread_mutex_unlock(&core->job_scheduler->stupidmtx);
    } 
    

    //If the distance is LE to the threshold then add it to result
    if(distance<=threshold){
        add_entry(result, create_entry_with_payload(node->word, node->payload));
    }
    
    //free(cand_word);

    //Now we should add all the children of our current node that fullfil the [d-n, d+n] requirement
    int i;

    for( i=0 ; i<node->children_number ; i++ ){
        if((node->children[i]->parent_distance >= (distance-threshold)) && (node->children[i]->parent_distance <= (distance+threshold))){
            //add_word_to_list(node->children[i]->word, candidates);
            recursive_search(core, w,node->children[i], threshold, result, type);
        }
    }

    
}

void lookup_entry_index(Core core, const String w, Index_ptr ix, int threshold, EntryList result, MatchType type){

    //we might come here by a hamming match type and there is high possibility that there is no BK tree with such word length
    if(ix==NULL){
        //so,set the result entry list empty, and return
        result->head=NULL;
        return;
    }
    recursive_search(core, w,ix->root, threshold, result, type);
}

void destroy_hamming_array(Index_ptr* array){

    for(int i=0 ; i<28 ; i++){
        if(array[i]!=NULL){
            destroy_entry_index(array[i]);
        }
    }

    free(array);
    
}


void printbk(index_node_ptr root){
    if(root==NULL){
        return;
    }

    printf("%s\n", root->word);
    for(int i=0 ; i<root->children_number ; i++ ){
        printbk(root->children[i]);
    }
}