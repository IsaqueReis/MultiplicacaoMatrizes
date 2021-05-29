#ifndef BIGINT_H_INCLUDED
#define BIGINT_H_INCLUDED

typedef struct bigInt * BigInt;

BigInt criaBigInt(char sinal, int qntNos, int* valores);
BigInt novoBigIntPorStr(char *strnum);
BigInt novoBigIntPorLLInt(long long num);

void deletaBigInt(BigInt *a);
int stringBigInt(char *str, BigInt a);

int somaBigInt(BigInt *r, BigInt a, BigInt b);
int multiplicaBigInt(BigInt *r, BigInt a, BigInt b);

#endif // BIGINT_H_INCLUDED
