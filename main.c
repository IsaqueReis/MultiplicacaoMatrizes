#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define N 3

int *multiplicaMatriz(int n[N*N], int m[N*N]){

    int *resul = (int *) malloc(sizeof(int) * N*N);
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

int *potenciaMatriz(int n[N*N], int potencia){

    int *tmp;
    int *resul = (int *) malloc(sizeof(int) * N*N);
    if(resul == NULL) {
        printf("Erro ao alocar memoria!");
        return NULL;
    }
    memcpy(resul, n, sizeof(int) * N*N);

    for(int i=1; i<potencia; i++) {
        tmp = multiplicaMatriz(n, resul);
        free(resul);
        resul = tmp;
    }

    return resul;
}

void imprimeMatriz(int *r){

    int primeiro = 1;

    for(int i=0; i<N; i++) {
        for (int j=0; j<N; j++) {
            if (primeiro == 1) {
                printf("%d", r[N*i + j]);
                primeiro = 0;
            }
            else{
                printf(" %d", r[N*i + j]);
            }

            if (j + 1 == N) {
                printf("\n");
                primeiro = 1;
            }
        }
    }
}

int main(int argc, char *argv[]) {

    int n[N*N] = { 2, 5, 8, 7, 4, 3, 9, 3, 4 };
    int m[N*N] = { 2, 7, 9, 3, 1, 1, 1, 5, 5 };

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
    imprimeMatriz(potenciaMatriz(n, 3));
    printf("\n");

    return 0;
}
