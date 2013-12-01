/***************************************************************************
*
*  Módulo de implementação: GRA  Grafo direcionado
*
*  Arquivo gerado:              grafo.c
*  Letras identificadoras:      GRA
*
*	Autores:
*     - rc: Robert Corrêa
*     - hg: Hugo Roque
*
*  Histórico de evolução:
*     Versão  Autor    Data        Observações
*     1.0     hg & rc  06/out/13   Preparação do módulo para trabalhar com grafos direcionados.
*
*  Assertivas Estruturais
*     Antecessor aponta de volta para o vértice como sucessor
*        Para todo vértice pVerAnt em pVer->pAntecessores existirá uma aresta pAreSuc em pVerAnt->pSucessores tal que pAreSuc->pVertice = pVer
*     
*     Sucessor aponta de volta para o vértice como antecessor
*        Para toda aresta pAreSuc em pVer->pSucessores existirá um vértice pVerAnt em pAreSuc->pVertice->pAntecessores tal que pVerAnt = pVer
*     
*     Se está na origem tem que estar na lista de vértices
*        Para todo vértice pVerOri em pGrafo->pOrigens existirá um vértice pVer em pGrafo->pVertices tal que pVerOri = pVer
*     
*     Se tem corrente ele tem q estar na lista de vértices
*        Se pGrafo->pCorrente != NULL => Existirá um vértice pVer em pGrafo->pVertices tal que pVer = pGrafo->pCorrente
*     
*     Todos os antecessores dos vértices de um grafo têm q estar na lista de vértices desse grafo  
*        Para todo vértice pVer e pVerAnt tal que pVer pertence à pGrafo->pVertices e pVerAnt pertence à pVer->Antecessores existirá um pVer2 pertencente à pGrafo->pVertices tal que pVerAnt = pVer2
*     
*     Todas as arestas dos vértices de um grafo G têm que apontar para um vértice que está na lista desse grafo
*        Para todo vértice pVer e aresta pAre tal que pVer pertence à pGrafo->pVertices e pAre pertence à pVer->pSucessores, existirá um pVer2 pertencente à pGrafo->pVertices tal que pVer2 = pAre->pVertice.
*
***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "lista.h"
#include "mem_manager.h"

#define GRAFO_OWN
#include "grafo.h"
#undef GRAFO_OWN

#ifdef _DEBUG
#include   "Generico.h"
#include   "Conta.h"
#include   "cespdin.h"
#include   "IdTiposEspaco.def"
#endif


/***********************************************************************
*  Tipo de dados: GRA Vertice do grafo
***********************************************************************/

typedef struct stVerticeGrafo {

	char *nome;
	/* Nome do vértice */

	LIS_tppLista pSucessores;
	/* Ponteiros para as arestas originadas neste vértice */

	LIS_tppLista pAntecessores;
	/* Ponteiro para os vértices que têm arestas 
	direcionadas para este vértice */

	void (*destruirValor)(void *pValor);
	/* Lógica responsável por destruir o valor do vértice do grafo */

	void * pValor;
	/* valor contido no vértice */

} tpVertice;

/***********************************************************************
*  Tipo de dados: GRA Aresta do grafo
***********************************************************************/

typedef struct stArestaGrafo {

	char *nome;
	/* Nome da aresta */

	tpVertice * pVertice;
	/* Ponteiro para o vértice destino da aresta */

} tpAresta;


/***********************************************************************
*  Tipo de dados: GRA Grafo
***********************************************************************/

typedef struct stGrafo {

	LIS_tppLista pOrigens;
	/* Vértices que são origens do grafo */

	LIS_tppLista pVertices;
	/* Todos os vértices do grafo */

	tpVertice * pCorrente;
	/* Vértice corrente */

	void (*destruirValor)(void *pValor);
	/* Lógica responsável por destruir o valor do vértice do grafo */

} tpGrafo;

/***** Dados encapsulados no módulo ******/
#ifdef _DEBUG

   static char EspacoLixo[256] =
            "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
         /* Espaço de dados lixo usado ao testar */
#endif

/***** Protótipos das funções encapuladas no módulo *****/

