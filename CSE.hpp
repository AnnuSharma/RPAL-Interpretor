//-------------------------------------------
/**
	File:CSE.hpp
	System:RPAL Interpretor
	Component Name: CSE Machine Header File
	Language: C++
	Author: Annu Sharma (36731206)
	Description: Contains CSE Evaluator Class definitions 
**/
//-------------------------------------------

#ifndef CSE_HPP
#define CSE_HPP
#include "Parser.hpp"
#include "map"
#include "vector"
#include "cmath"
using namespace std;

struct envNode;

typedef struct ctrlStructElement{
	string nodeName;
	int envMarker;
	envNode* lambdaEnvMarker;
	int deltaNum;
	//for function parameters
	vector<string> parameters;
	int numParameters;
	string dataType;
	//for tau node on Control 
	vector<ctrlStructElement*> tupleContent;
	bool isCommaLambda;
}ctrlStructElement;

//representation for tree of environments

typedef struct envNode{
	int envNum;
	map<string,ctrlStructElement*> envBindings;
	envNode* parent;
}envNode;

class CSE{
	private:
		envNode* rootEnv;
		envNode* currEnv;
		Node* rootNode;
		int currEnvNum;
		int currDelta;
		int toEvaluateControlStruct;
		map<int, vector<ctrlStructElement*> > controlStructures;
		stack<ctrlStructElement*> Control;
		stack<ctrlStructElement*> Stack;
		stack<envNode*> currEnvStack;
	public:
		void start(Node* root);
		void genControlStructures(Node* root,vector<ctrlStructElement*> &ctrlStruct,int i);
		void evaluateExpression();
		void pushControlStructure(int deltaNum);
		bool isConstant(ctrlStructElement* node);
		bool isID(ctrlStructElement* node);
		pair<bool,ctrlStructElement*> searchEnvTree(string identifier,envNode* currentEnv);
		void inbuiltFunctionCall(ctrlStructElement* functionNode);
		string printTuple(ctrlStructElement* tupleNode,stringstream& tuple);
		void toPrint(string printValue);
};

#endif /* CSE_HPP */