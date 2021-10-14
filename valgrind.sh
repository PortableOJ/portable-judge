g++ -o main -std=c++17 src/main.cpp -lpthread
valgrind -q --tool=memcheck ./main
rm main
