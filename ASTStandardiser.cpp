//-------------------------------------------
/**
	File:ASTStandardiser.cpp
	System:RPAL Parser
	Component Name: AST Standardiser
	Language: C++
	Author: Annu Sharma (36731206)
	Description: Modules for partial standardisation of the AST
**/
//-------------------------------------------

#include "ASTStandardiser.hpp"

void ASTStandardiser::start(Node* root)
{
	rootNode = root;
	postOrderTraverse(rootNode);
}

bool ASTStandardiser::nodeToStandardise(string nodeName)
{
	set<string> nodeNames;
	nodeNames.insert("let");
	nodeNames.insert("where");
	nodeNames.insert("tau");
	nodeNames.insert("within");
	nodeNames.insert("function_form");
	nodeNames.insert("and");
	nodeNames.insert("@");
	nodeNames.insert("rec");
	nodeNames.insert("lambda");
	if(nodeNames.find(nodeName) != nodeNames.end())
	{
		return true;
	}
	else
	{
		return false;
	}

}

void ASTStandardiser::postOrderTraverse(Node* rootNode)
{
	if(rootNode != NULL)
	{
		postOrderTraverse(rootNode->left);
		postOrderTraverse(rootNode->right);
		bool value = nodeToStandardise(rootNode->nodeName);
		if(value)
		{
			string nodeName = rootNode->nodeName;
			
			if(nodeName == "let")
			{			
				Node* tempNode = rootNode;
				tempNode->nodeName = "gamma";
				tempNode->left->nodeName = "lambda";
				tempNode = tempNode->left;
				Node* temp = tempNode->right;
				tempNode->right = tempNode->left->right;
				tempNode->left->right = temp;
			}
			else if(nodeName == "where")
			{
				Node* tempNode = rootNode;
				tempNode->nodeName = "gamma";
				Node* expression = tempNode->left;
				Node* lambdaNode = new Node();
				lambdaNode->nodeName = "lambda";
				Node* funcDef = tempNode->left->right->left->right;
				Node* func = tempNode->left->right->left;
				func->right = NULL;
				delete tempNode->left->right;		//deleting the "=" node
				tempNode->left = lambdaNode;
				tempNode = lambdaNode;	
				tempNode->right = funcDef;
				tempNode->left = func;
				tempNode = tempNode->left;
				tempNode->right = expression;
				expression->right = NULL;		     
			}
			else if(nodeName == "function_form")
			{				
				Node* tempNode = rootNode;
				tempNode->nodeName = "=";
				tempNode = tempNode->left;
				Node* temp = tempNode;
				while(temp->right != NULL)
				{
					temp = temp->right;
					if(temp->right != NULL)
					// Still a parameter of the function
					{
						Node* lambdaNode = new Node();
						lambdaNode->nodeName = "lambda";
						tempNode->right = lambdaNode;
						lambdaNode->left = temp;
						tempNode = temp;
					}		
				}
			}
			else if(nodeName == "within")
			{
				Node* tempNode = rootNode;
				Node* func1 = tempNode->left->left;
				Node* expr1 = tempNode->left->left->right;
				Node* func2 = tempNode->left->right->left;
				Node* expr2 = tempNode->left->right->left->right;

				func1->right = NULL;
				func2->right = NULL;

				tempNode->nodeName = "=";
				//deleting the "=" signs
				delete tempNode->left->right;
				delete tempNode->left;
				tempNode->left = func2;
				tempNode = tempNode->left;
				Node* gammaNode = new Node();
				gammaNode->nodeName = "gamma";
				tempNode->right = gammaNode;
				tempNode = tempNode->right;
				Node* lambdaNode = new Node();
				lambdaNode->nodeName = "lambda";
				tempNode->left = lambdaNode;
				tempNode = tempNode->left;
				tempNode->left = func1;
				tempNode->right = expr1;
				tempNode = tempNode->left;
				tempNode->right = expr2;
			}
			else if(nodeName == "and")
			{
				Node* tempNode = rootNode;
				tempNode->nodeName = "=";
				Node* temp = tempNode->left;
				Node* commaNode = new Node();
				commaNode->nodeName = ",";
				Node* tauNode = new Node();
				tauNode->nodeName = "tau";
				tempNode->left = commaNode;
				tempNode = tempNode->left;
				tempNode->right = tauNode;
				Node* commaExtension = tempNode;
				Node* tauExtension = tempNode->right;

				//for first func, expr pair
				Node* expr = temp->left->right;
				Node* func = temp->left;
				func->right = NULL;
				commaExtension->left = func;
				commaExtension = commaExtension->left;
				tauExtension->left = expr;
				tauExtension = tauExtension->left;
				temp = temp->right;

				while(temp != NULL)
				{
					Node* expr = temp->left->right;
					Node* func = temp->left;
					func->right = NULL;
					commaExtension->right = func;
					commaExtension = commaExtension->right;
					tauExtension->right = expr;
					tauExtension = tauExtension->right;
					temp = temp->right;
				}
			}
			else if(nodeName == "@")
			{
				Node* tempNode = rootNode;
				Node* expr2 = tempNode->left->right->right;
				Node* func = tempNode->left->right;
				func->right = NULL;
				Node* expr1 = tempNode->left;
				expr1->right = NULL;
				
				//distinguishing between inbuilt and user defined infix functions
				/*if(func->nodeName == "<ID:Conc>")
				{
					tempNode->nodeName = func->nodeName;
					expr1->right = expr2;
				}
				else
				{*/
					tempNode->nodeName = "gamma";
					Node* gammaNode = new Node();
					gammaNode->nodeName = "gamma";
					tempNode->left = gammaNode;
					gammaNode->left = func;
					gammaNode->right = expr2;
					gammaNode->left->right = expr1;
				/*}
				*/

			}
			//rec case
			else if(nodeName == "rec")
			{

				Node* tempNode = rootNode;
				Node* expr = tempNode->left->left->right;
				Node* func = tempNode->left->left;
				func->right = NULL;
				tempNode->nodeName = "=";
				tempNode->left = func;
				tempNode = tempNode->left;
				string funcName = tempNode->nodeName;
				Node* gammaNode = new Node();
				gammaNode->nodeName = "gamma";
				tempNode->right = gammaNode;
				tempNode = tempNode->right;
				Node* yNode = new Node();
				yNode->nodeName = "Ystar";
				tempNode->left = yNode;
				tempNode = tempNode->left;
				Node* lambdaNode = new Node();
				lambdaNode->nodeName = "lambda";
				tempNode->right = lambdaNode;
				tempNode = tempNode->right;
				Node* funcNode = new Node();
				funcNode->nodeName = funcName;
				tempNode->left = funcNode;
				tempNode->left->right = expr;
			}
			else if(nodeName == "lambda")
			{
				Node* tempNode = rootNode;
				tempNode = tempNode->left;
				while(tempNode->right->right != NULL)
				{
					Node* nextParameter = tempNode->right;
					Node* lambdaNode = new Node();
					lambdaNode->nodeName = "lambda";
					tempNode->right = lambdaNode;
					tempNode = tempNode->right;
					tempNode->left = nextParameter;
					tempNode = tempNode->left;
				}
			}
		}
	}
}


