/***************************************************************************
*
*  M�dulo de implementa��o: GRA  Grafo direcionado
*
*  Arquivo gerado:              grafo.c
*  Letras identificadoras:      GRA
*
*	Autores:
*     - rc: Robert Corr�a
*     - hg: Hugo Roque
*
*  Hist�rico de evolu��o:
*     Vers�o  Autor    Data        Observa��es
*     1.0     hg & rc  06/out/13   Prepara��o do m�dulo para trabalhar com grafos direcionados.
*
*  Assertivas Estruturais
*     Antecessor aponta de volta para o v�rtice como sucessor
*        Para todo v�rtice pVerAnt em pVer->pAntecessores existir� uma aresta pAreSuc em pVerAnt->pSucessores tal que pAreSuc->pVertice = pVer
*     
*     Sucessor aponta de volta para o v�rtice como antecessor
*        Para toda aresta pAreSuc em pVer->pSucessores existir� um v�rtice pVerAnt em pAreSuc->pVertice->pAntecessores tal que pVerAnt = pVer
*     
*     Se est� na origem tem que estar na lista de v�rtices
*        Para todo v�rtice pVerOri em pGrafo->pOrigens existir� um v�rtice pVer em pGrafo->pVertices tal que pVerOri = pVer
*     
*     Se tem corrente ele tem q estar na lista de v�rtices
*        Se pGrafo->pCorrente != NULL => Existir� um v�rtice pVer em pGrafo->pVertices tal que pVer = pGrafo->pCorrente
*     
*     Todos os antecessores dos v�rtices de um grafo t�m q estar na lista de v�rtices desse grafo  
*        Para todo v�rtice pVer e pVerAnt tal que pVer pertence � pGrafo->pVertices e pVerAnt pertence � pVer->Antecessores existir� um pVer2 pertencente � pGrafo->pVertices tal que pVerAnt = pVer2
*     
*     Todas as arestas dos v�rtices de um grafo G t�m que apontar para um v�rtice que est� na lista desse grafo
*        Para todo v�rtice pVer e aresta pAre tal que pVer pertence � pGrafo->pVertices e pAre pertence � pVer->pSucessores, existir� um pVer2 pertencente � pGrafo->pVertices tal que pVer2 = pAre->pVertice.
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
	/* Nome do v�rtice */

	LIS_tppLista pSucessores;
	/* Ponteiros para as arestas originadas neste v�rtice */

	LIS_tppLista pAntecessores;
	/* Ponteiro para os v�rtices que t�m arestas 
	direcionadas para este v�rtice */

	void (*destruirValor)(void *pValor);
	/* L�gica respons�vel por destruir o valor do v�rtice do grafo */

	void * pValor;
	/* valor contido no v�rtice */

} tpVertice;

/***********************************************************************
*  Tipo de dados: GRA Aresta do grafo
***********************************************************************/

typedef struct stArestaGrafo {

	char *nome;
	/* Nome da aresta */

	tpVertice * pVertice;
	/* Ponteiro para o v�rtice destino da aresta */

} tpAresta;


/***********************************************************************
*  Tipo de dados: GRA Grafo
***********************************************************************/

typedef struct stGrafo {

	LIS_tppLista pOrigens;
	/* V�rtices que s�o origens do grafo */

	LIS_tppLista pVertices;
	/* Todos os v�rtices do grafo */

	tpVertice * pCorrente;
	/* V�rtice corrente */

	void (*destruirValor)(void *pValor);
	/* L�gica respons�vel por destruir o valor do v�rtice do grafo */

#ifdef _DEBUG
   unsigned int qntVertices;
#endif

} tpGrafo;

/***** Dados encapsulados no m�dulo ******/
#ifdef _DEBUG

   static char EspacoLixo[256] =
            "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
         /* Espa�o de dados lixo usado ao testar */
#endif

/***** Prot�tipos das fun��es encapuladas no m�dulo *****/

