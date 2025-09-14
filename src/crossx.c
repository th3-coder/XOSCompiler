// file to put together .ps1 scripts and get local parameters

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define boolean char
#define MAX_CHAR 48

typedef struct {
    char *configFile, *inputFile, *language, *compiler, *venv;
    // use functions to find working directory
    char *wd;
    char **libs;
    boolean wsl_gui, srvx_gui, X11;
} InputParameters;

typedef struct {
    size_t size_libs; 
} metaData;

void iparam_DisplayInfo(InputParameters Iparam, metaData meta);
void ParseParameters(InputParameters *Iparam, metaData meta, int count, char *parameter[]);

int main(int argc, char *argv[]) {
    size_t max_size;
    if(argc > 6){
        max_size = 300000;
    }
    else {
        max_size = 150000;
    }

    InputParameters *Iparam = malloc(max_size);
    metaData meta;

    ParseParameters(Iparam, meta, argc, argv);
    iparam_DisplayInfo(*Iparam, meta);
    // free memory
    if(Iparam->configFile != NULL)
        free(Iparam->configFile);
    if(Iparam->inputFile != NULL)
        free(Iparam->inputFile);
    if(Iparam->language != NULL)
        free(Iparam->language);
    if(Iparam->compiler != NULL)
        free(Iparam->compiler);
    if(Iparam->venv != NULL)
        free(Iparam->venv);
    if(Iparam->libs != NULL)
        free(Iparam->libs);

    return 0;
}

void iparam_DisplayInfo(InputParameters Iparam, metaData meta){
    printf("\n--------------------\n");
    printf("Config file: %s\n", Iparam.configFile);
    printf("Input file: %s\n", Iparam.inputFile);
    printf("Language: %s\n", Iparam.language);
    printf("Compiler: %s\n", Iparam.compiler);
    printf("Venv_Name: %s\n", Iparam.venv);
    printf("WSL_X11: %d\n", Iparam.wsl_gui);
    printf("SRVX_X11: %d\n", Iparam.srvx_gui);
    printf("X11: %d\n", Iparam.X11);

    if((Iparam.libs[0][0] != '\0')){
        for (int i = 0; i < meta.size_libs; i++){
            printf("Lib %d: %s\n", i, Iparam.libs[i]);
        }
    }
    return;
}

void addNull_Term(char *input, size_t len){
    input[len] = '\0';
}

void ParseParameters(InputParameters *Iparam, metaData meta, int count, char *parameters[]){
    for(int i = 0; i < count; i++){

        printf("%s\n", parameters[i]);
        if((strcmp(parameters[i], "-config") == 0)){
            Iparam->configFile = (char*)malloc(MAX_CHAR);
            strncpy((Iparam->configFile), (parameters[i+1]), strlen(parameters[i+1]));
            addNull_Term(Iparam->configFile, strlen(Iparam->configFile));
            // Iparam.configFile[(strlen(Iparam.configFile))] = '\0';
        }
        else if(strncmp(parameters[i], "-input", sizeof(parameters[i])) == 0){
            Iparam->inputFile = (char*)malloc(MAX_CHAR);
            strncpy(Iparam->inputFile, parameters[i+1], strlen(parameters[i+1]));
            addNull_Term(Iparam->inputFile, strlen(Iparam->inputFile));
        }
        else if(strncmp(parameters[i], "-lang", sizeof(parameters[i])) == 0){
            Iparam->language = (char*)malloc(MAX_CHAR);
            strncpy(Iparam->language, parameters[i+1], strlen(parameters[i]));
            addNull_Term(Iparam->language, strlen(Iparam->language));
        }
        else if(strncmp(parameters[i], "-compiler", sizeof(parameters[i])) == 0){
            Iparam->compiler = (char*)malloc(MAX_CHAR);
            strncpy(Iparam->compiler, parameters[i+1], strlen(parameters[i]));
            addNull_Term(Iparam->compiler, strlen(Iparam->compiler));
        }
        else if(strncmp(parameters[i], "-venv", sizeof(parameters[i])) == 0){
            Iparam->venv = (char*)malloc(MAX_CHAR);
            strncpy(Iparam->venv, parameters[i+1], strlen(parameters[i]));
            addNull_Term(Iparam->venv, strlen(Iparam->venv));
        }
        else if(strncmp(parameters[i], "-libs", sizeof(parameters[i])) == 0){
            Iparam->libs = (char**)malloc(MAX_CHAR*count);
            strncpy((Iparam->libs[0]), parameters[i+1], strlen(parameters[i+1]));
            addNull_Term(Iparam->libs[0], strlen(Iparam->libs[0]));

            break;
        }
        //printf("Hello\n");
    }
    return;
}
