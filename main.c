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

#define N 1000
#define OUTPUT_DIR "output"
#define INPUT_DIR "input"
#define M_FILENAME "m.dat"
#define N_FILENAME "n.dat"
#define R_FILENAME "r.dat"

#define numeroAleatorioEntre(min, max) ((rand() % (max - min + 1)) + min)

typedef unsigned long long int ulli;

typedef struct potenciacaoArgs
{
    char *mtzName;
    ulli *n;
    ulli potencia;
} PotenciacaoArgs;

sem_t mutex; //semáforo para as threads;
int progresso = 0;
int finalizou = 0;

void imprimeMatriz(ulli *r){

    int primeiro = 1;

    for(int i=0; i<N; i++) {
        for (int j=0; j<N; j++) {
            if (primeiro == 1) {
                printf("%llu", r[N*i + j]);
                primeiro = 0;
            }
            else{
                printf(" %llu", r[N*i + j]);
            }

            if (j + 1 == N) {
                printf("\n");
                primeiro = 1;
            }
        }
    }
}

void inicializaArquivoDeMatriz(char *filename){

    FILE *fp = fopen(filename, "r");

    if(fp != NULL){ // o arquivo já existe
        //printf("O arquivo de matriz %s já existe!Removendo...\n", filename);
        remove(filename);
        //printf("Arquivo removido com sucesso!\n");
    } 

    fp = fopen(filename, "a+");

    if(!fp){
        perror("Criação do arquivo de matriz");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL)); //gera uma seed baseada no tempo atual em milisegundos

    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            j == N 
            ? fprintf(fp, "%d",  numeroAleatorioEntre(1, N)) 
            : fprintf(fp, "%d,", numeroAleatorioEntre(1, N));  
        }
        fprintf(fp,"\n");
    }

    fflush(fp);
    fclose(fp);
}

void escreverArquivoDeMatriz(char *filename, ulli *mtz){

    FILE *fp = fopen(filename, "r");

    if(fp != NULL){ // o arquivo já existe
        //printf("O arquivo de matriz %s já existe!Removendo...\n", filename);
        remove(filename);
        //printf("Arquivo removido com sucesso!\n");
    } 

    fp = fopen(filename, "a+");

    if(!fp){
        perror("Criação do arquivo de matriz");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL)); //gera uma seed baseada no tempo atual em milisegundos

    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            j == N 
            ? fprintf(fp, "%llu",  mtz[N*i + j]) 
            : fprintf(fp, "%llu,", mtz[N*i + j]);  
        }
        fprintf(fp,"\n");
    }

    fflush(fp);
    fclose(fp);
}

ulli *leArquivoDeMatriz(char *filename){

    FILE *fp;
    int k = 0;
    ulli *ret =  (ulli *) calloc(1, sizeof(ulli) * N*N);

    inicializaArquivoDeMatriz(filename);

    if( (fp = fopen(filename, "r")) == NULL ){
        perror("Leitura arquivo de Matriz");
        exit(EXIT_FAILURE);
    }

    if(!ret){
        fprintf(stderr, "Erro de alocação!\n");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            j == N 
            ? fscanf(fp, "%llu", &ret[k]) 
            : fscanf(fp, "%llu,", &ret[k]); 
            k++;
        }
        fgetc(fp); //le o '\n' e o descarta
    }
    
    printf("Matriz lida do arquivo %s:\n", filename);
    //imprimeMatriz(ret);
    return ret;
}

ulli *multiplicaMatriz(ulli n[N*N], ulli m[N*N]){

    ulli *resul = (ulli *) malloc(sizeof(ulli) * N*N);
    if(resul == NULL) {
        printf("Erro ao alocar memoria!");
        return NULL;
    }

    for(int i=0; i<N*N; i++)
        resul[i] = 0;

    // i seleciona a iésima e j a jésima coluna da matriz resultante,
    //      operações dadas por iterações de k que calculam o resultado aplicado a n e m.
    for(int i=0; i<N; i++) {
        for(int j=0; j<N; j++) {
            for(int k=0; k<N; k++) {
                resul[N*i + j] += n[N*i + k] * m[N*k + j];
            }
        }
        progresso += i;
    }

    return resul;
}

void *operacoesIo(void *args)
{
    sem_wait(&mutex);

    sem_post(&mutex);
}

ulli *potenciaMatriz(char *mtzName, ulli n[N*N], ulli potencia){

    ulli *tmp;
    ulli *resul = (ulli *) malloc(sizeof(ulli) * N*N);
    char contadorItrStr[TAMANHO_BUFFER_STR_NUMERICA];

    if(resul == NULL) {
        printf("Erro ao alocar memoria!");
        return NULL;
    }
    memcpy(resul, n, sizeof(ulli) * N*N);

    for(int i=1; i<potencia; i++) {
        tmp = multiplicaMatriz(n, resul);
        free(resul);
        resul = tmp;
        sprintf(contadorItrStr, "%d", i);
        char *filename = concatenarStrings(4, mtzName, "_", contadorItrStr, 
                                                                   ".dat");
        escreverArquivoDeMatriz(combinarDiretorios(3, pegarDiretorioAtual(), 
                                              OUTPUT_DIR, filename), resul);
        progresso++;
    }

    finalizou = 1;
    return resul;
}

void *testeThread(void *args)
{
    PotenciacaoArgs *pArgs = (PotenciacaoArgs*) args;

    sem_wait(&mutex);
    potenciaMatriz(pArgs->mtzName, pArgs->n, pArgs->potencia);
    sem_post(&mutex);
}

int main(int argc, char *argv[]) {

    pthread_t t1, t2;
    ulli *n = leArquivoDeMatriz(combinarDiretorios(3, pegarDiretorioAtual(), 
                                                    INPUT_DIR, N_FILENAME));
    //ulli *m = leArquivoDeMatriz(combinarDiretorios(3, pegarDiretorioAtual(), 
    //                                                INPUT_DIR, M_FILENAME));

    //printf("nXm:\n");
    //imprimeMatriz(n);
    //printf("\n");
    //imprimeMatriz(m);
    //printf("\n");
    //imprimeMatriz(multiplicaMatriz(n,m));
   //printf("\n");

    //obs: se for alto grau de potência ocorre overflow e com isso geração de números negativos...
    //printf("n^10:\n");
    //imprimeMatriz(n);
    //printf("\n");

    PotenciacaoArgs args = { "n", n, 10 };
    //PotenciacaoArgs args2 = { "m", m, 10 };
    sem_init(&mutex, 0, 1);

    pthread_create(&t1,NULL,testeThread,&args);
    //pthread_create(&t2,NULL,testeThread,&args2);

    while(1) {
        printf("\rProcessando [%.2f]\r",  progresso / 10000.0);
        fflush(stdout);
        if(finalizou)
            break;
    }

    pthread_join(t1,NULL);
    //pthread_join(t2,NULL);
    ////escreverArquivoDeMatriz(combinarDiretorios(3, pegarDiretorioAtual(), 
    ////                                        OUTPUT_DIR, R_FILENAME), r);
    sem_destroy(&mutex);

    printf("\n");
    return 0;
}
