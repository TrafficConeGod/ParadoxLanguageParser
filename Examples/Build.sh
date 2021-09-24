
for file in ./*.cpp
do
    g++ -Wall -std=c++17 -I ../Source -c "$file" $1
    g++ $(basename "$file" .cpp).o $(find ../ -maxdepth 1 -name '*.o') -o $(basename "$file" .cpp).elf
done