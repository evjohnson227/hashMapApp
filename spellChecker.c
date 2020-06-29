#include "hashMap.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Adapted from: https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C
#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

/**
 * Allocates a string for the next word in the file and returns it. This string
 * is null terminated. Returns NULL after reaching the end of the file.
 * @param file
 * @return Allocated string or NULL.
 */
char* nextWord(FILE* file)
{
    int maxLength = 16;
    int length = 0;
    char* word = malloc(sizeof(char) * maxLength);
    while (1)
    {
        char c = fgetc(file);
        if ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            c == '\'')
        {
            if (length + 1 >= maxLength)
            {
                maxLength *= 2;
                word = realloc(word, maxLength);
            }
            word[length] = c;
            length++;
        }
        else if (length > 0 || c == EOF)
        {
            break;
        }
    }
    if (length == 0)
    {
        free(word);
        return NULL;
    }
    word[length] = '\0';
    return word;
}

// Function that calculates the Levenshtein Distance between two strings
// Adapted from: https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C
int calcLevenshteinDistance(char* s1, char* s2) {
    unsigned int s1len, s2len, x, y, lastdiag, olddiag;
    s1len = strlen(s1);
    s2len = strlen(s2);
    unsigned int column[s1len+1];
    for (y = 1; y <= s1len; y++)
        column[y] = y;
    for (x = 1; x <= s2len; x++) {
        column[0] = x;
        for (y = 1, lastdiag = x-1; y <= s1len; y++) {
            olddiag = column[y];
            column[y] = MIN3(column[y] + 1, column[y-1] + 1, lastdiag + (s1[y-1] == s2[x-1] ? 0 : 1));
            lastdiag = olddiag;
        }
    }
    return(column[s1len]);
}

/**
 * Loads the contents of the file into the hash map.
 * @param file
 * @param map
 */
void loadDictionary(FILE* file, HashMap* map)
{
    // FIXME: implement
    assert(file != NULL);
    assert(map != NULL);

    char* word = nextWord(file);

    while (word != NULL) {
        hashMapPut(map, word, 1);
        free(word);
        word = nextWord(file);
    }

}

/**
* Converts the given string to lower case
* @param word
* Adapted from: https://www.programmingsimplified.com/c/program/c-program-change-case
*/
void lower_string(char* word) {
    int c = 0;

    while (word[c] != '\0') {
        if(word[c] >= 'A' && word[c] <= 'Z') {
            word[c] = word[c] + 32;
        }
        c++;
    }
}


/**
 * Checks the spelling of the word provded by the user. If the word is spelled incorrectly,
 * print the 5 closest words as determined by a metric like the Levenshtein distance.
 * Otherwise, indicate that the provded word is spelled correctly. Use dictionary.txt to
 * create the dictionary.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, const char** argv)
{
    // FIXME: implement
    HashMap* map = hashMapNew(1000);

    FILE* file = fopen("dictionary.txt", "r");
    clock_t timer = clock();
    loadDictionary(file, map);
    timer = clock() - timer;
    printf("Dictionary loaded in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
    fclose(file);

    char inputBuffer[256];
    int quit = 0;
    while (!quit)
    {
        printf("Enter a word or \"quit\" to quit: ");
        scanf("%s", inputBuffer);

        // Implement the spell checker code here..

        //Convert user string to lower case chars
        lower_string(inputBuffer);

        //Allocate memory for word input by user
        char* word = malloc(sizeof(struct HashLink*));
        word = inputBuffer;

        if (strcmp(inputBuffer, "quit") == 0)
        {
            quit = 1;
        }

        if(!quit) {
            //Check to see if word is in the dictionary
            if(hashMapGet(map, word) == NULL) {
                printf("The inputted word %s is spelled incorrectly\n", word);

                //Allocate array of 5 words to suggest to user
                struct HashLink** suggestWords = malloc(5 * sizeof(struct HashLink*));

                //Loop to initialize array to NULL pointers
                for(int i = 0; i < 5; i++) {
                    suggestWords[i] = NULL;
                }

                //Loop to get suggested words using Levenshtein Distance
                for(int j = 0; j < hashMapCapacity(map); j++) {
                    //Struct used to iterate through the table
                    struct HashLink *current = map->table[j];

                    if(current != NULL) {
                        while(current != NULL) {
                            int distance = calcLevenshteinDistance(word, current->key);

                            //Use Levenshtein Distance as key for each link
                            hashMapPut(map, current->key, distance);

                            //Initialize variables to determine what words to suggest based on Levenshtein Distance
                            int smallestDistance = 9999;
                            int smallestDistanceIndex = -1;


                            //Loop to allocate and store suggested words into array
                            for(int k = 0; k < 5; k++) {
                                //Fill array with suggested words
                                if(suggestWords[k] == NULL) {
                                    suggestWords[k] = malloc(sizeof(struct HashLink*));
                                    //Set data members of each suggested word
                                    suggestWords[k]->key = current->key;
                                    suggestWords[k]->value = current->value;
                                    suggestWords[k]->next = NULL;
                                } else if(current->value < suggestWords[k]->value && suggestWords[k]->value < smallestDistance) {
                                    smallestDistance = suggestWords[k]->value;
                                    smallestDistanceIndex = k;
                                }
                            }

                            //Initialize data members of suggested words
                            if(smallestDistanceIndex > -1) {
                                suggestWords[smallestDistanceIndex]->key = current->key;
                                suggestWords[smallestDistanceIndex]->value = current->value;
                            }

                            //Iterate to next word
                            current=current->next;
                        }
                    }
                }

                printf("Did you mean...?\n");

                // Print the suggested 5 words
                for(int x = 0; x < 5; x++) {
                    printf("%s\n", suggestWords[x]->key);
                }  

            } else {
                printf("The inputted word %s is spelled correctly\n", word);
            }
        }
    }

    hashMapDelete(map);
    return 0;
}