#ifdef _DEBUG
   // deturpacao
   static void DET_EliminaElementoCorrente(tpGrafo *pGrafo);
   static void DET_AtribuiNullParaVerticeSucessor(tpGrafo *pGrafo);
   static void DET_AtribuiNullAOPonteiroDoVerticePredecessor(tpGrafo *pGrafo);
   static void DET_LixoNaReferenciaParaSucessor(tpGrafo *pGrafo);

   //Verificacao
   static GRA_tpCondRet VER_VerticeSucessorNaoEhNulo(tpGrafo *pGrafo);
   static GRA_tpCondRet VER_VerticePredecessorNaoEhNulo(tpGrafo *pGrafo);
   static GRA_tpCondRet VER_NaoExisteLixoNaReferenciaParaSucessor(tpGrafo *pGrafo);
   static GRA_tpCondRet VER_NaoExisteLixoNaReferenciaParaAntecessor(tpGrafo *pGrafo);

   static GRA_tpCondRet VER_VerticesNaoPossuemConteudoNulo(tpGrafo *pGrafo);
   static GRA_tpCondRet VER_NenhumVerticeTemConteudoComOTipoCorrompido(tpGrafo *pGrafo);
   static GRA_tpCondRet VER_NenhumVerticeEstaDestacado(tpGrafo *pGrafo);
   static GRA_tpCondRet VER_ReferenciasDoVerticeEstaoCorretas(tpVertice *pVerticeVerificado);
   static GRA_tpCondRet VER_CorrenteNaoEhNulo(tpGrafo *pGrafo);
   static GRA_tpCondRet VER_NaoExisteOrigemNula(tpGrafo *pGrafo);

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
static void DestacarVertice(tpGrafo *pGrafo, tpVertice *pAlvo);

/*****  C�digo das fun��es exportadas pelo m�dulo  *****/

/***************************************************************************
*  Fun��o: GRA Criar grafo
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
   #endif

	*ppGrafo = (GRA_tppGrafo) pGrafo;

	return GRA_CondRetOK;
}

/***************************************************************************
*  Fun��o: GRA Destruir grafo
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
*  Fun��o: GRA Inserir vertice
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

	LIS_CriarLista(&pVertice->pAntecessores, NULL, CompararVerticeENome);
	LIS_CriarLista(&pVertice->pSucessores, DestruirAresta, CompararArestaENome);

   #ifdef _DEBUG
      CED_DefinirTipoEspaco(pVertice, GRA_TipoEspacoVertice);
      CED_DefinirTipoEspaco(pVertice->pValor, GRA_TipoEspacoValorVertice);
      pGrafo->qntVertices++;
   #endif

	pGrafo->pCorrente = pVertice;
	LIS_InserirElementoApos(pGrafo->pVertices, pVertice);

	return GRA_CondRetOK;
}

/***************************************************************************
*  Fun��o: GRA Inserir aresta
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

   // Procura v�rtice origem
   graCondRet = ProcurarVertice(pGrafo, nomeVerticeOrigem, &pVerticeOrigem);
   if (graCondRet != GRA_CondRetOK)
   {
      return graCondRet;
   }

   // Procura v�rtice destino
   graCondRet = ProcurarVertice(pGrafo, nomeVerticeDestino, &pVerticeDestino);
   if (graCondRet != GRA_CondRetOK)
   {
      return graCondRet;
   }

   // Verifica se j� existe uma aresta com este nome
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

   // Atualiza os antecessores do v�rtice destino
	lisCondRet = LIS_InserirElementoApos(pVerticeDestino->pAntecessores, pVerticeOrigem);
	if (lisCondRet == LIS_CondRetFaltouMemoria)
	{
		return GRA_CondRetFaltouMemoria;
	}

   // Atualiza os sucessores do v�rtice origem
	lisCondRet = LIS_InserirElementoApos(pVerticeOrigem->pSucessores, pAresta);
	if (lisCondRet == LIS_CondRetFaltouMemoria)
	{
		return GRA_CondRetFaltouMemoria;
	}

	pAresta->nome = nomeAresta;
   pAresta->pVertice = pVerticeDestino;

   #ifdef _DEBUG
      CED_DefinirTipoEspaco( pAresta , GRA_TipoEspacoAresta ) ;
   #endif

	return GRA_CondRetOK;
}

/***************************************************************************
*  Fun��o: GRA Obter valor do v�rtice corrente
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
*  Fun��o: GRA Alterar valor do v�rtice corrente
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
*  Fun��o: GRA Tornar v�rtice corrente uma origem
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
*  Fun��o: GRA Deixar de ser origem
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
*  Fun��o: GRA Destruir v�rtice corrente
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
*  Fun��o: GRA Destruir aresta adjacente
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

	// Busca Aresta � remover
   graCondRet = ProcurarAresta(pGrafo->pCorrente, nomeAresta, &pAresta);
   if (graCondRet != GRA_CondRetOK)
   {
      return GRA_CondRetNaoAchou;
   }

	// Remove refer�ncia do v�rtice corrente
	LIS_IrInicioLista(pAresta->pVertice->pAntecessores);
	LIS_ProcurarValor(pAresta->pVertice->pAntecessores,pGrafo->pCorrente->nome);

	LIS_ExcluirElemento(pAresta->pVertice->pAntecessores);
	LIS_ExcluirElemento(pGrafo->pCorrente->pSucessores);

	return GRA_CondRetOK;
}

/***************************************************************************
*  Fun��o: GRA Ir para o v�rtice adjacente
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
*  Fun��o: GRA Seguir pela aresta
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
*  Fun��o: GRA Ir para a origem
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
*  Fun��o: GRA  Deturpar grafo
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

/***************************************************************************
*
*  Fun��o: GRA  Verificar grafo
*  ****/

