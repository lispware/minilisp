echo off
set "libuv=..\libuv-1.47.0"

if exist "%libuv%" (
        echo Found libuv
) else (
        git clone -b libuv-1.47.0 https://github.com/ckkashyap/WindowsBinaries.git %libuv%
)
