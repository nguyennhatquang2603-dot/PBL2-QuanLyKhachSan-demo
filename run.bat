@echo off
REM Chay web server; tu bien dich neu chua co file exe
if not exist hotelweb.exe call build.bat
if not exist hotelweb.exe exit /b 1
start "" http://localhost:8080
hotelweb.exe %1
