/*
 * CS 261 Data Structures
 * Assignment 5
 * Name: Eric Johnson
 * Date: 12/1/19
 */

#include "hashMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

int hashFunction1(const char* key)
{
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        r += key[i];
    }
    return r;
}

int hashFunction2(const char* key)
{
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        r += (i + 1) * key[i];
    }
    return r;
}

/**
 * Creates a new hash table link with a copy of the key string.
 * @param key Key string to copy in the link.
 * @param value Value to set in the link.
 * @param next Pointer to set as the link's next.
 * @return Hash table link allocated on the heap.
 */
HashLink* hashLinkNew(const char* key, int value, HashLink* next)
{
    HashLink* link = malloc(sizeof(HashLink));
    link->key = malloc(sizeof(char) * (strlen(key) + 1));
    strcpy(link->key, key);
    link->value = value;
    link->next = next;
    return link;
}

/**
 * Free the allocated memory for a hash table link created with hashLinkNew.
 * @param link
 */
static void hashLinkDelete(HashLink* link)
{
    free(link->key);
    free(link);
}

/**
 * Initializes a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param map
 * @param capacity The number of table buckets.
 */
void hashMapInit(HashMap* map, int capacity)
{
    map->capacity = capacity;
    map->size = 0;
    map->table = malloc(sizeof(HashLink*) * capacity);
    for (int i = 0; i < capacity; i++)
    {
        map->table[i] = NULL;
    }
}

/**
 * Removes all links in the map and frees all allocated memory. You can use
 * hashLinkDelete to free the links.
 * @param map
 */
void hashMapCleanUp(HashMap* map)
{
    // FIXME: implement
    assert(map != NULL);

    //Declare structs that will be used to iterate through each link
    struct HashLink *link;
    struct HashLink *next;

    //Loop to go through each link in map
    for(int i = 0; i < map->capacity; i++){
        link = map->table[i];

        if(link != NULL){
            do {
                // Iterate and remove each link
                next = link->next;
                hashLinkDelete(link);
                link = next;
            } while (next != NULL);
        }
    }

    // Free allocated memory
    free(map->table);
    map->table = NULL;
}

/**
 * Creates a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param capacity The number of buckets.
 * @return The allocated map.
 */
HashMap* hashMapNew(int capacity)
{
    HashMap* map = malloc(sizeof(HashMap));
    hashMapInit(map, capacity);
    return map;
}

/**
 * Removes all links in the map and frees all allocated memory, including the
 * map itself.
 * @param map
 */
void hashMapDelete(HashMap* map)
{
    hashMapCleanUp(map);
    free(map);
}

/**
 * Returns a pointer to the value of the link with the given key  and skip traversing as well. Returns NULL
 * if no link with that key is in the table.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 * 
 * @param map
 * @param key
 * @return Link value or NULL if no matching link.
 */
int* hashMapGet(HashMap* map, const char* key)
{
    // FIXME: implement
    assert(map != NULL);
    assert(key != NULL);

    //Compute hash value to find the correct bucket
    int hashIndex = HASH_FUNCTION(key) % map->capacity;
    if(hashIndex < 0) {
        hashIndex += map->capacity;
    }

    struct HashLink *link = map->table[hashIndex];

    //Loop through map to find given key value
    while(link != NULL){
        //Compare iterator link's key to key passed as parameter
        if(strcmp(link->key, key) == 0) {
            //Return pointer to value of link with the given key
            return &link->value;
        } else {
            link = link->next;
        }
    }

    return NULL;
}

/**
 * Resizes the hash table to have a number of buckets equal to the given 
 * capacity (double of the old capacity). After allocating the new table, 
 * all of the links need to rehashed into it because the capacity has changed.
 * 
 * Remember to free the old table and any old links if you use hashMapPut to
 * rehash them.
 * 
 * @param map
 * @param capacity The new number of buckets.
 */
void resizeTable(HashMap* map, int capacity)
{
    // FIXME: implement
    assert(map != NULL);
    assert(capacity != 0);

    //Initialize new hash map with capcity passed as parameter
    struct HashMap *temp = hashMapNew(capacity);

    //Declare HashLink struct to be used to put values into new table
    struct HashLink *link;

    //Loop to go through each link of the old table
    for(int i = 0; i < map->capacity; i++){

        //Copy value from old table
        link = map->table[i];
        
        //Loop to add links using hashMapPut
        while(link != NULL){
            hashMapPut(temp, link->key, link->value);
            //Iterate to next HashLink
            link = link->next;
        }
    }

    //Free old map
    hashMapCleanUp(map);

    // Set data members of map to new table
    map->table = temp->table;
    map->size = temp->size;
    map->capacity = temp->capacity;

    // Free allocated memory of temp table
    free(temp);
}

/**
 * Updates the given key-value pair in the hash table. If a link with the given
 * key already exists, this will just update the value and skip traversing. Otherwise, it will
 * create a new link with the given key and value and add it to the table
 * bucket's linked list. You can use hashLinkNew to create the link.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket.
 * 
 * @param map
 * @param key
 * @param value
 */