#ifdef _DEBUG
   static void DET_EliminaElementoCorrente(tpGrafo *pGrafo);
   static void DET_AtribuiNullParaVerticeSucessor(tpGrafo *pGrafo);
   static void DET_AtribuiNullAOPonteiroDoVerticePredecessor(tpGrafo *pGrafo);
   static void DET_LixoNaReferenciaParaSucessor(tpGrafo *pGrafo);

   static void DET_LixoNaReferenciaParaAntecessor(tpGrafo *pGrafo);
   static void DET_ConteudoDoVerticeNULL(tpGrafo *pGrafo);
   static void DET_AlteraTipoDoValorDoVertice(tpGrafo *pGrafo);
   static void DET_DestacaVertice(tpGrafo *pGrafo);
   static void DET_AtribuiNullAoCorrente(tpGrafo *pGrafo);
   static void DET_AtribuiNullParaUmaOrigem(tpGrafo *pGrafo);
#endif

static void DestruirVertice(void *pVazio);
static void DestruirAresta(void *pVazio);
static int CompararVerticeENome (void *pVazio1, void *pVazio2);
static int CompararArestaENome (void *pVazio1, void *pVazio2);
static int EstaVazio(tpGrafo *pGrafo);
static GRA_tpCondRet ProcurarVertice(tpGrafo *pGrafo, char *nome, tpVertice **pVertice);
static GRA_tpCondRet ProcurarOrigem(tpGrafo *pGrafo, char *nome, tpVertice **pVertice);
static GRA_tpCondRet ProcurarAresta(tpVertice *pVertice, char *nome, tpAresta **pAresta);
static int ExisteVertice(tpGrafo *pGrafo, char *nome);
static int ExisteOrigem(tpGrafo *pGrafo, char *nome);
static int ExisteAresta(tpVertice *pVertice, char *nome);

/*****  Código das funções exportadas pelo módulo  *****/

/***************************************************************************
*  Função: GRA Criar grafo
******/
GRA_tpCondRet GRA_CriarGrafo(GRA_tppGrafo *ppGrafo,
	void (*destruirValor)(void *pValor))
{
	tpGrafo *pGrafo;
   MEM_Alloc(sizeof(tpGrafo), (void **) &pGrafo);

	pGrafo->pCorrente = NULL;
	pGrafo->destruirValor = destruirValor;

	LIS_CriarLista(&pGrafo->pOrigens, NULL, CompararVerticeENome);
	LIS_CriarLista(&pGrafo->pVertices, DestruirVertice, CompararVerticeENome);

   #ifdef _DEBUG
      CED_DefinirTipoEspaco( pGrafo , GRA_TipoEspacoCabeca ) ;
   #endif

	*ppGrafo = (GRA_tppGrafo) pGrafo;

	return GRA_CondRetOK;
}

/***************************************************************************
*  Função: GRA Destruir grafo
******/
GRA_tpCondRet GRA_DestruirGrafo(GRA_tppGrafo *ppGrafo)
{
	tpGrafo *pGrafo = (tpGrafo*) *ppGrafo;

	if (pGrafo == NULL)
	{
		return GRA_CondRetOK;
	}

	LIS_DestruirLista(pGrafo->pVertices);
	LIS_DestruirLista(pGrafo->pOrigens);

	MEM_Free(pGrafo);
	pGrafo = NULL;
	*ppGrafo = NULL;
	
	return GRA_CondRetOK;
}

/***************************************************************************
*  Função: GRA Inserir vertice
******/
GRA_tpCondRet GRA_InserirVertice(GRA_tppGrafo pGrafoParm, char *nomeVertice, void *pValor)
{
	tpGrafo *pGrafo = (tpGrafo*) pGrafoParm;
	tpVertice *pVertice;

	if (pGrafo == NULL)
	{
		return GRA_CondRetGrafoNaoFoiCriado;
	}

   if (ExisteVertice(pGrafo, nomeVertice))
   {
      return GRA_CondRetJaExiste;
   }

	MEM_Alloc(sizeof(tpVertice), (void **) &pVertice);
	if (pVertice == NULL)
	{
		return GRA_CondRetFaltouMemoria;
	}

	pVertice->nome = nomeVertice;
	pVertice->pValor = pValor;
	pVertice->destruirValor = pGrafo->destruirValor;

	LIS_CriarLista(&pVertice->pAntecessores, NULL, CompararVerticeENome);
	LIS_CriarLista(&pVertice->pSucessores, DestruirAresta, CompararArestaENome);

   #ifdef _DEBUG
      CED_DefinirTipoEspaco( pVertice , GRA_TipoEspacoVertice ) ;
   #endif

	pGrafo->pCorrente = pVertice;
	LIS_InserirElementoApos(pGrafo->pVertices, pVertice);

	return GRA_CondRetOK;
}

