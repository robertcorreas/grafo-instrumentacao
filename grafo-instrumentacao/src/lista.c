/***************************************************************************
*  M�dulo de implementa��o: LIS  Lista duplamente encadeada gen�rica
*
*  Arquivo gerado:              lista.c
*  Letras identificadoras:      LIS
*
*	Autores:
*     - rc: Robert Correa
*
*  Hist�rico de evolu��o:
*     Vers�o  Autor    Data     Observa��es
*     1.0     rc    12/set/2013 Adapta��o da fun��o CriarLista para ser testavel e inser��o de somente valores char.
*     2.0     rc    15/set/2013 Implementa��o e adapta��o do restante das fun��es para trabalhar somente com o tipo char.
*     3.0     hg    10/out/2013 Adapta��o do m�dulo para trabalhar com uma lista cujos valores ter�o um tipo qualquer.
*
*  Assertivas estruturais da lista
*    Refer�ncias entre os elementos
*     - pNo1->pProx = pNo2  <=>  pNo2 = NULL  ou  pNo2->pAnt  = pNo1
*     - pNo1->pAnt  = pNo2  <=>  pNo2 = NULL  ou  pNo2->pProx = pNo1
*
*    Refer�ncias dos elementos nas extremidades da lista
*     - pCabeca->pOrigemLista != NULL  =>  pCabeca->pOrigemLista->pAnt = NULL
*     - pCabeca->pFimLista    != NULL  =>  pCabeca->pFimLista->pProx = NULL
*
***************************************************************************/

#include   <stdio.h>
#include   <string.h>
#include   <memory.h>
#include   <malloc.h>
#include   <assert.h>

#define LISTA_OWN
#include "lista.h"
#undef LISTA_OWN

/***********************************************************************
*  Tipo de dados: LIS Elemento da lista
***********************************************************************/

   typedef struct stElemLista {

         void * pValor ;
               /* valor contido no elemento */

         struct stElemLista * pAnt ;
               /* Ponteiro para o elemento predecessor */

         struct stElemLista * pProx ;
               /* Ponteiro para o elemento sucessor */

   } tpElemLista ;

/***********************************************************************
*  Tipo de dados: LIS Descritor da cabe�a de lista
***********************************************************************/

   typedef struct LIS_stLista {

      tpElemLista * pOrigemLista ;
            /* Ponteiro para a origem da lista */

      tpElemLista * pFimLista ;
            /* Ponteiro para o final da lista */

      tpElemLista * pElemCorr ;
            /* Ponteiro para o elemento corrente da lista */

      int numElem ;
            /* N�mero de elementos da lista */

       void (*destruirValor)(void * pValor);
            /* L�gica respons�vel por destruir o valor da lista */

       int (*compararValores)(void * pValor1 , void * pValor2);
            /* L�gica respons�vel por comparar dois valores */

   } LIS_tpLista ;

/***** Prot�tipos das fun��es encapuladas no m�dulo *****/

   static void LiberarElemento( LIS_tppLista   pLista ,
                                tpElemLista  * pElem   ) ;

   static tpElemLista * CriarElemento( LIS_tppLista pLista ,
                                       void * pValor ) ;

   static void LimparCabeca( LIS_tppLista pLista ) ;

/*****  C�digo das fun��es exportadas pelo m�dulo  *****/

/***************************************************************************
*  Fun��o: LIS Criar lista
*  ****/
   LIS_tpCondRet LIS_CriarLista(LIS_tppLista *ppLista,
      void (*destruirValor)(void *pValor),
      int (*compararValores)(void *pValor1, void *pValor2))
   {
       LIS_tpLista *pLista;
       
       pLista = (LIS_tpLista*) malloc(sizeof( LIS_tpLista));
       if (pLista == NULL)
       {
          return LIS_CondRetFaltouMemoria;
       }
       
       LimparCabeca(pLista);
       
       pLista->destruirValor = destruirValor;
       pLista->compararValores = compararValores;
       
       *ppLista = pLista;
       
       return LIS_CondRetOK;
   }

/***************************************************************************
*  Fun��o: LIS Destruir lista
*  ****/
   LIS_tpCondRet LIS_DestruirLista(LIS_tppLista pLista)
   {
      LIS_EsvaziarLista(pLista);

      free(pLista);

      return LIS_CondRetOK;
   }

/***************************************************************************
*  Fun��o: LIS Esvaziar lista
*  ****/

   LIS_tpCondRet LIS_EsvaziarLista(LIS_tppLista pLista)
   {
       tpElemLista *pElem;
       tpElemLista *pProx;
       
       pElem = pLista->pOrigemLista;
       while (pElem != NULL)
       {
          pProx = pElem->pProx;
          LiberarElemento(pLista, pElem);
          pElem = pProx;
       }
       
       LimparCabeca(pLista);

       return LIS_CondRetOK;
   }