GRA_tpCondRet GRA_Verificar(void *pGrafoParm)
{
   int erroNaEstrutura = 0;
   tpGrafo *pGrafo = NULL;
   GRA_tpCondRet ret;

   if (pGrafoParm == NULL)
   {
      CNT_CONTAR("ver00 grafo nao existe");
      TST_NotificarFalha("Tentou verificar um grafo que n�o existe.");
      return GRA_CondRetErroNaEstrutura;
   }
   CNT_CONTAR("ver00 grafo existe");

   pGrafo = (tpGrafo*) pGrafoParm;

   ret = VER_NenhumVerticeFoiLiberado(pGrafo);
   if (ret == GRA_CondRetErroNaEstrutura)
   {
      CNT_CONTAR("ver00 existe vertice liberado");
      erroNaEstrutura = 1;
   }
   CNT_CONTAR("ver00 nao existe vertice liberado");


    ret = VER_VerticeSucessorNaoEhNulo(pGrafo);
    if (ret == GRA_CondRetErroNaEstrutura)
    {
       CNT_CONTAR("ver00 existe vertice cujo sucessor eh nulo");
       erroNaEstrutura = 1;
    }
    CNT_CONTAR("ver00 nao existe vertice cujo sucessor eh nulo");

    ret = VER_VerticePredecessorNaoEhNulo(pGrafo);
    if (ret == GRA_CondRetErroNaEstrutura)
    {
       erroNaEstrutura = 1;
    }

   ret = VER_NaoExisteLixoNaReferenciaParaSucessor(pGrafo);
   if (ret == GRA_CondRetErroNaEstrutura)
   {
      CNT_CONTAR("ver00 existe vertice que tem lixo como referencia para sucessor");
      erroNaEstrutura = 1;
   }
   CNT_CONTAR("ver00 nao existe vertice que tem lixo como referencia para sucessor");

   ret = VER_NaoExisteLixoNaReferenciaParaAntecessor(pGrafo);
   if (ret == GRA_CondRetErroNaEstrutura)
   {
      CNT_CONTAR("ver00 existe vertice que tem lixo como referencia para antecessor");
      erroNaEstrutura = 1;
   }
   CNT_CONTAR("ver00 nao existe vertice que tem lixo como referencia para antecessor");

   ret = VER_VerticesNaoPossuemConteudoNulo(pGrafo);
   if (ret == GRA_CondRetErroNaEstrutura)
   {
      CNT_CONTAR("ver00 existe um vertice com conteudo nulo");
      erroNaEstrutura = 1;
   }
   CNT_CONTAR("ver00 nao existe um vertice com conteudo nulo");
   
   ret = VER_NenhumVerticeTemConteudoComOTipoCorrompido(pGrafo);
   if (ret == GRA_CondRetErroNaEstrutura)
   {
      erroNaEstrutura = 1;
      CNT_CONTAR("ver00 existe vertice com conteudo de tipo errado");
   }
   CNT_CONTAR("ver00 nao existe vertice com conteudo de tipo errado");

    ret = VER_NenhumVerticeEstaDestacado(pGrafo);
    if (ret == GRA_CondRetErroNaEstrutura)
    {
      CNT_CONTAR("ver00 existe vertice destacado");
       erroNaEstrutura = 1;
    }
    CNT_CONTAR("ver00 nao existe vertice destacado");
   
   ret = VER_CorrenteNaoEhNulo(pGrafo);
   if (ret == GRA_CondRetErroNaEstrutura)
   {
      erroNaEstrutura = 1;
      CNT_CONTAR("ver00 corrente eh nulo");
   }
   CNT_CONTAR("ver00 corrente nao eh nulo");
   
   ret = VER_NaoExisteOrigemNula(pGrafo);
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

#endif

/*****  C�digo das fun��es encapsuladas no m�dulo  *****/

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

static GRA_tpCondRet VER_NenhumVerticeFoiLiberado(tpGrafo *pGrafo)
{
   int numVerElem = 0;
   int erroNaEstrutura = 0;

   LIS_NumELementos(pGrafo->pVertices, &numVerElem);
   LIS_IrInicioLista(pGrafo->pVertices);

   while(numVerElem > 0)
   {
      int numElem = 0;
      tpVertice *pVertice = NULL;

      LIS_ObterValor(pGrafo->pVertices,(void**)&pVertice);

      if (CED_ObterTipoEspaco(pVertice) != GRA_TipoEspacoVertice)
      {
         TST_NotificarFalha("Encontrado vertice que foi liberado.");
         erroNaEstrutura = 1;
      }

      LIS_AvancarElementoCorrente(pGrafo->pVertices,1);
      numVerElem--;
   }

   if (erroNaEstrutura)
   {
      return GRA_CondRetErroNaEstrutura;
   }
   else
   {
      return GRA_CondRetOK;
   }
}

static GRA_tpCondRet VER_VerticeSucessorNaoEhNulo(tpGrafo *pGrafo)
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
         LIS_NumELementos(pVertice->pSucessores,&numElem);
         LIS_IrInicioLista(pVertice->pSucessores);

         while(numElem > 0)
         {
            tpAresta *pAresta = NULL;
         
            CNT_CONTAR("ver02 percorrendo sucessores");

            LIS_ObterValor(pVertice->pSucessores, (void**)&pAresta);
            if(pAresta->pVertice == NULL)
            {
               CNT_CONTAR("ver02 vertice destino da aresta eh nulo");
               TST_NotificarFalha("Encontrado vertice sucessor nulo");
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

      LIS_AvancarElementoCorrente(pGrafo->pVertices,1);
      numVerElem--;
   }
   CNT_CONTAR("ver02 percorreu todos os vertices");

   return GRA_CondRetOK;
}

static GRA_tpCondRet VER_VerticePredecessorNaoEhNulo(tpGrafo *pGrafo)
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

      LIS_AvancarElementoCorrente(pGrafo->pVertices,1);
      numVerElem--;
   }
   CNT_CONTAR("ver03 percorreu todos os vertices");

   return GRA_CondRetOK;
}

