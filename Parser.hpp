//-------------------------------------------
/**
	File:Parser.hpp
	System:RPAL Parser
	Component Name: Parser Header File
	Language: C++
	Author: Annu Sharma (36731206)
	Description: Contains Parser Class definitions 
	as well as Abstract Syntax Tree structure 
	definition.
**/
//-------------------------------------------

#ifndef PARSER_HPP
#define PARSER_HPP
#include "Lexer.hpp"
#include "string"
#include "set"
#include "stack"
#include "sstream"
using namespace std;

typedef struct Node{
	string nodeName;
	Node* left;
	Node* right;
}Node;

class Parser{
	private:
		Lexer lexObj;
		string nextToken;
		string updatedToken;
		stack<Node*> treeStack;
		int ASTFlag;
	public:
		void printHelper(Node* root);
		void start(int astFlag, string File);
		Node* stop();
		void procE();
		void procD();
		void procDr();
		void procT();
		void procTa();
		void procTc();
		void procB();
		void procBt();
		void procBs();
		void procBp();
		void procA();
		void procAt();
		void procAf();
		void procAp();
		void procR();
		void procRn();
		void procDa();
		void procDb();
		bool isIdentifier();
		bool isString();
		bool isInteger();
		void procEw();
		int procVb();
		void procVl(int flag);
		void read(string root);
		bool firstSetRn();
		bool firstSetDr();
		void buildAST(string root,int numToPop);
		void getNextToken();
		void printAST(Node* root,int numDots);
};

#endif /* PARSER_HPP */