# TCP-Client-Server-Example

## Repository Overview

This repository holds a demonstration of TCP communication established between a server and client. The server is a persistent program that should be started prior to client execution. Once the server is listening, the client can ping the server over an available port. The client program first takes a filename in the CWD as input to include in the request to the server. The server recieves this request, opens the file and writes the content back to the client. The client recieves the response and outputs it to a `.txt` file.

## Prerequisites

In order to execute the code in this repository, you must have a recent version  of the Gnu Compiler Collection installed on your computer. a reference to download (can be found here)[https://gcc.gnu.org/install/]

Please note, C programs do not run identical across environments. This application may attempt to perform system/network actions that are prohibited by the permissions set of your OS.

# Execution Instructions

To execute this code run the `compile.sh` shell script. 

Following this, start the server on an open port `./server 4005`

Once the server logs 'listening...', open a new terminal.

Run the client specifying the IP, Port and filename `./client 127.0.0.1 4005 "input.txt"`