/***************************************************************************
*  Fun��o: LIS Inserir elemento antes
*  ****/
   LIS_tpCondRet LIS_InserirElementoAntes(LIS_tppLista pLista,
                                           void * pValor)
   {
      tpElemLista *pElem;

      /* Criar elemento a inerir antes */

      pElem = CriarElemento(pLista, pValor);
      if (pElem == NULL)
      {
         return LIS_CondRetFaltouMemoria;
      }

      /* Encadear o elemento antes do elemento corrente */

      if (pLista->pElemCorr == NULL)
      {
         pLista->pOrigemLista = pElem;
         pLista->pFimLista = pElem;
      }
      else
      {
         if (pLista->pElemCorr->pAnt != NULL)
         {
            pElem->pAnt = pLista->pElemCorr->pAnt;
            pLista->pElemCorr->pAnt->pProx = pElem;
         }
         else
         {
            pLista->pOrigemLista = pElem;
         }

         pElem->pProx = pLista->pElemCorr;
         pLista->pElemCorr->pAnt = pElem;
      }

      pLista->pElemCorr = pElem;

      return LIS_CondRetOK;
   }


/***************************************************************************
*  Fun��o: LIS Inserir elemento ap�s
*  ****/
   LIS_tpCondRet LIS_InserirElementoApos(LIS_tppLista pLista,
                                          void *pValor)
   {
      tpElemLista *pElem;

      /* Criar elemento a inerir ap�s */

      pElem = CriarElemento(pLista, pValor);
      if (pElem == NULL)
      {
         return LIS_CondRetFaltouMemoria;
      }

      /* Encadear o elemento ap�s o elemento */

      if (pLista->pElemCorr == NULL)
      {
         pLista->pOrigemLista = pElem;
         pLista->pFimLista = pElem;
      }
      else
      {
         if (pLista->pElemCorr->pProx != NULL)
         {
            pElem->pProx  = pLista->pElemCorr->pProx;
            pLista->pElemCorr->pProx->pAnt = pElem;
         } else
         {
            pLista->pFimLista = pElem;
         }

         pElem->pAnt = pLista->pElemCorr;
         pLista->pElemCorr->pProx = pElem;
      }
                  
      pLista->pElemCorr = pElem;
                  
      return LIS_CondRetOK;
   }


/***************************************************************************
*  Fun��o: LIS Excluir elemento
*  ****/
   LIS_tpCondRet LIS_ExcluirElemento(LIS_tppLista pLista)
   {

      tpElemLista *pElem;

      if (pLista->pElemCorr == NULL)
      {
         return LIS_CondRetListaVazia;
      }

      pElem = pLista->pElemCorr;
      pLista->pElemCorr = NULL;

      /* Desencadeia � esquerda */

         if (pElem->pAnt != NULL)
         {
            pElem->pAnt->pProx   = pElem->pProx;
            pLista->pElemCorr    = pElem->pAnt;
         }
         else
         {
            pLista->pOrigemLista = pElem->pProx;
         }

      /* Desencadeia � direita */

      if (pElem->pProx != NULL)
      {
         pElem->pProx->pAnt = pElem->pAnt;
         pLista->pElemCorr  = pElem->pProx;
      }
      else
      {
         pLista->pFimLista = pElem->pAnt;
      }

      LiberarElemento(pLista, pElem);

      return LIS_CondRetOK;
   }

/***************************************************************************
*  Fun��o: LIS Obter o valor contido no elemento
*  ****/
   LIS_tpCondRet LIS_ObterValor(LIS_tppLista pLista, void **ppValor)
   {
      if (pLista->pElemCorr == NULL)
      {
        return LIS_CondRetListaVazia;
      }

     *ppValor = pLista->pElemCorr->pValor;

     return LIS_CondRetOK;
   }

/***************************************************************************
*  Fun��o: LIS Ir para o elemento inicial
*  ****/
   LIS_tpCondRet LIS_IrInicioLista(LIS_tppLista pLista)
   {
      pLista->pElemCorr = pLista->pOrigemLista;

      return LIS_CondRetOK;
   }

/***************************************************************************
*  Fun��o: LIS Ir para o elemento final
*  ****/
   LIS_tpCondRet LIS_IrFinalLista(LIS_tppLista pLista)
   {
      pLista->pElemCorr = pLista->pFimLista ;

      return LIS_CondRetOK ;
   }

