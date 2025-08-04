 #!/bin/bash

echo "开始编译ThsFactorSdk同步接口..."

# 创建输出目录
mkdir -p build
mkdir -p build/bin
mkdir -p build/lib

# 设置编译器
CC=g++
CFLAGS="-std=c++11 -Iinclude -O2 -Wall -fPIC"

# 编译同步接口库
echo "编译同步接口库..."
$CC $CFLAGS -c src/ThsFactorSdkSync.cpp -o build/ThsFactorSdkSync.o
if [ $? -ne 0 ]; then
    echo "编译同步接口库失败！"
    exit 1
fi

# 链接同步接口库
echo "链接同步接口库..."
$CC -shared -o build/lib/libThsFactorSdkSync.so build/ThsFactorSdkSync.o -Llib -lThsFactorSdk
if [ $? -ne 0 ]; then
    echo "链接同步接口库失败！"
    exit 1
fi

# 编译示例程序
echo "编译示例程序..."
$CC $CFLAGS demo/main_sync.cpp -o build/bin/demo_sync -Lbuild/lib -lThsFactorSdkSync -Llib -lThsFactorSdk
if [ $? -ne 0 ]; then
    echo "编译示例程序失败！"
    exit 1
fi

# 复制依赖库
echo "复制依赖库..."
cp lib/*.so build/bin/ 2>/dev/null || true
cp build/lib/libThsFactorSdkSync.so build/bin/ 2>/dev/null || true

echo "编译完成！"
echo "可执行文件位置: build/bin/demo_sync"
echo ""
echo "运行示例程序:"
echo "cd build/bin"
echo "./demo_sync"