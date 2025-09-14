// file to put together .ps1 scripts and get local parameters

#include <stdio.h>
#include <stdlib.h>

#define boolean char

void ParseParameters(int count, char *parameter[]);

typedef struct {
    char *configFile, *inputFile, $char;
    boolean wsl_gui;
} InputParameters;

int main(int argc, char *argv[]) {
    ParseParameters(argc, argv);
    
    return 0;
}

void ParseParameters(int count, char *parameters[]){
    for(int i = 0; i < count; i++){
        printf("%d: %s\n", i+1, parameters[i]);
    }
    return;
}