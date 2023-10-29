@echo off

..\src\picolisp ..\lib.l ..\lib\misc.l src\main_miniPicoLisp.l -bye

..\src\picolisp ..\lib.l ..\lib\misc.l src\apply.l -bye

..\src\picolisp ..\lib.l ..\lib\misc.l src\flow_miniPicoLisp.l -bye

..\src\picolisp ..\lib.l ..\lib\misc.l src\sym_miniPicoLisp.l -bye

..\src\picolisp ..\lib.l ..\lib\misc.l src\io_miniPicoLisp.l -bye

..\src\picolisp ..\lib.l ..\lib\misc.l ..\lib\pilog.l friends.l -bye

echo ALL TESTS PASSED
