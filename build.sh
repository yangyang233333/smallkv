rm -rf build
rm -rf output

start=`date +%s`
cmake -S . -B build
cmake --build build --target all -- -j$(nproc)
end=`date +%s`
time=`echo $start $end | awk '{print $2-$1}'`

echo "---------------------------------------"
echo "编译完成！编译总耗时(cmake+make)：${time}s"
echo "---------------------------------------"