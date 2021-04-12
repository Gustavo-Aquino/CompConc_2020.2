//--Gustavo Martins Ferreira de Aquino
//--DRE:119044012

#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include "timer.h"

long long int tam;      //quantidade de elementos a ser somados na serie
long long int nthreads; //numero de threads

typedef struct {        //estrutura basica para guardar os argumentos da funcao
    long long int ini, end;
    double aux;
    double div;
} t_Args;

void * func_conc(void * arg){
    t_Args *args = (t_Args *) arg;
    long long int i=0;
    double *localsum; //soma local
    localsum = (double*) malloc(sizeof(double));
    
    for(i=args->ini;i<args->end;i++){
        //printf("%lld",i);
        args->div=(i+1)*2-1;
        args->aux =1/args->div;
        //printf("%.2lf ",args->aux);
        if(i%2==0) *localsum+=args->aux;
        if(i%2==1) *localsum-=args->aux;
        
    }
    free(arg);
    pthread_exit((void *) localsum);
}

int main(int argc, char *argv[]){
    //recebe e valida os parametros de entrada (dimensao do vetor, numero de threads)
    if(argc < 3) { fprintf(stderr, "Digite: %s <dimensao do vetor> <numero threads>\n", argv[0]);return 1;}
    tam = atoll(argv[1]);
    nthreads = atoll(argv[2]);

    double ini, end, t_seq, t_conc; //tomada de tempo

    //Parte sequencial, inicio da tomada de tempo
    GET_TIME(ini);
    double pi_seq=0;
    double aux=1;
    double j=1;
    
    for(long long int i=0;i<tam;i++){
        aux=1/j;
        if(i%2==0) pi_seq+=aux;
        if(i%2==1) pi_seq-=aux;
        j+=2;
    }
    pi_seq=pi_seq*4;
    GET_TIME(end);
    t_seq=end-ini;

    //Parte Concorrente
    GET_TIME(ini);
    t_Args *arg;
    pthread_t *tid;
    double *retorno;
    double pi_conc= 0;

    tid = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
    if(tid==NULL) {
        fprintf(stderr, "ERRO--malloc\n");
        return 2;
    }

    for(long long int id=0; id<nthreads; id++){
        arg = malloc(sizeof(t_Args));
        if (arg == NULL) {
            printf("--ERRO: malloc()\n"); return 2;
        }
        arg->aux = 1;
        arg->div = 1;
        arg->ini = id*tam/nthreads; 
        arg->end = arg->ini+tam/nthreads;

        if(pthread_create(tid+id, NULL, func_conc, (void*) arg)){
            fprintf(stderr, "ERRO--pthread_create\n");
            return 3;
        }
    }
    for(long long int id=0; id<nthreads; id++) {
        if(pthread_join(*(tid+id),(void **) &retorno)){
            fprintf(stderr, "ERRO--pthread_create\n");
            return 3;
        }
        pi_conc += *retorno; 
    }
    pi_conc=pi_conc*4;
    GET_TIME(end);
    t_conc=end-ini;

    fprintf(stdout,"Usando %lld elementos:\n\n",tam);
    fprintf(stdout,"Valor real: %.15lf\nValor sequencial: %.15lf\nValor concorrente: %.15lf\n\n",M_PI,pi_seq,pi_conc);
    fprintf(stdout,"Tempo sequencial: %.5lf\nTempo concorrente: %.5lf\n",t_seq,t_conc);

    free(tid);
}