#if ! defined( GRAFO_ )
#define GRAFO_
/***************************************************************************
*
*  M�dulo de defini��o: GRA  Grafo direcionado
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
*  Descri��o do m�dulo
*     Implementa grafos direcionados com manipula��o, ou seja, � possivel criar, inserir
*     v�rtices no grafo, conect�-los atrav�s de arestas direcionais al�m de destruir o grafo,
*     um v�rtice ou uma aresta, navegar atrav�s das arestas usando o nome da aresta ou do
*     v�rtice de destino como refer�ncia. � poss�vel tamb�m marcar v�rtices como origem,
*     estes v�rtices poder�o ser acessados � qualquer momento utilizando a fun��o de navega��o
*     para origens.
*
*     Podem existir n grafos em opera��o simultaneamente.
*     Os grafos possuem uma cabe�a encapsulando o seu estado.
*
*     Cada grafo armazena dados gen�ricos.
*     Cada v�tice do grafo possui uma refer�ncia para o valor.
*
***************************************************************************/
 
#if defined( GRAFO_OWN )
   #define GRAFO_EXT
#else
   #define GRAFO_EXT extern
#endif

/***** Declara��es exportadas pelo m�dulo *****/

/* Tipo refer�ncia para um grafo */

typedef struct GRA_stGrafo * GRA_tppGrafo ;

/***********************************************************************
*
*  Tipo de dados: GRA Condi��es de retorno
*
*
*  Descri��o do tipo
*     Condi��es de retorno das fun��es do grafo
*
***********************************************************************/

typedef enum {

   GRA_CondRetOK,
      /* Concluiu corretamente */

   GRA_CondRetGrafoVazio,
      /* O grafo n�o cont�m elementos */

   GRA_CondRetGrafoNaoFoiCriado,
      /* O grafo n�o cont�m elementos */

   GRA_CondRetNaoAchou,
      /* N�o encontrou o v�rtice/aresta procurado(a) */
      
   GRA_CondRetJaExiste,
      /* Nome do v�rtice ou aresta est� repetido */

   GRA_CondRetFaltouMemoria
      /* Faltou mem�ria ao tentar alocar algo */
 
#ifdef _DEBUG
   ,GRA_CondRetErroNaEstrutura
#endif
} GRA_tpCondRet;

/***********************************************************************
*
*  $TC Tipo de dados: ARV Modos de deturpar
*
*
***********************************************************************/

#ifdef _DEBUG

typedef enum {

   DeturpaEliminaElementoCorrente            = 1,
      /* Elimina o elemento corrente da lista de v�rtices */

   GRA_DeturpaAtribuiNullParaVerticeSucessor = 2,
      /* Atribui NULL ao ponteiro para um v�rtice sucessor */

   GRA_AtribuiNullAOPonteiroDoVerticePredecessor = 3,
      /* Atribui NULL ao ponteiro para um v�rtice predecessor */

   GRA_LixoNaReferenciaParaSucessor = 4,
    /*  Atribui lixo ao ponteiro para a refer�ncia a um v�rtice sucessor */

   GRA_LixoNaReferenciaParaAntecessor        = 5,
      /* Atribui lixo ao ponteiro para a refer�ncia a um v�rtice predecessor. */

   GRA_ConteudoDoVerticeNULL                 = 6,
      /* Atribui NULL ao ponteiro para o conte�do do v�rtice. */

   GRA_AlteraTipoDoValorDoVertice            = 7,
      /* Altera o tipo de estrutura apontado no v�rtice. */

   GRA_DestacaVertice                        = 8,
      /* Destaca v�rtice do grafo sem liber�-lo com free. */

   GRA_AtribuiNullAoCorrente                 = 9,
      /* Atribui NULL ao ponteiro corrente. */

   GRA_AtribuiNullParaUmaOrigem              = 10
      /* Atribui NULL a um ponteiro de origem. */

} GRA_tpModosDeturpacao;

#endif

