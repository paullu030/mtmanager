FROM mcr.microsoft.com/windows/nanoserver:1809

COPY x64/Debug/Manager.exe C:\\Manager.exe
COPY MT5APIManager.dll C:\\MT5APIManager.dll
COPY MT5APIManager64.dll C:\\MT5APIManager64.dll
COPY vcruntime140_1d.dll C:\\vcruntime140_1d.dll
COPY ucrtbased.dll C:\\ucrtbased.dll
COPY msvcp140d.dll C:\\msvcp140d.dll
COPY vcruntime140d.dll C:\\vcruntime140d.dll


CMD ["\\Manager.exe"]
