#include "main.h"

sem_t mutex[4]; //semáforo para as threads;
ulli *m, *n, *mE10, *nE10, *r; //matrizes m e n, m elevado a 10 potencia, n elevado a decima potencia e r
FilaImpressao *f;
bool finalizou = false, finalizouCalculo = false, imprimindoR = false;
ulli progresso = 0;
char strProgresso[BUFFER_BARRA_PROGRESSO] = BARRA_DE_PROGRESSO;

double calculaOTempoEmSegundos(struct timespec t1, struct timespec t2) {
    return (((NANO_SEGUNDO_PARA_SEGUNDO) * (t2.tv_sec - t1.tv_sec) +
           ((t2.tv_nsec) - (t1.tv_nsec))) / 
           ((double) (NANO_SEGUNDO_PARA_SEGUNDO)));  //obtem o tempo em segundos
}

ulli *copiarMatriz(ulli *entrada, size_t tamanhoMatriz) {

    ulli *ret = (ulli *) calloc(1, tamanhoMatriz);

    if(!ret) {
        printf("Erro ao alocar memoria!");
        return NULL;
    }

    memcpy(ret, entrada, tamanhoMatriz);

    return ret;
}

void imprimeEspacador(int vezes) {
    for(int i = 0; i < vezes; i++){
        printf("\n");
        sleep(1);
    }
}

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

double calcularProgresso()
{
    ulli maxOperacoes = MAX_OPERACOES;
    double ret = ((((progresso) / ((maxOperacoes * 19.0))) * (100)));
    int posicaoBarra = (int) round(((ret * 50) / (100)));

    for(int i = 1; i <= posicaoBarra; i++)
        strProgresso[i] = '#';

    return ret;
}

//adiciona uma matriz a fila de impressão f
void AdicionarAFilaDeImpressao(FilaImpressao *f, ulli *matriz, 
                                            char *nomeArquivo) {

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

//remove o primeiro elemento da fila de impressão f, seu valor é atribuido
//ao ponteiro ret
ulli *RemoverFilaDeImpressao(FilaImpressao *f, char *nomeArquivo) {

    ulli *ret = NULL;

    if(f->Contagem > 0) {

        ret = copiarMatriz(f->Fila[0],  sizeof(ulli) * N*N);
        strcpy(nomeArquivo, f->NomeArquivos[0]);

        if(f->Fila[0] != NULL)
            free(f->Fila[0]);

        if(f->NomeArquivos[0] != NULL)
            free(f->NomeArquivos[0]);

        for(int i = 1; i < f->Contagem; i++)
            f->Fila[i - 1] = f->Fila[i];

        f->Contagem--;
    }

    return ret;
}

void inicializaArquivoDeMatriz(char *nomeArquivo){

    FILE *fp = fopen(nomeArquivo, "r");

    if(fp != NULL){ // o arquivo já existe
        //printf("O arquivo de matriz %s já existe!Removendo...\n", filename);
        remove(nomeArquivo);
        //printf("Arquivo removido com sucesso!\n");
    } 

    fp = fopen(nomeArquivo, "a+");

    if(!fp){
        perror("Criação do arquivo de matriz");
        exit(EXIT_FAILURE);
    }

    //gera uma seed baseada no tempo atual e id da thread atual, em milisegundos
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

void escreverArquivoDeMatriz(char *nomeArquivo, ulli *mtz){

    FILE *fp = fopen(nomeArquivo, "r");

    if(fp != NULL){ // o arquivo já existe
        //printf("O arquivo de matriz %s já existe!Removendo...\n", filename);
        remove(nomeArquivo);
        //printf("Arquivo removido com sucesso!\n");
    } 

    fp = fopen(nomeArquivo, "a+");

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
        fgetc(fp); //le o '\n' e o descarta
    }
    
    printf("Matriz lida do arquivo %s.\n", nomeArquivo);
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
                progresso++;
            }
        }
    }

    return resul;
}

ulli *potenciaMatriz(char *nomeMatriz, ulli n[N*N], ulli potencia){

    ulli *tmp;
    ulli *resul = copiarMatriz(n, sizeof(ulli) * N*N);
    char contadorItrStr[TAMANHO_BUFFER_STR_NUMERICA];

    for(int i=1; i<potencia; i++) {
        tmp = multiplicaMatriz(n, resul);
        free(resul);
        resul = tmp;

        sem_wait(&mutex[3]);

        sprintf(contadorItrStr, "%d", i+1);
        char *nomeArquivo = concatenarStrings(3, nomeMatriz, contadorItrStr, 
                                                                    ".dat");
        AdicionarAFilaDeImpressao(f, resul, nomeArquivo);

        sem_post(&mutex[3]);
    }
    
    return resul;
}

