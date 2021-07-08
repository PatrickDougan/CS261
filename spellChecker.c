/*
 * CS 261 Data Structures
 * Assignment 5
 * Name:Patrick Dougan 
 * Date: 3/11/2019
 */
 
#include "hashMap.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

/**
 * Loads the contents of the file into the hash map.
 * @param file
 * @param map
 */
void loadDictionary(FILE* file, HashMap* map)
{
	char* key = nextWord(file);//declares key as pointer to string then uses nextWord to get the string
	
	while(key != NULL)//while not last string in file
	{
		hashMapPut(map,key,0);//put key into map with value 0
		free(key);//frees memory used by key
		key = nextWord(file);//gets next word
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
    
//declares variables
    HashMap* map = hashMapNew(1000);
    struct HashLink *ptr;
    int count = 0;
    int length;
    struct HashLink *closest[5];
	for(int x = 0; x < 5; x++)//sets closest to null ptrs
	{
		closest[x] = NULL;
	}


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

	for(int x = 0; x < strlen(inputBuffer);x++)//for each character in string
	{
		inputBuffer[x] = tolower(inputBuffer[x]);//sets char to lowercase
	}

        // Implement the spell checker code here..

        if (strcmp(inputBuffer, "quit") == 0)
        {
            quit = 1;
        }
	else if(hashMapContainsKey(map,inputBuffer))//input is in hashmap
	{
		printf("Spelled Correctly\n");//it's spelled correctly
	}
	else//if not in map
	{
		for(int x = 0; x < map->capacity; x++)//for each bucket
		{	
			ptr = map->table[x];//set ptr to first link
			while(ptr != NULL)//while not last link
			{
				
				if(strlen(inputBuffer) > strlen(ptr->key))//if length of input buffer > than length of link key
				{
					//for each length difference it counts as an add/delete in Levenshtein distance
					count = (strlen(inputBuffer) - strlen(ptr->key));//counts add/delete edit
					length = strlen(ptr->key);//sets length equal to string length
				}
				else
				{
					//for each length difference it counts as an add/delete in Levenshtein distance
					count = (strlen(ptr->key) - strlen(inputBuffer));//counts add/delete distance
					length = strlen(inputBuffer);//sets length equal to string length
				}
				for(int y = 0; y < length ; y++)//for each character in the shorter string
				{
					if(ptr->key[y] != inputBuffer[y])//compares if characters are not equal
					{
						count++;//adds to Levenshtein distance if characters are not equal
					}
				}
				hashMapPut(map,ptr->key,count);//updates value to Levenshtein distance in hash map
				count = 0;//resets count
				ptr = ptr->next;//gets next pointer
			}					
		}
		for(int x = 0; x < map->capacity; x++)//for each bucket
		{
			ptr = map->table[x];//sets pointer to first link in bucket
			while(ptr != NULL)//while not last link
			{
				for(int y = 0; y < 5; y++)//for each link in closest
				{
					if(closest[y] == NULL)//if no value set in closest
					{
						closest[y] = ptr;//sets that element to equal to ptr
					}						
					else if(closest[y]->value > ptr->value)//if new link has smaller distance
					{	
						closest[y] = ptr;//update element to lowest pointer
						break;//stops program from updating all five values with lowest distance
					}
				}
				ptr = ptr->next;//get next pointer
			}	
		}	
		for(int x = 0; x < 5; x++)//for each link in closest
		{
			printf("word %d:%s\n",(x+1),closest[x]->key);//prints out five closest words
		}			
  	 }
}

    hashMapDelete(map);
    return 0;
}
