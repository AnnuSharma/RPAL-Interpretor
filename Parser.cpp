//-------------------------------------------
/**
	File:Parser.cpp
	System:RPAL Parser
	Component Name: Parser
	Language: C++
	Author: Annu Sharma (36731206)
	Description: This class repeatedly nvokes the 
	lexer and parses through the given input while
	building the Abstract Syntax Tree and prints 
	the resulting Abstract Syntax Tree when invoked 
	with -ast option.
**/
//-------------------------------------------

#include "Parser.hpp"

void Parser::printHelper(Node* root)
{
	printAST(root,0);
}

void Parser::start(int astFlag, string File)
{
	lexObj.start(File);
	ASTFlag = astFlag;
}

void Parser::getNextToken()
{
	int flag;
	if(updatedToken.empty())
	{
		nextToken = lexObj.getNextToken();
		flag = 0;
	}
	else
	{
		nextToken = updatedToken;
		updatedToken.clear();
		flag = 1;
	}
}

void Parser::procE()
{
	getNextToken();
	int countTree = 0;
	if(nextToken == "let")
	{
		read(nextToken);
		procD();
		getNextToken();
		if(nextToken == "in")
		{
			read(nextToken);
			procE();
			buildAST("let",2);
		}
		else
		{
			cout<<"Error in parsing :"<<"Expected token 'in'"<<endl;
			return;
		}

	}
	else if(nextToken == "fn")
	{
		read(nextToken);
		countTree = procVb();
		if(countTree == 0)
		{
			cout<<"Unexpected token :"<<nextToken<<endl;
			return;
		}
		getNextToken();
		if(nextToken == ".")
		{
			read(nextToken);
			procE();
			buildAST("lambda",countTree+1);
		}
		else
		{
			cout<<"Error in parsing in E():"<<"Expected '.'"<<endl;
			return;
		}
	}
	else 
	{
		updatedToken = nextToken;
		procEw();
	}
}

void Parser::procEw()
{
	procT();
	getNextToken();
	if(nextToken == "where")
	{
		read(nextToken);
		procDr();
		buildAST("where",2);
	}
	else 
	{
		updatedToken = nextToken;
	}
}

int Parser::procVb()
{
	int count = 0;
	getNextToken();
	do{
		if(isIdentifier())
		{
			read(nextToken);
		}
		else if(nextToken == "(")
		{
			read(nextToken);
			getNextToken();
			if(nextToken == ")")
			{
				read(nextToken);
				buildAST("()",0);
			}
			else
			{	
				updatedToken = nextToken;
				procVl(0);
				getNextToken();
				if(nextToken == ")")
				{
					read(nextToken);
				}
				else
				{
					cout<<"Unexpected token in Vb():"<<nextToken<<endl;
					return(0);
				}
			}
		}
		else
		{
			cout<<"Unexpected token in Vb():"<<nextToken<<endl;
			return(0);
		}
		count++;
		getNextToken();
	}while(isIdentifier() || nextToken == "(");
	updatedToken = nextToken;
	return count;
}

void Parser::procVl(int flag)
{
	if(flag == 0)
	{
		int countTree = 0;
		getNextToken();
		if(isIdentifier())
		{
			read(nextToken);
			getNextToken();
			countTree++;
		}
		else
		{
			cout<<"Unexpected token in Vl():"<<nextToken<<endl;
			return;
		}
		while(nextToken == ",")
		{
			read(nextToken);
			getNextToken();
			if(isIdentifier())
			{
				read(nextToken);
				getNextToken();
				countTree++;
			}
			else
			{
				cout<<"Unexpected token in Vl():"<<nextToken<<endl;
				return;
			}
		}
		updatedToken = nextToken;
		if(countTree > 1)
		{
			buildAST(",",countTree);
		}
	}
	else
	{
		int countTree = 1;
		while(nextToken == ",")
		{
			read(nextToken);
			getNextToken();
			if(isIdentifier())
			{
				read(nextToken);
				getNextToken();
				countTree++;
			}
			else
			{
				cout<<"Unexpected token in Vl():"<<nextToken<<endl;
				return;
			}
		}
		updatedToken = nextToken;
		if(countTree > 1)
		{
			buildAST(",",countTree);
		}
	}
}

Node* Parser::stop()
{
	lexObj.stop();
	if(ASTFlag == 1)
	{
		Node *root = treeStack.top();
		printAST(root,0);
		return NULL;
	}
	else
	{
		Node *root = treeStack.top();
		return root;
	}
}