void hashMapPut(HashMap* map, const char* key, int value)
{
    // FIXME: implement
    assert(map != NULL);
    assert(key != NULL);

    //Compute hash value to find the correct bucket
    int hashIndex = HASH_FUNCTION(key) % map->capacity;
    if(hashIndex < 0) {
        hashIndex += map->capacity;
    }

    struct HashLink *link = map->table[hashIndex];

    //Loop to find link with given key
    while (link != NULL) {
        if(strcmp(link->key, key) == 0){
            // Update the value and return to skip traversing
            link->value = value;
            return;
        }
        
        //Iterate through each HashLink
        link = link->next;
    }

    // No matching value found, create new link with the given key and value to bucket's linked list
    link = hashLinkNew(key, value, map->table[hashIndex]);
    map->table[hashIndex] = link;
    map->size++;

    // Resize table if table load is greater than the max
    if(hashMapTableLoad(map) > MAX_TABLE_LOAD){
        resizeTable(map, 2 * hashMapCapacity(map));
    }
}

/**
 * Removes and frees the link with the given key from the table. If no such link
 * exists, this does nothing. Remember to search the entire linked list at the
 * bucket. You can use hashLinkDelete to free the link.
 * @param map
 * @param key
 */
void hashMapRemove(HashMap* map, const char* key)
{
    // FIXME: implement
    assert(map != NULL);
    assert(key != NULL);

    //Compute hash value to find the correct bucket
    int hashIndex = HASH_FUNCTION(key) % map->capacity;
    if(hashIndex < 0) {
        hashIndex += map->capacity;
    }

    //Declare structs to iterate through bucket
    struct HashLink *link = map->table[hashIndex];
    struct HashLink *next = map->table[hashIndex]->next;

    //Return if given link does not exist
    if(link == NULL){
        return;
    }

    //Delete link if it matches key passed in parameter
    if(strcmp(link->key, key) == 0){
        hashLinkDelete(link);

        //Update deleted link to next HashLink and update the table size
        map->table[hashIndex] = next;
        map->size--;
        return;
    }

    //Loop to search through list for given key at the bucket
    while(next != NULL){
        //Delete HashLink if match is found with given key and update table size
        if(strcmp(next->key, key) == 0){
            link->next = next->next;
            hashLinkDelete(next);
            map->size--;
            return;
        //Iterate HashLinks
        } else {
            link = link->next;
            next = next->next;
        }
    }
}

/**
 * Returns 1 if a link with the given key is in the table and 0 otherwise.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 * 
 * @param map
 * @param key
 * @return 1 if the key is found, 0 otherwise.
 */
int hashMapContainsKey(HashMap* map, const char* key)
{
    // FIXME: implement
    assert(map != NULL);
    assert(key != NULL);

    //Compute hash value to find the correct bucket
    int hashIndex = HASH_FUNCTION(key) % map->capacity;
    if(hashIndex < 0) {
        hashIndex += map->capacity;
    }

    struct HashLink *link = map->table[hashIndex];

    //Loop to find given key, returns 1 if found
    while(link != NULL){
        if(strcmp(link->key, key) == 0) {
            return 1;
        }
        //Iterate link
        else {
            link = link->next;
        }

    }
    return 0;
}

/**
 * Returns the number of links in the table.
 * @param map
 * @return Number of links in the table.
 */
int hashMapSize(HashMap* map)
{
    // FIXME: implement
    assert(map != NULL);

    return map->size;
}

/**
 * Returns the number of buckets in the table.
 * @param map
 * @return Number of buckets in the table.
 */
int hashMapCapacity(HashMap* map)
{
    // FIXME: implement
    assert(map != NULL);

    return map->capacity;
}

/**
 * Returns the number of table buckets without any links.
 * @param map
 * @return Number of empty buckets.
 */
int hashMapEmptyBuckets(HashMap* map)
{
    // FIXME: implement
    assert(map != NULL);

    int count = 0;

    //Loop to find empty buckets without any links
    for(int i = 0; i < map->capacity; i++){
        if(map->table[i] == NULL){
            count++;
        }
    }
    return count;
}

/**
 * Returns the ratio of (number of links) / (number of buckets) in the table.
 * Remember that the buckets are linked lists, so this ratio tells you nothing
 * about the number of empty buckets. Remember also that the load is a floating
 * point number, so don't do integer division.
 * @param map
 * @return Table load.
 */
float hashMapTableLoad(HashMap* map)
{
    // FIXME: implement
    assert(map != NULL);

    return (double)map->size / map->capacity;
}

/**
 * Prints all the links in each of the buckets in the table.
 * @param map
 */
void hashMapPrint(HashMap* map)
{
    for (int i = 0; i < map->capacity; i++)
    {
        HashLink* link = map->table[i];
        if (link != NULL)
        {
            printf("\nBucket %i ->", i);
            while (link != NULL)
            {
                printf(" (%s, %d) ->", link->key, link->value);
                link = link->next;
            }
        }
    }
    printf("\n");
}