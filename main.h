#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "utilidades.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <math.h>

#define N 5000
#define MAX_OPERACOES 125000000000
#define POTENCIA 10
#define BUFFER_BARRA_PROGRESSO 53
#define NANO_SEGUNDO_PARA_SEGUNDO 1000000000
#define MAX_ELEMENTOS_FILA_IMPRESSAO 20

#define DIRETORIO_SAIDA "output"
#define DIRETORIO_ENTRADA "input"
#define NOME_ARQUIVO_M "m.dat"
#define NOME_ARQUIVO_N "n.dat"
#define NOME_ARQUIVO_R "r.dat"
#define BARRA_DE_PROGRESSO "[..................................................]"

#define numeroAleatorioEntre(min, max) ((rand() % (max - min + 1)) + min)

typedef unsigned long long int ulli;

typedef struct filaImpressao{
    int  Contagem;
    char **NomeArquivos; //guarda o nome dos arquivos que serão inseridos
    ulli **Fila;         //guarda as matrizes resultado da potenciação
} FilaImpressao;

typedef enum {
    Inicializa, Destroi
} ComandoSemaforo;