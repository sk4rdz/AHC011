g++ solve.cpp -std=c++17 -D=LOCAL -Wall -Wextra -Wno-unknown-pragmas -g -ftrapv -fsanitize=address,undefined -fstack-protector-all -D_GLIBCXX_DEBUG 

if [ -e a.out ]; then
    ./a.out < input.txt
    rm -f a.out
fi
