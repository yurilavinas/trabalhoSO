#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include<sys/sem.h>
#include<sys/shm.h>

#define RUNNING 1
#define PENDING 0
#define NUM_TAB 50

int idsem;
struct sembuf operacao[2];

typedef struct processo{
	int nreq;
	char max_time[9];
	int num_proc;
	time_t start_time;
	int status;
	char proc[50];
}PROCESSO_T;

void getValue(FILE **fp,char* str){

	fscanf(*fp,"%s",str);
	fscanf(*fp,"%s",str);getc(*fp);
	fscanf(*fp,"%[^\n]s",str);

}


int std2sec(char *std_time){

	char hr[3],min[3],sec[3];
	int time;



	strncpy(hr,std_time,2);
	strncpy(min,std_time+3,2);
	strncpy(sec,std_time+6,2);

	hr[2] = 0;
	min[2] = 0;
	sec[2] = 0;

	time = atoi(hr)*3600 + atoi(min)*60 + atoi(sec);


	return time;

}


void printProcesso(PROCESSO_T processo){
	printf("Program = %s",processo.proc);
	printf("\nMax_Time = %s\n",processo.max_time);
	printf("N_proc = %d\n",processo.num_proc);
	printf("ah");
}

int p_sem(){
     operacao[0].sem_num = 0;
     operacao[0].sem_op = 0;
     operacao[0].sem_flg = 0;
     operacao[1].sem_num = 0;
     operacao[1].sem_op = 1;
     operacao[1].sem_flg = 0;
     if ( semop(idsem, operacao, 2) < 0)
       printf("erro no p=%d\n", errno);
}

int v_sem(){
     operacao[0].sem_num = 0;
     operacao[0].sem_op = -1;
     operacao[0].sem_flg = 0;
     if ( semop(idsem, operacao, 1) < 0)
       printf("erro no p=%d\n", errno);
}


int main (int argc, char* argv[]){
	
	FILE* fp;
	char arqName[50], temp[50];
	PROCESSO_T processo[NUM_TAB], aux;
	int idshm;
	int *pshm;
	int i = 0;

	if(argc == 1){
		printf("Nenhum arquivo de entrada informado\n");
		exit(-1);
	}
	
	strcpy(arqName, argv[1]);

	fp = fopen(arqName, "r");
	if(fp == NULL){
		printf("Arquivo nao encontrado\n");
		exit(-1);
	}

	/*da um shmget na mem compartilhada*/	
	if ((idshm = shmget(0x0901080942, NUM_TAB*sizeof(struct processo), IPC_CREAT|0x1ff)) < 0){
	    printf("erro na criacao da memoria compartilhada\n");
	    exit(1);
	}
	
	/*da um attach na mem compartilhada*/
	pshm = (int *) shmat(idshm, (char *)0, 0);
	if (pshm == (int *)-1) {
        printf("erro no attach\n");
        exit(1);
    }

	/*da um semget em semaforo*/
	if ((idsem = semget(0x090015266, 1, IPC_CREAT|0x1ff)) < 0){
	     printf("erro na criacao da fila\n");
	     exit(1);
	}

	getValue(&fp,aux.proc);/*pega o nome do programa*/
	getValue(&fp,aux.max_time);/*Pega o tempo máximo de execução*/
	getValue(&fp,temp);
	aux.num_proc = atoi(temp);/*Pega o numero de processos e transforma em inteiro*/
	aux.start_time = time(NULL);/*Pega o tempo de inicio da execução*/
	aux.status = PENDING;
	
	printProcesso(aux);	

	



	fclose(fp);
	return 0;
}