/***************************************************************************
*  Função: GRA Inserir aresta
******/
GRA_tpCondRet GRA_InserirAresta(GRA_tppGrafo pGrafoParm,
	char *nomeAresta, char *nomeVerticeOrigem, char *nomeVerticeDestino)
{
	tpGrafo *pGrafo = (tpGrafo*) pGrafoParm;
	tpAresta *pAresta;
	LIS_tpCondRet lisCondRet;
   GRA_tpCondRet graCondRet;
   tpVertice *pVerticeOrigem, *pVerticeDestino;

	if (pGrafo == NULL)
	{
		return GRA_CondRetGrafoNaoFoiCriado;
	}

   // Procura vértice origem
   graCondRet = ProcurarVertice(pGrafo, nomeVerticeOrigem, &pVerticeOrigem);
   if (graCondRet != GRA_CondRetOK)
   {
      return graCondRet;
   }

   // Procura vértice destino
   graCondRet = ProcurarVertice(pGrafo, nomeVerticeDestino, &pVerticeDestino);
   if (graCondRet != GRA_CondRetOK)
   {
      return graCondRet;
   }

   // Verifica se já existe uma aresta com este nome
   if (ExisteAresta(pVerticeOrigem, nomeAresta))
   {
      return GRA_CondRetJaExiste;
   }

	MEM_Alloc(sizeof(tpAresta), (void **) &pAresta);
	if (pAresta == NULL)
	{
		return GRA_CondRetFaltouMemoria;
	}

   // Atualiza os antecessores do vértice destino
	lisCondRet = LIS_InserirElementoApos(pVerticeDestino->pAntecessores, pVerticeOrigem);
	if (lisCondRet == LIS_CondRetFaltouMemoria)
	{
		return GRA_CondRetFaltouMemoria;
	}

   // Atualiza os sucessores do vértice origem
	lisCondRet = LIS_InserirElementoApos(pVerticeOrigem->pSucessores, pAresta);
	if (lisCondRet == LIS_CondRetFaltouMemoria)
	{
		return GRA_CondRetFaltouMemoria;
	}

	pAresta->nome = nomeAresta;
   pAresta->pVertice = pVerticeDestino;

   #ifdef _DEBUG
      CED_DefinirTipoEspaco( pAresta , GRA_TipoEspacoCabeca ) ;
   #endif

	return GRA_CondRetOK;
}

/***************************************************************************
*  Função: GRA Obter valor do vértice corrente
******/
GRA_tpCondRet GRA_ObterValorCorrente(GRA_tppGrafo pGrafoParm, void **ppValor)
{
	tpGrafo *pGrafo = (tpGrafo*) pGrafoParm;

	if (pGrafo == NULL)
	{
		*ppValor = NULL;
		return GRA_CondRetGrafoNaoFoiCriado;
	}

	if (EstaVazio(pGrafo))
	{
		*ppValor = NULL;
		return GRA_CondRetGrafoVazio; 
	}

	*ppValor = pGrafo->pCorrente->pValor;
	return GRA_CondRetOK;
}

/***************************************************************************
*  Função: GRA Alterar valor do vértice corrente
******/
GRA_tpCondRet GRA_AlterarValorCorrente(GRA_tppGrafo pGrafoParm, void *pValor)
{
	tpGrafo *pGrafo = (tpGrafo*) pGrafoParm;

	if (pGrafo == NULL)
	{
		return GRA_CondRetGrafoNaoFoiCriado;
	}

	if (EstaVazio(pGrafo))
	{
		return GRA_CondRetGrafoVazio;
	}

	pGrafo->pCorrente->pValor = pValor;
	return GRA_CondRetOK;
}


/***************************************************************************
*  Função: GRA Tornar vértice corrente uma origem
******/
GRA_tpCondRet GRA_TornarCorrenteUmaOrigem(GRA_tppGrafo pGrafoParm)
{
	tpGrafo *pGrafo = (tpGrafo*) pGrafoParm;

	if (pGrafo == NULL)
	{
		return GRA_CondRetGrafoNaoFoiCriado;
	}

	if (EstaVazio(pGrafo))
	{
		return GRA_CondRetGrafoVazio;
	}

   if (ExisteOrigem(pGrafo, pGrafo->pCorrente->nome))
   {
      return GRA_CondRetJaExiste;
   }

	LIS_InserirElementoApos(pGrafo->pOrigens, pGrafo->pCorrente);
	return GRA_CondRetOK;
}