void Parser::read(string nextToken)
{
	//add tree node to stack of trees
	if(nextToken == "true" || nextToken == "false" || nextToken == "nil" || nextToken == "dummy")
	{
		nextToken.insert(0,1,'<');
		nextToken.insert(nextToken.length(),1,'>');
		buildAST(nextToken,0);
	}
	else if(isIdentifier() || isInteger() || isString())
	{
		nextToken.insert(0,1,'<');
		nextToken.insert(nextToken.length(),1,'>');
		if(isIdentifier())
		{
			nextToken.insert(1,"ID:");
		}
		if(isInteger())
		{
			nextToken.insert(1,"INT:");
		}
		if(isString())
		{
			nextToken.insert(1,"STR:");
		}
		buildAST(nextToken,0);
	}
}

void Parser::procT()
{
	int countTree = 0;
	procTa();
	getNextToken();	
	while(nextToken == ",")
	{
		read(nextToken);
		procTa();
		countTree++;
		getNextToken();
	}
	if(countTree != 0)
	{
		buildAST("tau",countTree+1);
	}
	updatedToken = nextToken;
}

void Parser::procTa()
{
	procTc();
	getNextToken();
	while(nextToken == "aug")
	{
		read(nextToken);
		procTc();
		buildAST("aug",2);
		getNextToken();
	}
	updatedToken = nextToken;

}

void Parser::procTc(){
	procB();
	getNextToken();
	if(nextToken == "->")
	{	
		read(nextToken);
		procTc();
		getNextToken();
		if(nextToken == "|")
		{
			read(nextToken);
			procTc();
			buildAST("->",3);
		}
		else
		{
			cout<<"Error in parsing :"<<"Expected '|'"<<endl;
			return;
		}
	}
	else
	{
		updatedToken = nextToken;
	}
}

void Parser::procB()
{
	procBt();
	getNextToken();
	while(nextToken == "or")
	{
		read(nextToken);
		procBt();
		buildAST("or",2);
		getNextToken();
	}
	updatedToken = nextToken;
}

void Parser::procBt()
{
	procBs();
	getNextToken();
	while(nextToken == "&")
	{
		read(nextToken);
		procBs();
		buildAST("&",2);
		getNextToken();
	}
	updatedToken = nextToken;
}

void Parser::procBs()
{
	getNextToken();
	if(nextToken == "not")
	{
		read(nextToken);
		procBp();
		buildAST("not",1);
	}
	else
	{
		updatedToken = nextToken;
		procBp();
	}
}

void Parser::procBp()
{
	procA();
	getNextToken();
	if(nextToken == "gr" || nextToken == ">")
	{
		read(nextToken);
		procA();
		buildAST("gr",2);
	}
	else if(nextToken == "ge" || nextToken == ">=")
	{
		read(nextToken);
		procA();
		buildAST("ge",2);
	}
	else if(nextToken == "ls" || nextToken == "<")
	{
		read(nextToken);
		procA();
		buildAST("ls",2);
	}
	else if(nextToken == "le" || nextToken == "<=")
	{
		read(nextToken);
		procA();
		buildAST("le",2);
	}
	else if(nextToken == "eq")
	{
		read(nextToken);
		procA();
		buildAST("eq",2);
	}
	else if(nextToken == "ne")
	{
		read(nextToken);
		procA();
		buildAST("ne",2);
	}
	else
	{
		updatedToken = nextToken;
	}
}

void Parser::procA()
{
	string tempToken;
	getNextToken();
	if(firstSetRn())
	{
		updatedToken = nextToken;
		procAt();
		getNextToken();
		while(nextToken == "+" || nextToken == "-")
		{
			tempToken = nextToken;
			read(nextToken);
			procAt();
			buildAST(tempToken,2);
			getNextToken();
		}
		updatedToken = nextToken;
	}
	else if(nextToken == "+" || nextToken == "-")
	{
		while(nextToken == "+" || nextToken == "-")
		{
			tempToken = nextToken;
			read(nextToken);
			procAt();
			if(tempToken == "-")
			{
				buildAST("neg",1);
			}
			getNextToken();
		}
		updatedToken = nextToken;
	}
	else
	{
		cout<<"Unexpected token in A():"<<nextToken<<endl;
		return;
	}
}

void Parser::procAt()
{
	procAf();
	getNextToken();
	while(nextToken == "*" || nextToken == "/")
	{
		read(nextToken);
		if(nextToken == "*")
		{	
			procAf();
			buildAST("*",2);
		}
		else
		{
			procAf();
			buildAST("/",2);
		}
		getNextToken();
	}
	updatedToken = nextToken;
}

