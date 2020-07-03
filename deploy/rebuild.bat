@echo off

cd /d .\
echo %cd%


REM set /p version=input version(like 1.2.3):


rmdir /S /Q .\files\
mkdir files
echo.
echo copying files...
copy /Y ..\output\build-tracker-Desktop_Qt_5_12_7_MSVC2017_32bit-Release\release\tracker.exe .\files\
xcopy /Y /E ..\map .\files\map\


set PATH=D:\Qt\Qt5.12.7\5.12.7\msvc2017\bin;%PATH%
set PATH=C:\Program Files (x86)\Inno Setup 5;%PATH%


echo.
echo qt deploying...
windeployqt .\files\tracker.exe

echo.
echo iss compiling...
REM compil32 /cc ".\issproj.iss"
iscc /Qp /O".\setup\" /F"tracker_%version%_setup" /DMyAppVersion="%version%" ".\issproj.iss"
 
set /p run=Run the setup(Y/N)?
if "%run%"=="Y" (
  goto runsetup
)
if "%run%"=="y" (
  goto runsetup
)

goto finished

:runsetup
 start .\setup\tracker_%version%_setup
:finished
