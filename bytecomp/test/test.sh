nasm -f macho64 test.s
nasm -f macho64 util.s
gcc -o a.out test.o util.o
