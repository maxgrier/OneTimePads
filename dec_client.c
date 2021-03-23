/******************************
* Author: Max Grier
* Course: CS344 - 400 - W2021
* Date Modified: 3-8-2021
* Due Date: 3-8-2021
* Description: This will work with the dec_server to get the decrypted message.
* Source code credit: Parts of this code are either from, or based on,
*                     the code in the modules. This was heavily influenced by the sample file.
******************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>  // ssize_t
#include <sys/socket.h> // send(),recv()
#include <netdb.h>      // gethostbyname()
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define MAX_BUFF 150000

/**
* Client code
* 1. Create a socket and connect to the server specified in the command arugments.
* 2. Prompt the user for input and send that input as a message to the server.
* 3. Print the message received from the server and exit the program.
*/

// Error function used for reporting issues
void error(const char* msg) {
    perror(msg);
    exit(0);
}

// Set up the address struct
void setupAddressStruct(struct sockaddr_in* address,
                        int portNumber,
                        char* hostname) {

    // Clear out the address struct
    memset((char*)address, '\0', sizeof(*address));

    // The address should be network capable
    address->sin_family = AF_INET;
    // Store the port number
    address->sin_port = htons(portNumber);

    // Get the DNS entry for this host name
    struct hostent* hostInfo = gethostbyname(hostname);
    if (hostInfo == NULL) {
        fprintf(stderr, "CLIENT: ERROR, no such host\n");
        exit(0);
    }
    // Copy the first IP address from the DNS entry to sin_addr.s_addr
    memcpy((char*)&address->sin_addr.s_addr,
           hostInfo->h_addr_list[0],
           hostInfo->h_length);
}

int main(int argc, char* argv[]) {
    int socketFD, portNumber, charsWritten, charsRead;
    struct sockaddr_in serverAddress;

    size_t len = 0;

    // Set up buffer for the key
    char* key_buff = calloc((MAX_BUFF) + 1, sizeof(char));
    //int key_len = 0;

    // Set up the buffers for characters
    char* plain_txt_buff = calloc((MAX_BUFF) + 1, sizeof(char));
    int plain_txt_len = 0;

    // For the full message
    char* complete = calloc((MAX_BUFF) + 1, sizeof(char));
    int complete_len = 0;

    // Buffers for the characters received
    char* collected = calloc((MAX_BUFF) + 1, sizeof(char));
    char* collected_len = calloc((MAX_BUFF) + 1, sizeof(char));


    int childPid = 0;
    int childStatus = 0;
    // Start the forking for the processes
    pid_t spawnpid = -5;
    spawnpid = fork();
    switch (spawnpid) {
        case -1:
            perror("fork() failed!");
            exit(1);
        case 0:
            // Make sure the inputs are there
            if (argc < 4) {
                fprintf(stderr, "Error: format is plaintext, key, port.\n");
                exit(0);
            }

            // Read the plaintext file
            FILE* plain_txt = fopen(argv[1], "r");
            getline(&plain_txt_buff, &len, plain_txt);
            // Get the length of the plain text
            plain_txt_len = strlen(plain_txt_buff);

            // Get the key data from the key file
            len = 0;
            FILE* key_f = fopen(argv[2], "r");
            getline(&key_buff, &len, key_f);
            //key_len = strlen(key_buff);

            // Make the string to send to server starting with plain txt
            strcpy(complete, plain_txt_buff);

            // Separate the plain text and key
            strcat(complete, "@@");

            // Add the key_buff to the complete
            strcat(complete, key_buff);

            // Create a socket
            socketFD = socket(AF_INET, SOCK_STREAM, 0);
            if (socketFD < 0) {
                error("CLIENT: ERROR opening socket");
            }

            // Set up the server address struct
            setupAddressStruct(&serverAddress, atoi(argv[3]), "localhost");

            // Connect to server
            if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
                error("CLIENT: ERROR connecting");
            }

            //Send message to server
            charsWritten = send(socketFD, complete, strlen(complete), 0);
            if (charsWritten < 0) {
                error("CLIENT: ERROR writing to socket");
            }
            if (charsWritten < strlen(complete)) {
                printf("CLIENT: WARNING: Not all data written to socket!\n");
            }

            // Get return message from server
            // Need to: check here for output to test for enc_server
            while (strlen(collected_len) <= plain_txt_len - 2) {
                // Read data from the socket, leaving \0 at end
                charsRead = recv(socketFD, collected, 1, 0);
                strcat(collected_len, collected);
                if (charsRead < 0) {
                    error("CLIENT: ERROR reading from socket");
                }
            }
            // Output the complete message to stdout
            printf("%s", collected_len);
            fflush(stdout);
            printf("%s", "\n");
            fflush(stdout);

            // Close the socket
            close(socketFD);
        default:
            childPid = wait(&childStatus);
            break;
    }
    return 0;
}