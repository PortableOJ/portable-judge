g++ -o main -std=c++17 src/main.cpp -lpthread -lseccomp
valgrind -q --tool=memcheck ./main
rm main
