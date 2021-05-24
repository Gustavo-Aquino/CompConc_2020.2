//--Gustavo Martins Ferreira de Aquino
//--DRE:119044012

#include "functions.h"
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

int atuador = 0;//contador de atuadores lendo
int sensor = 0; //contador de sensores escrevendo
int s_wait = 0; //contador de sensores esperando
int count = 0;  //contador de entradas no vetor temperaturas[]
int modo = 0;   //"booleano" para o modo padrao ou de teste

int main(int argc, char *argv[]){
  //Processa os argumentos do console
  if(argc<3){ 
    printf("Passe na execucao o Nthreads e o modo (apenas_resultado(0) / log_execucao(1))\n");
    printf("./<programa> <Nthreads> <modo>\n");
    return 1;
  }
  int Nthreads = atoi(argv[1]);
  modo = atoi(argv[2]);

  srand(time(NULL));

  //identificadores das threads
  pthread_t tid[Nthreads * 2];
  tArg ids[Nthreads];

  //inicializa as variaveis de sincronizacao
  pthread_mutex_init(&mutex, NULL);
  pthread_mutex_init(&teste, NULL);
  pthread_cond_init(&cond_atuador, NULL);
  pthread_cond_init(&cond_sensor, NULL);

  //cria os atuadorores e sensores
  for (int i = 0; i < Nthreads; i++){
    ids[i].id = i + 1;
    ids[i].ult_pos = 0;
    if (pthread_create(&tid[i], NULL, Atuador, (void *)&ids[i]))
      exit(-1);
    if (pthread_create(&tid[i + Nthreads], NULL, Sensor, (void *)&ids[i]))
      exit(-1);
  }
  pthread_exit(NULL);
  return 0;
}
