//--Gustavo Martins Ferreira de Aquino
//--DRE:119044012

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include "timer.h"

float *mat1;    //matriz de entrada 1
float *mat2;    //matriz de entrada 2
float *saida;   //matriz de saida
int nthreads;   //numero de threads

typedef struct{
   int id;  //identificador da thread
   int dim; //dimensao das matrizes de entrada
} tArgs;

//funcao que as threads executarao
void * multiplicacao(void *arg) {
    tArgs *args = (tArgs*) arg;

    for(int i=args->id; i<args->dim; i+=nthreads){  //multiplicacao intercalada pelas threads
        for(int j=0; j<args->dim; j++){             //cada thread multiplica as linhas id+n*nthreads
            for(int k=0; k<args->dim; k++){
                saida[i*args->dim+j] += mat1[i*(args->dim) + k] * mat2[k*(args->dim) + j];
            }
        }
    }
   pthread_exit(NULL);
}

//funcao teste, compara as matrizes saida(threads) e saida2(sequencial)
void teste(float *saida2,int tam){
    float *t_saida=saida2;
    for(int i=0; i<tam; i++){                       
        for(int j=0; j<tam; j++){
            if(saida[i*tam+j]!=t_saida[i*tam+j]) return;
        }
    }
    printf("Multiplicação correta\n\n");
}

//fluxo principal
int main(int argc, char* argv[]) {
    int dim;         //dimensao da matriz de entrada
    pthread_t *tid;  //identificadores das threads no sistema
    tArgs *args;     //identificadores locais das threads e dimensao

    double inicio, fim;             //variaveis para tomada de tempo
    double t_inic,t_finaliz;        //tempos divididos pelas duas estrategias
    double t_multi_seq,t_multi_thr; //tempo de multiplicacao de cada estrategia

    GET_TIME(inicio);
    //leitura da entrada
    if(argc<3) {
        printf("Digite: %s <dimensao da matriz> <numero de threads>\n", argv[0]);
        return 1;
    }
    dim = atoi(argv[1]);
    nthreads = atoi(argv[2]);
    if (nthreads > dim) nthreads=dim;

    //alocacao de memoria para as estruturas de dados
    mat1 = (float *) malloc(sizeof(float) * dim * dim);
    if (mat1 == NULL) {printf("ERRO--malloc\n"); return 2;}
    mat2 = (float *) malloc(sizeof(float) * dim * dim);
    if (mat2 == NULL) {printf("ERRO--malloc\n"); return 2;}
    saida = (float *) malloc(sizeof(float) * dim * dim);
    if (saida == NULL) {printf("ERRO--malloc\n"); return 2;}

    //inicializacao das estruturas de dados de entrada e saida
    for(int i=0; i<dim; i++) {
        for(int j=0; j<dim; j++){
            mat1[i*dim+j] = 1;  //equivalente mat1[i][j]
            mat2[i*dim+j] = 2;  //equivalente mat2[i][j]
            saida[i*dim+j] = 0; //equivalente saida[i][j]
        }    
    }
    GET_TIME(fim);
    t_inic = fim - inicio;  //Tempo inicializacao

    //Multiplicacao de matrizes usando Threads
    GET_TIME(inicio);
    //alocacao das estruturas
    tid = (pthread_t*) malloc(sizeof(pthread_t)*nthreads);
    if(tid==NULL) {puts("ERRO--malloc"); return 2;}
    args = (tArgs*) malloc(sizeof(tArgs)*nthreads);
    if(args==NULL) {puts("ERRO--malloc"); return 2;}
    //criacao das threads
    for(int i=0; i<nthreads; i++) {
        (args+i)->id = i;   //numera as threads
        (args+i)->dim = dim;
        if(pthread_create(tid+i, NULL, multiplicacao, (void*) (args+i))){
            puts("ERRO--pthread_create"); return 3;
        }
    } 

    //espera pelo termino da threads
    for(int i=0; i<nthreads; i++) {
        pthread_join(*(tid+i), NULL);
    }
    GET_TIME(fim);   
    t_multi_thr = fim - inicio; //Tempo de multiplicacao pela estrategia threads

    //Multiplicacao de matrizes usando sequencial
    //cria uma nova estrutura de saida
    float *saida2;
    saida2 = (float *) malloc(sizeof(float) * dim * dim);
    if (saida2 == NULL) {printf("ERRO--malloc\n"); return 2;}
    for(int i=0; i<dim; i++) {
        for(int j=0; j<dim; j++){
            saida2[i*dim+j] = 0; 
        }    
    }
    GET_TIME(inicio);
    //multiplicacao
    for(int i=0; i<dim; i++){                       
        for(int j=0; j<dim; j++){             
            for(int k=0; k<dim; k++){
                saida2[i*dim+j] += mat1[i*dim + k] * mat2[k*dim + j];
            }
        }
    }
    GET_TIME(fim);
    t_multi_seq = fim - inicio; //Tempo de multiplicacao pela estrategia sequencial 

    teste(saida2, dim);

    //liberacao da memoria
    GET_TIME(inicio);
    free(mat1);
    free(mat2);
    free(saida);
    free(args);
    free(tid);
    GET_TIME(fim);   
    t_finaliz = fim - inicio;   //Tempo finalizacao

    //Print de resultados
    double t_threads = t_inic + t_multi_thr + t_finaliz;
    double t_sequenc = t_inic + t_multi_seq + t_finaliz;
    printf("Tempo de execução pela estratégia sequencial: %lf\n",t_sequenc);
    printf("Tempo pelas threads, matriz %dx%d com %d threads: %lf\n",dim,dim,nthreads,t_threads);
    printf("Avaliação de desempenho ideal Tseq/Ts+Tp(P) = %lf\n",t_sequenc/(t_inic+t_finaliz+t_multi_seq/nthreads));
    printf("Avaliação de desempenho real Tseq/Tthr = %lf\n",t_sequenc/t_threads);
    

    return 0;
}