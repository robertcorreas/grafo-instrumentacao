#if ! defined( LISTA_ )
#define LISTA_
/***************************************************************************
*
*  Módulo de definição: LIS  Lista duplamente encadeada genérica
*
*  Arquivo gerado:              lista.h
*  Letras identificadoras:      LIS
*
*	Autores:
*     - rc: Robert Correa
*     - hg: Hugo Roque
*
*  Histórico de evolução:
*     Versão  Autor    Data     Observações
*     1.0     rc    12/set/2013 Adaptação da função CriarLista para ser testavel e inserção de somente valores char.
*     2.0     rc    15/set/2013 Implementação e adaptação do restante das funções para trabalhar somente com o tipo char.
*     3.0     hg    10/out/2013 Adaptação do módulo para trabalhar com uma lista cujos valores terão um tipo qualquer.
*     4.0     hg    03/dez/2013 Instrumentação para a estrutura saber o total de espaço alocado por ela.
*
*  Descrição do módulo
*     Implementa listas genéricas duplamente encadeadas com manipulação,
*     ou seja, é possivel criar, inserir elementos na lista, antes,
*     ou após além de esvaziar a lista, remover elementos, procurar elementos,
*     navegar para o inicio e final da lista e destruir a lista
*
*     Podem existir n listas em operação simultaneamente.
*     As listas possuem uma cabeça encapsulando o seu estado.
*
*     Cada lista armazena dados genéricos.
*     Cada elemento da lista possui uma referência para o valor.
*
***************************************************************************/
 
#if defined( LISTA_OWN )
   #define LISTA_EXT
#else
   #define LISTA_EXT extern
#endif

/***** Declarações exportadas pelo módulo *****/

/* Tipo referência para uma lista */
typedef struct LIS_stLista * LIS_tppLista ;


/***********************************************************************
*
*  Tipo de dados: LIS Condições de retorno
*
*
*  Descrição do tipo
*     Condições de retorno das funções da lista
*
***********************************************************************/

   typedef enum {

         LIS_CondRetOK ,
               /* Concluiu corretamente */

         LIS_CondRetListaVazia ,
               /* A lista não contém elementos */

         LIS_CondRetFimLista ,
               /* Foi atingido o fim de lista */

         LIS_CondRetNaoAchou ,
               /* Não encontrou o valor procurado */

         LIS_CondRetFaltouMemoria
               /* Faltou memória ao tentar criar um elemento de lista */

   } LIS_tpCondRet ;


/***********************************************************************
*
*  Função: LIS Criar lista
*
*  Descrição
*     Cria uma lista genérica duplamente encadeada.
*
*  Parâmetros
*     ppLista           - Referência usada para retorno da lista que será criado.
*     destruirValor     - Função que será utilizada para destruir o valor
                          e liberar seu espaço de memória.
*     compararValores   - Função que será utilizada para comparar os valores quando
*                         for efetuada a busca por um elemento da lista.
*
*  Condições de retorno
*     - LIS_CondRetOK
*     - LIS_CondRetFaltouMemoria
* 
*  Retorno por referência
*     ppLista:
*       Se executou corretamente retorna o ponteiro para a lista.
*       Este ponteiro será utilizado pelas funções que manipulem esta lista.
*       
*       Se ocorreu algum erro, por exemplo falta de memória ou dados errados,
*       a função retornará NULL.
*       Não será dada mais informação quanto ao problema ocorrido.
*
*  Assertivas de entrada
*     <não tem>
*
*  Assertivas de saida
*     - Valem as assertivas estruturais da lista duplamente encadeada com cabeça.
*     - Ponteiros do corrente, inicio e final da lista não nulos.
*     - Número de elementos é 0.
*     - Funções de destruir e comparar serão iguais às passadas por parâmetro.
*
***********************************************************************/
   LIS_tpCondRet LIS_CriarLista(LIS_tppLista* ppLista , 
      void (*destruirValor)(void * pValor) ,
      int (*compararValores)(void * pValor1, void * pValor2) ) ;

/***********************************************************************
*
*  Função: LIS Destruir lista
*
*  Descrição
*     Destrói a lista fornecida.
*     O parâmetro ponteiro para a lista não é modificado.
*     Se ocorrer algum erro durante a destruição, a lista resultará
*     estruturalmente incorreta.
*     OBS. não existe previsão para possíveis falhas de execução.
*
*
*  Parâmetros
*     pLista  - Referência da lista a ser destruida.
*
*  Condições de retorno
*     - LIS_CondRetOK
*
*  Assertivas de entrada
*     - Valem as assertivas estruturais da lista duplamente encadeada com cabeça.
*
*  Assertivas de saída
*     - A lista e seus elementos foram destruidos.
*
***********************************************************************/
   LIS_tpCondRet LIS_DestruirLista(LIS_tppLista pLista) ;

