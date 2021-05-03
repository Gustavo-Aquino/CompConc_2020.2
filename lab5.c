//--Gustavo Martins Ferreira de Aquino
//--DRE:119044012

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/* Variaveis globais */
int NTHREADS;
int iteracoes;
int bloqueadas = 0;
int* vector=NULL;
pthread_mutex_t lock;
pthread_cond_t cond;

int pow2(int exp){
    int base = 2;
    int result = 1;
    while (exp)
    {
        if (exp % 2)
           result *= base;
        exp /= 2;
        base *= base;
    }
    return result;
}

//funcao barreira ( por ser padrão retirada do slide)
void barreira(int nthreads) {
    pthread_mutex_lock(&lock);
    //testa a quantidade de Threads na barreira 
    if (bloqueadas == (nthreads-1)) {
        //ultima thread a chegar na barreira
        pthread_cond_broadcast(&cond);  //libera todas as threads
        bloqueadas=0;                   //reseta o contador
    } else {
        bloqueadas++;                   //aumenta o contador
        pthread_cond_wait(&cond, &lock);//se bloqueia
    }
    pthread_mutex_unlock(&lock);
}

//funcao das threads
void *parcial_sum (void *arg) {
    int id = *(int*)arg;
    int aux=0;
    for (int passo=0; passo < iteracoes; passo++) {
        //Checa se estamos em uma posição real do vetor
        if(id-pow2(passo)>=0){
            aux = vector[id-pow2(passo)];
        }else{
            aux=0;
        }
        //Aguarda todas as threads lerem os valores
        barreira(NTHREADS);
        vector[id]+=aux;    //Faz a mudança dos valores do vetor
        barreira(NTHREADS); //e espera o termino de todas para a
    }                       //próxima iteração do for
    pthread_exit(NULL);
}


/* Funcao principal */
int main(int argc, char *argv[]) {
    //recebe e valida o parametro de entrada, potencia de 2 que representa o vetor
    if(argc < 2) { fprintf(stderr, "Digite: %s <x> tal que 2^x = tamanho do vetor\n", argv[0]);return 1;}
    iteracoes = atoi(argv[1]);
    NTHREADS = pow2(iteracoes);

    pthread_t threads[NTHREADS];
    int id[NTHREADS];

    /* Inicializa o vetor com valor=posição */
    vector = malloc(sizeof(int)*NTHREADS);
    for(int i=0; i<NTHREADS; i++) vector[i] = i+1;

    /* Inicilaiza o mutex (lock de exclusao mutua) e a variavel de condicao */
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init (&cond, NULL);
    
    /* Cria as threads */
    for(int i=0;i<NTHREADS;i++){id[i]=i; pthread_create(&threads[i], NULL, parcial_sum, (void *) &id[i]);}

    /* Espera todas as threads completarem */
    for (int i = 0; i < NTHREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    /* Imprime o vetor */
    for(int i=0; i<NTHREADS; i++) printf("%d ",vector[i]);
    printf("\n");

    /* Desaloca variaveis e termina */
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);
    free(vector);
    return 0;
}