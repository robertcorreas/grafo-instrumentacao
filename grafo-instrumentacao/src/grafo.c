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
*     2.0     hg & rc  03/dez/13   Adaptação para uma estrutura auto-verificável.
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

#ifdef _DEBUG
   struct stGrafo *pGrafo;
   /* Ponteiro para o cabeça cujo vertice pertence */
#endif

} tpVertice;

/***********************************************************************
*  Tipo de dados: GRA Aresta do grafo
***********************************************************************/

typedef struct stArestaGrafo {

	char *nome;
	/* Nome da aresta */

	tpVertice * pVertice;
	/* Ponteiro para o vértice destino da aresta */

#ifdef _DEBUG
   struct stGrafo *pGrafo;
   /* Ponteiro para o cabeça cuja aresta pertence */
#endif

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

#ifdef _DEBUG
   unsigned int qntVertices;
   /* Quantidade de vértices que foram adicionados através da interface
      exportada */

   unsigned long totalEspacoAlocado;
   /* Tamanho em bytes de todo espaço alocado para o armazenamento do grafo */
#endif

} tpGrafo;

/***** Dados encapsulados no módulo ******/
#ifdef _DEBUG

   static char EspacoLixo[256] =
            "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
         /* Espaço de dados lixo usado ao testar */
   
   static unsigned long espacoAlocado;
#endif

/***** Protótipos das funções encapuladas no módulo *****/

#ifdef _DEBUG
   // deturpacao
   static void DET_EliminaElementoCorrente(tpGrafo *pGrafo);
   static void DET_AtribuiNullParaVerticeSucessor(tpGrafo *pGrafo);
   static void DET_AtribuiNullAOPonteiroDoVerticePredecessor(tpGrafo *pGrafo);
   static void DET_LixoNaReferenciaParaSucessor(tpGrafo *pGrafo);

   //Verificacao
   static GRA_tpCondRet VER_VerticeSucessorNaoEhNulo(tpGrafo *pGrafo, int *numFalhas);
   static GRA_tpCondRet VER_VerticePredecessorNaoEhNulo(tpGrafo *pGrafo, int *numFalhas);
   static GRA_tpCondRet VER_NaoExisteLixoNaReferenciaParaSucessor(tpGrafo *pGrafo, int *numFalhas);
   static GRA_tpCondRet VER_NaoExisteLixoNaReferenciaParaAntecessor(tpGrafo *pGrafo, int *numFalhas);
   static GRA_tpCondRet VER_VerticesNaoPossuemConteudoNulo(tpGrafo *pGrafo, int *numFalhas);
   static GRA_tpCondRet VER_NenhumVerticeTemConteudoComOTipoCorrompido(tpGrafo *pGrafo, int *numFalhas);
   static GRA_tpCondRet VER_NenhumVerticeEstaDestacado(tpGrafo *pGrafo, int *numFalhas);
   static GRA_tpCondRet VER_NenhumVerticeFoiLiberado(tpGrafo *pGrafo, int *numFalhas);
   static GRA_tpCondRet VER_CorrenteNaoEhNulo(tpGrafo *pGrafo, int *numFalhas);
   static GRA_tpCondRet VER_NaoExisteOrigemNula(tpGrafo *pGrafo, int *numFalhas);

   static GRA_tpCondRet VerificaVertice(tpVertice *pVertice, int *numFalhas);

   static void DET_LixoNaReferenciaParaAntecessor(tpGrafo *pGrafo);
   static void DET_ConteudoDoVerticeNULL(tpGrafo *pGrafo);
   static void DET_AlteraTipoDoValorDoVertice(tpGrafo *pGrafo);
   static void DET_DestacaVertice(tpGrafo *pGrafo);
   static void DET_AtribuiNullAoCorrente(tpGrafo *pGrafo);
   static void DET_AtribuiNullParaUmaOrigem(tpGrafo *pGrafo);

   static void DET_EspalharLixosPelaEstrutura(tpGrafo *pGrafo);
   static void DET_GrafoEhEspacoLixo(tpGrafo *pGrafo);
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
static void DestacarVertice(tpGrafo *pGrafo, tpVertice *pAlvo);

/*****  Código das funções exportadas pelo módulo  *****/

