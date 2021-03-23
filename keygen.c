/******************************
* Author: Max Grier
* Course: CS344 - 400 - W2021
* Date Modified: 3-8-2021
* Due Date: 3-8-2021
* Description:  This creates the key that will be used for encryption.
* Source code credit: Parts of this code are either from, or based on,
*                     the code in the modules. This was heavily influenced by the sample file.
******************************/

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main(int argc, char *argv[]) {

    // Set the seed
    srand(time(NULL));

    // Error if a key length wasn't provided
    if (argc != 2) {
        fprintf(stderr, "Error: Usage is keygen key_length.\n\n");
        exit(1);
    }

    // All possible characters for key
    char possible_chars[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";


    // Set the key length
    char *character;
    long key_length = strtol(argv[1], &character, 10);

    // If key length wasn't entered
    if(key_length == '\0') {
        fprintf(stderr, "Error: Invalid input: %s\n", argv[1]);
        exit(1);
    }

    // Create random characters up to the key length
    int count = 0;
    while(count < key_length) {
        // Generate rand_num and choose it from the list
        int rand_num = rand() % 27;

        char cipher;
        if(rand_num == 26){
            cipher = ' ';
        }else{
            cipher = possible_chars[rand_num];
        }

        fprintf(stdout, "%c", cipher);
        count++;
    }

    fprintf(stdout, "\n");

    return 0;
}
