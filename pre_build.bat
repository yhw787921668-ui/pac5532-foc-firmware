@echo off

echo executing pre_build.bat...

IF %1.==. GOTO ParameterError
IF %2.==. GOTO ParameterError


IF EXIST %1\..\..\..\build_date_time.bat (
    %1\..\..\..\build_date_time.bat %1 %2
) ELSE (
    echo %1\..\..\..\build_date_time.bat doesn't exist
)   
    
rem Normal Exit
exit

:ParameterError
echo !!!!! ERROR: 1st parameter must be the path to the project directory
echo !!!!! ERROR: 2nd parameter must be one of {coide, iar, keil, eclipse}
exit 1