//Thread responsável por carregar os valores das matrizes M e N
void *tCarregarMatrizes(void *args) {

    sem_wait(&mutex[0]); //bloqueia as threads dependentes de M 

    m = leArquivoDeMatriz(combinarDiretorios(3, pegarDiretorioAtual(), 
                                  DIRETORIO_ENTRADA, NOME_ARQUIVO_M));

    sem_post(&mutex[0]);

    sem_wait(&mutex[1]); //bloqueia as threads dependentes de N

    n = leArquivoDeMatriz(combinarDiretorios(3, pegarDiretorioAtual(), 
                                  DIRETORIO_ENTRADA, NOME_ARQUIVO_N));

    sem_post(&mutex[1]);
}

void *tImprimeArquivos(void *args){

    while(true) {

        if(finalizouCalculo) { //espera que R seja calculado
            
            imprimindoR = true;

            printf("Escrevendo o arquivo da matriz resultante...\n");

            escreverArquivoDeMatriz(combinarDiretorios(3, pegarDiretorioAtual(), 
                                           DIRETORIO_SAIDA, NOME_ARQUIVO_R), r);

            printf("Resultado Calculo: \n");

            imprimeMatriz(r);
            printf("\n");

            finalizou = true;
            finalizouCalculo = false;
            imprimindoR = false;

            break;
        } else {

            sem_wait(&mutex[3]);
            char contadorItrStr[TAMANHO_BUFFER_STR_NUMERICA];
            while(f->Contagem > 0) {

                char *nomeArquivo = (char *) calloc(TAMANHO_BUFFER_STRING, 
                                                            sizeof(char));
                ulli *matriz = RemoverFilaDeImpressao(f, nomeArquivo);

                if(matriz != NULL && nomeArquivo != NULL) {
                    escreverArquivoDeMatriz(combinarDiretorios(3, 
                          pegarDiretorioAtual(), DIRETORIO_SAIDA, 
                                           nomeArquivo), matriz);
                    free(matriz);
                    free(nomeArquivo);
                }
            }
            
            sem_post(&mutex[3]);
        }
    }
}

void *tPotencializao(void *args) {

    
    sem_wait(&mutex[2]); //bloqueia a thread que calcula o resultado final até que tenha-se M e N potenciado.

    sem_wait(&mutex[0]); //aguarda M
    mE10 = potenciaMatriz("m", m, POTENCIA);
    sem_post(&mutex[0]);

    sem_wait(&mutex[1]); //aguarda N
    nE10 = potenciaMatriz("n", n, POTENCIA);
    sem_post(&mutex[1]);
    
    sem_post(&mutex[2]);
}

void *tCalculaR(void *args) { //1.000.000.000

    sem_wait(&mutex[2]);

    r = multiplicaMatriz(mE10, nE10);
    finalizouCalculo = true;

    sem_post(&mutex[2]);
}

void gerenciaSemaforos(ComandoSemaforo comando) {
    for(int i = 0; i < 4; i++) {
        if(comando == Inicializa)
            sem_init(&mutex[i], 0, 1);
        else if(comando == Destroi)
            sem_destroy(&mutex[i]);
    } 
}

int main(int argc, char *argv[]) {

    pthread_t t1, t2, t3, t4;
    struct timespec inicioProcessamento, fimProcessamento; 

    clock_gettime(CLOCK_MONOTONIC, &inicioProcessamento); //pega o tempo inicial

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

    pthread_create(&t1,NULL,tCarregarMatrizes, (void*)(0));
    pthread_create(&t2,NULL,tPotencializao,    (void*)(0));
    pthread_create(&t3,NULL,tCalculaR,         (void*)(0));
    pthread_create(&t4,NULL,tImprimeArquivos,  (void*)(0));

    printf("Inicializando o cálculo de potenciação de matrizes [R = M^10 * N^10]...\n");

    while(true) {
        if(!imprimindoR) {
            double progressoAtual = calcularProgresso();
            printf("\r%s %.2f %% Processado\r", strProgresso, progressoAtual);
            fflush(stdout);
            if(finalizou)
                break;
        }
    }

    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    pthread_join(t3,NULL);
    pthread_join(t4,NULL);

    gerenciaSemaforos(Destroi);

    clock_gettime(CLOCK_MONOTONIC, &fimProcessamento); //marca o fim da execução

    imprimeEspacador(2);

    printf("\nCalculo Finalizado! Tempo de execução: [%.2f segundos]\n", 
          calculaOTempoEmSegundos(inicioProcessamento, fimProcessamento));
    
    imprimeEspacador(2);

    printf("Pressione qualquer tecla para continuar... ");

    getchar();

    return 0;
}