/***********************************************************************
*
*  Fun��o: GRA Criar grafo
*
*  Descri��o
*     Cria um grafo direcionado.
*
*  Par�metros
*     ppGrafo           - Refer�ncia usada para retorno do grafo que ser� criado.
*     destruirValor     - Fun��o que ser� utilizada para destruir o valor
                          e liberar seu espa�o na mem�ria.
*
*  Condi��es de retorno
*     - GRA_CondRetOK
*     - GRA_CondRetFaltouMemoria
* 
*  Retorno por refer�ncia
*     ppGrafo:
*       Se executou corretamente retorna o ponteiro para o grafo.
*       Este ponteiro ser� utilizado pelas fun��es que manipulem este grafo.
*       
*       Se ocorreu algum erro, por exemplo falta de mem�ria, a fun��o retornar� NULL.
*       N�o ser� dada mais informa��o quanto ao problema ocorrido.
*
*  Assertivas de entrada
*     <n�o tem>
*
*  Assertivas de saida
*     - Valem as assertivas estruturais do grafo direcionado.
*     - Ponteiros do corrente � nulo e as listas de origens e v�rtices est�o vazias.
*     - Fun��o de destruir ser� igual � passada por par�metro.
*
***********************************************************************/
GRA_tpCondRet GRA_CriarGrafo(GRA_tppGrafo *ppGrafo, void (*destruirValor)(void *pValor));

/***********************************************************************
*
*  Fun��o: GRA Destruir grafo
*
*  Descri��o
*     Destr�i o grafo fornecido.
*     OBS. n�o existe previs�o para poss�veis falhas de execu��o.
*
*
*  Par�metros
*     ppGrafo  - Refer�ncia do a ser destruido.
*
*  Condi��es de retorno
*     - GRA_CondRetOK
*
*  Assertivas de entrada
*     - Valem as assertivas estruturais do grafo direcionado.
*
*  Assertivas de sa�da
*     - A o grafo, seus v�rtices e suas arestas foram destruidos.
*
***********************************************************************/
GRA_tpCondRet GRA_DestruirGrafo(GRA_tppGrafo *ppGrafo);

/***********************************************************************
*
*  Fun��o: GRA Inserir v�rtice
*
*  Descri��o
*     Insere novo v�rtice no grafo.
*     Obs.: Nenhuma rela��o do novo v�rtice com qualquer outro v�rtice ou
*     aresta existente, para isso � necess�rio utilizar outras fun��es.
*
*  Par�metros
*     pGrafoParm    - ponteiro para o grafo onde deve ser inserido o v�rtice
*     nomeVertice  - nome do v�rtice � ser criado.
*     pValor        - ponteiro para valor do novo v�rtice.
*
*  Condi��es de retorno
*     GRA_CondRetOK
*     GRA_CondRetGrafoVazio
*     GRA_CondRetGrafoNaoFoiCriado
*     GRA_CondRetJaExiste
*     GRA_CondRetFaltouMemoria
*
*  Assertivas de entrada
*     - Valem as assertivas estruturais do grafo direcionado.
*
*  Assertivas de sa�da
*     - Valem as assertivas estruturais do grafo direcionado.
*     - Corrente apontar� para o v�rtice inserido.
*     - Se j� existe v�rtice com esse nome, nenhum v�rtice � adicionado e o
*       corrente permanece o mesmo.
*
***********************************************************************/
GRA_tpCondRet GRA_InserirVertice(GRA_tppGrafo pGrafoParm, char *nomeVertice, void *pValor);

/***********************************************************************
*
*  Fun��o: GRA Inserir aresta
*
*  Descri��o
*     Insere nova aresta ligando dois v�rtices � escolha.
*
*  Par�metros
*     pGrafoParm           - ponteiro para o grafo onde deve ser inserido a aresta
*     nomeAresta          - nome da aresta � ser criada.
*     nomeVerticeOrigem   - nome do v�rtice cuja a aresta partir�.
*     nomeVerticeDestino  - nome do v�rtice cuja a aresta apontar�.
*
*  Condi��es de retorno
*     GRA_CondRetOK
*     GRA_CondRetFaltouMemoria
*     GRA_CondRetGrafoVazio
*     GRA_CondRetGrafoNaoFoiCriado
*     GRA_CondRetJaExiste
*     GRA_CondRetNaoAchou
*
*  Assertivas de entrada
*     - Valem as assertivas estruturais do grafo direcionado.
*
*  Assertivas de sa�da
*     - Valem as assertivas estruturais do grafo direcionado.
*
***********************************************************************/
GRA_tpCondRet GRA_InserirAresta(GRA_tppGrafo pGrafoParm, char *nomeAresta,
     char *nomeVerticeOrigem, char *nomeVerticeDestino);

