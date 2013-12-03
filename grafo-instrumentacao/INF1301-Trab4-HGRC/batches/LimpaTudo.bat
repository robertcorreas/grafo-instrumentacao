REM  Elimina todos os arquivos gerados

if exist resultadosDosTestes goto removeDir

del *.log
del *.estat

:removeDir

rmdir /s /q resultadosDosTestes


