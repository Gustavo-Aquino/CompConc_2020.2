#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define TAM 60      //tamanho do nosso vetor compartilhado de inputs
#define red 5       //sinal vermelho = 5 medidas criticas em  5 elementos
#define yellow 5    //sinal amarelo  = 5 medidas criticas em 15 elementos
#define critical 35 //temperatura critica que ativa os sinais vermelho e amarelo

//Estrutura usada para interpretar o input dos sensores
typedef struct{
  int temperatura;
  int id_sensor;
  int id_escrita;
} sensor_input;
//Vetor que vai armazenar o input dos sensores
sensor_input temperaturas[60];

//Estrutura usada como argumento para as threads
//a fim de facilitar compartilhamento de informacao.
typedef struct{
  int id;
  int ult_pos;
} tArg;

//variaveis auxiliares ao problema
extern int atuador;//contador de atuadores lendo
extern int sensor; //contador de sensores escrevendo
extern int s_wait; //contador de sensores esperando
extern int count;  //contador de entradas no vetor temperaturas[]
extern int modo;   //"booleano" para o modo padrao ou de teste

//variaveis para sincronizacao
pthread_mutex_t mutex, teste;
pthread_cond_t cond_atuador, cond_sensor;

void writeInput(int id_s, int id_e, int temp, int pos);
void treatInput(sensor_input temps[], int media,int id);

void InicAtuador();
void FimAtuador();
void InicSensor();
void FimSensor();

void *Atuador(void *argumento);
void *Sensor(void *argumento);