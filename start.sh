rm -rf build
mkdir build && cd build
cmake ..
make clean && make -j4

echo "编译完成！"
echo ""

./smallkv