/***********************************************************************
*
*  Função: LIS Esvaziar lista
*
*  Descrição
*     Elimina todos os elementos, sem contudo eliminar a lista
*
*  Parâmetros
*     pLista - ponteiro para a lista a ser esvaziada
*
*  Condições de retorno
*     - LIS_CondRetOK
*
*  Assertivas de entrada
*     - Valem as assertivas estruturais da lista duplamente encadeada com cabeça.
*
*  Assertivas de saída
*     - Valem as assertivas estruturais da lista duplamente encadeada com cabeça.
*     - Os elementos da lista foram destruidos.
*
***********************************************************************/
   LIS_tpCondRet LIS_EsvaziarLista(LIS_tppLista pLista) ;

/***********************************************************************
*
*  Função: LIS Inserir elemento antes
*
*  Descrição
*     Insere novo elemento antes do elemento corrente.
*     Caso a lista esteja vazia, insere o primeiro elemento da lista.
*
*  Parâmetros
*     pLista  - ponteiro para a lista onde deve ser inserido o elemento
*     pValor  - ponteiro para valor do novo elemento
*
*  Condições de retorno
*     LIS_CondRetOK
*     LIS_CondRetFaltouMemoria
*
*  Assertivas de entrada
*     - Valem as assertivas estruturais da lista duplamente encadeada com cabeça.
*
*  Assertivas de saída
*     - Valem as assertivas estruturais da lista duplamente encadeada com cabeça.
*     - Corrente apontará para o elemento inserido.
*     - Se antigo corrente != NULL então o anterior do antigo corrente será o
*       elemento inserido.
*     - Se antigo corrente != NULL então o próximo do elemento inserido será o
*       antigo corrente.
*     - Se antigo corrente = NULL então o elemento inserido será a origem e o fim
*       da lista.
*     - Se anterior do corrente = NULL então o elemento inserido será o inicio da lista.
*     - Número de elementos será incrementado em 1.
*
***********************************************************************/
   LIS_tpCondRet LIS_InserirElementoAntes(LIS_tppLista pLista ,
                                           void *pValor);


/***********************************************************************
*
*  Função: LIS  Inserir elemento após
*
*  Descrição da função
*     Insere novo elemento após o elemento corrente.
*     Caso a lista esteja vazia, insere o primeiro elemento da lista.
*
*  Parâmetros
*    pLista  - ponteiro para a lista onde deve ser inserido o elemento
*    pValor  -  ponteiro para o valor do novo elemento
*           
*
*  Condições de retorno
*        LIS_CondRetOK
*        LIS_CondRetFaltouMemoria
*
*  Assertivas de entrada
*     - Valem as assertivas estruturais da lista duplamente encadeada com cabeça.
*
*  Assertivas de saída
*     - Valem as assertivas estruturais da lista duplamente encadeada com cabeça.
*     - Corrente apontará para o elemento inserido.
*     - Se antigo corrente != NULL então o próximo do antigo corrente será o
*       elemento inserido.
*     - Se antigo corrente != NULL então o anterior do elemento inserido será o
*       antigo corrente.
*     - Se antigo corrente = NULL então o elemento inserido será a origem e o fim
*       da lista.
*     - Se próximo do corrente = NULL então o elemento inserido será o fim da lista.
*     - Número de elementos será incrementado em 1.
*
***********************************************************************/
   LIS_tpCondRet LIS_InserirElementoApos(LIS_tppLista pLista,
                                          void *pValor);


/***********************************************************************
*
*  Função: LIS  Excluir elemento
*
*  Descrição
*     Exclui o elemento corrente da lista dada.
*     Se existir o elemento à esquerda do corrente será o novo corrente.
*     Se não existir e existir o elemento à direita, este se tornará corrente.
*     Se este também não existir a lista tornou-se vazia.
*
*  Parâmetros
*     pLista    - ponteiro para a lista na qual deve excluir.
*
*  Condições de retorno
*     LIS_CondRetOK
*     LIS_CondRetListaVazia
*
*  Assertivas de entrada
*     - Valem as assertivas estruturais da lista duplamente encadeada com cabeça.
*
*  Assertivas de saída
*     - Valem as assertivas estruturais da lista duplamente encadeada com cabeça.
*     - O elemento foi excluído.
*     - Se anterior e próximo do excluido forem nulos, então o corrente, inicio
*     - e fim da lista são nulos.
*     - Se dentre o anterior e próximo do excluido um for não-nulo, este será o
*     - novo corrente.
*     - Se o anterior e o próximo do excluidos forem não-nulos, o próximo será o
*     - novo corrente.
*
***********************************************************************/
   LIS_tpCondRet LIS_ExcluirElemento(LIS_tppLista pLista) ;

