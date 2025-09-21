#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>

int main(int argc, char *argv[]){
    int c;
    bool searchRecursive=false;
    bool caseSensitive=true;

    int currentArgument=1;
    while((c=getopt(argc, argv, "Ri")) != -1){
        currentArgument++;
        switch (c)
        {
        case 'R':
            searchRecursive=true;
            printf("\nRekursiv");
            break;
        case 'i':
            caseSensitive=false;
            printf("\ncase insensitive");
            break;
        case '?':
            break;
        }
    }
    int argremain = argc - optind;
    if (argremain <= 1) {
        return 1;
    }
    char* filepath;
    filepath=argv[argremain];
    argremain++;
    printf("\n%s", filepath);
    while (argremain<argc){
        printf("\n%s",argv[argremain]);
        argremain++;
    }
    
    return 1;
}