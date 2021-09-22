g++ -Wall -std=c++17 -c $(find Source -name '*.cpp')
ld -relocatable $(find -name '*.o') -o ParadoxLanguageParser.o