/***************************************************************************
*  Função: GRA Deixar de ser origem
******/
GRA_tpCondRet GRA_DeixarDeSerOrigem(GRA_tppGrafo pGrafoParm)
{
   tpGrafo *pGrafo = (tpGrafo*) pGrafoParm;

   if (pGrafo == NULL)
   {
      return GRA_CondRetGrafoNaoFoiCriado;
   }

   if (EstaVazio(pGrafo))
   {
      return GRA_CondRetGrafoVazio;
   }

   if (!ExisteOrigem(pGrafo, pGrafo->pCorrente->nome))
   {
      return GRA_CondRetNaoAchou;
   }

   LIS_ExcluirElemento(pGrafo->pOrigens);

   return GRA_CondRetOK;
}

/***************************************************************************
*  Função: GRA Destruir vértice corrente
******/
GRA_tpCondRet GRA_DestruirVerticeCorrente(GRA_tppGrafo pGrafoParm)
{
	tpGrafo *pGrafo = NULL;
	tpVertice  *pVerticeOrigem = NULL, *pVertice = NULL;
	tpAresta *pAresta = NULL;
	int estaVazia = -1, numElemLista = 0;

	if (pGrafoParm == NULL)
	{
		return GRA_CondRetGrafoNaoFoiCriado;
	}

	pGrafo = (tpGrafo*) pGrafoParm;

	if (EstaVazio(pGrafo))
	{
		return GRA_CondRetGrafoVazio;
	}

	LIS_EstaVazia(pGrafo->pOrigens,&estaVazia);

	if (estaVazia)
	{
		return GRA_CondRetNaoAchou;
	}

	// remove corrente vai para origem
	// Navega para o inicio da lista de origens
	LIS_IrInicioLista(pGrafo->pOrigens);
	//Pega o valor do primeiro vértice de origem
	LIS_ObterValor(pGrafo->pOrigens,(void**)&pVerticeOrigem);

	//remove arestas do vertice corrente
	LIS_EsvaziarLista(pGrafo->pCorrente->pSucessores);

	// Para cada item da lista de anteressores, remover aresta que aponta para o corrente
	LIS_NumELementos(pGrafo->pCorrente->pAntecessores,&numElemLista);
	LIS_IrInicioLista(pGrafo->pCorrente->pAntecessores);

	while(numElemLista > 0)
	{
		int nElem = 0;
		LIS_ObterValor(pGrafo->pCorrente->pAntecessores,(void**)&pVertice);

		LIS_NumELementos(pVertice->pSucessores, &nElem);
		LIS_IrInicioLista(pVertice->pSucessores);

		while(nElem > 0)
		{
			LIS_ObterValor(pVertice->pSucessores,(void**)&pAresta);

			if(!strcmp(pAresta->pVertice->nome, pGrafo->pCorrente->nome))
			{
				LIS_ExcluirElemento(pVertice->pSucessores);
				break;
			}

			LIS_AvancarElementoCorrente(pVertice->pSucessores,1);
			nElem--;
		}
		LIS_AvancarElementoCorrente(pGrafo->pCorrente->pAntecessores,1);
		numElemLista--;
	}

	//Exclui elemento corrente
	LIS_IrInicioLista(pGrafo->pVertices);
	LIS_ProcurarValor(pGrafo->pVertices,pGrafo->pCorrente->nome);
	LIS_ExcluirElemento(pGrafo->pVertices);

	pGrafo->pCorrente = pVerticeOrigem;
	

	return GRA_CondRetOK;
}


/***************************************************************************
*  Função: GRA Destruir aresta adjacente
******/
GRA_tpCondRet GRA_DestruirArestaAdjacente(GRA_tppGrafo pGrafoParm, char *nomeAresta)
{
	tpGrafo *pGrafo = NULL;
	tpAresta *pAresta = NULL;
	GRA_tpCondRet graCondRet;

	if (pGrafoParm == NULL)
	{
		return GRA_CondRetGrafoNaoFoiCriado;
	}

	pGrafo = (tpGrafo*) pGrafoParm;

	if (EstaVazio(pGrafo))
	{
		return GRA_CondRetGrafoVazio;
	}

	// Busca Aresta à remover
   graCondRet = ProcurarAresta(pGrafo->pCorrente, nomeAresta, &pAresta);
   if (graCondRet != GRA_CondRetOK)
   {
      return GRA_CondRetNaoAchou;
   }

	// Remove referência do vértice corrente
	LIS_IrInicioLista(pAresta->pVertice->pAntecessores);
	LIS_ProcurarValor(pAresta->pVertice->pAntecessores,pGrafo->pCorrente->nome);

	LIS_ExcluirElemento(pAresta->pVertice->pAntecessores);
	LIS_ExcluirElemento(pGrafo->pCorrente->pSucessores);

	return GRA_CondRetOK;
}

