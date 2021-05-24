//--Gustavo Martins Ferreira de Aquino
//--DRE:119044012

#include "functions.h"
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

//funcao para escrever as informacoes no vetor compartilhado
void writeInput(int id_s, int id_e, int temp, int pos){
  temperaturas[pos].id_sensor = id_s;
  temperaturas[pos].id_escrita = id_e;
  temperaturas[pos].temperatura = temp;
}

//funcao para lidar com as meididas lidas por um dado atuador
void treatInput(sensor_input temps[], int media,int id){
  int redcount = 0;   //variavel de controle do sinal vermelho
  int yellowcount = 0;//variavel de controle do sinal amarelo
  int countTemp = 0;  //variavel de controle de temperaturas

  //varredura do vetor que contem as ultimas 15 temperaturas lidas
  for (int k = 0; k < 15; k++){
    if (temps[k].temperatura == 0)//caso tenhamos menos de 15 temps
      break;
    countTemp++;

    //Tratar sinais
    if (temps[k].temperatura > critical){
      redcount++;     //temperatura acima de 35 acrescemos
      yellowcount++;  //as variaveis de controle de sinais
    }

    //caso tenhamos as 5 primeiras temperaturas criticas, logo redcount=5; countTemp=5 tal que RED=5
    //e com isso devemos imprimir um Sinal Vermelho e retornar
    if (redcount == countTemp && redcount == red){
      pthread_mutex_lock(&teste);
      printf("SINAL VERMELHO Media: %d\n", media);
        //se estamos no modo teste, imprimos mais informacoes para checar a corretude
        if(modo==1){
          printf("Sensor%d [",id);
            for (int i = 0; i < 15; i++)
                printf(" %d ", temps[i].temperatura);
            printf("]\n");
        }
      pthread_mutex_unlock(&teste);
      return;
    }
  }
  //se varrido o vetor tivermos 5 ou mais temperaturas criticas imprimimos o Sinal Amarelo
  if (yellowcount >= yellow){
    pthread_mutex_lock(&teste);
    printf("Sinal Amarelo Media: %d\n", media);
    //novamente modo teste, informacoes a mais para checagem de corretude
    if(modo==1){
        printf("Sensor%d [",id);
        for (int i = 0; i < 15; i++)
            printf(" %d ", temps[i].temperatura);
        printf("]\n");
    }
    pthread_mutex_unlock(&teste);
    return;
  }
  //caso nao tenhamos sinais, imprimimos a condicao normal
  pthread_mutex_lock(&teste);
  printf("Condicao normal Media: %d\n", media);
  pthread_mutex_unlock(&teste);
  return;
}

//Entrada do atuador
void InicAtuador(){
  pthread_mutex_lock(&mutex);
  while (sensor > 0 || s_wait > 0)
    pthread_cond_wait(&cond_atuador, &mutex);
  atuador++;
  pthread_mutex_unlock(&mutex);
}

//Saida do atuador
void FimAtuador(){
  pthread_mutex_lock(&mutex);
  atuador--;
  if (atuador == 0 && s_wait > 0)
    pthread_cond_signal(&cond_sensor);
  pthread_mutex_unlock(&mutex);
}

//Entrada do sensor
void InicSensor(){
  pthread_mutex_lock(&mutex);
  s_wait++;
  while ((atuador > 0) || (sensor > 0))
    pthread_cond_wait(&cond_sensor, &mutex);
  s_wait--;
  sensor++;
  pthread_mutex_unlock(&mutex);
}

//Saida do sensor
void FimSensor(){
  pthread_mutex_lock(&mutex);
  sensor--;
  if (s_wait > 0)
    pthread_cond_signal(&cond_sensor);
  else
    pthread_cond_broadcast(&cond_atuador);
  pthread_mutex_unlock(&mutex);
}

//thread atuadorora
void *Atuador(void *argumento){
  tArg *args = (tArg *)argumento; //id e ultima posicao de escrita do sensor correspondente
  sensor_input localtemp[15];     //vetor para checagem de alertas

  int contador_last15;  //variavel auxiliar para geracao do vetor localtemp[]
  int contador_geral;   //variavel auxiliar para medir a media
  int mediaTemp;        //media de todas as temperaturas lidas pelo atuador
  int index;            //ponto de inicio da varredura

  while (1){
    mediaTemp = 0;
    contador_geral = 0;
    contador_last15 = 0;
    index = args->ult_pos;  
    //a varredura comeca no ultimo valor escrito pelo sensor e vai voltando
    //pelo vetor global implementar FIFO sabemos que assim teremos as medidas em ordem 

    InicAtuador();
    for (int k = 0; k < 60; k++){
      if (temperaturas[index].temperatura == 0)
        break;
      if (temperaturas[index].id_sensor == args->id){
        //modo teste para checagem de corretude imprime com detalhes o que esta acontecendo
        if(modo==1){  
          pthread_mutex_lock(&teste);
          fprintf(stdout,"L[%d] leu do sensor[%d] na pos[%d]\n", args->id, temperaturas[index].id_sensor,index);
          pthread_mutex_unlock(&teste);
        }
        contador_geral++;
        mediaTemp = mediaTemp + temperaturas[index].temperatura;
        //se ainda estamos entre as ultimas 15 preenchemos o vetor localtemp
        if (contador_last15 < 15){
          localtemp[contador_last15] = temperaturas[index];
          contador_last15++;
        }
      }
      //como rodamos no sentido anti horario precisamos desse "reset"
      if (index == 0)
        index = TAM;
      index--;
    }
    FimAtuador();
    //so fazemos a media se tivermos alguma entrada
    if (contador_geral > 0)
      mediaTemp = mediaTemp / contador_geral;
    //tratamos as ultimas 15 para checar por sinais 
    treatInput(localtemp, mediaTemp, args->id);
    //reset do vetor localtemp
    for (int i = 0; i < 15; i++)
      localtemp[i].temperatura = 0;
    sleep(2);
  }
  free(args);
  pthread_exit(NULL);
}

//thread escritora
void *Sensor(void *argumento){
  tArg *args = (tArg *)argumento;
  int escritas = 0; //quantidade de escrituras feitas por este sensor

  while (1){
    int temp = rand() % (45 + 1 - 20) + 20; //temperatura random entre 20 e 45
    InicSensor();
    if (temp > 30){ //escrevemos apenas inputs de temperatura > 30
      writeInput(args->id, escritas, temp, count);  
      args->ult_pos = count;  //atualizamos a ultima posicao de escrita do sensor
      escritas++;
      //modo teste para checagem de corretude imprime com detalhes o que esta acontecendo
      if(modo==1){
        pthread_mutex_lock(&teste);
        printf("E[%d] escreveu %dC na posicao[%d]\n", args->id,temp,count);
        pthread_mutex_unlock(&teste);
      }
      count = (count + 1) % TAM;
    }
    FimSensor();
    sleep(1);
  }
  free(args);
  pthread_exit(NULL);
}
