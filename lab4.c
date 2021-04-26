//--Gustavo Martins Ferreira de Aquino
//--DRE:119044012


#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>

#define NTHREADS  4

int x = 0;                      // Contador para sinalizar condições
pthread_mutex_t lock;           // Variáveis usadas para implementação
pthread_cond_t cond_t4, cond_t1;// dos locks      

void *T4(void *arg){
    // T4 é obrigatoriamente nossa primeira thread a ser executada de acordo 
    // com as restrições do problema, então não temos nenhuma condição, damos
    // o lock apenas para garantirmos segurança a variável 'x'.
    pthread_mutex_lock(&lock);
        fprintf(stdout,"Seja bem-vindo!\n");
        x++;
    pthread_mutex_unlock(&lock);

    // Como sabemos que a T4 é condição para que as T2 e T3 sejam executadas podemos
    // agora liberar suas execuções com o broadcast
    pthread_cond_broadcast(&cond_t4);

    pthread_exit(NULL);
}

void *T3(void *arg){
    pthread_mutex_lock(&lock);
        // Nossas restrições dizem que T4 deve rodar primeiro, portanto
        // enquanto a variável 'x' for 0, vamos bloquear essa thread
        while (x==0) pthread_cond_wait(&cond_t4,&lock);
        
        // Podemos agora executar a thread e incrementar a variável 'x',
        // cuidando para sinalizar a validade da T1 quando necessario
        fprintf(stdout,"Sente-se por favor.\n");
        x++;
        if(x==3) pthread_cond_signal(&cond_t1);
    pthread_mutex_unlock(&lock);

    pthread_exit(NULL);
}

void *T2(void *arg){
    pthread_mutex_lock(&lock);
        // Nossas restrições dizem que T4 deve rodar primeiro, portanto
        // enquanto a variável 'x' for 0, vamos bloquear essa thread
        while (x==0) pthread_cond_wait(&cond_t4,&lock);
        
        // Podemos agora executar a thread e incrementar a variável 'x',
        // cuidando para sinalizar a validade da T1 quando necessario
        fprintf(stdout,"Fique a vontade.\n");
        x++;
        if(x==3) pthread_cond_signal(&cond_t1);
    pthread_mutex_unlock(&lock);

    pthread_exit(NULL);
}

void *T1(void *arg){
    pthread_mutex_lock(&lock);
    // Precisamos que T4,T3 e T2 já tenham rodado para podemos executar nossa T1, sendo assim
    // necessariamente nossa variável 'x' é 3, portanto bloqueamos a thread até esse cenário
    while (x<3) pthread_cond_wait(&cond_t1, &lock);
    fprintf(stdout,"Volte sempre!\n");
    pthread_mutex_unlock(&lock);

    pthread_exit(NULL);

}

int main(int argc, char *argv[]) {
    int i; 
    pthread_t threads[NTHREADS];

    /* Inicilaiza o mutex (lock de exclusao mutua) e as variaveis de condicao */
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init (&cond_t1, NULL);
    pthread_cond_init (&cond_t4, NULL);

    /* Cria as threads */
    pthread_create(&threads[3], NULL, T1, NULL);
    pthread_create(&threads[2], NULL, T2, NULL);
    pthread_create(&threads[1], NULL, T3, NULL);
    pthread_create(&threads[0], NULL, T4, NULL);

    /* Espera todas as threads completarem */
    for (i = 0; i < NTHREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    /* Desaloca variaveis e termina */
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond_t4);
    pthread_cond_destroy(&cond_t1);

    return 0;
}