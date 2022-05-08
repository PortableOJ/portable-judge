pushd /usr/include/x86_64-linux-gnu/c++
for file in ./*
do
    if test -d ${file}
    then
        pushd ${file}/bits
        g++ -O2 -std=c++17 stdc++.h
        popd
    fi
done
popd

./main

#g++ -o main -std=c++17 src/main.cpp -lpthread -lseccomp
#valgrind -q --tool=memcheck ./main
#rm main
