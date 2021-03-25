//--Gustavo Martins Ferreira de Aquino
//--DRE:119044012

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

#define NTHREADS  2         //total de threads a serem criadas
#define VectorSize 1000     //tamanho do vetor

int vector[VectorSize];     //variavel global para o vetor

//estrutura basica para guardar os argumentos da funcao 
typedef struct {
   int ini, end;
} t_Args;

//funcao executada pelas threads
void *ChangeVector (void *arg) {
    t_Args *args = (t_Args *) arg;
    int i=0;

    //simples acrescimo nos valores da funcao
    for(i=args->ini;i<args->end;i++) vector[i]++;

    free(arg); //aqui pode liberar a alocacao feita na main
    pthread_exit(NULL);
}

//funcao principal do programa
int main() {
    pthread_t tid_sistema[NTHREADS];//identificadores das threads no sistema
    int thread;                     //variavel auxiliar
    t_Args *arg;                    //receberá os argumentos para a thread

    for(int i=0;i<VectorSize;i++) vector[i]=4;

    for(thread=0; thread<NTHREADS; thread++) {
        arg = malloc(sizeof(t_Args));
        if (arg == NULL) {
            printf("--ERRO: malloc()\n"); exit(-1);
        }

        //Preenche a estrutura com as informacoes necessarias
        arg->ini = thread*VectorSize/2; 
        arg->end = arg->ini+VectorSize/2; 
        
        //Cria a thread e realiza sua funcao
        if (pthread_create(&tid_sistema[thread], NULL, ChangeVector, (void*) arg)) {
            printf("--ERRO: pthread_create()\n"); exit(-1);
        }
    }

    //espera todas as threads terminarem
    //garantindo que todo o vetor seja alterado
    for (thread=0; thread<NTHREADS; thread++) {
        if (pthread_join(tid_sistema[thread], NULL)) {
            printf("--ERRO: pthread_join() \n"); exit(-1); 
        } 
    }

    //imprime o vetor pra confirmar o resultado satisfatorio
    for(int i=0;i<VectorSize;i++) printf("%d ",vector[i]);

    printf("\n--Sequencia de '5' e apenas '5' representa resultado correto\n");
    pthread_exit(NULL);
}
