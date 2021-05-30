#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>

#define TAMANHO_BUFFER_STRING                                               4098
#define TAMANHO_BUFFER_STR_NUMERICA                                           25

char *concatenarStrings(int argc, ... );
char *unirStrings(char delimitador, int argc,  ... );
char *combinarDiretorios(int argc, ... );
char *pegarDiretorioAtual();
