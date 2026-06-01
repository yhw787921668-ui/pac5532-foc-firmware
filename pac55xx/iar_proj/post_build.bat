@echo off

rem %1 = $TARGET_PATH$ which is path\project_name.out
rem %2 = $TARGET_BPATH$ which is path\project_name with no extension

echo executing post_build.bat...

IF %1.==. GOTO ParameterError
IF %2.==. GOTO ParameterError

rem Insert crc16 into .out ELF FIle
ielftool --fill 0xFF;__vector_table-__crc16_checksum --checksum __crc16_checksum:2,crc16,0x0;__vector_table-__crc16_checksum --verbose %1 %1

rem Convert .out ELF to .bin
ielftool --bin %1 %2.bin
    
rem Normal Exit
exit

:ParameterError
echo !!!!! ERROR: 1st parameter must be the path to the project directory
echo !!!!! ERROR: 2nd parameter must be $TARGET_PATH$
exit 1


