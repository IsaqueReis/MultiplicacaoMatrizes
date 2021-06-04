# Multiplicação de Matrizes
Comparar o desempenho de processamento simétrico (SMP) vs. Assimétrico (ASMP). 

# Compilar e Executar
    sudo chmod 777 compile-and-execute.sh
    ./compile-and-execute.sh
  
# Observações Gerais 

  Repositório

    Uso de repositório para versior as versões(Git)
    Commitar só o que funcionar

  Estrutura do arquivo

    Sintaxe e semântica padrão C em Portuquês
    Estrutura com protótipos no topo no inicio e depois criação do .h com os protótipos
    Escopos com chave de abertura na mesma linha da declaração do escopo

# Sobre implementação(Pode mudar*)

  Preferencia Linux (SO em vez do WSL)

  Exemplo de matriz 3x3 será reconhecidas pelo numero definido pelo #define e linhas e colunas devem seguir o valor do define ou terá erro

    #define MTZ_MAX 3
    
    1 2 3 
    4 5 6 
    7 8 9 

  Matrizes serão salvas em um vetor com n*n inteiros

  Comunicação entre threads usando semáforo mutex e memória compartilhada
