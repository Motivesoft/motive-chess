@echo off
\Archive\motive-chess-uci-1.0.0.8.exe -input alltests.txt -logfile 1008.log
echo:
..\x64\Release\motive-chess-uci.exe -input alltests.txt -logfile 1020.log