/***********************************************************************
*
*  Fun��o: GRA Obter o valor contido no v�rtice corrente
*
*  Descri��o
*     Obtem o valor contido no v�rtice corrente do grafo
*
*  Par�metros
*     pGrafoParm - ponteiro para o grafo de onde se quer o valor
*     ppValor    - ponteiro que ter� o valor obtido.
*
*  Condi��es de retorno
*     GRA_CondRetOK
*     GRA_CondRetGrafoVazio
*     GRA_CondRetGrafoNaoFoiCriado
*
*  Assertivas de entrada
*     - Valem as assertivas estruturais do grafo direcionado.
*
*  Assertivas de sa�da
*     - Valem as assertivas estruturais do grafo direcionado.
*     - O valor retornado ser� o valor do v�rtice corrente do grafo.
*
***********************************************************************/
GRA_tpCondRet GRA_ObterValorCorrente(GRA_tppGrafo pGrafoParm, void **ppValor);

/***********************************************************************
*
*  Fun��o: GRA Alterar o valor contido no v�rtice corrente
*
*  Descri��o
*     Substitui o valor contido no v�rtice corrente do grafo
*     Obs.: A fun��o n�o destroi o antigo valor.
*
*  Par�metros
*     pGrafoParm - ponteiro para o grafo de onde se quer o valor
*     pValor     - ponteiro para o novo valor.
*
*  Condi��es de retorno
*     GRA_CondRetOK
*     GRA_CondRetGrafoVazio
*     GRA_CondRetGrafoNaoFoiCriado
*
*  Assertivas de entrada
*     - Valem as assertivas estruturais do grafo direcionado.
*
*  Assertivas de sa�da
*     - Valem as assertivas estruturais do grafo direcionado.
*     - O valor do v�rtice corrente ser� o valor do v�rtice passado por par�metro.
*
***********************************************************************/
GRA_tpCondRet GRA_AlterarValorCorrente(GRA_tppGrafo pGrafoParm, void *pValor);

/***********************************************************************
*
*  Fun��o: GRA Tornar o v�rtice corrente uma origem
*
*  Descri��o
*     Torna o v�rtice corrente uma origem.
*
*  Par�metros
*     pGrafoParm - ponteiro para o grafo de onde se quer o valor
*
*  Condi��es de retorno
*     GRA_CondRetOK
*     GRA_CondRetGrafoVazio
*     GRA_CondRetGrafoNaoFoiCriado
*     GRA_CondRetJaExiste
*
*  Assertivas de entrada
*     - Valem as assertivas estruturais do grafo direcionado.
*
*  Assertivas de sa�da
*     - Valem as assertivas estruturais do grafo direcionado.
*     - V�rtice corrente � adicionado � lista de origens.
*
***********************************************************************/
GRA_tpCondRet GRA_TornarCorrenteUmaOrigem(GRA_tppGrafo pGrafoParm);


/***********************************************************************
*
*  Fun��o: GRA Deixar de ser origem
*
*  Descri��o
*     V�rtice corrente deixa de ser origem.
*
*  Par�metros
*     pGrafoParm - ponteiro para o grafo de onde se quer o valor
*
*  Condi��es de retorno
*     GRA_CondRetOK
*     GRA_CondRetGrafoVazio
*     GRA_CondRetGrafoNaoFoiCriado
*     GRA_CondRetGrafoNaoAchou
*
*  Assertivas de entrada
*     - Valem as assertivas estruturais do grafo direcionado.
*
*  Assertivas de sa�da
*     - Valem as assertivas estruturais do grafo direcionado.
*     - V�rtice corrente � removido da lista de origens.
*
***********************************************************************/
GRA_tpCondRet GRA_DeixarDeSerOrigem(GRA_tppGrafo pGrafoParm);

/***********************************************************************
*
*  Fun��o: GRA Ir para o v�rtice adjacente
*
*  Descri��o
*     Navega com o v�rtice corrente para o v�rtice contendo o nome passado como argumento.
*
*  Par�metros
*     pGrafoParm   - ponteiro para o grafo
*     nomeVertice - ponteiro para o nome do v�rtice
*
*  Condi��es de retorno
*     GRA_CondRetOK
*     GRA_CondRetGrafoVazio
*     GRA_CondRetGrafoNaoFoiCriado
*     GRA_CondRetNaoAchou
*
*  Assertivas de entrada
*     - Valem as assertivas estruturais do grafo direcionado.
*
*  Assertivas de sa�da
*     - Valem as assertivas estruturais do grafo direcionado.
*     - V�rtice corrente aponta para v�rtice com o nome passado como argumento.
*
***********************************************************************/
GRA_tpCondRet GRA_IrParaVerticeAdjacente(GRA_tppGrafo pGrafoParm, char *nomeVertice);