/***************************************************************************
*  Fun��o: LIS Avan�ar elemento
*  ****/
   LIS_tpCondRet LIS_AvancarElementoCorrente(LIS_tppLista pLista,
                                              int numElem)
   {
      int i;

      tpElemLista *pElem;

      /* Tratar lista vazia */

      if (pLista->pElemCorr == NULL)
      {
         return LIS_CondRetListaVazia;
      }

      /* Tratar avan�ar para frente */

      if (numElem > 0)
      {
         pElem = pLista->pElemCorr;
         for(i = numElem; i > 0; i--)
         {
            if (pElem == NULL)
            {
               break;
            }
            pElem = pElem->pProx;
         }

         if (pElem != NULL)
         {
            pLista->pElemCorr = pElem;
            return LIS_CondRetOK;
         }

         pLista->pElemCorr = pLista->pFimLista;
         return LIS_CondRetFimLista;
      }

      /* Tratar avan�ar para tr�s */

      else if (numElem < 0)
      {
         pElem = pLista->pElemCorr ;
         for(i = numElem; i < 0; i++)
         {
            if (pElem == NULL)
            {
               break;
            }
            pElem = pElem->pAnt;
         }

         if ( pElem != NULL )
         {
            pLista->pElemCorr = pElem;
            return LIS_CondRetOK;
         }

         pLista->pElemCorr = pLista->pOrigemLista;
         return LIS_CondRetFimLista;
      }

      /* Tratar n�o avan�ar */

      return LIS_CondRetOK;

   }

/***************************************************************************
*  Fun��o: LIS Procurar elemento contendo 
*  ****/
   LIS_tpCondRet LIS_ProcurarValor(LIS_tppLista pLista,
                                    void *pValor)
   {
      tpElemLista *pElem;

      if (pLista->pElemCorr == NULL)
      {
         return LIS_CondRetListaVazia;
      }

      for ( pElem  = pLista->pElemCorr ;
            pElem != NULL ;
            pElem  = pElem->pProx )
      {
         if (pLista->compararValores(pElem->pValor, pValor) == 0)
         {
            pLista->pElemCorr = pElem;
            return LIS_CondRetOK;
         }
      }

      return LIS_CondRetNaoAchou;

   }

/***************************************************************************
*  Fun��o: LIS Esta Vazia
*  ****/
   LIS_tpCondRet LIS_EstaVazia(LIS_tppLista pLista,
                                              int *pResposta)
   {
      if(pLista->pElemCorr == NULL)
      {
         *pResposta = 1;
      }
      else
      {
         *pResposta = 0;
      }

      return LIS_CondRetOK;
   }


/***************************************************************************
*  Fun��o: LIS Numero de elementos
*  ****/
   LIS_tpCondRet LIS_NumELementos(LIS_tppLista pLista,
                                      int *pNumElementos)
   {
      *pNumElementos = pLista->numElem;
      return LIS_CondRetOK;
   }


/*****  C�digo das fun��es encapsuladas no m�dulo  *****/

/***********************************************************************
*
*  Fun��o: LIS Criar o elemento
*
*  Descri��o:
*    Cria um elemento da lista, com anterior e pr�ximo nulos.
*
***********************************************************************/
   tpElemLista * CriarElemento(LIS_tppLista pLista,
                                void *pValor)
   {
      tpElemLista *pElem ;

      pElem = (tpElemLista*) malloc(sizeof(tpElemLista));
      if (pElem == NULL)
      {
         return NULL;
      }

      pElem->pValor = pValor;
      pElem->pAnt   = NULL;
      pElem->pProx  = NULL;

      pLista->numElem++;

      return pElem;
   }

/***********************************************************************
*
*  Fun��o: LIS Liberar elemento da lista
*
*  Descri��o:
*    Destroi um elemento e o seu valor, liberando-os da mem�ria.
*
***********************************************************************/
   void LiberarElemento(LIS_tppLista pLista,
                        tpElemLista  *pElem)
   {
      if (pLista->destruirValor != NULL)
      {
	      pLista->destruirValor(pElem->pValor);
      }
      free(pElem);

	   pLista->numElem--;

   }

/***********************************************************************
*
*  Fun��o: LIS Limpar a cabe�a da lista
*
*  Descri��o:
*    Atualiza os atributos da lista para que ela esteja vazia.
*
***********************************************************************/

   void LimparCabeca(LIS_tppLista pLista)
   {
      pLista->pOrigemLista = NULL;
      pLista->pFimLista    = NULL;
      pLista->pElemCorr    = NULL;
      pLista->numElem      = 0;
   }

/********** Fim do m�dulo de implementa��o: LIS Lista duplamente encadeada gen�rica **********/