/***************************************************************************
*  Função: GRA Criar grafo
******/
GRA_tpCondRet GRA_CriarGrafo(GRA_tppGrafo *ppGrafo,
	void (*destruirValor)(void *pValor))
{
	tpGrafo *pGrafo;
   pGrafo = (tpGrafo*) malloc(sizeof(tpGrafo));

	pGrafo->pCorrente = NULL;
	pGrafo->destruirValor = destruirValor;

	LIS_CriarLista(&pGrafo->pOrigens, NULL, CompararVerticeENome);
	LIS_CriarLista(&pGrafo->pVertices, DestruirVertice, CompararVerticeENome);

   #ifdef _DEBUG
      CED_DefinirTipoEspaco(pGrafo, GRA_TipoEspacoCabeca);
      pGrafo->qntVertices = 0;
      
      pGrafo->totalEspacoAlocado = _msize(pGrafo);
      
      LIS_TotalEspacoAlocado(pGrafo->pOrigens, &espacoAlocado);
      pGrafo->totalEspacoAlocado += espacoAlocado;
      
      LIS_TotalEspacoAlocado(pGrafo->pVertices, &espacoAlocado);
      pGrafo->totalEspacoAlocado += espacoAlocado;
      
      CED_MarcarEspacoAtivo(pGrafo);
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

	free(pGrafo);
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

	pVertice = (tpVertice*) malloc(sizeof(tpVertice));
	if (pVertice == NULL)
	{
		return GRA_CondRetFaltouMemoria;
	}

	pVertice->nome = nomeVertice;
	pVertice->pValor = pValor;
	pVertice->destruirValor = pGrafo->destruirValor;
   pVertice->pGrafo = pGrafo;

	LIS_CriarLista(&pVertice->pAntecessores, NULL, CompararVerticeENome);
	LIS_CriarLista(&pVertice->pSucessores, DestruirAresta, CompararArestaENome);

   #ifdef _DEBUG
      LIS_TotalEspacoAlocado(pGrafo->pVertices, &espacoAlocado);
      pGrafo->totalEspacoAlocado -= espacoAlocado;
   #endif

	pGrafo->pCorrente = pVertice;
	LIS_InserirElementoApos(pGrafo->pVertices, pVertice);
   
   #ifdef _DEBUG
      CED_DefinirTipoEspaco(pVertice, GRA_TipoEspacoVertice);
      CED_DefinirTipoEspaco(pVertice->pValor, GRA_TipoEspacoValorVertice);
      pGrafo->qntVertices++;
      CED_MarcarEspacoAtivo(pVertice);
      CED_MarcarEspacoAtivo(pVertice->pValor);

      pGrafo->totalEspacoAlocado += _msize(pVertice);

      LIS_TotalEspacoAlocado(pVertice->pAntecessores, &espacoAlocado);
      pGrafo->totalEspacoAlocado += espacoAlocado;

      LIS_TotalEspacoAlocado(pVertice->pSucessores, &espacoAlocado);
      pGrafo->totalEspacoAlocado += espacoAlocado;
      
      LIS_TotalEspacoAlocado(pGrafo->pVertices, &espacoAlocado);
      pGrafo->totalEspacoAlocado += espacoAlocado;
   #endif

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

	pAresta = (tpAresta*) malloc(sizeof(tpAresta));

   #ifdef _DEBUG
      CED_DefinirTipoEspaco(pAresta, GRA_TipoEspacoAresta);
   #endif

	if (pAresta == NULL)
	{
		return GRA_CondRetFaltouMemoria;
	}

   #ifdef _DEBUG
      LIS_TotalEspacoAlocado(pVerticeDestino->pAntecessores, &espacoAlocado);
      pGrafo->totalEspacoAlocado -= espacoAlocado;

      LIS_TotalEspacoAlocado(pVerticeOrigem->pSucessores, &espacoAlocado);
      pGrafo->totalEspacoAlocado -= espacoAlocado;
   #endif

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
   pAresta->pGrafo = pGrafo;

   #ifdef _DEBUG
      CED_DefinirTipoEspaco( pAresta , GRA_TipoEspacoAresta ) ;
      CED_MarcarEspacoAtivo(pAresta);

      pGrafo->totalEspacoAlocado += _msize(pAresta);
      
      LIS_TotalEspacoAlocado(pVerticeDestino->pAntecessores, &espacoAlocado);
      pGrafo->totalEspacoAlocado += espacoAlocado;

      LIS_TotalEspacoAlocado(pVerticeOrigem->pSucessores, &espacoAlocado);
      pGrafo->totalEspacoAlocado += espacoAlocado;
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

#ifdef _DEBUG
   LIS_TotalEspacoAlocado(pGrafo->pOrigens, &espacoAlocado);
   pGrafo->totalEspacoAlocado -= espacoAlocado;
#endif

	LIS_InserirElementoApos(pGrafo->pOrigens, pGrafo->pCorrente);

#ifdef _DEBUG
   LIS_TotalEspacoAlocado(pGrafo->pOrigens, &espacoAlocado);
   pGrafo->totalEspacoAlocado += espacoAlocado;
#endif

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

#ifdef _DEBUG
   LIS_TotalEspacoAlocado(pGrafo->pOrigens, &espacoAlocado);
   pGrafo->totalEspacoAlocado -= espacoAlocado;
#endif

   LIS_ExcluirElemento(pGrafo->pOrigens);

#ifdef _DEBUG
   LIS_TotalEspacoAlocado(pGrafo->pOrigens, &espacoAlocado);
   pGrafo->totalEspacoAlocado += espacoAlocado;
#endif

   return GRA_CondRetOK;
}

