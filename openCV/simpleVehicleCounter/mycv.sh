g++ -ggdb `pkg-config --cflags opencv` $1 `pkg-config --libs opencv`
./a.out $2
