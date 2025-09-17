// file to put together .ps1 scripts and get local parameters

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Shlwapi.h"

#define bool char
#define MAX_CHAR 48

// check system OS
#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    // GetCurrentDirectory(bufferLength, LPSTR);
    // or 
    // #include <direct.h> 
    // _getcwd();
    #define HOST "Windows"

#elif __linux__ || __unix__
    #include <unistd.h>
    // getcwd();
    // allocate buffer string
    // free after using
    #define HOST "Linux"
#elif __APPLE__ || __MACH__
    define HOST "Mac"
    define HOST "Linux"
    #include <unistd.h>
    // getcwd();
#endif

typedef struct {
    char *configFile, *inputFile, *language, *compiler, *venv;
    // use functions to find working directory
    char **libs;
    bool wsl_gui, srvx_gui, X11;
    int parameters, numLibs;
} InputParameters;

typedef struct {
    size_t size_libs;
    char *client_os, *host_os, *wd, *proj_dir;
} MetaData;

// 'member functions'
void checkHostOS(MetaData *meta);
void GetWD(MetaData *meta);
int findInput(MetaData *meta, InputParameters Iparam);
void meta_displayInfo(MetaData meta);
void meta_free(MetaData *meta);

void iparam_DisplayInfo(InputParameters Iparam);
void iparam_free(InputParameters *Iparam);
void ParseParameters(InputParameters *Iparam, int count, char *parameter[]);
void fillLibsArray(InputParameters *Iparam, char *parameters[], int i);

// common functions
void addNull_Term(char *input, size_t len);
int LastChar(char *input, size_t max);
int splitText(char* input, size_t inputLen, char delim , char *output[]);
void combineText(char *input[], int len, char *output);
void addSpace(char *input, int index);
void addSingleQuote(char *input, int index);
void addComma(char *command, int charCounter);
void callScript(InputParameters Iparam, MetaData meta);

int main(int argc, char *argv[]) {
    size_t max_size;
    if(argc > 6){
        max_size = 3e06;
    }
    else {
        max_size = 1.5e06;
    }

    InputParameters *Iparam = calloc(1, sizeof(*Iparam));
    if(!Iparam){
        perror("Calloc");
        return 1;
    }
    MetaData meta;
    (*Iparam).parameters = argc;

    checkHostOS(&meta);
    GetWD(&meta);
    ParseParameters(Iparam, argc, argv);
    if(findInput(&meta, *Iparam) == -1){
        return 1;
    }

    iparam_DisplayInfo(*Iparam);
    meta_displayInfo(meta);

    callScript(*Iparam, meta);

    // free memory
    iparam_free(Iparam);
    meta_free(&meta);
    
    printf("Complete");
    return 0;
}

