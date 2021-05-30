#include "bigint.h"

BigInt criaBigInt(char sinal, int qntNos, int* valores){
    BigInt resul = (BigInt) malloc(sizeof(struct bigInt));
    if(resul != NULL){
        for(int i = qntNos; i > 0; i--){
            if(*valores != 0 || qntNos == 1)
                break;
            valores++;
            qntNos--;
        }
        resul->valores = (int *) malloc(sizeof(int) * qntNos);
        if(resul->valores == NULL){
            free(resul);
            return NULL;
        }
        resul->sinal = sinal;
        resul->qntNos = qntNos;
        for(int i = 0; i < qntNos; i++) resul->valores[i] = valores[i];
    }
    return resul;
}

BigInt novoBigIntPorStr(char *strnum){
    int lenTmp = strlen(strnum);
    if(lenTmp == 0)
        return NULL;

    BigInt bigI = (BigInt) malloc(sizeof(struct bigInt));

    if(bigI != NULL){
        int initIndex = lenTmp - 1;
        int endIndex;
        switch(strnum[0]){
            case '+':
                bigI->sinal = '+';
                endIndex = 1; break;
            case '-':
                bigI->sinal = '-';
                endIndex = 1; break;
            default:
                bigI->sinal = '+';
                endIndex = 0;
        }
        int tamanhoNumero = (lenTmp - endIndex);
        int nValores =
            (tamanhoNumero / (float) MAX_CASAS_DECIM_NO_BIGINT) > (tamanhoNumero / MAX_CASAS_DECIM_NO_BIGINT) ?
            (tamanhoNumero / MAX_CASAS_DECIM_NO_BIGINT) + 1 : (tamanhoNumero / MAX_CASAS_DECIM_NO_BIGINT);

        bigI->qntNos = nValores;
        bigI->valores = (int *) malloc(sizeof(int) * nValores);
        if(bigI->valores == NULL){
            free(bigI);
            return NULL;
        }
        for(int i = nValores - 1; i >= 0 && initIndex >= endIndex; i--){
            bigI->valores[i] = 0;
            int base = 1;
            for(int j = 0; j < MAX_CASAS_DECIM_NO_BIGINT && initIndex >= endIndex; j++){
                int digito = (strnum[initIndex--] - '0');
                if(digito < 0 || digito > 9){
                    free(bigI->valores);
                    free(bigI);
                    return NULL;
                }
                bigI->valores[i] += base * digito;
                base *= 10;
            }
        }
    }
    return bigI;
}

BigInt novoBigIntPorLLInt(long long num){
    // 22 é a quantidade maxima de casas de um long long com sobra
    char strnum[23];
    sprintf(strnum, "%lld", num);
    return novoBigIntPorStr(strnum);
}

BigInt novoBigIntPorULLInt(unsigned long long num){
    // 22 é a quantidade maxima de casas de um long long com sobra
    char strnum[23];
    sprintf(strnum, "%llu", num);
    return novoBigIntPorStr(strnum);
}

void deletaBigInt(BigInt *a){
    if(a != NULL && (*a) != NULL){
        if((*a)->valores != NULL)
            free((*a)->valores);
        free((*a));
        *a = NULL;
    }
}

void imprimeBigInt(BigInt a){
    if(a == NULL)
        return;
    //printf("\n(%c, %d, [ ", a->sinal, a->qntNos);
    for(int i = 0; i < a->qntNos; i++)
        printf("%d ", a->valores[i]);
    //printf("])");
}

int stringBigInt(char *str, BigInt a){
    *str = '\0';
    if(a == NULL)
        return 0;

    char aux[10];

    strcpy(str, a->sinal == '+' ? "" : "-");
    for(int i = 0; i < a->qntNos; i++){
        sprintf(aux,"%d", a->valores[i]);
        int len = strlen(aux);
        int resto = MAX_CASAS_DECIM_NO_BIGINT - len;
        if(i > 0){
            for(int j = len - 1; j >= 0 && resto - j > 0; j--)
                aux[j + resto] = aux[i];
            for(int j = 0; j < resto; j++)
                aux[j] = '0';
        }
        strcat(str, aux);
    }
    return 1;
}

