@echo off
REM ============================================================
REM  Bien dich web server Quan Ly Khach San (PBL2)
REM  Yeu cau: g++ (MinGW-w64 / MSYS2) co trong PATH
REM ============================================================
setlocal
set OUT=hotelweb.exe

g++ -std=c++17 -O2 -Wall -static -static-libgcc -static-libstdc++ ^
    server\main.cpp server\store.cpp -o %OUT% -lws2_32 -lwsock32

if %errorlevel%==0 (
    echo.
    echo [OK] Da tao %OUT%
    echo     Chay:  hotelweb.exe        ^(cong mac dinh 8080^)
    echo     Hoac:  hotelweb.exe 9000   ^(chi dinh cong^)
) else (
    echo.
    echo [LOI] Bien dich that bai.
)
endlocal