void callScript(InputParameters Iparam, MetaData meta){
    char command[300];
    char mainScript[] = "C:\\Users\\haydo\\Desktop\\Home_Server\\scripts\\crossOS_compiler\\src\\crossx.ps1";
    size_t charCounter = 0;

    // initial command set up
    addSingleQuote(command, charCounter++);
    // strncpy(command + charCounter, meta.proj_dir, strlen(meta.proj_dir));
    // charCounter += strlen(meta.proj_dir);
    // strncpy(command+charCounter++, "\\", 2);
    strncpy(command+charCounter, mainScript, strlen(mainScript));
    charCounter += strlen(mainScript);
    addSingleQuote(command, charCounter++);
    addComma(command, charCounter++);
    addSpace(command, charCounter++);

    // check each param and apply if necessary

    // -OS Windows\Linux\Mac
    if(meta.host_os != NULL){
        char buffer[] = "-OS', ";
        addSingleQuote(command, charCounter++);
        strncpy(command + charCounter, buffer, strlen(buffer));
        charCounter += strlen(buffer);
        
        addSingleQuote(command, charCounter++);
        strncpy(command + charCounter, meta.host_os, strlen(meta.host_os));
        charCounter += strlen(meta.host_os);
        addSingleQuote(command, charCounter++);
        addComma(command, charCounter++);
        strncpy(command + charCounter, " ", 2);
        charCounter++;
    }
    // -wd <path/to/dir>
    if(meta.wd != NULL){
        char buffer[] = "'-wd', ";
        strncpy(command + charCounter, buffer, strlen(buffer));
        charCounter += strlen(buffer);
        
        addSingleQuote(command, charCounter++);
        strncpy(command + charCounter, meta.proj_dir, strlen(meta.proj_dir)+1);
        charCounter += strlen(meta.proj_dir);
        addSingleQuote(command, charCounter++);
        addComma(command, charCounter++);
        strncpy(command + charCounter, " ", 2);
        charCounter++;
    }
    // -inputFile <file>
    if(Iparam.inputFile != NULL){
        char buffer[] = "'-inputFile', ";
        strncpy(command + charCounter, buffer, strlen(buffer));
        charCounter += strlen(buffer);
    
        addSingleQuote(command, charCounter++);
        strncpy(command + charCounter, Iparam.inputFile, strlen(Iparam.inputFile));
        charCounter += strlen(Iparam.inputFile);
        addSingleQuote(command, charCounter++);
        addComma(command, charCounter++);
        strncpy(command + charCounter, " ", 2);
        charCounter++;
    }    
    else{
        printf("Error no input file\n");
    }
    if(Iparam.language != NULL){
        char buffer[] = "'-lang', ";
        strncpy(command + charCounter, buffer, strlen(buffer));
        charCounter += strlen(buffer);
    
        addSingleQuote(command, charCounter++);
        strncpy(command + charCounter, Iparam.language, strlen(Iparam.language));
        charCounter += strlen(Iparam.language);
        addSingleQuote(command, charCounter++);
        addComma(command, charCounter++);
        strncpy(command + charCounter, " ", 2);
        charCounter++;
    }
    if(Iparam.compiler != NULL){
        char buffer[] = "'-compiler', ";
        strncpy(command + charCounter, buffer, strlen(buffer));
        charCounter += strlen(buffer);
    
        addSingleQuote(command, charCounter++);
        strncpy(command + charCounter, Iparam.compiler, strlen(Iparam.compiler));
        charCounter += strlen(Iparam.compiler);
        addSingleQuote(command, charCounter++);
        addComma(command, charCounter++);
        strncpy(command + charCounter, " ", 2);
        charCounter++;
    }
    if(Iparam.venv != NULL){
        char buffer[] = "'-venv', ";
        strncpy(command + charCounter, buffer, strlen(buffer));
        charCounter += strlen(buffer);
    
        addSingleQuote(command, charCounter++);
        strncpy(command + charCounter, Iparam.venv, strlen(Iparam.venv));
        charCounter += strlen(Iparam.venv);
        addSingleQuote(command, charCounter++);
        addComma(command, charCounter++);
        strncpy(command + charCounter, " ", 2);
        charCounter++;
    }

    addNull_Term(command, charCounter);
    printf("Command: %s\n", command);
    
    char prefix[] = "powershell -command ^ Start-Process powershell -ArgumentList ` '-NoExit', '-ExecutionPolicy', 'Bypass', '-File', ";
    // char suffix[] = "";
    char bufferCommand[350];

    charCounter = 0;
    strncpy(bufferCommand, prefix, strlen(prefix));
    charCounter += strlen(prefix);
    strncpy(bufferCommand + charCounter, command, strlen(command));
    charCounter += strlen(command);
    for (int i = charCounter+1; i > 0; i--){
        if(bufferCommand[i] == ','){
            bufferCommand[i] = ' ';
            break;
        }
    }

    addNull_Term(bufferCommand, charCounter);
    printf("Full Command: %s\n", bufferCommand);
    system(bufferCommand);
    
    return;    
}
void addSpace(char *input, int index){
    strncpy(input + index, " ", 2);
    return;
}
void addSingleQuote(char *input, int index){
    strncpy(input + index, "'", 2);
    return;
}
void addComma(char *input, int index){
    strncpy(input + index, ",", 2);
    return;
}

void meta_displayInfo(MetaData meta) {
    printf("-------------Meta------------\n");
    if(&(meta.host_os) != NULL)
        printf("Host OS: %s\n", meta.host_os);
    if(&(meta.wd) != NULL)
        printf("Working Directory: %s\n", meta.wd);
    if(&(meta.wd) != NULL)
        printf("Project Directory: %s\n", meta.proj_dir);
    printf("-------------------------------\n");
    
    return;
}
void meta_free(MetaData *meta) {
    if (meta->wd != NULL)
        free(meta->wd);
    if(meta->host_os != NULL)
        free(meta->host_os);
    if(meta->proj_dir != NULL)
        free(meta->proj_dir);

    printf("All metadata memory cleared\n");

    return;
}

