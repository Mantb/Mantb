#include<stdio.h>
int main(int argc, char* argv[]) {// lets start with the args
    switch (argc) {
        case 1:
            printf("No arguments provided.\n");
            break;
        case 2:
            printf("One argument provided: %s\n", argv[1]);
            break;
        default:
            printf("%d arguments provided.\n", argc - 1);
            break;
    }
    printf("Hello, World!\n");
    return 0;
}