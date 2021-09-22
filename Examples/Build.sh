for file in ./*.cpp
do
    g++ -Wall -std=c++17 -I ../Source -c "$file"
    g++ $(basename "$file" .cpp).o ../ParadoxLanguageParser.o -o $(basename "$file" .cpp).elf
done