README file for the Assignment 5 - One-Time Pads
Author: Max Grier
Data: 03-08-21
Project: Assignment 5 - One-Time Pads

Included files:
    compileall.sh
	This is a bash script that will compile all of the files listed below.

    keygen.c
	This file will be used to generate a key used for the encryption of the plaintext files.  It will be any capital letter in the alphabet or a space.

    enc_client.c
	This will connect to enc_server and create a one-time pad style encryption.

    enc_server.c
	This is the encryption server that will run in the background.  It will communicate with the enc_client and send back an encrypted message.

    dec_client.c
	This will connect to the dec_server and will ask for it to decrypt the cipher text using the cipher text and key.

    dec_server.c
	This will connect to the dec_client and will be passed the cipher text and key.  Then it will return the plaintext again.	


How to compile:
	You will just need to run the compileall.sh script to compile:
		bash compileall.sh

	chmod +x p5testscript
    For this program, we are utilizing the gcc compiler.
    To compile main.c, you will need to be in the directory where the main.c file resides,
    then enter the following command in your terminal:
        gcc --std=gnu99 -o line_processor main.c

How to run to code:
	Note: for the test script, make sure to run: chmod +x p5testscript
	      this will give the test script the correct permissions.
	Once you have the executables, you can run the code like this:
	./p5testscript RANDOM_PORT1 RANDOM_PORT2 > mytestresults 2>&1

After that, you can review the test results in mytestresults.