/***********************************************************************
*
*  Fun��o: GRA Seguir pela aresta
*
*  Descri��o
*     Navega com o v�rtice corrente para o v�rtice apontado pela aresta contendo nome passado como argumento.
*
*  Par�metros
*     pGrafoParm  - ponteiro para o grafo
*     nomeAresta - ponteiro para o nome da aresta que ir� seguir
*
*  Condi��es de retorno
*     GRA_CondRetOK
*     GRA_CondRetGrafoVazio
*     GRA_CondRetGrafoNaoFoiCriado
*     GRA_CondRetNaoAchou
*
*  Assertivas de entrada
*     - Valem as assertivas estruturais do grafo direcionado.
*
*  Assertivas de sa�da
*     - Valem as assertivas estruturais do grafo direcionado.
*     - V�rtice corrente aponta para v�rtice apontado pela aresta passada como argumento.
*
***********************************************************************/
GRA_tpCondRet GRA_SeguirPelaAresta(GRA_tppGrafo pGrafoParm, char *nomeAresta);

/***********************************************************************
*
*  Fun��o: GRA Ir para a origem
*
*  Descri��o
*     Navega com o v�rtice corrente para um v�rtice origem
*
*  Par�metros
*     pGrafoParm   - ponteiro para o grafo
*     nomeVertice - ponteiro para o nome do vertice
*
*  Condi��es de retorno
*     GRA_CondRetOK
*     GRA_CondRetGrafoVazio
*     GRA_CondRetGrafoNaoFoiCriado
*     GRA_CondRetNaoAchou
*
*  Assertivas de entrada
*     - Valem as assertivas estruturais do grafo direcionado.
*
*  Assertivas de sa�da
*     - Valem as assertivas estruturais do grafo direcionado.
*     - V�rtice corrente aponta para v�rtice origem contido na lista de origens.
*
***********************************************************************/
GRA_tpCondRet GRA_IrParaAOrigem(GRA_tppGrafo pGrafoParm, char *nomeVertice);

/***********************************************************************
*
*  Fun��o: GRA Destruir v�rtice corrente
*
*  Descri��o
*     Destroi v�rtice corrente, liberando-o da mem�ria
*
*  Par�metros
*     pGrafoParm   - ponteiro para o grafo
*
*  Condi��es de retorno
*     GRA_CondRetOK
*     GRA_CondRetGrafoVazio
*     GRA_CondRetGrafoNaoFoiCriado
*     GRA_CondRetNaoAchou
*
*  Assertivas de entrada
*     - Valem as assertivas estruturais do grafo direcionado.
*
*  Assertivas de sa�da
*     - Valem as assertivas estruturais do grafo direcionado.
*     - V�rtice corrente passa a apontar para o primeiro v�rtice de origem.
*
***********************************************************************/
GRA_tpCondRet GRA_DestruirVerticeCorrente(GRA_tppGrafo pGrafoParm);

/***********************************************************************
*
*  Fun��o: GRA Destruir aresta adjacente
*
*  Descri��o
*     Destroi aresta do vertice corrente
*
*  Par�metros
*     pGrafoParm   - ponteiro para o grafo
*     nomeAresta  - ponteiro para o nome da aresta a ser destruida

*  Condi��es de retorno
*     GRA_CondRetOK
*     GRA_CondRetGrafoVazio
*     GRA_CondRetGrafoNaoFoiCriado
*     GRA_CondRetNaoAchou
*
*  Assertivas de entrada
*     - Valem as assertivas estruturais do grafo direcionado.
*
*  Assertivas de sa�da
*     - Valem as assertivas estruturais do grafo direcionado.
*     - Removido referencia do v�rtice corrente da lista de antecessores do vertice apontado pela aresta a ser destruida.

***********************************************************************/
GRA_tpCondRet GRA_DestruirArestaAdjacente(GRA_tppGrafo pGrafoParm, char *nomeAresta);


#ifdef _DEBUG
   /* Fun��o de deturpa��o */
   void GRA_Deturpar(void *pGrafoParm, GRA_tpModosDeturpacao ModoDeturpa);
   /* Funcao de verifica��o */
   GRA_tpCondRet GRA_Verificar(void *pGrafoParm);
#endif


/***********************************************************************/
#undef GRAFO_EXT

/********** Fim do m�dulo de defini��o: GRA Grafo direcionado **********/

#else
#endif