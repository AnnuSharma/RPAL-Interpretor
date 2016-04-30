//-------------------------------------------
/**
	File:main.cpp
	System:RPAL Parser
	Component Name: Main Function
	Language: C++
	Author: Annu Sharma (36731206)
	Description: Driver function for Parser
**/
//-------------------------------------------

#include "CSE.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "ASTStandardiser.hpp"

int main(int argc, char* argv[])
{	
	Parser parseObj;
	int astFlag;
	if(argc < 2)
	{
		cout<<"Please provide the input file name ";
		return(0);
	}
	if(argc == 3)
	{
		if(strcmp(argv[1],"-ast") == 0)
		{
			astFlag = 1;
			parseObj.start(astFlag,string(argv[2]));
		}
		else
		{
			astFlag = 0;
			parseObj.start(astFlag,string(argv[2]));
		}
	}
	else
	{
		astFlag = 0;
		parseObj.start(astFlag,string(argv[1]));
	}
	parseObj.procE();
	Node* root;
	root = parseObj.stop();
	if(astFlag == 0)
	{
		ASTStandardiser standardiserObj;
		standardiserObj.start(root);
		CSE evaluatorObj;
		evaluatorObj.start(root);
		cout<<endl;
	}
	return(1);
}