/***************************************************************************
*  Função: GRA Ir para o vértice adjacente
******/
GRA_tpCondRet GRA_IrParaVerticeAdjacente(GRA_tppGrafo pGrafoParm, char *nomeVertice)
{
	tpGrafo *pGrafo = NULL;
	tpAresta *pAresta = NULL;
	tpVertice *pVertice = NULL;
	int numElemLista = 0;

	if (pGrafoParm == NULL)
	{
		return GRA_CondRetGrafoNaoFoiCriado;
	}

	pGrafo = (tpGrafo*) pGrafoParm;
	if (EstaVazio(pGrafo))
	{
		return GRA_CondRetGrafoVazio;
	}

	LIS_NumELementos(pGrafo->pCorrente->pSucessores, &numElemLista);
	LIS_IrInicioLista(pGrafo->pCorrente->pSucessores);

	while(numElemLista > 0)
	{
		LIS_ObterValor(pGrafo->pCorrente->pSucessores, (void**)&pAresta);

		if (!strcmp(nomeVertice, pAresta->pVertice->nome))
		{
			pGrafo->pCorrente = pAresta->pVertice;
			return GRA_CondRetOK;
		}

		LIS_AvancarElementoCorrente(pGrafo->pCorrente->pSucessores,1);

		numElemLista--;
	}

	return GRA_CondRetNaoAchou;
}

/***************************************************************************
*  Função: GRA Seguir pela aresta
******/
GRA_tpCondRet GRA_SeguirPelaAresta(GRA_tppGrafo pGrafoParm, char *nomeAresta)
{
	tpGrafo *pGrafo = NULL;
	tpAresta *pAresta = NULL;
   GRA_tpCondRet graCondRet;

	if (pGrafoParm == NULL)
	{
		return GRA_CondRetGrafoNaoFoiCriado;
	}

	pGrafo = (tpGrafo*) pGrafoParm;

	if (EstaVazio(pGrafo))
	{
		return GRA_CondRetGrafoVazio;
	}

   graCondRet = ProcurarAresta(pGrafo->pCorrente, nomeAresta, &pAresta);
   if (graCondRet != GRA_CondRetOK)
   {
      return GRA_CondRetNaoAchou;
   }

	pGrafo->pCorrente = pAresta->pVertice;

	return GRA_CondRetOK;
}

/***************************************************************************
*  Função: GRA Ir para a origem
******/
GRA_tpCondRet GRA_IrParaAOrigem(GRA_tppGrafo pGrafoParm, char *nomeVertice)
{
	tpGrafo *pGrafo = NULL;
   GRA_tpCondRet graCondRet;
	tpVertice *pVertice;
	if (pGrafoParm == NULL)
	{
		return GRA_CondRetGrafoNaoFoiCriado;
	}

	pGrafo = (tpGrafo*) pGrafoParm;

	if (EstaVazio(pGrafo))
	{
		return GRA_CondRetGrafoVazio;
	}

   graCondRet = ProcurarOrigem(pGrafo, nomeVertice, &pVertice);
   if (graCondRet != GRA_CondRetOK)
   {
      return GRA_CondRetNaoAchou;
   }

	pGrafo->pCorrente = pVertice;

	return GRA_CondRetOK;
}

#ifdef _DEBUG
/***************************************************************************
*
*  Função: ARV  &Deturpar grafo
*  ****/

