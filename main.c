/*
    11821BCC003 - André Luiz Morais Peres de Quinta
    11811BSI247 - Isaque dos Reis
    11521BSI258 - Luiz Sergio Amaral Resende
    11911BCC039 - Vinícius Calixto Rocha
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <math.h>
#include <sched.h>
#include <dirent.h>
#include <errno.h>

#include "utilidades.h"
#include "main.h"

sem_t mutex[CONTAGEM_MUTEX];   // semáforo para as threads;
ulli *m, *n, *mE10, *nE10, *r; // matrizes m e n, m elevado a 10 potencia, n elevado a decima potencia e r
ulli progresso = 0;            // contador usado para calcular o prograsso da aplicacao

FilaImpressao *f;

bool finalizouPotenciacaoM = false, finalizouPotenciacaoN = false, 
     finalizouR = false, finalizou = false, leuM = false, leuN = false;

char strProgresso[BUFFER_BARRA_PROGRESSO] = BARRA_DE_PROGRESSO;

// retorna o tempo em segundos da diferença de dois timespec(segundos e nanosegundos)
double calculaOTempoEmSegundos(struct timespec t1, struct timespec t2) {
    return (((NANO_SEGUNDO_PARA_SEGUNDO) * (t2.tv_sec - t1.tv_sec) +
           ((t2.tv_nsec) - (t1.tv_nsec))) / 
           ((double) (NANO_SEGUNDO_PARA_SEGUNDO)));
}

// retorna uma cópia da matriz(ponteiro para uma região alocada dinamicamente)
ulli *copiarMatriz(ulli *entrada, size_t tamanhoMatriz) {

    ulli *ret = (ulli *) calloc(1, tamanhoMatriz);

    if(!ret) {
        printf("Erro ao alocar memoria!");
        return NULL;
    }

    memcpy(ret, entrada, tamanhoMatriz);

    return ret;
}

// imprime n quebras de linha seguida por n sleeps de 1 segundo
void imprimeEspacador(int vezes) {
    for(int i = 0; i < vezes; i++) {
        printf("\n");
    }
}

// imprime a matriz passada por argumento
void imprimeMatriz(ulli *r) {

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

// calcula o progresso atual multiplicando a variável progresso
double calcularProgresso() {
    double ret = ((((progresso) / ((N * 19.0))) * (100)));
    int posicaoBarra = (int) floor(((ret * 50) / (100)));

    for(int i = 1; i <= posicaoBarra; i++)
        strProgresso[i] = '#';

    return ret;
}

// adiciona uma matriz a fila de impressão f
void adicionarAFilaDeImpressao(FilaImpressao *f, ulli *matriz, char *nomeArquivo) {

    ulli *paraInserir = copiarMatriz(matriz,  sizeof(ulli) * N*N );

    if(f->Contagem < MAX_ELEMENTOS_FILA_IMPRESSAO) {
        for(int i = 0; i <= f->Contagem; i++) 
            if(i == f->Contagem) {
                f->Fila[i] = paraInserir; 
                f->NomeArquivos[i] = nomeArquivo;
            }
                
        f->Contagem++;
    }
}

// remove o primeiro elemento da fila de impressão f, seu valor é atribuido ao ponteiro ret
ulli *removerFilaDeImpressao(FilaImpressao *f, char *nomeArquivo) {

    ulli *ret = NULL;

    if(f->Contagem > 0) {

        ret = copiarMatriz(f->Fila[0],  sizeof(ulli) * N*N);
        strcpy(nomeArquivo, f->NomeArquivos[0]);
        
        if(f->Fila[0] != NULL)
            free(f->Fila[0]);
            
        for(int i = 1; i < f->Contagem; i++) {
            f->Fila[i - 1] = f->Fila[i];
            strcpy(f->NomeArquivos[i - 1], f->NomeArquivos[i]);
        }
           
        f->Contagem--;
    }

    return ret;
}

// cria o arquivo de matriz e o inicializa com números aleatórios de acordo com o definição N
void inicializaArquivoDeMatriz(char *nomeArquivo){

    FILE *fp = fopen(nomeArquivo, "r");

    if(fp != NULL){ // o arquivo já existe
        remove(nomeArquivo);
    }

    fp = fopen(nomeArquivo, "a+");

    if(!fp){
        perror("Criação do arquivo de matriz");
        exit(EXIT_FAILURE);
    }

    // gera uma seed baseada no tempo atual e id da thread atual, em milisegundos
    srand(time(NULL) * (int) pthread_self()); 

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

// escreve a estrutura de matriz no arquivo de nome passado nos argumentos
void escreverArquivoDeMatriz(char *nomeArquivo, ulli *mtz){

    FILE *fp = fopen(nomeArquivo, "r");

    if(fp != NULL){ // o arquivo já existe
        remove(nomeArquivo);
    } 

    fp = fopen(nomeArquivo, "a+");

    if(!fp){
        perror("Criação do arquivo de matriz");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL)); // gera uma seed baseada no tempo atual em milisegundos

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

// realiza leitura de um arquivo de matriz e retorna ponteiro da estrutura da matriz presente no arquivo
ulli *leArquivoDeMatriz(char *nomeArquivo){

    FILE *fp;
    int k = 0;
    ulli *ret =  (ulli *) calloc(1, sizeof(ulli) * N*N);

    inicializaArquivoDeMatriz(nomeArquivo);

    if( (fp = fopen(nomeArquivo, "r")) == NULL ){
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
        fgetc(fp); // le o '\n' e o descarta
    }
    
    printf("Matriz lida do arquivo %s.\n", nomeArquivo);
    return ret;
}

// retorna o resultado da multiplicação das matrizes n e m
ulli *multiplicaMatriz(ulli n[N*N], ulli m[N*N]){

    ulli *resul = (ulli *) malloc(sizeof(ulli) * N*N);

    if(resul == NULL) {
        printf("Erro ao alocar memoria!");
        return NULL;
    }

    // inicializa todos os valores da matriz de resultado criada
    for(int i=0; i<N*N; i++)
        resul[i] = 0;

    // o primeiro laço seleciona as linhas
    for(int i=0; i<N; i++) {
        // o segundo seleciona colunas
        for(int j=0; j<N; j++) {
            // dado a linha e coluna selecionada para a matriz resul,
            //      o terceiro laço realiza as iterações para soma de multiplicações em n e m
            //      produzindo o resultado para o valor de resul selecionado
            for(int k=0; k<N; k++) {
                resul[N*i + j] += n[N*i + k] * m[N*k + j];
            }
        }

        // atualiza o progresso das threads de forma sincronizada
        sem_wait(&mutex[MUTEX_PROGRESSO]);
        progresso++;
        sem_post(&mutex[MUTEX_PROGRESSO]);
    }

    return resul;
}

// retorna o resultado da potenciação da matriz n por um inteiro representando a potência
ulli *potenciaMatriz(char *nomeMatriz, ulli n[N*N], ulli potencia){

    ulli *tmp;
    ulli *resul = copiarMatriz(n, sizeof(ulli) * N*N);
    char contadorItrStr[TAMANHO_BUFFER_STR_NUMERICA];
    
    for(int i=1; i<potencia; i++) {
        tmp = multiplicaMatriz(n, resul);
        free(resul);
        resul = tmp;

        // seleciona o arquivo a salvar e adiciona a fila de impressão de forma sincronizada
        sprintf(contadorItrStr, "%d", i+1);
        char *nomeArquivo = concatenarStrings(3, nomeMatriz, contadorItrStr, ".dat");

        sem_wait(&mutex[MUTEX_OPERACAO_FILA]); // bloqueia a fila de impressão para liberação de novas entradas
        adicionarAFilaDeImpressao(f, resul, nomeArquivo);
        sem_post(&mutex[MUTEX_OPERACAO_FILA]); // libera a fila de impressão para liberação de novas entradas
    }
    
    return resul;
}

// realiza remoção das matrizes presentes na fila de impressão f
void esvaziaFilaDeImpressao() {

    while(f->Contagem > 0) {

        char *nomeArquivo = (char *) calloc(TAMANHO_BUFFER_STRING, sizeof(char));

        sem_wait(&mutex[MUTEX_OPERACAO_FILA]); // aguarda alguma operação na fila de impressão
        ulli *matriz = removerFilaDeImpressao(f, nomeArquivo);
        sem_post(&mutex[MUTEX_OPERACAO_FILA]); // libera a fila de impressão para recepção de novas entradas

        if(matriz != NULL && nomeArquivo != NULL) {
            escreverArquivoDeMatriz(combinarDiretorios(3, pegarDiretorioAtual(), DIRETORIO_SAIDA, nomeArquivo), matriz);
            free(matriz);
            free(nomeArquivo);
        }
    }
}

// função que as threads IoBound são atribuidas, possui o fluxo com as operações de IO
void *tIoBound(void *args){

    ArgumentosThread *tArgs = (ArgumentosThread*) args;

    // leitura da matriz M
    if(!strcmp(tArgs->nomeDaMatriz, "m")) {
        m = leArquivoDeMatriz(combinarDiretorios(3, pegarDiretorioAtual(), 
                                      DIRETORIO_ENTRADA, NOME_ARQUIVO_M));
        leuM = true;
    }
    
    // leitura da matriz N
    if(!strcmp(tArgs->nomeDaMatriz, "n")) {
        n = leArquivoDeMatriz(combinarDiretorios(3, pegarDiretorioAtual(), 
                                      DIRETORIO_ENTRADA, NOME_ARQUIVO_N));
        leuN = true;
    }

    while(true) {
        
        // escrita do resultado se o progresso estiver concluido
        if(calcularProgresso() >= 100.0) {

            // sincronização das duas threads para que apenas uma imprima R
            sem_wait(&mutex[MUTEX_IMPRIME_R]); 

            if(finalizou)
                break;

            imprimeEspacador(1);
            printf("Escrevendo o arquivo da matriz resultante...\n");
            escreverArquivoDeMatriz(combinarDiretorios(3, pegarDiretorioAtual(), DIRETORIO_SAIDA, NOME_ARQUIVO_R), r);
            printf("Resultado Calculo: \n");
            imprimeMatriz(r);
            printf("\n");

            finalizou = true; // só uma saí campeã

            sem_post(&mutex[MUTEX_IMPRIME_R]);

            break;
        } else {
            esvaziaFilaDeImpressao();
        }
    }
}

// função que as threads CpuBound são atribuidas, possui o fluxo com as operações de multiplicação
void *tCpuBound(void *args) {
    
    ArgumentosThread *tArgs = (ArgumentosThread*) args;

    while(true) {
        if(leuM && leuN)
            break;
    }

    if(!strcmp(tArgs->nomeDaMatriz, "m")) {
        mE10 = potenciaMatriz("m", m, POTENCIA);
        finalizouPotenciacaoM = true;
    }
    
    if(!strcmp(tArgs->nomeDaMatriz, "n")) {
        nE10 = potenciaMatriz("n", n, POTENCIA);
        finalizouPotenciacaoN = true;
    }

    while(true) {
        if(finalizouPotenciacaoM && finalizouPotenciacaoN) {

            // duas threads competem para ver quem finaliza primeiro...
            sem_wait(&mutex[MUTEX_FINALIZA_CALCULO]);

            if(finalizouR)
                break;

            r = multiplicaMatriz(nE10, mE10);
            finalizouR = true; //só uma saí campeã

            sem_post(&mutex[MUTEX_FINALIZA_CALCULO]);

            break;
        }
    }
}

// Inicializa e destroi os semaforos da aplicação
void gerenciaSemaforos(ComandoSemaforo comando) {
    for(int i = 0; i < CONTAGEM_MUTEX; i++) {
        if(comando == Inicializa)
            sem_init(&mutex[i], 0, 1);
        else if(comando == Destroi)
            sem_destroy(&mutex[i]);
    } 
}

// Define o thread affinty para o procesador 'numProcessLog'
int threadAffinity(pthread_t tid, int numProcessLog){
    cpu_set_t cpuset;  // mascara de affinidade
    CPU_ZERO(&cpuset); // zera a mascara
    CPU_SET(numProcessLog, &cpuset); //define 1 na mascara para o processador numProcessLog 
  
    return !pthread_setaffinity_np(tid, sizeof(cpuset), &cpuset); //define afinidade para a thread
}

// retorna o número correspondente a string
// caso a string nao seja numérica retorna -1
int ehRetornaNumero(char *str){
    int res = 0;
    
    for(int i = 0; str[i]; i++)
        if (!isdigit(str[i]))
            return -1;

    return atoi(str);
}

// define o processor affinite para todos os processos do sistemas
// para os processadores no intervalo [numIniProcessLog, numFimProcessLog]
int processorAffinityTodosProcessos(int numIniProcessLog, int numFimProcessLog){
    cpu_set_t cpuset; // mascara para a afinidade
    DIR *dirProc;
    struct dirent *dir;
    int meuPid;
    char *erro;

    // iniciando a mascara que contem as cpus que serao utilizadas pelos processos
    CPU_ZERO(&cpuset);
    for(int i = numIniProcessLog; i <= numFimProcessLog; i++)
        CPU_SET(i, &cpuset);

    // o diretorio proc contem os pids com os processos em execucao
    dirProc = opendir("/proc");
    if(dirProc == NULL)
        return 0;
    
    int primeiroErro = 1;
    while( (dir = readdir(dirProc)) != NULL){
        meuPid = ehRetornaNumero(dir->d_name);
        if(meuPid >= 0){
            // define o processor affinity para o processo com  o pid correspondente
            int resul = sched_setaffinity(meuPid, sizeof(cpu_set_t), &cpuset);
            if(resul == -1){ 
                // Algum error ocorreu definir o processor affinity
                // Definimos que encerraremos o programa caso aponte diferente de EINVAL
                // Ex: Erros de privilegios não apropiados encerram o programa 
                if (errno != EINVAL){
                    erro = strerror(errno);
                    fprintf(stderr, "Erro %d - Processo %d: %s\n", errno, meuPid, erro);
                    exit(EXIT_FAILURE);
                }
                if(primeiroErro){
                    // Não é possivel alterar o processor affinity de Processos do Kernel
                    erro = strerror(errno);
                    printf("Os seguintes processos não podem ter o process afinnity alterado (%s) \n Processos: ", erro);
                    primeiroErro = 0;
                }
                printf("%d ", meuPid);
            }
        }
    }
    printf("\n");
    
    closedir(dirProc);
    return 1;
}

int main(int argc, char *argv[]) {
    pthread_t t1, t2, t3, t4;
    struct timespec inicioProcessamento, fimProcessamento; 
    ArgumentosThread argumentosTM = {"m"};
    ArgumentosThread argumentosTN = {"n"};

    long int procesadoresLogicos = sysconf(_SC_NPROCESSORS_CONF);
    int ok;
    
    printf("%d\n",getpid());

    clock_gettime(CLOCK_MONOTONIC, &inicioProcessamento); // pega o tempo inicial

    f = (FilaImpressao*) calloc(1, sizeof(FilaImpressao));

    if(!f) {
        fprintf(stderr, "Erro de alocação!");
        exit(EXIT_FAILURE);
    }

    f->Fila = (ulli**) calloc(MAX_ELEMENTOS_FILA_IMPRESSAO, sizeof(ulli*));
    f->NomeArquivos = (char**) calloc(MAX_ELEMENTOS_FILA_IMPRESSAO, 
                                                    sizeof(char*));

    if(!f->Fila || !f->NomeArquivos) {
        fprintf(stderr, "Erro de alocação!");
        exit(EXIT_FAILURE);
    }

    gerenciaSemaforos(Inicializa);

    printf("Inicializando o cálculo de potenciação de matrizes [R = M^10 * N^10]...\n");

    pthread_create(&t1,NULL,tIoBound,  &argumentosTM);
    pthread_create(&t2,NULL,tIoBound,  &argumentosTN);
    pthread_create(&t3,NULL,tCpuBound, &argumentosTM);
    pthread_create(&t4,NULL,tCpuBound, &argumentosTN);

    if(USAR_PROCESSOR_AFFINITY){
	    // verificar quantos processadores lógicos existem na cpu, se tiver mais de 4 ok, se não retorna erro
        if(procesadoresLogicos < 4){
            fprintf(stderr, "Erro, quantidade de processadores lógicos menor que o mínimo exigido!");
            exit(EXIT_FAILURE);
        }
        
        // processor afinitty das threads CPU bound e IO bound
        ok = 1;
        ok &= threadAffinity(t1, 0);
        ok &= threadAffinity(t2, 1);
        ok &= threadAffinity(t3, 2);
        ok &= threadAffinity(t4, 3);

        if(ok){
            printf("Afinidade de processadores lógicos para as threads foram configuradas\n");
        } else {
            fprintf(stderr, "Erro ao configurar a afinidade de processadores lógicos para as threads!\n");
            exit(EXIT_FAILURE);
        }
        // coloca o processor affnity para em todos os processos do sistema
        // nos processadores restantes
        if(procesadoresLogicos == 4){
            if(!processorAffinityTodosProcessos(3, 3)){
                fprintf(stderr, "Erro ao obter os pids dos processos em execução!\n");
                exit(EXIT_FAILURE);
            }
        } else {
            if(!processorAffinityTodosProcessos(4, procesadoresLogicos - 1)){
                fprintf(stderr, "Erro ao obter os pids dos processos em execução!\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    // Aguarda até realizar a leitura das matrizes para então iniciar a barra de progresso
    while(!leuM || !leuN)
        ;

    while(true) {
        double progressoAtual = calcularProgresso();
        // imprime barra de carregamento
        printf("\r%s %.2f %% Processado\r", strProgresso, progressoAtual);
        fflush(stdout);

        if(progressoAtual >= 100.0) {
            esvaziaFilaDeImpressao();
            break;
        }
    }

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);

    gerenciaSemaforos(Destroi);

    clock_gettime(CLOCK_MONOTONIC, &fimProcessamento); // marca o fim da execução

    imprimeEspacador(2);
    
    int tempoGasto = (int) calculaOTempoEmSegundos(inicioProcessamento, fimProcessamento);
    int minutos = (int) tempoGasto/60;
    int horas = (int) minutos/60;

    printf("\nCalculo Finalizado! Tempo de execução no formato HH:MM:SS: %d:%d:%d\n", horas, minutos%60, tempoGasto%60);
    
    imprimeEspacador(2);

    printf("Pressione qualquer tecla para continuar... ");

    getchar();

    return 0;
}