void Parser::procAf()
{
	procAp();
	getNextToken();
	if(nextToken == "**")
	{
		read(nextToken);
		procAf();
		buildAST("**",2);
	}
	else
	{
		updatedToken = nextToken;
	}
}

void Parser::procAp()
{
	procR();
	getNextToken();
	while(nextToken == "@")
	{
		read(nextToken);
		getNextToken();
		if(isIdentifier())
		{
			read(nextToken);
			procR();
			buildAST("@",3);
			getNextToken();
		}
		else
		{
			cout<<"Unexpected token in Ap():"<<nextToken<<endl;
			return;
		}
	}
	updatedToken = nextToken;
}
void Parser::procR()
{
	procRn();
	getNextToken();
	while(firstSetRn())
	{
		updatedToken = nextToken;
		procRn();
		buildAST("gamma",2);
		getNextToken();
	}
	updatedToken = nextToken;
}

void Parser::procRn()
{
	getNextToken();
	if(nextToken == "true")
	{
		read("true");
	}
	else if(nextToken == "false")
	{
		read("false");
	}
	else if(nextToken == "nil")
	{
		read("nil");
	}
	else if(nextToken == "(")
	{
		read(nextToken);
		procE();
		getNextToken();
		if(nextToken == ")")
		{
			read(nextToken);
		}
		else
		{
			cout<<"Expected ')'"<<endl;
			return;
		}
	}
	else if(nextToken == "dummy")
	{
		read("dummy");
	}
	else if(isIdentifier() || isInteger() || isString())
	{
		read(nextToken);
	}
	else
	{
		cout<<"Unexpected token in Rn():"<<nextToken<<endl;
		return;
	}
}

void Parser::procD()
{
	procDa();
	getNextToken();
	if(nextToken == "within")
	{
		read(nextToken);
		procD();
		buildAST("within",2);
	}
	else
	{
		updatedToken = nextToken;
	}
}

void Parser::procDa()
{
	int countTree = 0;
	procDr();
	getNextToken();
	while(nextToken == "and")
	{
		read(nextToken);
		procDr();
		countTree++;
		getNextToken();
	}
	if(countTree != 0)
	{
		buildAST("and",countTree+1);
	}
	updatedToken = nextToken;
}

void Parser::procDr() 
{
	getNextToken();
	if(nextToken == "rec")
	{
		read(nextToken);
		procDb();
		buildAST("rec",1);
	}
	else
	{   
		updatedToken = nextToken;
		procDb();
	}
}

void Parser::procDb()
{
	getNextToken();
	if(isIdentifier())
	{	
		read(nextToken);
		getNextToken();
		if(nextToken == "," || nextToken == "=")
		{	
			//first production
			read(nextToken);
			if(nextToken == "=")
			{
				procE();
				buildAST("=",2);
			}
			else
			{
				procVl(1);
				getNextToken();
				if(nextToken == "=")
				{
					read(nextToken);
					procE();
					buildAST("=",2);
				}
				else
				{
					cout<<"Expected '=' "<<endl;
					return;
				}
			}

		}
		else if(nextToken == "(" || isIdentifier())
		{
			//second production
			updatedToken = nextToken;
			int countTree=0;
			countTree = procVb();
			getNextToken();
			if(nextToken == "=")
			{
				read(nextToken);
				procE();
				buildAST("function_form",countTree+2);
			}
			else
			{
				cout<<"Expected \"=\" "<<endl;
				return;
			}
						
		}
		else
		{
			cout<<"Unexpected token in Db():"<<nextToken<<endl;
			return;
		}
	}
	else if(nextToken == "(")
	{
		read(nextToken);
		procD();
		getNextToken();
		if(nextToken == ")")
		{
			read(nextToken);
		}
		else
		{
			cout<<"Expected ')' "<<endl;
			return;
		}
	}
	else
	{
		cout<<"Unexpected token in Db():"<<nextToken<<endl;
		return;
	}
		
}

