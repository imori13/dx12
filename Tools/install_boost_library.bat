@echo off
chcp 65001

SET /P ANSWER="boostライブラリをインストールしますか (所要時間:約1時間) (Y/N)？"
if /i {%ANSWER%}=={y} (goto :YES)
if /i {%ANSWER%}=={yes} (goto :YES)
echo 終了
pause
exit

:YES
call :TIMER START
git clone -b master --single-branch --depth 1 --recursive git@github.com:boostorg/boost.git "boost temporary file"
call :ERROR_CHECK

cd "boost temporary file"
call bootstrap.bat
call :ERROR_CHECK

b2.exe install -j8 toolset=msvc-14.2 threading=multi variant=debug,release link=static runtime-link=static,shared address-model=32 --libdir=C:\\Boost\\lib\\x64 install
call :ERROR_CHECK
b2.exe install -j8 toolset=msvc-14.2 threading=multi variant=debug,release link=static runtime-link=static,shared address-model=64 --libdir=C:\\Boost\\lib\\x32 install
call :ERROR_CHECK

goto EXIT
exit /b

:EXIT
call :TIMER STOP
echo.
call :TIMER PRINT
echo.
echo 正常に終了しました。
echo.
call :CLONE_REMOVE_CHECK
pause
exit

exit /b

:ERROR_CHECK
if errorlevel 1 call :ERROR
exit/b

:ERROR
call :TIMER STOP
echo.
call :TIMER PRINT
echo.
echo 異常終了しました。修正してもう一度実行してください。
echo.
pause
exit

:CLONE_REMOVE_CHECK
SET /P REM_FLAG="使用した一時ファイルを削除しますか？ (Y/N)？"
if /i {%REM_FLAG%}=={y} (goto :REMOVE_YES)
if /i {%REM_FLAG%}=={yes} (goto :REMOVE_YES)
exit /b

:REMOVE_YES
echo 削除中...
rd /s /q "boost temporary file"
echo 終了しました
pause
exit /b

:TIMER
if "%1" == "START"  goto BEGIN
if "%1" == "STOP"   goto END
if "%1" == "PRINT"  goto PRINT
goto EOF

:BEGIN
SET T=%TIME: =0%
SET H=%T:~0,2%
SET M=%T:~3,2%
SET S=%T:~6,2%
SET L=%T:~9,2%
set /a H=1%H%-100
set /a M=1%M%-100
set /a S=1%S%-100
goto EOF

:END
SET T1=%TIME: =0%
SET H1=%T1:~0,2%
SET M1=%T1:~3,2%
SET S1=%T1:~6,2%
SET L1=%T1:~9,2%
set /a H1=1%H1%-100
set /a M1=1%M1%-100
set /a S1=1%S1%-100
SET /a H2=H1-H
SET /a M2=M1-M
if %M2% LSS 0 set /a H2=H2-1
if %M2% LSS 0 set /a M2=M2+60
SET /a S2=S1-S
if %S2% LSS 0 set /a M2=M2-1
if %S2% LSS 0 set /a S2=S2+60
SET /a L2=L1-L
if %L2% LSS 0 set /a S2=S2-1
if %L2% LSS 0 set /a L2=L2+100
SET /a DPS=%H2%*3600+%M2%*60+%S2%
SET DPS2=%DPS%.%L2%
set DPS_STAMP=%H2%:%M2%:%S2%
set DPS_STAMP2=%DPS_STAMP%.%L2%
goto EOF

:PRINT 
echo 経過時間：%DPS_STAMP%
:EOF

exit /b