//TODO [RCS] checar se soluciona o problema de checar espa�oLixo
static GRA_tpCondRet VER_NaoExisteLixoNaReferenciaParaSucessor(tpGrafo *pGrafo)
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
         LIS_NumELementos(pVertice->pSucessores,&numElemSucces);
         LIS_IrInicioLista(pVertice->pSucessores);

         while(numElemSucces > 0)
         {
            tpAresta *pAresta = NULL;
            int numElem = 0;

            CNT_CONTAR("ver04 percorre os sucessores");

            LIS_ObterValor(pVertice->pSucessores,(void**)&pAresta);
         
            if(!CED_VerificarEspaco(pAresta->pVertice,NULL))
            {
               CNT_CONTAR("ver04 vertice da aresta eh um espaco invalido");
               TST_NotificarFalha( "Controle do espa�o acusou erro." ) ;
               return GRA_CondRetErroNaEstrutura;
            }
            CNT_CONTAR("ver04 vertice da aresta nao eh um espaco invalido");

            LIS_AvancarElementoCorrente(pVertice->pSucessores,1);
            numElemSucces--;
         }
         CNT_CONTAR("ver04 percorreu todos os sucessores");
      }

      LIS_AvancarElementoCorrente(pGrafo->pVertices,1);
      numVerElem--;
   }
   CNT_CONTAR("ver04 percorreu todos os vertices");
}