void checkHostOS(MetaData *meta) {
    meta->host_os = (char*)malloc(MAX_CHAR/2);
    strcpy(meta->host_os, HOST); 
    addNull_Term(meta->host_os, strlen(HOST));
    
    return;
}
void GetWD(MetaData *meta){
    size_t maxLen = MAX_CHAR*1.5;
    #if defined(_WIN32) || defined(_WIN64)
            TCHAR buffer[maxLen];
            DWORD dwRet;
            dwRet = GetCurrentDirectoryA(maxLen, buffer);    
    
        if(dwRet == 0){
            printf("Error getting path");
        }
        else if (dwRet > maxLen){
            printf("Path too long %s\n", buffer);
        }
        else {
            // printf("WD: %s\n", buffer);
            char *buf = buffer;
            meta->wd = (char*)malloc(maxLen);
            strncpy(meta->wd, buf, strlen(buf));
            addNull_Term((meta->wd), strlen(meta->wd));
        }
        char **output = (char**)malloc(MAX_CHAR*2);
        size_t inputLen = strlen(meta->wd);
        int numSplits = splitText(meta->wd, inputLen, '\\' , output);
        
        if(numSplits == 0){
            printf("Error splitting string\n");
            return;
        }

        meta->proj_dir = (char*)malloc(MAX_CHAR*2);
        combineText(output, numSplits-1, meta->proj_dir);

    #elif __linux__ || __unix__
        char *buffer = (char*)malloc(maxLen);
        getcwd(buffer, maxLen);
        meta->wd = (char*)calloc(maxLen, 1);
        printf("WD: %s\n", buffer);
        strncpy(meta->wd, buffer, strlen(buffer));
        free(buffer);
        char **output = (char**)malloc(MAX_CHAR*2);
        size_t inputLen = strlen(meta->wd);
        int numSplits = splitText(meta->wd, inputLen, '/' , output);
        
        if(numSplits == 0){
            printf("Error splitting string\n");
            return;
        }
        
        meta->proj_dir = (char*)malloc(MAX_CHAR*2);
        combineText(output, numSplits-1, meta->proj_dir);
    #endif

    return;
}
int findInput(MetaData *meta, InputParameters Iparam){
    // check typical proj_dir and also working directory for input files 
    char bufPath[90];
    int charCounter = 0;

    strncpy(bufPath, meta->proj_dir, strlen(meta->proj_dir));
    charCounter += strlen(meta->proj_dir);
    strncpy(bufPath + charCounter, "\\", 2);
    charCounter++;
    strncpy(bufPath + charCounter, Iparam.inputFile, strlen(Iparam.inputFile));
    charCounter += strlen(Iparam.inputFile);
    // addNull_Term(bufPath, charCounter);
    // LPSTR lpPath = bufPath;
    char *lpstr = bufPath;
    if(PathFileExists(lpstr)){
        printf("File exists in proj_dir");
        return 1;
    }
    else {
        int nullTerm = 0;
        bufPath[0] = '\0';
        charCounter = 0;

        strncpy(bufPath, meta->wd, strlen(meta->wd));
        charCounter += strlen(meta->wd);
        nullTerm = charCounter;
        strncpy(bufPath + charCounter, "\\", 2);
        charCounter++;
        strncpy(bufPath + charCounter, Iparam.inputFile, strlen(Iparam.inputFile));
        charCounter += strlen(Iparam.inputFile);
        // addNull_Term(bufPath, charCounter);
        char *lpstr2 = bufPath;
        if(PathFileExists(lpstr2)){
            printf("File exists in working directory\n");
            // update project_directory
            meta->proj_dir[0] = '\0';
            bufPath[nullTerm] = '\0';            
            strncpy(meta->proj_dir, bufPath, strlen(bufPath)+1);
        }
        else {
            printf("Error - No input file found\n");
            return -1;
        }
    }
    printf("Buffer path: %s\n", meta->proj_dir);
    
}

int splitText(char *input, size_t inputLen, char delim, char *output[]){
    size_t numSplits = 0, maxSplits = 30;

    for(int i = 0; i < inputLen; i++){
        if(input[i] == (u_char)delim){
            output[numSplits] = malloc(maxSplits);
            strncpy(output[(numSplits)], input, i);
            addNull_Term(output[(numSplits)++], i);
            // printf("%d: %s\n", numSplits, output[numSplits-1]);
            char buffer[MAX_CHAR*2];
            strncpy(buffer, input + i + 1, inputLen - i + 1);
            if(numSplits > maxSplits){
                return 0;
            }
        }
    }
    // printf("PROJECT_DIR: %s\n", output[numSplits-1]);

    return numSplits;
}
void combineText(char *input[], int len, char *output){
    strncpy(output, input[len], strlen(input[len]));
    addNull_Term(output, strlen(input[len]));
    
    return;
}