void GRA_Deturpar(void *pGrafoParm, GRA_tpModosDeturpacao ModoDeturpar)
{

   tpGrafo *pGrafo = NULL ;

   if ( pGrafoParm == NULL )
   {
      return ;
   } /* if */

   pGrafo = (tpGrafo*) pGrafoParm;

   switch ( ModoDeturpar ) {

      /* Elimina o elemento corrente da lista de vértices */

   case DeturpaEliminaElementoCorrente :
      {
         DET_EliminaElementoCorrente(pGrafo);
         break;
      } 

   case GRA_DeturpaAtribuiNullParaVerticeSucessor:
      {
         DET_AtribuiNullParaVerticeSucessor(pGrafo);
         break;
      }

   case GRA_AtribuiNullAOPonteiroDoVerticePredecessor:
      {
         DET_AtribuiNullAOPonteiroDoVerticePredecessor(pGrafo);
         break;
      }

   case GRA_LixoNaReferenciaParaSucessor:
      {
         DET_LixoNaReferenciaParaSucessor(pGrafo);
         break;
      }
   
   case GRA_LixoNaReferenciaParaAntecessor:
      {
         DET_LixoNaReferenciaParaAntecessor(pGrafo);
         break;
      }

   case GRA_ConteudoDoVerticeNULL:
      {
         DET_ConteudoDoVerticeNULL(pGrafo);
         break;
      }

   case GRA_AlteraTipoDoValorDoVertice:
      {
         DET_AlteraTipoDoValorDoVertice(pGrafo);
         break;
      }

   case GRA_DestacaVertice:
      {
         DET_DestacaVertice(pGrafo);
         break;
      }

   case GRA_AtribuiNullAoCorrente:
      {
         DET_AtribuiNullAoCorrente(pGrafo);
         break;
      }

   case GRA_AtribuiNullParaUmaOrigem:
      {
         DET_AtribuiNullParaUmaOrigem(pGrafo);
         break;
      }
   }
} 

#endif

/*****  Código das funções encapsuladas no módulo  *****/

#ifdef _DEBUG

static void DET_EliminaElementoCorrente(tpGrafo *pGrafo)
{
   tpVertice* pVertice = NULL;
   LIS_ObterValor(pGrafo->pVertices, (void**) pVertice);
   free(pVertice);
}

static void DET_AtribuiNullParaVerticeSucessor(tpGrafo *pGrafo)
{
   tpAresta *pAresta = NULL;
   LIS_ObterValor(pGrafo->pCorrente->pSucessores,(void**)pAresta);
   pAresta->pVertice = NULL;
}

static void DET_AtribuiNullAOPonteiroDoVerticePredecessor(tpGrafo *pGrafo)
{
   tpVertice *pVertice = NULL;
   LIS_IrInicioLista(pGrafo->pCorrente->pAntecessores);
   LIS_ObterValor(pGrafo->pCorrente->pAntecessores, (void**)pVertice);
   pVertice = NULL;
   
}

static void DET_LixoNaReferenciaParaSucessor(tpGrafo *pGrafo)
{
   tpAresta *pAresta;

   LIS_ObterValor(pGrafo->pCorrente->pSucessores, (void **) &pAresta);

   *(char**) pAresta = EspacoLixo;
}


#endif

/***********************************************************************
*
*  Função: GRA Destruir vértice
*
*  Descrição:
*    Função responsável por destruir vértices. Passada como ponteiro de funcao para listas de vértices.
*
***********************************************************************/
void DestruirVertice(void *pVazio)
{
	tpVertice *pVertice = (tpVertice*) pVazio;

	LIS_DestruirLista(pVertice->pAntecessores);
	LIS_DestruirLista(pVertice->pSucessores);

	pVertice->destruirValor(pVertice->pValor);

	MEM_Free(pVertice->nome);

	MEM_Free(pVertice);

	
}

/***********************************************************************
*
*  Função: GRA Destruir aresta
*
*  Descrição:
*    Função responsável por destruir arestas. Passada como ponteiro de funcao para listas de sucessores do vértice.
*
***********************************************************************/
void DestruirAresta(void *pVazio)
{
	tpAresta *pAresta = (tpAresta*) pVazio;

	MEM_Free(pAresta->nome);
	MEM_Free(pAresta);
}

/***********************************************************************
*
*  Função: GRA Comparar vértice e nome
*
*  Descrição:
*    Função responsável por comparar vértices pelo nome. Usado para fazer a busca na lista de vertices.
*
***********************************************************************/
int CompararVerticeENome( void *pVazio1, void *pVazio2 )
{
	tpVertice *pVertice1 = (tpVertice*) pVazio1;
	char *nomeBuscado = (char*) pVazio2;

	return strcmp(pVertice1->nome, nomeBuscado);
}