bool Parser::isIdentifier()
{
	//Checking if the identifier does not correspond to a keyword

	set<string> keywords;
	keywords.insert("let");
	keywords.insert("in");
	keywords.insert("fn");
	keywords.insert("where");
	keywords.insert("aug");
	keywords.insert("not");
	keywords.insert("gr");
	keywords.insert("ge");
	keywords.insert("ls");
	keywords.insert("le");
	keywords.insert("eq");
	keywords.insert("ne");
	keywords.insert("within");
	keywords.insert("and");
	keywords.insert("rec");
	keywords.insert("true");
	keywords.insert("false");
	keywords.insert("nil");
	keywords.insert("dummy");
	keywords.insert("or");

	if(nextToken[0] == 'A' || nextToken[0] == 'B' || nextToken[0] == 'C' || nextToken[0] == 'D' || nextToken[0] == 'E' || nextToken[0] == 'F' || 
			nextToken[0] == 'G' || nextToken[0] == 'H' ||  nextToken[0] == 'I' ||nextToken[0] == 'J' || nextToken[0] == 'K' || nextToken[0] == 'L' ||
			nextToken[0] == 'M' || nextToken[0] == 'N' || nextToken[0] == 'O' || nextToken[0] == 'P' || nextToken[0] == 'Q' || nextToken[0] == 'R' ||
			nextToken[0] == 'S' || nextToken[0] == 'T' || nextToken[0] == 'U' ||nextToken[0] == 'V' || nextToken[0] == 'W' || nextToken[0] == 'X' ||
			nextToken[0] == 'Y' || nextToken[0] == 'Z'|| nextToken[0] == 'a' || nextToken[0] == 'b' || nextToken[0] == 'c' || nextToken[0] == 'd' || 
			nextToken[0] == 'e' || nextToken[0] == 'f' || nextToken[0] == 'g' || nextToken[0] == 'h' || nextToken[0] == 'i' || nextToken[0] == 'j' || 
			nextToken[0] == 'k' || nextToken[0] == 'l' || nextToken[0] == 'm' || nextToken[0] == 'n' || nextToken[0] == 'o' || nextToken[0] == 'p' || 
			nextToken[0] == 'q' || nextToken[0] == 'r' || nextToken[0] == 's' || nextToken[0] == 't' || nextToken[0] == 'u' || nextToken[0] == 'v' || 
			nextToken[0] == 'w' || nextToken[0] == 'x' || nextToken[0] == 'y' || nextToken[0] == 'z')
	{
		if(keywords.find(nextToken) == keywords.end())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool Parser::isInteger()
{
	if(nextToken[0] == '0' || nextToken[0] == '1' || nextToken[0] == '2' || nextToken[0] == '3' || nextToken[0] == '4' || nextToken[0] == '5' ||
			nextToken[0] == '6' || nextToken[0] == '7' || 
			nextToken[0] == '8' || nextToken[0] == '9')
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Parser::isString()
{
	if(nextToken[0] == '\'')
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Parser::firstSetRn()
{
	set<string> firstRn;
	firstRn.insert("(");
	firstRn.insert("true");
	firstRn.insert("false");
	firstRn.insert("nil");
	firstRn.insert("dummy");
	if(firstRn.find(nextToken) != firstRn.end())
	{
		return true;
	}
	else 
	{
		if(isIdentifier() || isInteger() || isString())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

void Parser::buildAST(string root,int numToPop)
{
	if(numToPop == 0)
	{
		//build a single node and push into the stack
		Node *newNode = new Node();
		newNode->nodeName = root;
		newNode->left = NULL;
		newNode->right = NULL;
		treeStack.push(newNode);
	}
	else if(numToPop == 1)
	{
		//pop 'numToPop' trees and push one tree tree with root as 'root'
		Node* newNode = new Node();
		newNode->nodeName = root;
		newNode->left = NULL;
		newNode->right = NULL;
		Node* popNode = treeStack.top();
		newNode->left = popNode;
		treeStack.pop();
		treeStack.push(newNode);
	}
	else
	{
		Node* newNode = new Node();
		newNode->nodeName = root;
		newNode->left = NULL;
		newNode->right = NULL;
		stack<Node*> tempStack;
		int numTemp = numToPop;
		while(numTemp> 0)
		{
			Node* popNode = treeStack.top();
			tempStack.push(popNode);
			treeStack.pop();
			numTemp = numTemp -1;	
		}

		Node* popNode = tempStack.top();
		newNode->left = popNode;
		tempStack.pop();
		Node* tempTop = newNode;
		
		numToPop = numToPop-1;
		newNode = newNode->left;
		while(numToPop >0)
		{
			Node* popNode = tempStack.top();
			newNode->right = popNode;
			tempStack.pop();
			newNode = newNode->right;
			numToPop = numToPop -1;
		}
		treeStack.push(tempTop);
	}
	
}

void Parser::printAST(Node* root,int numDots)
{
	
	if(root != NULL)
	{
		int temp = numDots;
		while(temp > 0)
		{
			cout<<".";
			temp = temp-1;
		}
		cout<<root->nodeName<<endl;
		printAST(root->left,++numDots);
		printAST(root->right,--numDots);
	}

}
