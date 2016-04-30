//-------------------------------------------
/**
	File:Lexer.hpp
	System:RPAL Parser 
	Component Name: Lexical Analyser Header File
	Language: C++
	Author: Annu Sharma (36731206)
	Description: This file has class definition
	for the Lexer
**/
//-------------------------------------------

#ifndef LEXER_HPP
#define LEXER_HPP

#include <fstream>
#include <string>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
using namespace std;

class Lexer{
private:
	ifstream fileptr;
public:
	void start(string File);
	string getNextToken();
	void stop();
};

#endif /* LEXER_HPP */