/***********************************************************************
*
*  Função: GRA Comparar aresta e nome
*
*  Descrição:
*    Função responsável por comparar arestas pelo nome. Usado para fazer a busca na lista de arestas.
*
***********************************************************************/
int CompararArestaENome(void *pVazio1, void *pVazio2)
{
	tpAresta *pAresta1 = (tpAresta*) pVazio1;
	char *nomeBuscado = (char*) pVazio2;

	return strcmp(pAresta1->nome, nomeBuscado);
}

/***********************************************************************
*
*  Função: GRA Está vazio
*
*  Descrição:
*    Função responsável por verificar se o grafo está vazio.
*
***********************************************************************/
int EstaVazio(tpGrafo *pGrafo)
{
   return pGrafo->pCorrente == NULL;
}


/***********************************************************************
*
*  Função: GRA Procurar Vértice
*
*  Descrição:
*    Função responsável por realizar uma pesquisa na lista de vértices.
*
***********************************************************************/
GRA_tpCondRet ProcurarVertice(tpGrafo *pGrafo, char *nome, tpVertice **pVertice)
{
   LIS_tpCondRet lisCondRet;
   *pVertice = NULL;

   if (nome == NULL)
   {
      return GRA_CondRetNaoAchou;
   }

	LIS_IrInicioLista(pGrafo->pVertices);
	lisCondRet = LIS_ProcurarValor(pGrafo->pVertices, nome);
	if (lisCondRet != LIS_CondRetOK)
   {
      return GRA_CondRetNaoAchou;
   }
   else
   {
		void *pVazio;
		LIS_ObterValor(pGrafo->pVertices, &pVazio);
      *pVertice = (tpVertice*) pVazio;
      return GRA_CondRetOK;
	}
}


/***********************************************************************
*
*  Função: GRA Procurar Origem
*
*  Descrição:
*    Função responsável por realizar uma pesquisa na lista de origens.
*
***********************************************************************/
GRA_tpCondRet ProcurarOrigem(tpGrafo *pGrafo, char *nome, tpVertice **pVertice)
{
   LIS_tpCondRet lisCondRet;
   *pVertice = NULL;

   if (nome == NULL)
   {
      return GRA_CondRetNaoAchou;
   }

	LIS_IrInicioLista(pGrafo->pOrigens);
	lisCondRet = LIS_ProcurarValor(pGrafo->pOrigens, nome);
	if (lisCondRet != LIS_CondRetOK)
   {
      return GRA_CondRetNaoAchou;
   }
   else
   {
		void *pVazio;
		LIS_ObterValor(pGrafo->pOrigens, &pVazio);
      *pVertice = (tpVertice*) pVazio;
      return GRA_CondRetOK;
	}
}


/***********************************************************************
*
*  Função: GRA Procurar Aresta
*
*  Descrição:
*    Função responsável por realizar uma pesquisa na lista de arestas partindo
*    de um dado vértice.
*
***********************************************************************/
GRA_tpCondRet ProcurarAresta(tpVertice *pVertice, char *nome, tpAresta **pAresta)
{
   LIS_tpCondRet lisCondRet;
   *pAresta = NULL;

   if (nome == NULL)
   {
      return GRA_CondRetNaoAchou;
   }

	LIS_IrInicioLista(pVertice->pSucessores);
	lisCondRet = LIS_ProcurarValor(pVertice->pSucessores, nome);
	if (lisCondRet != LIS_CondRetOK)
   {
      return GRA_CondRetNaoAchou;
   }
   else
   {
		void *pVazio;
		LIS_ObterValor(pVertice->pSucessores, &pVazio);
      *pAresta = (tpAresta*) pVazio;
      return GRA_CondRetOK;
	}
}


/***********************************************************************
*
*  Função: GRA Existe Aresta
*
*  Descrição:
*    Função responsável por verificar se existe uma aresta com o nome
*    procurado partindo de um dado vértice.
*
***********************************************************************/
int ExisteAresta(tpVertice *pVertice, char *nome)
{
   tpAresta*pAresta;
   GRA_tpCondRet condRet;

   condRet = ProcurarAresta(pVertice, nome, &pAresta);

   return condRet == GRA_CondRetOK;
}


/***********************************************************************
*
*  Função: GRA Existe Vértice
*
*  Descrição:
*    Função responsável por verificar se existe um vértice com o nome
*    procurado.
*
***********************************************************************/
int ExisteVertice(tpGrafo *pGrafo, char *nome)
{
   tpVertice *pVertice;
   GRA_tpCondRet condRet;

   condRet = ProcurarVertice(pGrafo, nome, &pVertice);

   return condRet == GRA_CondRetOK;
}


