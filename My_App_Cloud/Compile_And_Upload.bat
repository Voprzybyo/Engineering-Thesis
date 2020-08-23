set PATH_TO_MAKE="C:\Program Files (x86)\GnuWin32\bin\make.exe"

for %%i IN (SRC/*.c) DO (
    call ..\..\Framework\Tools\Compile_App.bat %%i
)

rem Remove unused files
del Output\*.o Output\*.map Output\*.elf Output\*.hex

for %%i IN (Output/*.bin) DO (
    call ..\..\Framework\Tools\Upload_App.bat %%i
)


echo;
echo;
@echo **********************************************
@echo ALL DONE!
@echo **********************************************

pause