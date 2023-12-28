gcc -Os -I /usr/include/python3.11/ -o hello test.c -lpython3.11 -lpthread -lm -lutil -ldl
riscv64-unknown-linux-gnu-gcc test.c -I /usr/include/python3.11/ -lpython3.11