/***********************************************************************
*
*  Função: GRA Existe Vértice
*
*  Descrição:
*    Função responsável por verificar se existe uma origem com o nome
*    procurado.
*
***********************************************************************/
int ExisteOrigem(tpGrafo *pGrafo, char *nome)
{
   tpVertice *pVertice;
   GRA_tpCondRet condRet;

   condRet = ProcurarOrigem(pGrafo, nome, &pVertice);

   return condRet == GRA_CondRetOK;
}

#ifdef _DEBUG
   // Det 05
   void DET_LixoNaReferenciaParaAntecessor(tpGrafo *pGrafo)
   {
      tpVertice *pVertice;

      LIS_ObterValor(pGrafo->pCorrente->pAntecessores, (void **) &pVertice);

      *(char**) pVertice = EspacoLixo;
   }

   // Det 06
   void DET_ConteudoDoVerticeNULL(tpGrafo *pGrafo)
   {
      *(int*) pGrafo->pCorrente->pValor = NULL;
   }

   // Det 07
   void DET_AlteraTipoDoValorDoVertice(tpGrafo *pGrafo)
   {
      CED_DefinirTipoEspaco(pGrafo->pCorrente->pValor, CED_ID_TIPO_VALOR_NULO);
   }

   // Det 08
   void DET_DestacaVertice(tpGrafo *pGrafo)
   {
      LIS_tppLista pSuc, pAnt;
      tpVertice *pCorr;
      LIS_tpCondRet lisCondRet;
      int estaVazia;

      pCorr = pGrafo->pCorrente;

      // Garantindo que não será antecessor de ninguém
      pSuc = pGrafo->pCorrente->pSucessores;
      LIS_EstaVazia(pSuc, &estaVazia);
      if (!estaVazia)
      {
         LIS_IrInicioLista(pSuc);
         lisCondRet = LIS_CondRetOK;
         while (lisCondRet == LIS_CondRetOK)
         {
            tpAresta *pAresta;
            LIS_tppLista pBackAnt;

            LIS_ObterValor(pSuc, (void**) &pAresta);
            pBackAnt = pAresta->pVertice->pAntecessores;
            LIS_IrInicioLista(pBackAnt);
            LIS_ProcurarValor(pBackAnt, pCorr->nome);
            LIS_ExcluirElemento(pBackAnt);
            lisCondRet = LIS_AvancarElementoCorrente(pSuc, 1);
         }
      }
      
      // Garantindo que não é sucessor de ninguém.
      pAnt = pGrafo->pCorrente->pAntecessores;
      LIS_EstaVazia(pAnt, &estaVazia);
      if (!estaVazia)
      {
         LIS_IrInicioLista(pAnt);
         lisCondRet = LIS_CondRetOK;
         while (lisCondRet == LIS_CondRetOK)
         {
            LIS_tpCondRet retBackSuc = LIS_CondRetOK;
            tpVertice *pVertice;
            LIS_tppLista pBackSuc;

            LIS_ObterValor(pAnt, (void**) &pVertice);
            pBackSuc = pVertice->pSucessores;
            LIS_IrInicioLista(pBackSuc);
            while(retBackSuc == LIS_CondRetOK)
            {
               tpAresta *pAresta;

               LIS_ObterValor(pBackSuc, (void**) &pAresta);
               if (strcmp(pVertice->nome, pCorr->nome) == 0)
               {
                  break;
               }

               retBackSuc = LIS_AvancarElementoCorrente(pBackSuc, 1);
            }
            LIS_ExcluirElemento(pBackSuc);
            
            lisCondRet = LIS_AvancarElementoCorrente(pAnt, 1);
         }
      }
   }

   // Det 09
   void DET_AtribuiNullAoCorrente(tpGrafo *pGrafo)
   {
      pGrafo->pCorrente = NULL;
   }

   // Det 10
   void DET_AtribuiNullParaUmaOrigem(tpGrafo *pGrafo)
   {
      int *pOrigem;

      LIS_IrFinalLista(pGrafo->pOrigens);
      LIS_ObterValor(pGrafo->pOrigens, (void**) &pOrigem);
      *pOrigem = NULL;
   }
#endif

/********** Fim do módulo de implementação: GRA Grafo direcionado **********/