clang-format -i -style="{BasedOnStyle: Mozilla, IndentWidth: 4}" src/*.cpp src/include/*.h test/*.cpp

if [ ! -d build ]; then
    mkdir build
fi
pushd build
cmake -B . -S ../
make -j8 || exit 1
for file in ./test/*; do
    if [ -f $file ] && [ -x $file ]; then
        $file
    fi
done
popd
