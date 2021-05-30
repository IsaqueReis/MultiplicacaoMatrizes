#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

#define MAX_CASAS_DECIM_NO_BIGINT 9
#define POW_10_9 1000000000

struct bigInt{
    char sinal;
    int qntNos;
    int *valores;
};

typedef struct bigInt * BigInt;

BigInt criaBigInt(char sinal, int qntNos, int* valores);
BigInt novoBigIntPorStr(char *strnum);
BigInt novoBigIntPorLLInt(long long num);
BigInt novoBigIntPorULLInt(unsigned long long num);

void deletaBigInt(BigInt *a);
void imprimeBigInt(BigInt a);
int stringBigInt(char *str, BigInt a);

int somaBigIntSinalDiferente(BigInt *r, BigInt a, BigInt b);
int somaBigIntSinalIgual(BigInt *r, BigInt a, BigInt b);
int somaBigInt(BigInt *r, BigInt a, BigInt b);

int adicionaMenosSignificativo(BigInt *a, int n);

int multiplicaBigIntNosDiferentes(BigInt *r, BigInt a, BigInt b);
int multiplicaBigIntNosIguais(BigInt *r, BigInt a, BigInt b);
int multiplicaBigInt(BigInt *r, BigInt a, BigInt b);
