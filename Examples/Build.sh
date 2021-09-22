g++ -Wall -std=c++17 -I ../Source -c CountryExample.cpp
for file in ./*.o
do
    g++ "$file" ../ParadoxLanguageParser.o -o $(basename "$file" .o).elf
done