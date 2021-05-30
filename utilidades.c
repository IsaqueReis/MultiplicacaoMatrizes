#include "utilidades.h"

//concatena uma ou mais strings
char *concatenarStrings(int argc, ... ) {
    
    va_list valist;
    char *ret = (char*) calloc(TAMANHO_BUFFER_STRING, sizeof(char));
    char tmp[TAMANHO_BUFFER_STRING];

    if(!ret) {
        fprintf(stderr, "Erro de alocação\n");
        return NULL;
    }

    //inicializa os argumentos da função
    va_start(valist, argc); 
        
    for(int i = 0; i < argc; i++) {
        char *s = va_arg(valist, char*);
        sprintf(tmp, "%s", s);
        strcat(ret,tmp);
    }

    //limpa a memoria reservada para lista de argumentos
    va_end(valist);

    return ret;
}

//combina a lista de argumentos passada, dado o delimitador
char *unirStrings(char delimitador, int argc,  ... ) {

    va_list valist;
    char *ret = (char*) calloc(TAMANHO_BUFFER_STRING, sizeof(char));
    char tmp[TAMANHO_BUFFER_STRING];

    if(!ret) {
        fprintf(stderr, "Erro de alocação\n");
        return NULL;
    }

    //inicializa os argumentos da função
    va_start(valist, argc); 
        
    for(int i = 0; i < argc; i++) {
        char *s = va_arg(valist, char*);

        if(i == argc - 1)
            sprintf(tmp, "%s", s);
        else 
            sprintf(tmp, "%s%c", s, delimitador);

        strcat(ret,tmp);
    }

    //limpa a memoria reservada para lista de argumentos
    va_end(valist);

    return ret;
}

char *combinarDiretorios(int argc, ... )
{
    va_list valist;
    char *ret = (char*) calloc(TAMANHO_BUFFER_STRING, sizeof(char));
    char tmp[TAMANHO_BUFFER_STRING];

    if(!ret) {
        fprintf(stderr, "Erro de alocação\n");
        return NULL;
    }

    //inicializa os argumentos da função
    va_start(valist, argc); 
        
    for(int i = 0; i < argc; i++) {
        char *s = va_arg(valist, char*);

        if(i == argc - 1)
            sprintf(tmp, "%s", s);
        else 
            sprintf(tmp, "%s/", s);

        strcat(ret,tmp);
    }

    //limpa a memoria reservada para lista de argumentos
    va_end(valist);

    return ret;
}

char *pegarDiretorioAtual()
{
    char bufferStrDiretorio[PATH_MAX];                           //caminho completo do diretório atual
    char *diretorioAtual = getcwd(bufferStrDiretorio, PATH_MAX); //syscall que retorna o diretório corrente ou NULL em caso de erro

    if(!diretorioAtual) {
        perror("Pegar diretorio atual");
        return NULL;
    }

    return diretorioAtual;
}

