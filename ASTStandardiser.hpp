//-------------------------------------------
/**
	File:ASTStandardiser.hpp
	System:RPAL Parser
	Component Name: Header for AST Standardiser 
	Language: C++
	Author: Annu Sharma (36731206)
	Description: This file has class definitions for AST Standardiser
**/
//-------------------------------------------
#ifndef AST_STANDARDISER_HPP
#define AST_STANDARDISER_HPP

#include "Parser.hpp"
#include <fstream>
#include <string>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
using namespace std;

class ASTStandardiser{
private:
	Node* rootNode;
public:
	void start(Node* root);
	void postOrderTraverse(Node* rootNode);
	bool nodeToStandardise(string nodeName);
};


#endif /* AST_STANDARDISER_HPP */
