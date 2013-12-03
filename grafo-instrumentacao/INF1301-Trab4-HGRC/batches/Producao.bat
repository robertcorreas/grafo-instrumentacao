REM executa os testes do módulo LISTA

del estatisticas.estat

..\RELEASE\TRAB4-2.EXE      /s..\src\teste_grafo.script     /lteste_grafo.log     /aestatisticas

..\ferramentas\CompileBanner     /c"Estatisticas acumuladas"
..\ferramentas\exbestat          /eestatisticas

PAUSE
