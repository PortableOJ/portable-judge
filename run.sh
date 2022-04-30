pushd /usr/include/x86_64-linux-gnu/c++/9/bits
sudo g++ -std=c++17 stdc++.h
popd

./main
#g++ -o main -std=c++17 src/main.cpp -lpthread -lseccomp
#valgrind -q --tool=memcheck ./main
#rm main
