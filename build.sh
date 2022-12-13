rm -rf build
cmake -S . -B build
cmake --build build --target all -- -j4

echo "编译完成！"
echo ""

