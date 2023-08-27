Get-ChildItem -File -Recurse | Where-Object { $_.Extension -notin ('.sh', '.ps1') } | Remove-Item -Force ; cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release .. ; mingw32-make
