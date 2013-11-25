#include <malloc.h>
#include <stdio.h>

#define MEM_OWN
#include "mem_manager.h"
#undef MEM_OWN

#include "lista.h"

static LIS_tppLista pLista;

static int CompararPonteiros (void *pointer1, void *pointer2);

void MEM_Alloc(int size, void ** ppResult)
{
   if (pLista == NULL)
   {
      LIS_CriarLista(&pLista, free, CompararPonteiros);
   }
   *ppResult = malloc(size);
   LIS_InserirElementoApos(pLista, *ppResult);
}


void MEM_Free(void *pointer)
{
   LIS_IrInicioLista(pLista);
   if (LIS_ProcurarValor(pLista, pointer) == LIS_CondRetOK)
   {
      LIS_ExcluirElemento(pLista);
   }
}

void MEM_LiberarTodaMemoriaAlocada()
{
   LIS_DestruirLista(pLista);
}

int CompararPonteiros(void *pointer1, void *pointer2)
{
   if (pointer1 == pointer2)
   {
      return 0;
   }

   return -1;
}