/***********************************************************************
*
*  Função: LIS Obter o valor contido no elemento
*
*  Descrição
*     Obtem o valor contido no elemento corrente da lista
*
*  Parâmetros
*     pLista - ponteiro para a lista de onde se quer o valor
*     pValor - ponteiro que terá o valor obtido.
*
*  Condições de retorno
*     LIS_CondRetOK
*     LIS_CondRetListaVazia
*
*  Assertivas de entrada
*     - Valem as assertivas estruturais da lista duplamente encadeada com cabeça.
*
*  Assertivas de saída
*     - O valor retornado será o valor do elemento corrente da lista.
*
***********************************************************************/
   LIS_tpCondRet LIS_ObterValor(LIS_tppLista pLista, void **ppValor);

/***********************************************************************
*
*  Função: LIS Ir para o elemento inicial
*
*  Descrição
*     Torna corrente o primeiro elemento da lista.
*     Faz nada se a lista está vazia.
*
*  Parâmetros
*     pLista - ponteiro para a lista a manipular
*     ppValor - ponteiro para o ponteiro do valor
*
*  Condições de retorno
*     - LIS_CondRetOK
*
*  Assertivas de entrada
*     - Valem as assertivas estruturais da lista duplamente encadeada com cabeça.
*
*  Assertivas de saída
*     - Valem as assertivas estruturais da lista duplamente encadeada com cabeça.
*     - Corrente aponta para o primeiro elemento da lista.
*
***********************************************************************/
   LIS_tpCondRet LIS_IrInicioLista(LIS_tppLista pLista);

/***********************************************************************
*
*  Função: LIS Ir para o elemento final
*
*  Descrição
*     Torna corrente o elemento final da lista.
*     Faz nada se a lista está vazia.
*
*  Parâmetros
*     pLista - ponteiro para a lista a manipular
*
*  Condições de retorno
*     - LIS_CondRetOK
*
*  Assertivas de entrada
*     - Valem as assertivas estruturais da lista duplamente encadeada com cabeça.
*
*  Assertivas de saída
*     - Valem as assertivas estruturais da lista duplamente encadeada com cabeça.
*     - Corrente aponta para o último elemento da lista.
*
***********************************************************************/
   LIS_tpCondRet LIS_IrFinalLista(LIS_tppLista pLista);

/***********************************************************************
*
*  Função: LIS Avançar elemento
*
*  Descrição
*     Avança o elemento corrente numElem elementos na lista
*     Se numElem for positivo avança em direção ao final
*     Se numElem for negativo avança em direção ao início
*     numElem pode ser maior do que o númro de elementos existentes na
*               direção desejada
*     Se numElem for zero somente verifica se a lista está vazia
*
*  Parâmetros
*     pLista  - ponteiro para a lista a ser manipulada
*     numElem - o número de elementos a andar
*
*  Condições de retorno
*     CondRetOK         - se numElem elementos tiverem sido andados
*     CondRetFimLista   - se encontrou o fim da lista antes de andar numElem
*                         elementos
*     CondRetListaVazia - se a lista está vazia
*
*  Assertivas de entrada
*     - Valem as assertivas estruturais da lista duplamente encadeada com cabeça.
*
*  Assertivas de saída
*     - Valem as assertivas estruturais da lista duplamente encadeada com cabeça.
*     - Se antigo corrente é o último, então o corrente permanece o mesmo.
*     - Se o antigo corrente não é o último, então o corrente será o próximo do
*       antigo corrente.
*     - Se a lista está vazia, então corrente permanece o mesmo.
*
***********************************************************************/
   LIS_tpCondRet LIS_AvancarElementoCorrente(LIS_tppLista pLista,
                                              int numElem);

