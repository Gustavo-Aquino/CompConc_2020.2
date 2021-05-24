//--Gustavo Martins Ferreira de Aquino
//--DRE:119044012

#include <semaphore.h>
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>

#define NTHREADS  4

//semaforos relacionados as condicoes impostas por t1 e t4
sem_t sem_t1, sem_t4;

void *T4(void *arg){
    printf("Seja bem-vindo!\n");
    sem_post(&sem_t4);
    sem_post(&sem_t4);
    //Como T4 deve rodar antes de T2 e T3 ela eh a responsavel por libera-las

    pthread_exit(NULL);
}

void *T3(void *arg){
    //aguarda o post da T4
    sem_wait(&sem_t4);
    printf("Sente-se por favor.\n");
    sem_post(&sem_t1);
    //gera um post para T1

    pthread_exit(NULL);
}

void *T2(void *arg){
    //aguarda o post da T4
    sem_wait(&sem_t4);
    printf("Fique a vontade.\n");
    sem_post(&sem_t1);
    //gera um post pra T1

    pthread_exit(NULL);
}

void *T1(void *arg){
    //espera por 2 post para poder rodar, dado so sera executada depois da T2 e T3
    sem_wait(&sem_t1);
    sem_wait(&sem_t1);
    printf("Volte sempre!\n");
    
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int i; 
    pthread_t threads[NTHREADS];

    /* Inicilaiza os semaforos */
    sem_init(&sem_t1,0,0);
    sem_init(&sem_t4,0,0);

    /* Cria as threads */
    pthread_create(&threads[3], NULL, T1, NULL);
    pthread_create(&threads[2], NULL, T2, NULL);
    pthread_create(&threads[1], NULL, T3, NULL);
    pthread_create(&threads[0], NULL, T4, NULL);

    /* Espera todas as threads completarem */
    for (i = 0; i < NTHREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}