void iparam_DisplayInfo(InputParameters Iparam){
    printf("\n-----------Config------------\n");
    if(Iparam.parameters != 0)
        printf("Input Parameters: %d\n", Iparam.parameters);
    if(Iparam.configFile != NULL)
        printf("Config file: %s\n", Iparam.configFile);
    if(Iparam.inputFile != NULL)
        printf("Input file: %s\n", Iparam.inputFile);
    if(Iparam.language != NULL)
        printf("Language: %s\n", Iparam.language);
    if(Iparam.compiler != NULL)
        printf("Compiler: %s\n", Iparam.compiler);
    if(Iparam.venv != NULL)
        printf("Venv_Name: %s\n", Iparam.venv);
    printf("Number of Libraries Used: %d\n", Iparam.numLibs);
    printf("WSL_X11: %d\n", Iparam.wsl_gui);
    printf("SRVX_X11: %d\n", Iparam.srvx_gui);
    printf("X11: %d\n", Iparam.X11);
    if((Iparam.libs == NULL)){
        printf("No input libraries\n");
        return;
    }
    printf("meta.Length in function: %d\n", Iparam.numLibs);
    for (int i = 0; i < Iparam.numLibs; i++){
            if(Iparam.libs[i] == NULL)
                break;
            printf("Lib %d: %s\n", i, Iparam.libs[i]);
    }
    printf("------------------------------\n");

    return;
}
void iparam_free(InputParameters *Iparam){
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
    
    //free 2d ptr's
    for (int j = 0; j < Iparam->numLibs; j++){
        if( Iparam->libs[j] != NULL){
            free(Iparam->libs[j]);
            // printf("row %d cleared\n", j);
        }
    }
    // clear pointer to pointer after clearing each pointer (reverse)
    if(Iparam->libs != NULL){
        free(Iparam->libs);
        Iparam->libs = NULL;
    }
    
    free(Iparam);
        printf("All memory allocations cleared for InputParameters\n");
    // for(int i )
}
void addNull_Term(char *input, size_t len){
    input[len] = '\0';
    return;
}
int LastChar(char *input, size_t max){
    for(int i = 0; i < max; i++){
        if(input[i] > 128 || input[i] < 0){
            return i;
        }
    }
    return strlen(input);
}
void ParseParameters(InputParameters *Iparam, int count, char *parameters[]){
    for(int i = 0; i < count; i++){
        // printf("%s\n", parameters[i]);
        if(count <= 1)
            break;

        if((strcmp(parameters[i], "-config") == 0)){
            Iparam->configFile = (char*)malloc(MAX_CHAR);
            strncpy((Iparam->configFile), (parameters[i+1]), strlen(parameters[i+1]) + 1);
            addNull_Term(Iparam->configFile, strlen(parameters[i+1]) + 1);
        }
        else if(strncmp(parameters[i], "-input", sizeof(parameters[i])) == 0){
            Iparam->inputFile = (char*)malloc(MAX_CHAR);
            strncpy(Iparam->inputFile, parameters[i+1], strlen(parameters[i+1]) + 1);
            addNull_Term(Iparam->inputFile, strlen(parameters[i+1]) + 1);
        }
        else if(strncmp(parameters[i], "-lang", sizeof(parameters[i])) == 0){
            Iparam->language = (char*)malloc(MAX_CHAR);
            strncpy(Iparam->language, parameters[i+1], strlen(parameters[i]) + 1);
            addNull_Term(Iparam->language, strlen(parameters[i+1]) + 1);
        }
        else if(strncmp(parameters[i], "-compiler", sizeof(parameters[i])) == 0){
            Iparam->compiler = (char*)malloc(MAX_CHAR);
            strncpy(Iparam->compiler, parameters[i+1], strlen(parameters[i+1]) + 1);
            addNull_Term(Iparam->compiler, strlen(parameters[i+1]) + 1);
        }
        else if(strncmp(parameters[i], "-venv", sizeof(parameters[i])) == 0){
            Iparam->venv = (char*)malloc(MAX_CHAR);
            strncpy(Iparam->venv, parameters[i+1], strlen(parameters[i+1]) + 1);
            addNull_Term(Iparam->venv, strlen(parameters[i+1]) + 1);
        }
        else if(strncmp(parameters[i], "-libs", sizeof(parameters[i])) == 0){
            Iparam->libs = (char**)malloc(MAX_CHAR);
            Iparam->numLibs = count - i - 1;
            fillLibsArray(Iparam, parameters, i);
            break;
        }
    }

    return;
}
void fillLibsArray(InputParameters *Iparam, char *parameters[], int i){
    int COL = 15;
    for (int j = 0; j < Iparam->numLibs; j++){
        printf("%d: %s\n", i, parameters[i+1]);
        Iparam->libs[j] = (char*)malloc(COL);
        strncpy(Iparam->libs[j], parameters[i+1], strlen(parameters[i+1]) + 1);
        addNull_Term(Iparam->libs[j], strlen(parameters[i+1]) + 1);
        i++;
    }

    return;
}