/***************************************************************************
*  Função: GRA Destruir vértice corrente
******/
GRA_tpCondRet GRA_DestruirVerticeCorrente(GRA_tppGrafo pGrafoParm)
{
	tpGrafo *pGrafo = NULL;
	int estaVazia = -1;

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

   DestacarVertice(pGrafo, pGrafo->pCorrente);
	
   #ifdef _DEBUG
      pGrafo->qntVertices--;
   #endif

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
   LIS_tppLista pAntecessores, pSucessores;

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

   pAntecessores = pAresta->pVertice->pAntecessores;
   pSucessores = pGrafo->pCorrente->pSucessores;

	// Remove referência do vértice corrente
	LIS_IrInicioLista(pAntecessores);
	LIS_ProcurarValor(pAntecessores, pGrafo->pCorrente->nome);

#ifdef _DEBUG
   LIS_TotalEspacoAlocado(pAntecessores, &espacoAlocado);
   pGrafo->totalEspacoAlocado -= espacoAlocado;
   
   LIS_TotalEspacoAlocado(pSucessores, &espacoAlocado);
   pGrafo->totalEspacoAlocado -= espacoAlocado;
#endif

	LIS_ExcluirElemento(pAntecessores);
	LIS_ExcluirElemento(pSucessores);
   
#ifdef _DEBUG
   LIS_TotalEspacoAlocado(pAntecessores, &espacoAlocado);
   pGrafo->totalEspacoAlocado += espacoAlocado;
   
   LIS_TotalEspacoAlocado(pSucessores, &espacoAlocado);
   pGrafo->totalEspacoAlocado += espacoAlocado;
#endif

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

GRA_tpCondRet VER_VerificaEspaçosDoGrafo(tpGrafo *pGrafo, int *numFalhas)
{
   int erroNaEstrutura = 0;
   int numElemVer = 0, numElemOri = 0;

   if(!CED_VerificarEspaco(pGrafo->pCorrente,NULL))
   {
      CNT_CONTAR("verEspacos corrente nao eh espaco valido");
      TST_NotificarFalha( "Controle do espaço acusou erro." ) ;
      (*numFalhas)++;
      return GRA_CondRetErroNaEstrutura;
   }
   else
   {
      CNT_CONTAR("verEspacos corrente eh espaco valido");
   }

   LIS_NumELementos(pGrafo->pVertices,&numElemVer);
   LIS_IrInicioLista(pGrafo->pVertices);

   while(numElemVer > 0)
   {
      tpVertice *pVertice = NULL;
      int numElemSuc = 0, numElemAnt = 0;

      CNT_CONTAR("verEspacos percorre vertices");

      LIS_ObterValor(pGrafo->pVertices,(void**)&pVertice);

      if(VerificaVertice(pVertice,numFalhas) == GRA_CondRetErroNaEstrutura)
      {
         CNT_CONTAR("verEspacos erro encontrado em vertice");
         erroNaEstrutura = 1;
      }
      else
      {
         CNT_CONTAR("verEspacos sem erros encontrados em vertice");
      }

      LIS_AvancarElementoCorrente(pGrafo->pVertices,1);
      numElemVer--;
   }
   CNT_CONTAR("verEspacos percorreu todos os vertices");

   LIS_NumELementos(pGrafo->pOrigens,&numElemOri);
   LIS_IrInicioLista(pGrafo->pOrigens);

   while(numElemOri > 0)
   {
      tpVertice *pVerticeOri = NULL;

      CNT_CONTAR("verEspacos percorre origens");

      LIS_ObterValor(pGrafo->pOrigens,(void**)&pVerticeOri);

      if(!CED_VerificarEspaco(pVerticeOri,NULL))
      {
         CNT_CONTAR("verEspacos erro encontrado em origem");
         TST_NotificarFalha( "Controle do espaço acusou erro no ponteiro vertice origem." ) ;
         (*numFalhas)++;
         erroNaEstrutura = 1;
      }
      else
      {
         CNT_CONTAR("verEspacos sem erros encontrados em origem");
      }
      LIS_AvancarElementoCorrente(pGrafo->pOrigens,1);
      numElemOri--;
   }
   CNT_CONTAR("verEspacos percorreu todas as origens");

   if (erroNaEstrutura)
   {
      CNT_CONTAR("verEspacos estrutura contem erros");
      return GRA_CondRetErroNaEstrutura;
   }
   else
   {
      CNT_CONTAR("verEspacos estrutura nao contem erros");
      return GRA_CondRetOK;
   }
}

// Checa os espaços do vertice, das arestas + vertices apontados por elas e os vertices antecessores
GRA_tpCondRet VerificaVertice(tpVertice *pVertice, int* numFalhas)
{
   int erroNaEstrutura = 0;
   int numElemSuc = 0, numElemAnt = 0;
   if(!CED_VerificarEspaco(pVertice,NULL))
   {
      CNT_CONTAR("verEspacos vertice nao eh valido");
      TST_NotificarFalha( "Controle do espaço acusou erro no vertice da lista de vertices." ) ;
      (*numFalhas)++;
      return GRA_CondRetErroNaEstrutura;
   }
   else
   {
      CNT_CONTAR("verEspacos vertice eh valido");
   }

   if(!CED_VerificarEspaco(pVertice->pValor,NULL))
   {
      CNT_CONTAR("verEspacos valor vertice nao eh valido");
      TST_NotificarFalha( "Controle do espaço acusou erro no valor do vertice." ) ;
      (*numFalhas)++;
      erroNaEstrutura = 1;
   }
   else
   {
      CNT_CONTAR("verEspacos valor vertice eh valido");
   }

   // Checa os espaços das arestas e vertices apontados por ela

   LIS_NumELementos(pVertice->pSucessores, &numElemSuc);
   LIS_IrInicioLista(pVertice->pSucessores);

   while(numElemSuc > 0)
   {
      tpAresta *pAresta = NULL;
    
      CNT_CONTAR("verEspacos percorrer sucessores");
      
      LIS_ObterValor(pVertice->pSucessores,(void**)&pAresta);

      if(!CED_VerificarEspaco(pAresta,NULL))
      {
         CNT_CONTAR("verEspacos aresta nao eh valida");
         TST_NotificarFalha( "Controle do espaço acusou erro na aresta." ) ;
         (*numFalhas)++;
         erroNaEstrutura = 1;
      }
      else
      {
         CNT_CONTAR("verEspacos aresta eh valida");
         
         if(!CED_VerificarEspaco(pAresta->pVertice,NULL))
         {
            CNT_CONTAR("verEspacos sucessor nao eh valido");
            TST_NotificarFalha( "Controle do espaço acusou erro no ponteiro vertice da aresta." ) ;
            (*numFalhas)++;
            erroNaEstrutura = 1;
         }
         else
         {
            CNT_CONTAR("verEspacos sucessor eh valido");
         }
      }
      LIS_AvancarElementoCorrente(pVertice->pSucessores,1);
      numElemSuc--;
   }
   CNT_CONTAR("verEspacos percorreu todos os sucessores");

   // Checa os espaços dos vertices antecessores

   LIS_NumELementos(pVertice->pAntecessores, &numElemAnt);
   LIS_IrInicioLista(pVertice->pAntecessores);

   while(numElemAnt > 0)
   {
      tpVertice *pVerticeAnt = NULL;
      
      CNT_CONTAR("verEspacos percorre antecessores");

      LIS_ObterValor(pVertice->pAntecessores,(void**)&pVerticeAnt);
      if(!CED_VerificarEspaco(pVerticeAnt,NULL))
      {
         CNT_CONTAR("verEspacos antecessor nao eh valido");
         TST_NotificarFalha( "Controle do espaço acusou erro no ponteiro vertice antecessor." ) ;
         (*numFalhas)++;
         erroNaEstrutura = 1;
      }
      else
      {
         CNT_CONTAR("verEspacos antecessor eh valido");
      }
      LIS_AvancarElementoCorrente(pVertice->pAntecessores,1);
      numElemAnt--;
   }
   CNT_CONTAR("verEspacos percorreu todos os antecessores");

   if (erroNaEstrutura)
   {
      CNT_CONTAR("verEspacos vertice contem erros");
      return GRA_CondRetErroNaEstrutura;
   }
   else
   {
      CNT_CONTAR("verEspacos vertice nao contem erros");
      return GRA_CondRetOK;
   }
}

/***************************************************************************
*
*  Função: GRA  Deturpar grafo
*  ****/

GRA_tpCondRet GRA_Deturpar(void *pGrafoParm, GRA_tpModosDeturpacao ModoDeturpar)
{

   tpGrafo *pGrafo = NULL ;

   if ( pGrafoParm == NULL )
   {
      return GRA_CondRetOK;
   } /* if */

   pGrafo = (tpGrafo*) pGrafoParm;

   switch ( ModoDeturpar ) {
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
   case GRA_EspalhaLixoPelaEstrutura:
      {
         DET_EspalharLixosPelaEstrutura(pGrafo);
         break;
      }
   case GRA_GrafoEhEspacoLixo:
      {
         DET_GrafoEhEspacoLixo(pGrafo);
         break;
      }
   }

   return GRA_CondRetOK;
}

/***************************************************************************
*
*  Função: GRA  Verificar grafo
*  ****/

GRA_tpCondRet GRA_Verificar(void *pGrafoParm, int *numFalhas)
{
   int erroNaEstrutura = 0;
   tpGrafo *pGrafo = NULL;
   GRA_tpCondRet ret;
   if (CED_ObterTipoEspaco(pGrafoParm) != GRA_TipoEspacoCabeca ||
      !CED_VerificarEspaco(pGrafo,NULL))
   {
      CNT_CONTAR("ver00 grafo nao existe");
      TST_NotificarFalha("Tentou verificar um grafo que não existe.");
      (*numFalhas)++;
      return GRA_CondRetErroNaEstrutura;
   }
   CNT_CONTAR("ver00 grafo existe");

   pGrafo = (tpGrafo*) pGrafoParm;

    ret = VER_VerificaEspaçosDoGrafo(pGrafo, numFalhas);
    if (ret == GRA_CondRetErroNaEstrutura)
    {
       CNT_CONTAR("ver00 foi encontrado algum espaco invalido");
       erroNaEstrutura = 1;
    }
    CNT_CONTAR("ver00 nao foi encontrado algum espaco invalido");

   ret = VER_NenhumVerticeFoiLiberado(pGrafo, numFalhas);
   if (ret == GRA_CondRetErroNaEstrutura)
   {
      CNT_CONTAR("ver00 existe vertice liberado");
      erroNaEstrutura = 1;
   }
   CNT_CONTAR("ver00 nao existe vertice liberado");

    ret = VER_VerticeSucessorNaoEhNulo(pGrafo, numFalhas);
    if (ret == GRA_CondRetErroNaEstrutura)
    {
       CNT_CONTAR("ver00 existe vertice cujo sucessor eh nulo");
       erroNaEstrutura = 1;
    }
    CNT_CONTAR("ver00 nao existe vertice cujo sucessor eh nulo");

    ret = VER_VerticePredecessorNaoEhNulo(pGrafo, numFalhas);
    if (ret == GRA_CondRetErroNaEstrutura)
    {
       erroNaEstrutura = 1;
    }

   ret = VER_NaoExisteLixoNaReferenciaParaSucessor(pGrafo, numFalhas);
   if (ret == GRA_CondRetErroNaEstrutura)
   {
      CNT_CONTAR("ver00 existe vertice que tem lixo como referencia para sucessor");
      erroNaEstrutura = 1;
   }
   CNT_CONTAR("ver00 nao existe vertice que tem lixo como referencia para sucessor");

   ret = VER_NaoExisteLixoNaReferenciaParaAntecessor(pGrafo, numFalhas);
   if (ret == GRA_CondRetErroNaEstrutura)
   {
      CNT_CONTAR("ver00 existe vertice que tem lixo como referencia para antecessor");
      erroNaEstrutura = 1;
   }
   CNT_CONTAR("ver00 nao existe vertice que tem lixo como referencia para antecessor");

   ret = VER_VerticesNaoPossuemConteudoNulo(pGrafo, numFalhas);
   if (ret == GRA_CondRetErroNaEstrutura)
   {
      CNT_CONTAR("ver00 existe um vertice com conteudo nulo");
      erroNaEstrutura = 1;
   }
   CNT_CONTAR("ver00 nao existe um vertice com conteudo nulo");
   
   ret = VER_NenhumVerticeTemConteudoComOTipoCorrompido(pGrafo, numFalhas);
   if (ret == GRA_CondRetErroNaEstrutura)
   {
      erroNaEstrutura = 1;
      CNT_CONTAR("ver00 existe vertice com conteudo de tipo errado");
   }
   CNT_CONTAR("ver00 nao existe vertice com conteudo de tipo errado");

    ret = VER_NenhumVerticeEstaDestacado(pGrafo, numFalhas);
    if (ret == GRA_CondRetErroNaEstrutura)
    {
      CNT_CONTAR("ver00 existe vertice destacado");
       erroNaEstrutura = 1;
    }
    CNT_CONTAR("ver00 nao existe vertice destacado");
   
   ret = VER_CorrenteNaoEhNulo(pGrafo, numFalhas);
   if (ret == GRA_CondRetErroNaEstrutura)
   {
      erroNaEstrutura = 1;
      CNT_CONTAR("ver00 corrente eh nulo");
   }
   CNT_CONTAR("ver00 corrente nao eh nulo");
   
   ret = VER_NaoExisteOrigemNula(pGrafo, numFalhas);
   if (ret == GRA_CondRetErroNaEstrutura)
   {
      CNT_CONTAR("ver00 existe origem nula");
      erroNaEstrutura = 1;
   }
   CNT_CONTAR("ver00 nao existe origem nula");

   if (erroNaEstrutura)
   {
      CNT_CONTAR("ver00 com erros na estrutura");
      return GRA_CondRetErroNaEstrutura;
   }
   CNT_CONTAR("ver00 sem erros na estrutura");

   return GRA_CondRetOK;
}



/***************************************************************************
*
*  Função: GRA  Total espaço alocado
*  ****/
GRA_tpCondRet GRA_TotalEspacoAlocado(GRA_tppGrafo pGrafoParm, unsigned long *pTotalEspaco)
{
   tpGrafo *pGrafo = (tpGrafo*) pGrafoParm;
   *pTotalEspaco = pGrafo->totalEspacoAlocado;
   return GRA_CondRetOK;
}

#endif

/*****  Código das funções encapsuladas no módulo  *****/

#ifdef _DEBUG

/************************************************************************/
/* Funcoes deturpacao                                                   */
/************************************************************************/

static void DET_EliminaElementoCorrente(tpGrafo *pGrafo)
{
  free(pGrafo->pCorrente);
}

static void DET_AtribuiNullParaVerticeSucessor(tpGrafo *pGrafo)
{
   tpAresta *pAresta = NULL;
   LIS_ObterValor(pGrafo->pCorrente->pSucessores,(void**)&pAresta);
   pAresta->pVertice = NULL;
}

static void DET_AtribuiNullAOPonteiroDoVerticePredecessor(tpGrafo *pGrafo)
{
   LIS_AlterarValor(pGrafo->pCorrente->pAntecessores,NULL);
}

static void DET_LixoNaReferenciaParaSucessor(tpGrafo *pGrafo)
{
   tpAresta *pAresta;

   LIS_ObterValor(pGrafo->pCorrente->pSucessores, (void **) &pAresta);

   pAresta->pVertice = (tpVertice*)(EspacoLixo);
}

/************************************************************************/
/*Funcao de verificacao                                                 */
/************************************************************************/

GRA_tpCondRet VER_NenhumVerticeFoiLiberado(tpGrafo *pGrafo, int *numFalhas)
{
   int numVerElem = 0;
   int erroNaEstrutura = 0;

   LIS_NumELementos(pGrafo->pVertices, &numVerElem);
   LIS_IrInicioLista(pGrafo->pVertices);

   while(numVerElem > 0)
   {
      int numElem = 0;
      tpVertice *pVertice = NULL;

      CNT_CONTAR("ver01 percorrendo vertices");

      LIS_ObterValor(pGrafo->pVertices,(void**)&pVertice);

      if (CED_ObterTipoEspaco(pVertice) != GRA_TipoEspacoVertice)
      {
         CNT_CONTAR("ver01 vertice foi liberado");
         TST_NotificarFalha("Encontrado vertice que foi liberado.");
         erroNaEstrutura = 1;
         (*numFalhas)++;
      }
      else
      {
         CNT_CONTAR("ver01 vertice nao foi liberado");
      }

      LIS_AvancarElementoCorrente(pGrafo->pVertices,1);
      numVerElem--;
   }
   CNT_CONTAR("ver01 percorreu todos os vertices");

   if (erroNaEstrutura)
   {
      CNT_CONTAR("ver01 com erros na estrutura");
      return GRA_CondRetErroNaEstrutura;
   }
   else
   {
      CNT_CONTAR("ver01 sem erros na estrutura");
      return GRA_CondRetOK;
   }
}

GRA_tpCondRet VER_VerticeSucessorNaoEhNulo(tpGrafo *pGrafo, int *numFalhas)
{
   int numVerElem = 0;

   LIS_NumELementos(pGrafo->pVertices, &numVerElem);
   LIS_IrInicioLista(pGrafo->pVertices);

   while(numVerElem > 0)
   {
      int numElem = 0;
      tpVertice *pVertice = NULL;

      CNT_CONTAR("ver02 percorrendo vertices");

      LIS_ObterValor(pGrafo->pVertices,(void**)&pVertice);

      if (CED_ObterTipoEspaco(pVertice) == GRA_TipoEspacoVertice)
      {
         CNT_CONTAR("ver02 vertice eh valido");
         LIS_NumELementos(pVertice->pSucessores,&numElem);
         LIS_IrInicioLista(pVertice->pSucessores);

         while(numElem > 0)
         {
            tpAresta *pAresta = NULL;
         
            CNT_CONTAR("ver02 percorrendo sucessores");

            LIS_ObterValor(pVertice->pSucessores, (void**)&pAresta);

            if(CED_ObterTipoEspaco(pAresta) == GRA_TipoEspacoAresta &&
               pAresta->pVertice == NULL)
            {
               CNT_CONTAR("ver02 vertice destino da aresta eh nulo");
               TST_NotificarFalha("Encontrado vertice sucessor nulo");
               (*numFalhas)++;
               return GRA_CondRetErroNaEstrutura;
            }
            else
            {
               CNT_CONTAR("ver02 vertice destino da aresta nao eh nulo");
            }
            LIS_AvancarElementoCorrente(pVertice->pSucessores,1);
            numElem--;
         }
         CNT_CONTAR("ver02 percorreu todos os sucessores");
      }
      else
      {
         CNT_CONTAR("ver02 vertice nao eh valido");
      }

      LIS_AvancarElementoCorrente(pGrafo->pVertices,1);
      numVerElem--;
   }
   CNT_CONTAR("ver02 percorreu todos os vertices");

   return GRA_CondRetOK;
}

GRA_tpCondRet VER_VerticePredecessorNaoEhNulo(tpGrafo *pGrafo, int *numFalhas)
{
   int numVerElem = 0;
   LIS_NumELementos(pGrafo->pVertices, &numVerElem);
   LIS_IrInicioLista(pGrafo->pVertices);

   while(numVerElem > 0)
   {
      int numElem = 0;
      tpVertice *pVertice = NULL;

      CNT_CONTAR("ver03 percorre vertices");

      LIS_ObterValor(pGrafo->pVertices, (void**)&pVertice);
      
      if (CED_ObterTipoEspaco(pVertice) == GRA_TipoEspacoVertice)
      {
         CNT_CONTAR("ver03 vertice eh valido");
         LIS_NumELementos(pVertice->pAntecessores, &numElem);
         LIS_IrInicioLista(pVertice->pAntecessores);
         while(numElem > 0)
         {
            tpVertice *pVerticeAnt = NULL;

            CNT_CONTAR("ver03 percorre antecessores");

            LIS_ObterValor(pVertice->pAntecessores, (void**)&pVerticeAnt);

            if(pVerticeAnt == NULL)
            {
               CNT_CONTAR("ver03 antecessor eh nulo");
               TST_NotificarFalha("Encontrado vertice antecessor nulo");
               (*numFalhas)++;
               return GRA_CondRetErroNaEstrutura;
            }
            else
            {
               CNT_CONTAR("ver03 antecessor nao eh nulo");
            }

            LIS_AvancarElementoCorrente(pVertice->pAntecessores,1);
            numElem--;
         }
         CNT_CONTAR("ver03 percorreu todos os antecessores");
      }
      else
      {
         CNT_CONTAR("ver03 vertice nao eh valido");
      }

      LIS_AvancarElementoCorrente(pGrafo->pVertices,1);
      numVerElem--;
   }
   CNT_CONTAR("ver03 percorreu todos os vertices");

   return GRA_CondRetOK;
}

GRA_tpCondRet VER_NaoExisteLixoNaReferenciaParaSucessor(tpGrafo *pGrafo, int *numFalhas)
{
   int numVerElem = 0;
   LIS_NumELementos(pGrafo->pVertices,&numVerElem);
   LIS_IrInicioLista(pGrafo->pVertices);

   while(numVerElem > 0)
   {
      tpVertice *pVertice = NULL;
      int numElemSucces = 0;

      CNT_CONTAR("ver04 percorre os vertices");

      LIS_ObterValor(pGrafo->pVertices,(void**)&pVertice);

      if (CED_ObterTipoEspaco(pVertice) == GRA_TipoEspacoVertice)
      {
         CNT_CONTAR("ver04 vertice eh valido");
         LIS_NumELementos(pVertice->pSucessores,&numElemSucces);
         LIS_IrInicioLista(pVertice->pSucessores);

         while(numElemSucces > 0)
         {
            tpAresta *pAresta = NULL;
            int numElem = 0;

            CNT_CONTAR("ver04 percorre os sucessores");

            LIS_ObterValor(pVertice->pSucessores,(void**)&pAresta);
         
            if(CED_ObterTipoEspaco(pAresta) == GRA_TipoEspacoAresta &&
               !CED_VerificarEspaco(pAresta->pVertice,NULL))
            {
               CNT_CONTAR("ver04 vertice da aresta eh um espaco invalido");
               TST_NotificarFalha( "Controle do espaço acusou erro." ) ;
               (*numFalhas)++;
               return GRA_CondRetErroNaEstrutura;
            }
            CNT_CONTAR("ver04 vertice da aresta nao eh um espaco invalido");

            LIS_AvancarElementoCorrente(pVertice->pSucessores,1);
            numElemSucces--;
         }
         CNT_CONTAR("ver04 percorreu todos os sucessores");
      }
      else
      {
         CNT_CONTAR("ver04 vertice nao eh valido");
      }

      LIS_AvancarElementoCorrente(pGrafo->pVertices,1);
      numVerElem--;
   }
   CNT_CONTAR("ver04 percorreu todos os vertices");

   return GRA_CondRetOK;
}

GRA_tpCondRet VER_NaoExisteLixoNaReferenciaParaAntecessor(tpGrafo *pGrafo, int *numFalhas)
{
   int numVerElem = 0;
   LIS_NumELementos(pGrafo->pVertices,&numVerElem);
   LIS_IrInicioLista(pGrafo->pVertices);

   while(numVerElem > 0)
   {
      int numElemAnt = 0;
      tpVertice *pVertice = NULL;

      CNT_CONTAR("ver05 percorre os vertices");
      
      LIS_ObterValor(pGrafo->pVertices,(void**)&pVertice);

      if (CED_ObterTipoEspaco(pVertice) == GRA_TipoEspacoVertice)
      {
         CNT_CONTAR("ver05 vertice eh valido");
         LIS_NumELementos(pVertice->pAntecessores,&numElemAnt);
         LIS_IrInicioLista(pVertice->pAntecessores);

         while(numElemAnt > 0)
         {
            tpVertice *pVerticeAnt = NULL;
      
            CNT_CONTAR("ver05 percorre os antecessores");

            LIS_ObterValor(pVertice->pAntecessores,(void**)&pVerticeAnt);

            if(!CED_VerificarEspaco(pVerticeAnt,NULL))
            {
               CNT_CONTAR("ver05 espaco do antecessor nao eh valido");
               TST_NotificarFalha("Problema de tipo de espaço na referencia para vertice anterior");
               (*numFalhas)++;
               return GRA_CondRetErroNaEstrutura;
            }
            else
            {
               CNT_CONTAR("ver05 espaco do antecessor eh valido");
            }
            LIS_AvancarElementoCorrente(pVertice->pAntecessores,1);
            numElemAnt--;
         }
         CNT_CONTAR("ver05 percorreu todos os antecessores");
      }
      else
      {
         CNT_CONTAR("ver05 vertice nao eh valido");
      }

      LIS_AvancarElementoCorrente(pGrafo->pVertices,1);
      numVerElem--;
   }
   CNT_CONTAR("ver05 percorreu todos os vertices");

   return GRA_CondRetOK;
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

#ifdef _DEBUG
   LIS_TotalEspacoAlocado(pVertice->pAntecessores, &espacoAlocado);
   pVertice->pGrafo->totalEspacoAlocado -= espacoAlocado;
   
   LIS_TotalEspacoAlocado(pVertice->pSucessores, &espacoAlocado);
   pVertice->pGrafo->totalEspacoAlocado -= espacoAlocado;
#endif

	LIS_DestruirLista(pVertice->pAntecessores);
	LIS_DestruirLista(pVertice->pSucessores);
   
#ifdef _DEBUG
   pVertice->pGrafo->totalEspacoAlocado -= _msize(pVertice);

   LIS_TotalEspacoAlocado(pVertice->pAntecessores, &espacoAlocado);
   pVertice->pGrafo->totalEspacoAlocado += espacoAlocado;
   
   LIS_TotalEspacoAlocado(pVertice->pSucessores, &espacoAlocado);
   pVertice->pGrafo->totalEspacoAlocado += espacoAlocado;
#endif

	pVertice->destruirValor(pVertice->pValor);

	free(pVertice->nome);

	free(pVertice);
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
   
#ifdef _DEBUG
   pAresta->pGrafo->totalEspacoAlocado -= _msize(pAresta);
#endif

	free(pAresta->nome);
	free(pAresta);
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



/***********************************************************************
*
*  Função: GRA Destacar Vértice
*
*  Descrição:
*    Função responsável por destacar o vértice do resto do grafo.
*    Operação realizada antes de excluir o vértice.
*
***********************************************************************/
void DestacarVertice(tpGrafo *pGrafo, tpVertice *pAlvo)
{
   
	tpVertice *pVertice = NULL, *pVerticeOrigem = NULL;
	tpAresta *pAresta = NULL;
   int numElemLista = 0;
   LIS_tppLista pAntecessores;
   
   // remove corrente vai para origem
	// Navega para o inicio da lista de origens
	LIS_IrInicioLista(pGrafo->pOrigens);
	//Pega o valor do primeiro vértice de origem
	LIS_ObterValor(pGrafo->pOrigens,(void**)&pVerticeOrigem);

   // Para cada item da lista de sucessores, remover elem que aponta para o corrente
	LIS_NumELementos(pAlvo->pSucessores, &numElemLista);
	LIS_IrInicioLista(pAlvo->pSucessores);

	while(numElemLista > 0)
	{
      int numElems;
      tpAresta *pAresta;
      LIS_ObterValor(pAlvo->pSucessores, (void**) &pAresta);
      
      pAntecessores = pAresta->pVertice->pAntecessores;

      #ifdef _DEBUG
         LIS_TotalEspacoAlocado(pAntecessores, &espacoAlocado);
         pGrafo->totalEspacoAlocado -= espacoAlocado;
      #endif

      LIS_NumELementos(pAntecessores, &numElems);
      LIS_IrInicioLista(pAntecessores);
      LIS_ProcurarValor(pAntecessores, pAlvo->nome);
      LIS_ExcluirElemento(pAntecessores);
      
      #ifdef _DEBUG
         LIS_TotalEspacoAlocado(pAntecessores, &espacoAlocado);
         pGrafo->totalEspacoAlocado += espacoAlocado;
      #endif

      LIS_AvancarElementoCorrente(pAlvo->pSucessores, 1);
      numElemLista--;
   }

	// Para cada item da lista de anteressores, remover aresta que aponta para o corrente
	LIS_NumELementos(pAlvo->pAntecessores,&numElemLista);
	LIS_IrInicioLista(pAlvo->pAntecessores);

	while(numElemLista > 0)
	{
      LIS_tppLista pSucessores;
		int nElem = 0;
		LIS_ObterValor(pAlvo->pAntecessores,(void**)&pVertice);

      pSucessores = pVertice->pSucessores;

		LIS_NumELementos(pSucessores, &nElem);
		LIS_IrInicioLista(pSucessores);
      
      #ifdef _DEBUG
         LIS_TotalEspacoAlocado(pSucessores, &espacoAlocado);
         pGrafo->totalEspacoAlocado -= espacoAlocado;
      #endif

		while(nElem > 0)
		{
			LIS_ObterValor(pSucessores,(void**)&pAresta);

			if(!strcmp(pAresta->pVertice->nome, pAlvo->nome))
			{
				LIS_ExcluirElemento(pSucessores);
				break;
			}

			LIS_AvancarElementoCorrente(pSucessores,1);
			nElem--;
		}
      
      #ifdef _DEBUG
         LIS_TotalEspacoAlocado(pSucessores, &espacoAlocado);
         pGrafo->totalEspacoAlocado += espacoAlocado;
      #endif

		LIS_AvancarElementoCorrente(pAlvo->pAntecessores,1);
		numElemLista--;
	}

   
   #ifdef _DEBUG
      LIS_TotalEspacoAlocado(pGrafo->pVertices, &espacoAlocado);
      pGrafo->totalEspacoAlocado -= espacoAlocado;
   #endif

	//Exclui elemento corrente
	LIS_IrInicioLista(pGrafo->pVertices);
	LIS_ProcurarValor(pGrafo->pVertices, pAlvo->nome);
	LIS_ExcluirElemento(pGrafo->pVertices);
   
   #ifdef _DEBUG
      LIS_TotalEspacoAlocado(pGrafo->pVertices, &espacoAlocado);
      pGrafo->totalEspacoAlocado += espacoAlocado;
   #endif

	pGrafo->pCorrente = pVerticeOrigem;
}

#ifdef _DEBUG
   // Det Grafo Lixo
   void DET_GrafoEhEspacoLixo(tpGrafo *pGrafo)
   {
      free(pGrafo);
   }

   // Det Espaços
   void DET_EspalharLixosPelaEstrutura(tpGrafo *pGrafo)
   {
      tpVertice *pVertice;
      LIS_AlterarValor(pGrafo->pOrigens, EspacoLixo);
      LIS_AlterarValor(pGrafo->pVertices, EspacoLixo);
      LIS_IrInicioLista(pGrafo->pVertices);
      
      LIS_AvancarElementoCorrente(pGrafo->pVertices, 1);
      LIS_ObterValor(pGrafo->pVertices, (void**) &pVertice);
      pVertice->pValor = EspacoLixo;
      
      LIS_AlterarValor(pVertice->pSucessores, EspacoLixo);
   }

   // Det 05
   void DET_LixoNaReferenciaParaAntecessor(tpGrafo *pGrafo)
   {
      LIS_AlterarValor(pGrafo->pCorrente->pAntecessores, EspacoLixo);
   }

   // Det 06
   void DET_ConteudoDoVerticeNULL(tpGrafo *pGrafo)
   {
      pGrafo->pCorrente->pValor = NULL;
   }

   // Ver 06
   GRA_tpCondRet VER_VerticesNaoPossuemConteudoNulo(tpGrafo *pGrafo, int *numFalhas)
   {
      int numElem = 0;
      LIS_NumELementos(pGrafo->pVertices,&numElem);
      LIS_IrInicioLista(pGrafo->pVertices);

      while(numElem > 0)
      {
         tpVertice *pVertice = NULL;

         CNT_CONTAR("ver06 percorre vertices");

         LIS_ObterValor(pGrafo->pVertices,(void**)&pVertice);

         if(pVertice->pValor == NULL)
         {
            CNT_CONTAR("ver06 valor do vertice eh null");
            TST_NotificarFalha("Encontrado valor NULL para conteudo do vertice");
            (*numFalhas)++;
            return GRA_CondRetErroNaEstrutura;
         }
         else
         {
            CNT_CONTAR("ver06 valor do vertice nao eh null");
         }

         LIS_AvancarElementoCorrente(pGrafo->pVertices,1);
         numElem--;
      }

      CNT_CONTAR("ver06 percorreu todos os vertices");

      return GRA_CondRetOK;
   }

   // Det 07
   void DET_AlteraTipoDoValorDoVertice(tpGrafo *pGrafo)
   {
      CED_DefinirTipoEspaco(pGrafo->pCorrente->pValor, CED_ID_TIPO_VALOR_NULO);
   }

   // Ver 07
   GRA_tpCondRet VER_NenhumVerticeTemConteudoComOTipoCorrompido(tpGrafo *pGrafo, int *numFalhas)
   {
      int numElem = 0;
      LIS_NumELementos(pGrafo->pVertices,&numElem);
      LIS_IrInicioLista(pGrafo->pVertices);

      while(numElem > 0)
      {
         tpVertice *pVertice;
         CED_tpIdTipoEspaco tipoValor;
         CNT_CONTAR("ver07 percorre vertices");
         LIS_ObterValor(pGrafo->pVertices,(void**)&pVertice);

         if (CED_ObterTipoEspaco(pVertice) == GRA_TipoEspacoVertice)
         {
            CNT_CONTAR("ver07 vertice eh valido");
            tipoValor = (CED_tpIdTipoEspaco) CED_ObterTipoEspaco(pVertice->pValor);
            if (tipoValor != GRA_TipoEspacoValorVertice)
            {
               CNT_CONTAR("ver07 tipo do valor esta correto");
               TST_NotificarFalha("Encontrado vértice cujo valor está com o tipo errado.");
               (*numFalhas)++;
               return GRA_CondRetErroNaEstrutura;
            }
            else
            {
               CNT_CONTAR("ver07 tipo do valor nao esta correto");
            }
         }
         else
         {
            CNT_CONTAR("ver07 vertice nao eh valido");
         }
         LIS_AvancarElementoCorrente(pGrafo->pVertices,1);
         numElem--;
      }

      CNT_CONTAR("ver07 percorreu todos os vertices");
      return GRA_CondRetOK;
   }

   // Det 08
   void DET_DestacaVertice(tpGrafo *pGrafo)
   {
      DestacarVertice(pGrafo, pGrafo->pCorrente);
   }
   
   // Ver 08
   GRA_tpCondRet VER_NenhumVerticeEstaDestacado(tpGrafo *pGrafo, int *numFalhas)
   {
      int tamListaVertices;

      LIS_NumELementos(pGrafo->pVertices, &tamListaVertices);

      if (pGrafo->qntVertices != tamListaVertices)
      {
         CNT_CONTAR("ver08 tamanho da lista de vertices diferente da quantidade de vertices");
         TST_NotificarFalha("Existem vertices destacados na estrutura.");
         (*numFalhas)++;
         return GRA_CondRetErroNaEstrutura;
      }
      
      CNT_CONTAR("ver08 tamanho da lista de vertices igual ah quantidade de vertices");

      return GRA_CondRetOK;
   }

   // Det 09
   void DET_AtribuiNullAoCorrente(tpGrafo *pGrafo)
   {
      pGrafo->pCorrente = NULL;
   }
   
   // Ver 09
   GRA_tpCondRet VER_CorrenteNaoEhNulo(tpGrafo *pGrafo, int *numFalhas)
   {
      if (pGrafo->pCorrente == NULL)
      {
         CNT_CONTAR("ver09 se corrente nulo");
         TST_NotificarFalha("Corrente não existe.");
         (*numFalhas)++;
         return GRA_CondRetErroNaEstrutura;
      }
      
      CNT_CONTAR("ver09 se corrente nao-nulo");

      return GRA_CondRetOK;
   }

   // Det 10
   void DET_AtribuiNullParaUmaOrigem(tpGrafo *pGrafo)
   {
      int *pOrigem;
      int numOrigens;
      LIS_NumELementos(pGrafo->pOrigens, &numOrigens);
      if (numOrigens == 0)
      {
         return;
      }

      LIS_IrFinalLista(pGrafo->pOrigens);
      LIS_ObterValor(pGrafo->pOrigens, (void**) &pOrigem);
      *pOrigem = NULL;
   }

   // Ver 10
   GRA_tpCondRet VER_NaoExisteOrigemNula(tpGrafo *pGrafo, int *numFalhas)
   {
      LIS_tpCondRet lisCondRet = LIS_CondRetOK;
      int estaVazia;
      LIS_EstaVazia(pGrafo->pOrigens, &estaVazia);
      if (estaVazia)
      {
         CNT_CONTAR("ver10 se origens vazia");
         return GRA_CondRetOK;
      }
      else
      {
         CNT_CONTAR("ver10 se origens nao vazia");
      }
      
      LIS_IrInicioLista(pGrafo->pOrigens);
      while (lisCondRet == LIS_CondRetOK)
      {
         int *pValor;
         CNT_CONTAR("ver10 percorre origens");
         LIS_ObterValor(pGrafo->pOrigens, (void**) &pValor);

         if (*pValor == NULL)
         {
            CNT_CONTAR("ver10 valor origem eh nula");
            TST_NotificarFalha("Encontrada origem nula.");
            (*numFalhas)++;
            return GRA_CondRetErroNaEstrutura;
         }
         else
         {
            CNT_CONTAR("ver10 valor origem nao eh nula");
         }

         lisCondRet = LIS_AvancarElementoCorrente(pGrafo->pOrigens, 1);
      }
      CNT_CONTAR("ver10 saiu do percorre origens");

      return GRA_CondRetOK;
   }
#endif

/********** Fim do módulo de implementação: GRA Grafo direcionado **********/