// Deve atender ao requisito |a| >= |b|
int somaBigIntSinalDiferente(BigInt *r, BigInt a, BigInt b){
    if(a == NULL || b == NULL){
        *r = NULL;
        return 0;
    }

    int totalNos = a->qntNos;
    int valoresR[a->qntNos];
    for(int i = 0; i < totalNos; i++) valoresR[i] = a->valores[i];

    int index = totalNos - 1;
    int nosB = b->qntNos;
    for(int i = nosB - 1; i >= 0; i--, index--){
        if(valoresR[index] < b->valores[i]){
            for(int j = index - 1; j >= 0; j--){
                valoresR[j]--;
                if(valoresR[j] >= 0)
                    break;
                valoresR[j] = POW_10_9 - 1;
            }
            valoresR[index] = valoresR[index] + POW_10_9 - b->valores[i];
        }else{
            valoresR[index] -= b->valores[i];
        }
    }
    for(int i = 0; i < a->qntNos && valoresR[i] == 0; i++, totalNos--);
    if(totalNos == 0) totalNos = 1;

    int *aux = valoresR + (a->qntNos - totalNos);
    *r = criaBigInt(a->sinal, totalNos, aux);

    return (*r) != NULL;
}

int somaBigIntSinalIgual(BigInt *r, BigInt a, BigInt b){
    if(a == NULL || b == NULL){
        *r = NULL;
        return 0;
    }

    int maxNos = a->qntNos > b->qntNos ? a->qntNos + 1 : b->qntNos + 1;
    int valoresR[maxNos];

    int totalNos = 0;
    int incrementoProx = 0;
    int nosA = a->qntNos - 1;
    int nosB = b->qntNos - 1;

    int index = maxNos - 1;
    for( ; nosA >= 0 || nosB >= 0; nosA--, nosB--){
        int valor = incrementoProx;
        if(nosA >= 0)
            valor += a->valores[nosA];
        if(nosB >= 0)
            valor += b->valores[nosB];

        if(valor >= POW_10_9){
            valor = valor - POW_10_9;
            incrementoProx = 1;
        }else{
            incrementoProx = 0;
        }
        valoresR[index--] = valor;
        totalNos++;
    }

    if(incrementoProx == 1){
        valoresR[0] = 1;
        totalNos++;
    }
    int *aux = valoresR + (maxNos - totalNos);
    *r = criaBigInt(a->sinal, totalNos, aux);

    return (*r) != NULL;
}

int somaBigInt(BigInt *r, BigInt a, BigInt b){
    if(a == NULL || b == NULL || a->qntNos == 0 || b->qntNos == 0){
        *r = NULL;
        return 0;
    }

    if(a->sinal == b->sinal)
        return somaBigIntSinalIgual(r, a, b);
    if(a->qntNos > b->qntNos)
        return somaBigIntSinalDiferente(r, a, b);
    if(a->qntNos < b->qntNos)
        return somaBigIntSinalDiferente(r, b, a);

    if(a->valores[0] >= b->valores[0])
        return somaBigIntSinalDiferente(r, a, b);
    return somaBigIntSinalDiferente(r, b, a);
}

int adicionaMenosSignificativo(BigInt *a, int n){
    if(n < 0 || a == NULL || *a == NULL) return 0;
    if(n == 0) return 1;

    int qntN = (*a)->qntNos;
    int *aux = realloc((*a)->valores, sizeof(int)*(qntN + n));
    if(aux == NULL)
        return 0;

    for(int i = 0; i < n; i++)
        aux[i + qntN] = 0;
    (*a)->qntNos += n;
    (*a)->valores = aux;
    return 1;
}