static GRA_tpCondRet VER_NaoExisteLixoNaReferenciaParaAntecessor(tpGrafo *pGrafo)
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
               TST_NotificarFalha("Problema de tipo de espa�o na referencia para vertice anterior");
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

      LIS_AvancarElementoCorrente(pGrafo->pVertices,1);
      numVerElem--;
   }
   CNT_CONTAR("ver05 percorreu todos os vertices");

   return GRA_CondRetOK;
}

#endif

/***********************************************************************
*
*  Fun��o: GRA Destruir v�rtice
*
*  Descri��o:
*    Fun��o respons�vel por destruir v�rtices. Passada como ponteiro de funcao para listas de v�rtices.
*
***********************************************************************/
void DestruirVertice(void *pVazio)
{
	tpVertice *pVertice = (tpVertice*) pVazio;

	LIS_DestruirLista(pVertice->pAntecessores);
	LIS_DestruirLista(pVertice->pSucessores);

	pVertice->destruirValor(pVertice->pValor);

	free(pVertice->nome);

	free(pVertice);

	
}

/***********************************************************************
*
*  Fun��o: GRA Destruir aresta
*
*  Descri��o:
*    Fun��o respons�vel por destruir arestas. Passada como ponteiro de funcao para listas de sucessores do v�rtice.
*
***********************************************************************/
void DestruirAresta(void *pVazio)
{
	tpAresta *pAresta = (tpAresta*) pVazio;

	free(pAresta->nome);
	free(pAresta);
}

/***********************************************************************
*
*  Fun��o: GRA Comparar v�rtice e nome
*
*  Descri��o:
*    Fun��o respons�vel por comparar v�rtices pelo nome. Usado para fazer a busca na lista de vertices.
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
*  Fun��o: GRA Comparar aresta e nome
*
*  Descri��o:
*    Fun��o respons�vel por comparar arestas pelo nome. Usado para fazer a busca na lista de arestas.
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
*  Fun��o: GRA Est� vazio
*
*  Descri��o:
*    Fun��o respons�vel por verificar se o grafo est� vazio.
*
***********************************************************************/
int EstaVazio(tpGrafo *pGrafo)
{
   return pGrafo->pCorrente == NULL;
}


