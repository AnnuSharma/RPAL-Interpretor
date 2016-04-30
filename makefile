all:
	g++ main.cpp Parser.cpp Lexer.cpp ASTStandardiser.cpp CSE.cpp -o p2
cl:
	rm -f *.o p2