/***********************************************************************
*
*  Função: LIS Procurar elemento contendo valor
*
*  Descrição
*     Procura o elemento que referencia o valor dado.
*
*  Parâmetros
*     pLista   - ponteiro para a lista onde procura
*     pValor   - ponteiro para o ponteiro do valor procurado
*
*  Condições de retorno
*     LIS_CondRetOK  - se encontrou.
*                      O elemento corrente é o primeiro elemento do
*                      elemento corrente inclusive para o fim da lista
*                      e que contém o ponteiro procurado
*
*     LIS_CondRetNaoEncontrou - se o ponteiro não foi encontrado
*                      O elemento corrente continua o mesmo
*     LIS_CondRetListaVazia   - se a lista estiver vazia
*
*  Assertivas de entrada
*     - Valem as assertivas estruturais da lista duplamente encadeada com cabeça.
*
*  Assertivas de saída
*     - Valem as assertivas estruturais da lista duplamente encadeada com cabeça.
*     - Se valor for encontrado então o corrente será o elemento que contém este valor.
*     - Se valor não for encontrado então o corrente permanece o mesmo.
*
***********************************************************************/
   LIS_tpCondRet LIS_ProcurarValor(LIS_tppLista pLista,
                                    void *pValor);

/***********************************************************************
*
*  Função: LIS Esta Vazia
*
*  Descrição
*     Checa se a lista está vazia
*
*  Parâmetros
*     pLista    - ponteiro para a lista onde procura
*     pResposta - ponteiro para valor de resposta
*
*  Retorno por referência
*    pResposta  - 1 se a lista estiver vazia e 0 caso contrário.
*
*  Condições de retorno
*     LIS_CondRetOK  - Elemento corrente está nulo, indicando que a lista esta vazia
*
*  Assertivas de entrada
*     - Valem as assertivas estruturais da lista duplamente encadeada com cabeça.
*
*  Assertivas de saída
*     - Valem as assertivas estruturais da lista duplamente encadeada com cabeça.
*     - Resposta será 0 ou 1.
*
***********************************************************************/
    LIS_tpCondRet LIS_EstaVazia(LIS_tppLista pLista,
                                              int *pResposta);

/***********************************************************************
*
*  Função: LIS Num Elementos
*
*  Descrição
*     Pega o numero de elementos contido na lista
*
*  Parâmetros
*     pLista        - ponteiro para a lista onde procura
*     pNumElementos - ponteiro que ira armazenar o numero de elementos da lista
*
*  Retorno por referência
*     pNumElementos - O número de elementos da lista.
*
*  Condições de retorno
*     LIS_CondRetOK
*
*  Assertivas de entrada
*     - Valem as assertivas estruturais da lista duplamente encadeada com cabeça.
*
*  Assertivas de saída
*     - Valem as assertivas estruturais da lista duplamente encadeada com cabeça.
*
***********************************************************************/

	LIS_tpCondRet LIS_NumELementos(LIS_tppLista pLista ,
                                      int *pNumElementos);


   /***********************************************************************
   *
   *  Função: LIS Altera Valor
   *
   *  Descrição
   *     Altera o valor do elemento corrente
   *
   *  Parâmetros
   *     ppLista - ponteiro para a lista onde procura
   *     pValor - ponteiro que armazena o novo valor
   *
   *
   *  Condições de retorno
   *     LIS_CondRetOK
   *
   *  Assertivas de entrada
   *     - Valem as assertivas estruturais da lista duplamente encadeada com cabeça.
   *
   *  Assertivas de saída
   *     - Valem as assertivas estruturais da lista duplamente encadeada com cabeça.
   *
   ***********************************************************************/
   LIS_tpCondRet LIS_AlterarValor(LIS_tppLista ppLista, void * pValor);

#ifdef _DEBUG

   /***********************************************************************
   *
   *  Função: LIS Total espaço alocado
   *
   *  Descrição
   *     Pega o total de espaço alocado pela lista
   *
   *  Parâmetros
   *     pLista              - ponteiro para a lista onde procura
   *     pTotalEspacoAlocado - ponteiro que ira armazenar o total de espaço alocado
   *
   *  Retorno por referência
   *     pTotalEspacoAlocado - O total de espaço alocado.
   *
   *  Condições de retorno
   *     LIS_CondRetOK
   *
   *  Assertivas de entrada
   *     - Valem as assertivas estruturais da lista duplamente encadeada com cabeça.
   *
   *  Assertivas de saída
   *     - Valem as assertivas estruturais da lista duplamente encadeada com cabeça.
   *
   ***********************************************************************/
   LIS_tpCondRet LIS_TotalEspacoAlocado(LIS_tppLista pLista, unsigned long *pTotalEspacoAlocado);
#endif

/***********************************************************************/
#undef LISTA_EXT

/********** Fim do módulo de definição: LIS  Lista duplamente encadeada **********/

#else
#endif