/***********************************************************************
*
*  Fun��o: GRA Procurar V�rtice
*
*  Descri��o:
*    Fun��o respons�vel por realizar uma pesquisa na lista de v�rtices.
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
*  Fun��o: GRA Procurar Origem
*
*  Descri��o:
*    Fun��o respons�vel por realizar uma pesquisa na lista de origens.
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
*  Fun��o: GRA Procurar Aresta
*
*  Descri��o:
*    Fun��o respons�vel por realizar uma pesquisa na lista de arestas partindo
*    de um dado v�rtice.
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
*  Fun��o: GRA Existe Aresta
*
*  Descri��o:
*    Fun��o respons�vel por verificar se existe uma aresta com o nome
*    procurado partindo de um dado v�rtice.
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
*  Fun��o: GRA Existe V�rtice
*
*  Descri��o:
*    Fun��o respons�vel por verificar se existe um v�rtice com o nome
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
*  Fun��o: GRA Existe V�rtice
*
*  Descri��o:
*    Fun��o respons�vel por verificar se existe uma origem com o nome
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



void DestacarVertice(tpGrafo *pGrafo, tpVertice *pAlvo)
{
   
	tpVertice *pVertice = NULL, *pVerticeOrigem = NULL;
	tpAresta *pAresta = NULL;
   int numElemLista = 0;
   
   // remove corrente vai para origem
	// Navega para o inicio da lista de origens
	LIS_IrInicioLista(pGrafo->pOrigens);
	//Pega o valor do primeiro v�rtice de origem
	LIS_ObterValor(pGrafo->pOrigens,(void**)&pVerticeOrigem);

   // Para cada item da lista de sucessores, remover elem que aponta para o corrente
	LIS_NumELementos(pAlvo->pSucessores, &numElemLista);
	LIS_IrInicioLista(pAlvo->pSucessores);

	while(numElemLista > 0)
	{
      int numElems;
      tpAresta *pAresta;
      LIS_ObterValor(pAlvo->pSucessores, (void**) &pAresta);

      LIS_NumELementos(pAresta->pVertice->pAntecessores, &numElems);
      LIS_IrInicioLista(pAresta->pVertice->pAntecessores);
      LIS_ProcurarValor(pAresta->pVertice->pAntecessores, pAlvo->nome);
      LIS_ExcluirElemento(pAresta->pVertice->pAntecessores);

      LIS_AvancarElementoCorrente(pAlvo->pSucessores, 1);
      numElemLista--;
   }

	// Para cada item da lista de anteressores, remover aresta que aponta para o corrente
	LIS_NumELementos(pAlvo->pAntecessores,&numElemLista);
	LIS_IrInicioLista(pAlvo->pAntecessores);

	while(numElemLista > 0)
	{
		int nElem = 0;
		LIS_ObterValor(pAlvo->pAntecessores,(void**)&pVertice);

		LIS_NumELementos(pVertice->pSucessores, &nElem);
		LIS_IrInicioLista(pVertice->pSucessores);

		while(nElem > 0)
		{
			LIS_ObterValor(pVertice->pSucessores,(void**)&pAresta);

			if(!strcmp(pAresta->pVertice->nome, pAlvo->nome))
			{
				LIS_ExcluirElemento(pVertice->pSucessores);
				break;
			}

			LIS_AvancarElementoCorrente(pVertice->pSucessores,1);
			nElem--;
		}
		LIS_AvancarElementoCorrente(pAlvo->pAntecessores,1);
		numElemLista--;
	}

	//Exclui elemento corrente
	LIS_IrInicioLista(pGrafo->pVertices);
	LIS_ProcurarValor(pGrafo->pVertices, pAlvo->nome);
	LIS_ExcluirElemento(pGrafo->pVertices);
   
	pGrafo->pCorrente = pVerticeOrigem;
}

#ifdef _DEBUG
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
   GRA_tpCondRet VER_VerticesNaoPossuemConteudoNulo(tpGrafo *pGrafo)
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
   GRA_tpCondRet VER_NenhumVerticeTemConteudoComOTipoCorrompido(tpGrafo *pGrafo)
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
            tipoValor = (CED_tpIdTipoEspaco) CED_ObterTipoEspaco(pVertice->pValor);
            if (tipoValor != GRA_TipoEspacoValorVertice)
            {
               CNT_CONTAR("ver07 tipo do valor esta correto");
               TST_NotificarFalha("Encontrado v�rtice cujo valor est� com o tipo errado.");
               return GRA_CondRetErroNaEstrutura;
            }
            else
            {
               CNT_CONTAR("ver07 tipo do valor nao esta correto");
            }
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
   GRA_tpCondRet VER_NenhumVerticeEstaDestacado(tpGrafo *pGrafo)
   {
      int tamListaVertices;

      LIS_NumELementos(pGrafo->pVertices, &tamListaVertices);

      if (pGrafo->qntVertices != tamListaVertices)
      {
         CNT_CONTAR("ver08 tamanho da lista de vertices diferente da quantidade de vertices");
         TST_NotificarFalha("Existem vertices destacados na estrutura.");
         return GRA_CondRetErroNaEstrutura;
      }
      
      CNT_CONTAR("ver08 tamanho da lista de vertices igual ah quantidade de vertices");

      return GRA_CondRetOK;
   }

   GRA_tpCondRet VER_ReferenciasDoVerticeEstaoCorretas(tpVertice *pVerticeVerificado)
   {
      LIS_tppLista pSuc, pAnt;
      LIS_tpCondRet lisCondRet;
      int estaVazia;
      int erroNaEstrutura = 0;

      if (CED_ObterTipoEspaco(pVerticeVerificado) != GRA_TipoEspacoVertice)
      {
         //CNT_CONTAR("ver08 vertice eh do tipo certo");
         return GRA_CondRetOK;
      }
      else
      {
         //CNT_CONTAR("ver08 vertice nao eh do tipo certo");
      }

      // Verificando se seus sucessores t�m o v�rtice como antecessor.
      pSuc = pVerticeVerificado->pSucessores;
      LIS_EstaVazia(pSuc, &estaVazia);
      if (!estaVazia)
      {
         //CNT_CONTAR("ver08 tem sucessores");
         LIS_IrInicioLista(pSuc);
         lisCondRet = LIS_CondRetOK;
         while (lisCondRet == LIS_CondRetOK)
         {
            tpAresta *pAresta;
            LIS_tppLista pBackAnt;
            LIS_tpCondRet retBackAnt;
            int antecessoresVazia;
            int ehAntecessorDoSucessor = 0;

            //CNT_CONTAR("ver08 percorrendo sucessores");

            LIS_ObterValor(pSuc, (void**) &pAresta);
            if (CED_ObterTipoEspaco(pAresta) == GRA_TipoEspacoAresta &&
               CED_ObterTipoEspaco(pAresta->pVertice) == GRA_TipoEspacoVertice)
            {
               //CNT_CONTAR("ver08 aresta e seu vertice sao do tipo certo");
               pBackAnt = pAresta->pVertice->pAntecessores;
               LIS_EstaVazia(pBackAnt, &antecessoresVazia);
               if (!antecessoresVazia)
               {
                  //CNT_CONTAR("ver08 sucessor tem antecessores");
                  LIS_IrInicioLista(pBackAnt);
                  retBackAnt = LIS_CondRetOK;
                  while(retBackAnt == LIS_CondRetOK)
                  {
                     tpVertice *pAntecessor;

                     //CNT_CONTAR("ver08 percorre antecessores do sucessor");

                     LIS_ObterValor(pBackAnt, (void**) &pAntecessor);
                     if (CED_ObterTipoEspaco(pAntecessor) == GRA_TipoEspacoVertice &&
                         strcmp(pAntecessor->nome, pVerticeVerificado->nome) == 0)
                     {
                        //CNT_CONTAR("ver08 encontrou antecessor do sucessor");
                        ehAntecessorDoSucessor = 1;
                        break;
                     }
                     else
                     {
                        //CNT_CONTAR("ver08 nao encontrou antecessor do sucessor");
                     }

                     retBackAnt = LIS_AvancarElementoCorrente(pBackAnt, 1);
                  }
               }
               else
               {
                  //CNT_CONTAR("ver08 sucessor nao tem antecessores");
               }
            
               if (!ehAntecessorDoSucessor)
               {
                  //CNT_CONTAR("ver08 vertice nao eh antecessor de algum sucessor");
                  TST_NotificarFalha("Encontrado sucessor de v�rtice que n�o possui ele como antecessor.");
                  erroNaEstrutura = 1;
               }
               else
               {
                  //CNT_CONTAR("ver08 vertice eh antecessor de algum sucessor");
               }
            }
            else
            {
               //CNT_CONTAR("ver08 aresta ou seu vertice nao sao do tipo certo");
            }

            lisCondRet = LIS_AvancarElementoCorrente(pSuc, 1);
         }
      }
      else
      {
         //CNT_CONTAR("ver08 nao tem sucessores");
      }
      
      // Verificando se seus antecessores tem o v�rtice como sucessor
      pAnt = pVerticeVerificado->pAntecessores;
      LIS_EstaVazia(pAnt, &estaVazia);
      if (!estaVazia)
      {
         //CNT_CONTAR("ver08 tem antecessores");
         LIS_IrInicioLista(pAnt);
         lisCondRet = LIS_CondRetOK;
         while (lisCondRet == LIS_CondRetOK)
         {
            LIS_tpCondRet retBackSuc = LIS_CondRetOK;
            tpVertice *pVertice;
            LIS_tppLista pBackSuc;
            int sucessoresVazia;
            int ehSucessorDoAntecessor = 0;

            //CNT_CONTAR("ver08 percorre antecessores");

            LIS_ObterValor(pAnt, (void**) &pVertice);

            if (CED_ObterTipoEspaco(pVertice) == GRA_TipoEspacoVertice)
            {
               //CNT_CONTAR("ver08 antecessor eh do tipo certo");
               pBackSuc = pVertice->pSucessores;
               LIS_EstaVazia(pBackSuc, &sucessoresVazia);
               if (!sucessoresVazia)
               {
                  //CNT_CONTAR("ver08 antecessor tem sucessores");
                  LIS_IrInicioLista(pBackSuc);
                  while(retBackSuc == LIS_CondRetOK)
                  {
                     tpAresta *pAresta;

                     //CNT_CONTAR("ver08 percorre sucessores do antecessor");

                     LIS_ObterValor(pBackSuc, (void**) &pAresta);
                     if (CED_ObterTipoEspaco(pAresta) == GRA_TipoEspacoAresta &&
                        CED_ObterTipoEspaco(pAresta->pVertice) == GRA_TipoEspacoVertice &&
                         strcmp(pAresta->pVertice->nome, pVerticeVerificado->nome) == 0)
                     {
                        //CNT_CONTAR("ver08 encontrou o sucessor do antecessor");
                        ehSucessorDoAntecessor = 1;
                        break;
                     }
                     else
                     {
                        //CNT_CONTAR("ver08 nao encontrou o sucessor do antecessor");
                     }

                     retBackSuc = LIS_AvancarElementoCorrente(pBackSuc, 1);
                  }
               }
               else
               {
                  //CNT_CONTAR("ver08 antecessor nao tem sucessores");
               }

               if (!ehSucessorDoAntecessor)
               {
                  //CNT_CONTAR("ver08 vertice eh sucessor do antecessor");
                  TST_NotificarFalha("Encontrado antecessor de v�rtice que n�o possui ele como sucessor.");
                  erroNaEstrutura = 1;
               }
               else
               {
                  //CNT_CONTAR("ver08 vertice nao eh sucessor do antecessor");
               }
            }
            else
            {
               //CNT_CONTAR("ver08 antecessor nao eh do tipo certo");
            }

            lisCondRet = LIS_AvancarElementoCorrente(pAnt, 1);
         }
      }
      else
      {
         //CNT_CONTAR("ver08 nao tem antecessores");
      }

      if (erroNaEstrutura)
      {
         //CNT_CONTAR("ver08 erros na estrutura");
         return GRA_CondRetErroNaEstrutura;
      }
      else
      {
         //CNT_CONTAR("ver08 sem erros na estrutura");
      }

      return GRA_CondRetOK;
   }

   // Det 09
   void DET_AtribuiNullAoCorrente(tpGrafo *pGrafo)
   {
      pGrafo->pCorrente = NULL;
   }
   
   // Ver 09
   GRA_tpCondRet VER_CorrenteNaoEhNulo(tpGrafo *pGrafo)
   {
      if (pGrafo->pCorrente == NULL)
      {
         CNT_CONTAR("ver09 se corrente nulo");
         TST_NotificarFalha("Corrente n�o existe.");
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
   GRA_tpCondRet VER_NaoExisteOrigemNula(tpGrafo *pGrafo)
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

/********** Fim do m�dulo de implementa��o: GRA Grafo direcionado **********/