int multiplicaBigIntNosDiferentes(BigInt *r, BigInt a, BigInt b){
    if(a == NULL || b == NULL)
        return 0;
    struct bigInt novo;
    if(a->qntNos < b->qntNos){
        int novosValores[b->qntNos];
        int nosAdicionados = b->qntNos - a->qntNos;
        for(int i = a->qntNos - 1; i >= 0; i--)
            novosValores[i + nosAdicionados] = a->valores[i];
        for(int i = 0; i < nosAdicionados; i++)
            novosValores[i] = 0;

        novo.sinal = a->sinal;
        novo.valores = novosValores;
        novo.qntNos = b->qntNos;

        return multiplicaBigIntNosIguais(r, &novo, b);
    }else{
        int novosValores[a->qntNos];
        int nosAdicionados = a->qntNos - b->qntNos;
        for(int i = 0; i < nosAdicionados; i++)
            novosValores[i] = 0;
        for(int i = b->qntNos - 1; i >= 0; i--)
            novosValores[i + nosAdicionados] = b->valores[i];

        novo.sinal = b->sinal;
        novo.valores = novosValores;
        novo.qntNos = a->qntNos;

        return multiplicaBigIntNosIguais(r, a, &novo);
    }
}
int multiplicaBigIntNosIguais(BigInt *r, BigInt a, BigInt b){
    if(a == NULL || b == NULL)
        return 0;
    if(a->qntNos == 0 || b->qntNos == 0){
        *r = novoBigIntPorStr("0");
        return (*r) != NULL;
    }

    long long int valor;
    if(a->qntNos == 1 && b->qntNos == 1){
        valor = (long long int)(a->valores[0]) * (long long int)(b->valores[0]);
        valor = a->sinal == b->sinal ? valor : (-valor);
        *r = novoBigIntPorLLInt(valor);

        return (*r) != NULL;
    }

    int nA2 = a->qntNos / 2;
    int nB2 = a->qntNos / 2;

    struct bigInt x, y, z, w;
    x.sinal = '+';
    y.sinal = '+';
    z.sinal = '+';
    w.sinal = '+';

    x.qntNos = nA2;
    y.qntNos = a->qntNos - nA2;
    z.qntNos = nB2;
    w.qntNos = b->qntNos - nB2;

    x.valores = a->valores;
    y.valores = a->valores + nA2;
    z.valores = b->valores;
    w.valores = b->valores + nB2;

    int ok = 1;
    BigInt S1, S2;
    ok &= multiplicaBigInt(&S1, &x, &z); // Step 1 -> S1 = (a * c)
    ok &= multiplicaBigInt(&S2, &y, &w); // Step 2 -> S2 = (b * d)

    // Step 3 -> S3 = (a+b) * (c+d)
    BigInt tmp1, tmp2, S3;
    ok &= somaBigInt(&tmp1, &x, &y);
    ok &= somaBigInt(&tmp2, &z, &w);
    ok &= multiplicaBigInt(&S3, tmp1, tmp2);

    // Step 4 -> S4 = (S3 - S2 - S1)
    BigInt tmp3, S4;
    S1->sinal = '-';
    S2->sinal = '-';
    ok &= somaBigInt(&tmp3, S1, S2);
    ok &= somaBigInt(&S4, S3, tmp3);

    // Step 5 -> R = (S1*P1 + S4*P2 + S2) -> P1 = 10^9^(n/2 + m/2)
    BigInt tmp4, R;
    S1->sinal = '+';
    S2->sinal = '+';
    ok &= adicionaMenosSignificativo(&S1, y.qntNos + w.qntNos);
    ok &= adicionaMenosSignificativo(&S4, y.qntNos);
    ok &= somaBigInt(&tmp4, S1, S2);
    ok &= somaBigInt(&R, tmp4, S4);

    deletaBigInt(&S1);deletaBigInt(&S2);deletaBigInt(&S3);deletaBigInt(&S4);
    deletaBigInt(&tmp1);deletaBigInt(&tmp2);deletaBigInt(&tmp3);deletaBigInt(&tmp4);

    R->sinal = a->sinal == b->sinal ? '+' : '-';
    *r = R;
    return ok;
}

int multiplicaBigInt(BigInt *r, BigInt a, BigInt b) {
    if(a == NULL || b == NULL)
        return 0;
    if(a->qntNos - (a->qntNos / 2) ==  b->qntNos - (b->qntNos / 2))
        return multiplicaBigIntNosIguais(r, a, b);
    return multiplicaBigIntNosDiferentes(r, a, b);
}
