# 单元测试

start=`date +%s`
./build/smallkv-unitest
end=`date +%s`
time=`echo $start $end | awk '{print $2-$1}'`

echo "--------------------------------------"
echo "         单元测试总耗时：${time}s        "
echo "--------------------------------------"