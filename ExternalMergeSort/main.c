//
//  main.c
//  ExternalMergeSort
//
//  Created by CAF on 22/06/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//Tamanho da memória interna
#define N 100

struct arquivo{
    FILE *f;
    int pos, MAX, *buffer;
};

void salvaArquivo(char *nome, int *V, int tam, int mudaLinhaFinal){
    int i;
    FILE *f = fopen(nome, "a");
    for(i=0; i < tam-1; i++)
        fprintf(f, "%d\n", V[i]);
    
    if(mudaLinhaFinal == 0){
        fprintf(f, "%d",V[tam-1]);
    }else{
        fprintf(f, "%d\n", V[tam-1]);
    }
    
    fclose(f);
}

void preencheBuffer(struct arquivo* arq, int K){
    int i;
    if(arq->f == NULL){
        return;
    }
    
    arq->pos = 0;
    arq->MAX = 0;
    for(i=0; i<K; i++){
        if(!feof(arq->f)){
            fscanf(arq->f, "%d", &arq->buffer[arq->MAX]);
            arq->MAX++;
        }else{
            fclose(arq->f);
            arq->f = NULL;
            break;
        }
    }
}

int procuraMenor(struct arquivo *arq, int numArqs, int K, int *menor){
    int i, idx = -1;
    for(i=0; i<numArqs; i++){
        int pos = arq[i].pos;
        int MAX = arq[i].MAX;
        if(pos < MAX){
            if(idx == -1){
                idx = i;
            }else{
                if(arq[i].buffer[arq[i].pos] < arq[idx].buffer[arq[idx].pos]){
                    idx = i;
                }
            }
        }
    }
    
    if(idx != -1){
        *menor = arq[idx].buffer[arq[idx].pos];
        arq[idx].pos++;
        
        //Se o buffer ficou vazio, preenche novamente
        if(arq[idx].pos == arq[idx].MAX){
            preencheBuffer(&arq[idx],K);
        }
        return 1;
    }else{
        return 0;
    }
}

void merge(char *nome, int numArqs, int K){
    char novo[20];
    int i;
    int *bufferDeSaida = (int*)malloc(K*sizeof(int));
    
    struct arquivo* arq;
    arq=(struct arquivo*)malloc(numArqs*sizeof(struct arquivo));
    
    
    for(i=0; i<numArqs; i++){
        sprintf(novo, "Temp%d.txt",i+1);
        arq[i].f = fopen(novo, "r");
        arq[i].MAX = 0;
        arq[i].pos = 0;
        arq[i].buffer = (int*)malloc(K*sizeof(int));
        preencheBuffer(&arq[i], K);
        
    }
    //Enquanto houver arquivos para processar
    int menor, posBuffer = 0;
    while(procuraMenor(arq, numArqs, K, &menor) == 1) {
        bufferDeSaida[posBuffer] = menor;
        posBuffer++;
        if(posBuffer == K){
            salvaArquivo(nome, bufferDeSaida, K, 1);
            posBuffer = 0;
        }
    }
    
    //Salva dados que restaram no buffer de saída. Realiza append
    if(posBuffer != 0){
        salvaArquivo(nome, bufferDeSaida, posBuffer, 1);
    }
    
    for(i=0; i<numArqs; i++)
        free(arq[i].buffer);
    
    free(arq);
    free(bufferDeSaida);
    
}

void criaArquivoTeste(char *nome){
    int i;
    FILE *f = fopen(nome, "w");
    srand(time(NULL));
    
    for(i=1000; i>1; i--){
        fprintf(f,"%d\n",i);
    }
    
    fprintf(f,"%d",1);
    fclose(f);
}

int comparador(const void *a, const void *b) {
    return ( *(int*)a - *(int*)b );
}

int criaArquivosOrdenados(char *nome){
    int V[N], cont = 0, total = 0;
    char novo[20];
    FILE *f = fopen(nome, "r");
    
    while(!feof(f)){
        fscanf(f,"%d",&V[total]);
        total++;
        if(total == N){
            cont++;
            sprintf(novo, "Temp%d.txt", cont);
            qsort(V, total, sizeof(int), comparador);
            salvaArquivo(novo, V, total, 0);
            total = 0;
        }
    }
    
    if(total > 0){
        cont++;
        sprintf(novo, "Temp%d.txt", cont);
        qsort(V,total, sizeof(int), comparador);
        salvaArquivo(novo, V, total, 0);
    }
    
    fclose(f);
    return cont;
}

void mergeSortExterno(char *nome){
    char novo[20];
    int numArqs = criaArquivosOrdenados(nome);
    int i, K = N / (numArqs + 1);
    
    remove(nome);
    merge(nome, numArqs, K);
    
    for(i=0; i<numArqs; i++){
        sprintf(novo, "Temp%d.txt",i+1);
        remove(novo);
    }
}

int main(int argc, const char * argv[]) {
    
    printf("Path relative to the working directory is: %s\n", argv[0]);
    
    criaArquivoTeste("dados.txt");
    mergeSortExterno("dados.txt");
    
    return 0;
}
