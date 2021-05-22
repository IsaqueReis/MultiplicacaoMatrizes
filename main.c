#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define N 1000
#define M_FILENAME "m.dat"
#define N_FILENAME "n.dat"

#define numeroAleatorioEntre(min, max) ((rand() % (max - min + 1)) + min)

typedef unsigned long long int ulli;

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
        printf("O arquivo de matriz %s já existe!\n", filename);
        return;
    } 

    if(fp != NULL);
        remove(filename);

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

ulli *leArquivoDeMatriz(char *filename){

    FILE *fp = fopen(filename, "r");
    ulli *ret =  (ulli *) calloc(1, sizeof(ulli) * N*N);
    int k = 0;

    if(!ret){
        fprintf(stderr, "Erro de alocação!\n");
        exit(EXIT_FAILURE);
    }

    if(!fp){ // o arquivo não existe
        inicializaArquivoDeMatriz(filename);
        if( (fp = fopen(filename, "r")) == NULL ){
            perror("Leitura arquivo de Matriz");
            exit(EXIT_FAILURE);
        }
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
    imprimeMatriz(ret);
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
    for(int i=0; i<N; i++)
        for(int j=0; j<N; j++)
            for(int k=0; k<N; k++)
                resul[N*i + j] += n[N*i + k] * m[N*k + j];

    return resul;
}

ulli *potenciaMatriz(ulli n[N*N], ulli potencia){

    ulli *tmp;
    ulli *resul = (ulli *) malloc(sizeof(ulli) * N*N);
    if(resul == NULL) {
        printf("Erro ao alocar memoria!");
        return NULL;
    }
    memcpy(resul, n, sizeof(ulli) * N*N);

    for(int i=1; i<potencia; i++) {
        tmp = multiplicaMatriz(n, resul);
        free(resul);
        resul = tmp;
    }

    return resul;
}

int main(int argc, char *argv[]) {

    ulli *n = leArquivoDeMatriz(N_FILENAME);
    ulli *m = leArquivoDeMatriz(M_FILENAME);

    printf("nXm:\n");
    imprimeMatriz(n);
    printf("\n");
    imprimeMatriz(m);
    printf("\n");
    imprimeMatriz(multiplicaMatriz(n,m));
    printf("\n");

    // obs: se for alto grau de potência ocorre overflow e com isso geração de números negativos...
    printf("n^10:\n");
    imprimeMatriz(n);
    printf("\n");
    imprimeMatriz(potenciaMatriz(n, 10));
    printf("\n");

    return 0;
}
