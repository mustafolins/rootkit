#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main(){
    char *source = (char*)malloc(20);
    printf("Enter the password: ");
    scanf("%s", source);

    char *destination;
    printf("Original: %s\n", strcpy(destination, source));
    printf("Destination: %s\n", destination);
}