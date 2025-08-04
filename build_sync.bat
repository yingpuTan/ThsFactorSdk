 @echo off
echo 开始编译ThsFactorSdk同步接口...

REM 创建输出目录
if not exist "build" mkdir build
if not exist "build\bin" mkdir build\bin
if not exist "build\lib" mkdir build\lib

REM 设置编译器
set CC=g++
set CFLAGS=-std=c++11 -Iinclude -O2 -Wall

REM 编译同步接口库
echo 编译同步接口库...
%CC% %CFLAGS% -c src/ThsFactorSdkSync.cpp -o build/ThsFactorSdkSync.o
if %ERRORLEVEL% neq 0 (
    echo 编译同步接口库失败！
    pause
    exit /b 1
)

REM 链接同步接口库
echo 链接同步接口库...
%CC% -shared -o build/lib/ThsFactorSdkSync.dll build/ThsFactorSdkSync.o -Llib -lThsFactorSdk
if %ERRORLEVEL% neq 0 (
    echo 链接同步接口库失败！
    pause
    exit /b 1
)

REM 编译示例程序
echo 编译示例程序...
%CC% %CFLAGS% demo/main_sync.cpp -o build/bin/demo_sync.exe -Lbuild/lib -lThsFactorSdkSync -Llib -lThsFactorSdk
if %ERRORLEVEL% neq 0 (
    echo 编译示例程序失败！
    pause
    exit /b 1
)

REM 复制依赖DLL
echo 复制依赖DLL...
copy dll\*.dll build\bin\ >nul 2>&1
copy build\lib\ThsFactorSdkSync.dll build\bin\ >nul 2>&1

echo 编译完成！
echo 可执行文件位置: build\bin\demo_sync.exe
echo.
echo 运行示例程序:
echo cd build\bin
echo demo_sync.exe
pause