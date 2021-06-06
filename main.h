#define N 5000
#define POTENCIA 10
#define BUFFER_BARRA_PROGRESSO 53
#define NANO_SEGUNDO_PARA_SEGUNDO 1000000000
#define MAX_ELEMENTOS_FILA_IMPRESSAO 18

#define CONTAGEM_MUTEX 4
#define MUTEX_FINALIZA_CALCULO 0
#define MUTEX_IMPRIME_R 1
#define MUTEX_PROGRESSO 2
#define MUTEX_OPERACAO_FILA 3

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

typedef struct argumentosThread {
    char nomeDaMatriz[TAMANHO_BUFFER_STRING];
} ArgumentosThread;

typedef enum {
    Inicializa, Destroi
} ComandoSemaforo;
