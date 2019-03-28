#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
  Hash table key/value pair with linked list pointer.

  Note that an instance of `LinkedPair` is also a node in a linked list.
  More specifically, the `next` field is a pointer pointing to the the 
  next `LinkedPair` in the list of `LinkedPair` nodes. 
 */
typedef struct LinkedPair {
  char *key;
  char *value;
  struct LinkedPair *next;
} LinkedPair;

/*
  Hash table with linked pairs.
 */
typedef struct HashTable {
  int capacity;
  LinkedPair **storage;
} HashTable;

/*
  Create a key/value linked pair to be stored in the hash table.
 */
LinkedPair *create_pair(char *key, char *value)
{
  LinkedPair *pair = malloc(sizeof(LinkedPair));
  pair->key = strdup(key);
  pair->value = strdup(value);
  pair->next = NULL;

  return pair;
}

/*
  Use this function to safely destroy a hashtable pair.
 */
void destroy_pair(LinkedPair *pair)
{
  if (pair != NULL) {
    free(pair->key);
    free(pair->value);
    free(pair);
  }
}

/*
  djb2 hash function

  Do not modify this!
 */
unsigned int hash(char *str, int max)
{
  unsigned long hash = 5381;
  int c;
  unsigned char * u_str = (unsigned char *)str;

  while ((c = *u_str++)) {
    hash = ((hash << 5) + hash) + c;
  }

  return hash % max;
}

/*
  Fill this in.

  All values in storage should be initialized to NULL
 */
HashTable *create_hash_table(int capacity)
{
  HashTable *ht;
  ht = malloc(capacity * sizeof(HashTable));
  ht->capacity = capacity;
  ht->storage = calloc(capacity, sizeof(LinkedPair *));
  return ht;
}

/*
  Fill this in.

  Inserting values to the same index with different keys should be
  added to the corresponding LinkedPair list.

  Inserting values to the same index with existing keys can overwrite
  the value in th existing LinkedPair list.
 */
void hash_table_insert(HashTable *ht, char *key, char *value)
{
  int index = hash(key, ht->capacity);
  //printf("Index: %d\n", index);
  LinkedPair *linkedPair = create_pair(key, value);

  //If there's nothing in the index insert and return
  if(!ht->storage[index]) {
    ht->storage[index] = linkedPair;
    return;
  }

  //Get a reference of the head pointer
  LinkedPair *head = ht->storage[index];
  LinkedPair *prev = NULL;

  //Iterate through linked list
  while(head) {
    if(strcmp(key, head->key) == 0) {
      head->value = value;
      return;
    }
    if(!head->next) prev = head;
    head = head->next;
  }

  head = linkedPair;
  prev->next = head;

}

/*
  Fill this in.

  Should search the entire list of LinkedPairs for existing
  keys and remove matching LinkedPairs safely.

  Don't forget to free any malloc'ed memory!
 */
void hash_table_remove(HashTable *ht, char *key)
{
  int index = hash(key, ht->capacity);

  LinkedPair *head = ht->storage[index];
  LinkedPair *prev = NULL;
  LinkedPair *new_next = NULL;

  if(!head) return;

  while(head) {

    //If a next head exists and its the key.
    //Assign this as previous
    //This is needed to link prev and new_next after
    //removing current key element
    if(head->next && strcmp(key, head->next->key) == 0){
      prev = head;;
    }

    //This is the head(Beginning element in Linked List)
    //Handling edge case and returning
    if(!prev) {
      prev = head;
      head = head->next;
      destroy_pair(prev);
      ht->storage[index] = head;
      return;
    }


    //Assign new_next
    //Even if it doesn't NULL by default
    //Free linked list node
    //Assign prev to next_next
    if(strcmp(key, head->key) == 0) {
      new_next = head->next;
      free(head);
      prev->next = new_next;
      return;
    }
    head = head->next;
  }

}

/*
  Fill this in.

  Should search the entire list of LinkedPairs for existing
  keys.

  Return NULL if the key is not found.
 */
char *hash_table_retrieve(HashTable *ht, char *key)
{
  int index = hash(key, ht->capacity);
  LinkedPair *head = ht->storage[index];

  if(!head) {
    return NULL;
  }

  while(head) {
    if(strcmp(key, head->key) == 0) {
      return head->value;
    }
    head = head->next;
  }

  printf("Key doesn't exist, can't retrieve\n");
  return NULL;
}

/*
  Fill this in.

  Don't forget to free any malloc'ed memory!
 */
void destroy_hash_table(HashTable *ht)
{
    for(int i = 0; i < ht->capacity; i++){
        if(ht->storage[i] != NULL) {
            destroy_pair(ht->storage[i]);
        }
    }
    free(ht->storage);
    free(ht);
}


/*
  Fill this in.

  Should create a new hash table with double the capacity
  of the original and copy all elements into the new hash table.

  Don't forget to free any malloc'ed memory!
 */
HashTable *hash_table_resize(HashTable *ht)
{
  int new_capacity = ht->capacity *  2;
  HashTable *new_ht = create_hash_table(new_capacity);

  //Iterate through array for each head. If head exists
  //in i execute while loop and insert each linked list node
  //to new hash table. If it doesn't don't execute while loop
  //instead continue the loop
  for(int i = 0; i < ht->capacity; i++) {
    LinkedPair *head;
    if(ht->storage[i]){
      head = ht->storage[i];
    } else {
      continue;
    }
    while(head) {
      hash_table_insert(new_ht, head->key, head->value);
      head = head->next;
    }
  }

  free(ht->storage);
  return new_ht;
}

void print_linked_list(HashTable *ht, char *key) {
  int index = hash(key, ht->capacity);
  LinkedPair *head = ht->storage[index];
  while(head) {
    printf("%s -> ", head->key);
    head = head->next;
  }
  printf("NULL\n");
}


#ifndef TESTING
int main(void)
{
  struct HashTable *ht = create_hash_table(2);

  hash_table_insert(ht, "line_1", "Tiny hash table\n"); //Index 1
  hash_table_insert(ht, "line_2", "Filled beyond capacity\n"); //Index 0
  hash_table_insert(ht, "line_3", "Linked list saves the day!\n"); //Index 1
  hash_table_insert(ht, "line_5", "Moin's Insert!\n"); //Index 1

  printf("%s", hash_table_retrieve(ht, "line_1"));
  printf("%s", hash_table_retrieve(ht, "line_2"));
  printf("%s", hash_table_retrieve(ht, "line_3"));
  printf("%s", hash_table_retrieve(ht, "line_5"));
  hash_table_remove(ht, "line_1");
  printf("%s\n", hash_table_retrieve(ht, "line_1"));
  print_linked_list(ht, "line_3");
  int old_capacity = ht->capacity;
  ht = hash_table_resize(ht);
  int new_capacity = ht->capacity;

  printf("\nResizing hash table from %d to %d.\n", old_capacity, new_capacity);
  print_linked_list(ht, "line_3");

  destroy_hash_table(ht);

  return 0;
}
#endif
