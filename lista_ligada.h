#ifndef __MYCODE_H__
#define __MYCODE_H__

#include<stdio.h>
#include<stdlib.h>

#include "lista_ligada.h"


// Definição da estrutura da lista
typedef struct{
	int tipo;
	double tempo;
	struct lista * proximo;
} lista;


// Função que remove o primeiro elemento da lista
extern lista * remover (lista * apontador);

// Função que adiciona novo elemento à lista, ordenando a mesma por tempo
extern lista * adicionar (lista * apontador, int n_tipo, double n_tempo);

// Função que imprime no ecra todos os elementos da lista
extern void imprimir (lista * apontador);

#endif

