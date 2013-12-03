REM executa os testes do módulo LISTA

del estatisticas.estat

..\DEBUG\TRAB4-1.EXE      /s..\src\teste_contador.script     /lteste_contador.log     /aestatisticas

..\DEBUG\TRAB4-1.EXE      /s..\src\teste_controle_espaco.script     /lteste_controle_espaco.log     /aestatisticas

..\DEBUG\TRAB4-1.EXE      /s..\src\teste_deturpa_grafo.script     /lteste_deturpa_grafo.log     /aestatisticas

..\ferramentas\CompileBanner     /c"Estatisticas acumuladas"
..\ferramentas\exbestat          /eestatisticas

PAUSE
