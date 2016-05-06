//-------------------------------------------
/**
	File:CSE.cpp
	System:RPAL Interpretor
	Component Name: CSE Evaluator
	Language: C++
	Author: Annu Sharma (36731206)
	Description: This class implements functionalities for creating 
	and evaluating control structures.  
**/
//-------------------------------------------

#include "CSE.hpp"

void CSE::start(Node* root)
{
	currDelta = 0;
	rootNode = root;
	vector<ctrlStructElement*> delta;
	genControlStructures(rootNode,delta,0);
	controlStructures.insert(pair<int,vector<ctrlStructElement*> >(0,delta));
	typedef map<int, vector<ctrlStructElement*> >::iterator it_type;
	
	ctrlStructElement* PE_Control = new ctrlStructElement();
	PE_Control->nodeName = "envVar";
	PE_Control->envMarker = 0;

	ctrlStructElement* PE_Stack = new ctrlStructElement();
	PE_Stack->nodeName = "envVar";
	PE_Stack->envMarker = 0;

	envNode* envRoot = new envNode();
	envRoot->envNum = 0;
	envRoot->parent = NULL;
	
	currEnvNum = 0;
	currEnv = envRoot;
	envNode* envStackElement = envRoot;
	currEnvStack.push(envStackElement);

	Control.push(PE_Control);
	Stack.push(PE_Stack);
	pushControlStructure(0);
	evaluateExpression();
}

void CSE::genControlStructures(Node* root,vector<ctrlStructElement*> &ctrlStruct,int i)
{	
	if(root != NULL)
	{
		if(root->nodeName != "lambda")
		{
			if(root->nodeName == "->")
			{
				//pushing then control structure marker
				ctrlStructElement* thenElem = new ctrlStructElement();
				thenElem->nodeName = "delta";
				thenElem->deltaNum = currDelta+1;
				ctrlStructElement* thenDeltaTrack = thenElem;
				ctrlStruct.push_back(thenElem);

				//pushing else control structure marker
				ctrlStructElement* elseElem = new ctrlStructElement();
				elseElem->nodeName = "delta";
				elseElem->deltaNum = currDelta+2;
				ctrlStructElement* elseDeltaTrack = elseElem;
				ctrlStruct.push_back(elseElem);

				//pushing the beta marker
				ctrlStructElement* betaElem = new ctrlStructElement();
				betaElem->nodeName = "beta";
				ctrlStruct.push_back(betaElem);

				Node* thenPart = root->left->right;
				Node* elsePart = root->left->right->right;
				thenPart->right = NULL;
				root->left->right = NULL;

				//generating conditional B
				genControlStructures(root->left,ctrlStruct,i);
				genControlStructures(root->right,ctrlStruct,i);

				//generating then 
				vector<ctrlStructElement*> thenDelta;
				currDelta++;
				i=currDelta;
				thenDeltaTrack->deltaNum = i;
				genControlStructures(thenPart,thenDelta,i);
				controlStructures.insert(pair<int,vector<ctrlStructElement*> >(i,thenDelta));

				//generating else
				vector<ctrlStructElement*> elseDelta;
				currDelta++;
				i=currDelta;
				elseDeltaTrack->deltaNum = i;
				genControlStructures(elsePart,elseDelta,i);
				controlStructures.insert(pair<int,vector<ctrlStructElement*> >(i,elseDelta));
			}
			else if(root->nodeName == "tau")
			{
				int countElements = 0;
				Node* tempNode = root;
				tempNode = tempNode->left;
				countElements++;
				while(tempNode->right != NULL)
				{
					tempNode = tempNode->right;
					countElements++;
				}
				ctrlStructElement* elem = new ctrlStructElement();
				elem->nodeName = root->nodeName;
				elem->numParameters = countElements;
				ctrlStruct.push_back(elem);
				genControlStructures(root->left,ctrlStruct,i);
				genControlStructures(root->right,ctrlStruct,i);
			}
			else
			{
				ctrlStructElement* elem = new ctrlStructElement();
				elem->nodeName = root->nodeName;
				ctrlStruct.push_back(elem);
				genControlStructures(root->left,ctrlStruct,i);
				genControlStructures(root->right,ctrlStruct,i);
			}
		}
		else
		{
			ctrlStructElement* elem = new ctrlStructElement();
			elem->nodeName = "lambda";
			elem->deltaNum = currDelta+1;
			
			//Checking for multiple parameterised lambda
			if(root->left->nodeName == ",")
			{
				int counter = 0;
				Node* temp = root;
				temp = root->left->left;
				while(temp->right != NULL)
				{
					elem->parameters.push_back(temp->nodeName);	
					temp = temp->right;
					counter = counter+1;
				}
				elem->parameters.push_back(temp->nodeName);	
				counter = counter+1;
				elem->numParameters = counter;
				elem->isCommaLambda = true;
				ctrlStruct.push_back(elem);
			}
			else
			{
				elem->parameters.push_back(root->left->nodeName);
				elem->numParameters = 1;
				elem->isCommaLambda = false;
				ctrlStruct.push_back(elem);
			}

			vector<ctrlStructElement*> delta;
			currDelta++;
			i=currDelta;
			genControlStructures(root->left->right,delta,i);
			controlStructures.insert(pair<int,vector<ctrlStructElement*> >(i,delta));
			genControlStructures(root->right,ctrlStruct,i-1);
		}
	}
}
void CSE::pushControlStructure(int deltaNum)
{
	vector<ctrlStructElement*> controlStruct = controlStructures[deltaNum];
	for(vector<ctrlStructElement*>::iterator it = controlStruct.begin(); it != controlStruct.end(); ++it) 
	{
		//making a copy of the component and pushing
		ctrlStructElement* tempComponent = new ctrlStructElement();
		tempComponent->nodeName = (*it)->nodeName;
		tempComponent->envMarker = (*it)->envMarker;
		tempComponent->lambdaEnvMarker = (*it)->lambdaEnvMarker;
		tempComponent->deltaNum = (*it)->deltaNum;
		tempComponent->parameters = (*it)->parameters;
		tempComponent->numParameters = (*it)->numParameters;
		tempComponent->dataType = (*it)->dataType;
		tempComponent->tupleContent = (*it)->tupleContent;
		tempComponent->isCommaLambda = (*it)->isCommaLambda;

    	Control.push(tempComponent);	
	}
}
void CSE::evaluateExpression()
{
	while(!Control.empty())
	{
		ctrlStructElement* topControl = Control.top();
		if(topControl->nodeName == "lambda")
		{
			topControl->lambdaEnvMarker = currEnv;
			topControl->dataType = "function";
			Control.pop();
			Stack.push(topControl);
		}
		else if(topControl->nodeName == "gamma")
		{
			if((Stack.top())->nodeName == "lambda")
			{
				Control.pop();
				ctrlStructElement* lambdaNode = Stack.top();
				Stack.pop();
				envNode* newEnv = new envNode();
				newEnv->envNum = currEnvNum+1;
				if(lambdaNode->isCommaLambda == false)
				{	
					vector<string>::iterator it = (lambdaNode->parameters).begin();
					for(int i=0;i<(lambdaNode->numParameters);i++)
					{
						ctrlStructElement* parameterValue = Stack.top();
						if(isConstant(parameterValue))
						{
							//assigned value to dataType and nodeName of parameterValue node
						}
						Stack.pop();

						if((*it) != "()") 
						{
							stringstream newParameterName;
							for(int i = 4;i<((*it).length()-1);i++)
							{
								newParameterName<<(*it)[i];
							}
							(newEnv->envBindings).insert(pair<string,ctrlStructElement*>(newParameterName.str(),parameterValue));
						}
						++it;	
					}
				}
				else
				{
					ctrlStructElement* parameterNode = Stack.top();
					Stack.pop();
					vector<string>::iterator it = (lambdaNode->parameters).begin();
					vector<ctrlStructElement*>::iterator itr = (parameterNode->tupleContent).begin();
					for(int i=0;i<(lambdaNode->numParameters);i++)
					{
						ctrlStructElement* parameterValue = (*itr);
						if(isConstant(parameterValue))
						{
							//assigned value to dataType and nodeName of parameterValue node
						}

						stringstream newParameterName;
						
						for(int i = 4;i<((*it).length()-1);i++)
						{
							newParameterName<<(*it)[i];
						}
						(newEnv->envBindings).insert(pair<string,ctrlStructElement*>(newParameterName.str(),parameterValue));
							
						++itr;	
						++it;
					}
				}
				newEnv->parent = lambdaNode->lambdaEnvMarker;

				ctrlStructElement* openEnvControl = new ctrlStructElement();
				openEnvControl->envMarker = currEnvNum+1;
				openEnvControl->nodeName = "envVar";

				ctrlStructElement* openEnvStack = new ctrlStructElement();
				openEnvStack->envMarker = currEnvNum+1;
				openEnvStack->nodeName = "envVar";

				Control.push(openEnvControl);
				Stack.push(openEnvStack);
				currEnvNum++;
				currEnv = newEnv;
				envNode* envStackElement = newEnv;
				currEnvStack.push(envStackElement);
				pushControlStructure(lambdaNode->deltaNum);
			}
			else if((Stack.top())->nodeName == "Ystar")
			{
				ctrlStructElement* Y = Stack.top();
				Stack.pop();
				ctrlStructElement* lambdaNode = Stack.top();
				Stack.pop();
				Control.pop();
				ctrlStructElement* etaNode = new ctrlStructElement();
				etaNode->nodeName = "eta";
				etaNode->lambdaEnvMarker = lambdaNode->lambdaEnvMarker;
				etaNode->deltaNum = lambdaNode->deltaNum;
				etaNode->parameters = lambdaNode->parameters;
				etaNode->numParameters = lambdaNode->numParameters;
				Stack.push(etaNode);
			}
			else if((Stack.top())->nodeName == "eta")
			{
				ctrlStructElement* etaNode = Stack.top();
				ctrlStructElement* gammaNode = new ctrlStructElement();
				gammaNode->nodeName = "gamma";
				Control.push(gammaNode);
				ctrlStructElement* lambdaNode = new ctrlStructElement();
				lambdaNode->nodeName = "lambda";
				lambdaNode->lambdaEnvMarker = etaNode->lambdaEnvMarker;
				lambdaNode->parameters = etaNode->parameters;
				lambdaNode->deltaNum = etaNode->deltaNum;
				lambdaNode->numParameters = etaNode->numParameters;
				Stack.push(lambdaNode);
			}
			//tuple selection case
			else if((Stack.top())->nodeName == "tau")
			{
				Control.pop();
				ctrlStructElement* tauNode = Stack.top();
				Stack.pop();
				ctrlStructElement* indexNode = Stack.top();
				Stack.pop();
				if(isConstant(indexNode))
				{
					//assigned value to dataType and nodeName of indexNode 
				}
				int index;
				stringstream stoi;
				if(indexNode->dataType == "integer")
				{
					stoi<<indexNode->nodeName;
					stoi>>index;
				}
				index = index-1;
				vector<ctrlStructElement*>::iterator itr;
				itr = (tauNode->tupleContent).begin();
				int counter = 0;
				while(counter < index)
				{
					itr = itr+1;
					counter++;
				}
				Stack.push((*itr));
			}
			else
			{
				Control.pop();
			}
		}

		//checking for constants -> push onto Stack
		else if(isConstant(topControl))
		{
			Control.pop();
			Stack.push(topControl);
		}

		else if(topControl->nodeName == "tau")
		{
			//tuple formation case
			ctrlStructElement* tauNode = new ctrlStructElement();
			tauNode = Control.top();
			Control.pop();

			ctrlStructElement* tupleNode = new ctrlStructElement();
			tupleNode->nodeName = "tau";
			tupleNode->dataType = "tuple";
			tupleNode->numParameters = tauNode->numParameters;

			int numElements = tauNode->numParameters;
			while(numElements != 0)
			{
				ctrlStructElement* tauElement = Stack.top();
				if(isConstant(tauElement))
				{
					//assigned value to dataType and nodeName of tauElement node
				}
				Stack.pop();
				tupleNode->tupleContent.push_back(tauElement);
				numElements = numElements-1;
			}
			Stack.push(tupleNode);
		}
		else if(topControl->nodeName == "envVar")
		{
			ctrlStructElement* temp = Stack.top();
			if(temp->nodeName == "envVar")
			{
				Stack.pop();
				Control.pop();
			}
			else
			{
				Stack.pop();
				Control.pop();
				Stack.pop();
				Stack.push(temp);
			}
			currEnvStack.pop();
			if(!currEnvStack.empty())
			{
				currEnv = currEnvStack.top();
				currEnvNum = currEnv->envNum;
			}
		}
		else if(topControl->nodeName == "beta")
		{
			ctrlStructElement* boolNode = Stack.top();
			Stack.pop();
			Control.pop();
			ctrlStructElement* elseNode = Control.top();
			Control.pop();
			ctrlStructElement* thenNode = Control.top();
			Control.pop();
			if(boolNode->dataType == "truthvalue")
			{
				if(boolNode->nodeName == "true")
				{
					int deltaNum = thenNode->deltaNum;
					pushControlStructure(deltaNum);
				}
				else
				{
					int deltaNum = elseNode->deltaNum;
					pushControlStructure(deltaNum);
				}
			}
			else
			{
				cout<<"Conditional truthvalue expected"<<endl;
			}
		}
		else if(topControl->nodeName == "Ystar")
		{
			ctrlStructElement* temp = Control.top();
			Control.pop();
			Stack.push(temp);
		}

		//covers for all inbuilt functions and ID lookup in the environment tree
		else 
		{
			//Value is first looked up in the environment tree and it not found PE built in functionas are invoked
			if(isID(topControl))
			{
				//assigned value to dataType and nodeName of topControl node
			}

			//search in nodes of the environment tree
			pair<bool,ctrlStructElement*> returnVal = searchEnvTree(topControl->nodeName,currEnv);
			if(returnVal.first == true)
			{
				Control.pop();
				Stack.push(returnVal.second);
			}
			else
			{
				//call for implicit functions
				inbuiltFunctionCall(topControl);
			}

		}
	}
}
void CSE::inbuiltFunctionCall(ctrlStructElement* functionNode)
{
	//boolean functions
	if(functionNode->nodeName == "Isinteger")
	{
		Control.pop();
		ctrlStructElement* toTestNode = Stack.top();
		ctrlStructElement* boolNode = new ctrlStructElement();
		boolNode->dataType = "truthvalue";
		if(toTestNode->dataType == "integer")
		{
			boolNode->nodeName = "true";
		}
		else
		{
			boolNode->nodeName = "false";
		}
		Stack.pop();
		Stack.push(boolNode);

	}
	else if(functionNode->nodeName == "Istruthvalue")
	{
		Control.pop();
		ctrlStructElement* toTestNode = Stack.top();
		ctrlStructElement* boolNode = new ctrlStructElement();
		boolNode->dataType = "truthvalue";
		if(toTestNode->dataType == "truthvalue")
		{
			boolNode->nodeName = "true";
		}
		else
		{
			boolNode->nodeName = "false";
		}
		Stack.pop();
		Stack.push(boolNode);
	}
	else if(functionNode->nodeName == "Isstring")
	{
		Control.pop();
		ctrlStructElement* toTestNode = Stack.top();
		ctrlStructElement* boolNode = new ctrlStructElement();
		boolNode->dataType = "truthvalue";
		if(toTestNode->dataType == "string")
		{
			boolNode->nodeName = "true";
		}
		else
		{
			boolNode->nodeName = "false";
		}
		Stack.pop();
		Stack.push(boolNode);
	}
	else if(functionNode->nodeName == "Istuple")
	{
		Control.pop();
		ctrlStructElement* toTestNode = Stack.top();
		ctrlStructElement* boolNode = new ctrlStructElement();
		boolNode->dataType = "truthvalue";
		if(toTestNode->dataType == "tuple" || toTestNode->dataType == "nil")
		{
			boolNode->nodeName = "true";
		}
		else
		{
			boolNode->nodeName = "false";
		}
		Stack.pop();
		Stack.push(boolNode);
	}
	else if(functionNode->nodeName == "Isfunction")
	{
		Control.pop();
		ctrlStructElement* toTestNode = Stack.top();
		ctrlStructElement* boolNode = new ctrlStructElement();
		boolNode->dataType = "truthvalue";
		if(toTestNode->dataType == "function")
		{
			boolNode->nodeName = "true";
		}
		else
		{
			boolNode->nodeName = "false";
		}
		Stack.pop();
		Stack.push(boolNode);
	}
	else if(functionNode->nodeName == "Isdummy")
	{
		Control.pop();
		ctrlStructElement* toTestNode = Stack.top();
		ctrlStructElement* boolNode = new ctrlStructElement();
		boolNode->dataType = "truthvalue";
		if(toTestNode->dataType == "dummy")
		{
			boolNode->nodeName = "true";
		}
		else
		{
			boolNode->nodeName = "false";
		}
		Stack.pop();
		Stack.push(boolNode);
	}
	else if(functionNode->nodeName == "or")
	{
		Control.pop();
		ctrlStructElement* firstOperand = Stack.top();
		Stack.pop();
		ctrlStructElement* secondOperand = Stack.top();
		Stack.pop();
		if(firstOperand->dataType == "truthvalue" && secondOperand->dataType == "truthvalue")
		{
			ctrlStructElement* boolNode = new ctrlStructElement();
			boolNode->dataType = "truthvalue";
			if(firstOperand->nodeName == "true" || secondOperand->nodeName == "true")
			{
				boolNode->nodeName = "true";
			}
			else
			{
				boolNode->nodeName = "false";
			}
			Stack.push(boolNode);
		}
		else
		{
			cout<<"Boolean 'or' operation attempted on non boolean values"<<endl;
		}
	}
	else if(functionNode->nodeName == "&")
	{
		Control.pop();
		ctrlStructElement* firstOperand = Stack.top();
		Stack.pop();
		ctrlStructElement* secondOperand = Stack.top();
		Stack.pop();
		if(firstOperand->dataType == "truthvalue" && secondOperand->dataType == "truthvalue")
		{
			ctrlStructElement* boolNode = new ctrlStructElement();
			boolNode->dataType = "truthvalue";
			if(firstOperand->nodeName == "true" && secondOperand->nodeName == "true")
			{
				boolNode->nodeName = "true";
			}
			else
			{
				boolNode->nodeName = "false";
			}
			Stack.push(boolNode);
		}
		else
		{
			cout<<"Boolean 'and' operation attempted on non boolean values"<<endl;
		}
	}
	else if(functionNode->nodeName == "not")
	{
		Control.pop();
		ctrlStructElement* operand = Stack.top();
		if(operand->dataType == "truthvalue")
		{
			if(operand->nodeName == "true")
			{
				operand->nodeName = "false";
			}
			else
			{
				operand->nodeName = "true";
			}
		}
		else
		{
			cout<<"Boolean not operation attempted on non boolean value"<<endl;
		}
	}
	//end boolean functions

	else if(functionNode->nodeName == "Print")
	{
		Control.pop();
		ctrlStructElement* toPrintNode = Stack.top();
		ctrlStructElement* dummyNode = new ctrlStructElement();
		dummyNode->nodeName = "dummy";
		dummyNode->dataType = "dummy";
		if(toPrintNode->dataType == "integer" || toPrintNode->dataType == "string" || toPrintNode->dataType == "identifier" || toPrintNode->dataType == "truthvalue" || toPrintNode->dataType == "nil" ||toPrintNode->dataType == "dummy")
		{
			toPrint(toPrintNode->nodeName);
			Stack.pop();
			Stack.push(dummyNode);
		}	
		else if(toPrintNode->dataType == "tuple")
		{
			stringstream tuple;
			string tupleValue = printTuple(toPrintNode,tuple);
			toPrint(tupleValue);
			Stack.pop();
			Stack.push(dummyNode);
		}
		else if(toPrintNode->dataType == "function")
		{
			if(toPrintNode->isCommaLambda == true)
			{
				string closureName = "null";
				int closureDelta = toPrintNode->deltaNum;
				cout<<"[lambda closure: ("<<closureName<<"): "<<closureDelta<<"]";
			}
			else
			{
				vector<string>::iterator itr;
				itr = toPrintNode->parameters.begin();
				stringstream newParameterName;
				for(int i = 4;i<((*itr).length()-1);i++)
				{
					newParameterName<<(*itr)[i];
				}
				int closureDelta = toPrintNode->deltaNum;
				cout<<"[lambda closure: "<<newParameterName.str()<<": "<<closureDelta<<"]";
			}
			Stack.pop();
			Stack.push(dummyNode);
		}
	}

	//tuple operations
	else if(functionNode->nodeName == "Order")
	{
		Control.pop();
		ctrlStructElement* tauNode = Stack.top();
		Stack.pop();
		if(tauNode->dataType == "tuple")
		{
			ctrlStructElement* orderNode = new ctrlStructElement();
			stringstream itoa;
			itoa<<tauNode->numParameters;
			orderNode->nodeName = itoa.str();
			orderNode->dataType = "integer";
			Stack.push(orderNode);
		}
		else if(tauNode->dataType == "nil")
		{
			ctrlStructElement* orderNode = new ctrlStructElement();
			orderNode->nodeName = "0";
			orderNode->dataType = "integer";
			Stack.push(orderNode);
		}
		else
		{
			cout<<"Order function attempted on non tuple"<<endl;
		}
	}
	else if(functionNode->nodeName == "Null")
	{
		Control.pop();
		ctrlStructElement* nilNode = Stack.top();
		Stack.pop();
		if(nilNode->dataType == "nil")
		{
			ctrlStructElement* nullNode = new ctrlStructElement();
			nullNode->nodeName = "true";
			nullNode->dataType = "truthvalue";
			Stack.push(nullNode);
		}
		else if(nilNode->dataType == "tuple")
		{
			ctrlStructElement* nullNode = new ctrlStructElement();
			nullNode->nodeName = "false";
			nullNode->dataType = "truthvalue";
			Stack.push(nullNode);
		}
		else
		{
			cout<<"Null function attempted on non tuple"<<endl;
		}
	}
	else if(functionNode->nodeName == "aug")
	{
		Control.pop();
		ctrlStructElement* operandOne = Stack.top();
		Stack.pop();
		ctrlStructElement* operandTwo = Stack.top();
		Stack.pop();
		if(operandOne->dataType == "nil")
		{
			if(operandTwo->dataType == "nil")
			{
				ctrlStructElement* nullNode = new ctrlStructElement();
				nullNode->nodeName = "tau";
				nullNode->dataType = "tuple";
				nullNode->numParameters = 1;
				ctrlStructElement* contentNode = new ctrlStructElement();
				contentNode->nodeName = "nil";
				contentNode->dataType = "nil";
				nullNode->tupleContent.push_back(contentNode);
				Stack.push(nullNode);
			}
			else if(operandTwo->dataType == "integer" || operandTwo->dataType == "truthvalue" || operandTwo->dataType == "string")
			{
				ctrlStructElement* nullNode = new ctrlStructElement();
				nullNode->nodeName = "tau";
				nullNode->dataType = "tuple";
				nullNode->numParameters = 1;
				ctrlStructElement* contentNode = new ctrlStructElement();
				contentNode->nodeName = operandTwo->nodeName;
				if(operandTwo->dataType == "integer")
				{
					contentNode->dataType = "integer";
				}
				else if(operandTwo->dataType == "truthvalue")
				{
					contentNode->dataType = "truthvalue";
				}
				else
				{
					contentNode->dataType = "string";
				}
				nullNode->tupleContent.push_back(contentNode);
				Stack.push(nullNode);
			}
			else if(operandTwo->dataType == "tuple")
			{
				ctrlStructElement* newTau = new ctrlStructElement();
				newTau->nodeName = "tau";
				newTau->dataType = "tuple";
				newTau->numParameters = 1;
				newTau->tupleContent.push_back(operandTwo);
				Stack.push(newTau);
			}
			else
			{
				cout<<"Illegal operands for aug"<<endl;
			}
		}
		else if(operandOne->dataType == "tuple")
		{
			operandOne->numParameters = (operandOne->numParameters)+1;
			operandOne->tupleContent.push_back(operandTwo);
			Stack.push(operandOne);
		}
		else
		{
			cout<<"First operand for aug needs to be a tuple"<<endl;
		}
	}
	//end of tuple operations

	//string operations
	else if(functionNode->nodeName == "Stem")
	{
		Control.pop();
		ctrlStructElement* stringNode = Stack.top();
		if(stringNode->dataType != "string")
		{
			cout<<"Operand for Stem needs to be a string"<<endl;
		}
		else
		{
			if(stringNode->nodeName != "")
			{
				stringNode->nodeName = (stringNode->nodeName).at(0);
			}
		}
	}
	else if(functionNode->nodeName == "Stern")
	{
		Control.pop();
		ctrlStructElement* stringNode = Stack.top();
		if(stringNode->dataType != "string")
		{
			cout<<"Operand for Stern needs to be a string"<<endl;
		}
		else
		{
			if(stringNode->nodeName != "")
			{
				(stringNode->nodeName).erase (0,1); 
			}
		}
	}
	else if(functionNode->nodeName == "Conc")
	{
		Control.pop();
		ctrlStructElement* stringOne = Stack.top();
		Stack.pop();
		ctrlStructElement* stringTwo = Stack.top();
		Stack.pop();
		if(stringOne->dataType == "string" && stringTwo->dataType == "string")
		{
			stringOne->nodeName = stringOne->nodeName.append(stringTwo->nodeName);
			Stack.push(stringOne);
		}
		else
		{
			cout<<"Operands for Conc need to be a strings"<<endl;
		}
		
	}
	else if(functionNode->nodeName == "ItoS")
	{
		Control.pop();
		ctrlStructElement* integerNode = Stack.top();
		Stack.pop();
		if(integerNode->dataType != "integer")
		{
			cout<<"Operand to ItoS needs to be an integer"<<endl;
		}
		else
		{
			ctrlStructElement* stringNode = new ctrlStructElement();
			stringNode->dataType = "string";
			stringstream itos;
			itos<<integerNode->nodeName;
			stringNode->nodeName = itos.str();
			Stack.push(stringNode);
		}
	}
	else if(functionNode->nodeName == "eq")
	{
		Control.pop();
		ctrlStructElement* stringOne = Stack.top();
		Stack.pop();
		ctrlStructElement* stringTwo = Stack.top();
		Stack.pop();
		if(stringOne->dataType == "string" && stringTwo->dataType == "string")
		{
			ctrlStructElement* eqNode = new ctrlStructElement();
			eqNode->dataType = "truthvalue";
			if(stringOne->nodeName != stringTwo->nodeName)
			{
				eqNode->nodeName = "false";
			}
			else
			{
				eqNode->nodeName = "true";
			}
			Stack.push(eqNode);
		}
		else if(stringOne->dataType == "integer" && stringTwo->dataType == "integer")
		{
			int intOne;
			int intTwo;
			istringstream(stringOne->nodeName)>>intOne;
			istringstream(stringTwo->nodeName)>>intTwo;
			ctrlStructElement *eqNode = new ctrlStructElement();
			if(intOne == intTwo)
			{
				eqNode->nodeName = "true";
			}
			else
			{
				eqNode->nodeName = "false";
			}
			eqNode->dataType = "truthvalue";
			Stack.push(eqNode);
		}
		else
		{
			cout<<"Operands to eq need to be both integers or both strings"<<endl;
		}
	}
	else if(functionNode->nodeName == "ne")
	{
		Control.pop();
		ctrlStructElement* stringOne = Stack.top();
		Stack.pop();
		ctrlStructElement* stringTwo = Stack.top();
		Stack.pop();
		if(stringOne->dataType == "string" || stringTwo->dataType == "string")
		{
			ctrlStructElement* neNode = new ctrlStructElement();
			neNode->dataType = "truthvalue";
			if(stringOne->nodeName != stringTwo->nodeName)
			{
				neNode->nodeName = "true";
			}
			else
			{
				neNode->nodeName = "false";
			}
			Stack.push(neNode);
		}
		else if(stringOne->dataType == "integer" && stringTwo->dataType == "integer")
		{
			int intOne;
			int intTwo;
			istringstream(stringOne->nodeName)>>intOne;
			istringstream(stringTwo->nodeName)>>intTwo;
			ctrlStructElement *neNode = new ctrlStructElement();
			if(intOne != intTwo)
			{
				neNode->nodeName = "true";
			}
			else
			{
				neNode->nodeName = "false";
			}
			neNode->dataType = "truthvalue";
			Stack.push(neNode);
		}
		else
		{
			cout<<"Operands to ne need to be both integers or both strings"<<endl;
		}
	}
	//end of string operations

	//Integer arithmetic functions
	else if(functionNode->nodeName == "+")
	{
		Control.pop();
		ctrlStructElement* firstInteger = Stack.top();
		Stack.pop();
		ctrlStructElement* secondInteger = Stack.top();
		Stack.pop();
		if(firstInteger->dataType == "integer" && secondInteger->dataType == "integer")
		{
			int intOne;
			int intTwo;
			istringstream(firstInteger->nodeName)>>intOne;
			istringstream(secondInteger->nodeName)>>intTwo;
			int sum = intOne+intTwo;
			ctrlStructElement *sumNode = new ctrlStructElement();
			stringstream itos;
			itos<<sum;
			sumNode->nodeName = itos.str();
			sumNode->dataType = "integer";
			Stack.push(sumNode);
		}
		else
		{
			cout<<"Operands to + function must be integers"<<endl;
		}
	}
	else if(functionNode->nodeName == "-")
	{
		Control.pop();
		ctrlStructElement* firstInteger = Stack.top();
		Stack.pop();
		ctrlStructElement* secondInteger = Stack.top();
		Stack.pop();
		if(firstInteger->dataType == "integer" && secondInteger->dataType == "integer")
		{
			int intOne;
			int intTwo;
			istringstream(firstInteger->nodeName)>>intOne;
			istringstream(secondInteger->nodeName)>>intTwo;
			int diff = intOne-intTwo;
			ctrlStructElement *diffNode = new ctrlStructElement();
			stringstream itos;
			itos<<diff;
			diffNode->nodeName = itos.str();
			diffNode->dataType = "integer";
			Stack.push(diffNode);
		}
		else
		{
			cout<<"Operands to - function must be integers"<<endl;
		}
	}
	else if(functionNode->nodeName == "*")
	{
		Control.pop();
		ctrlStructElement* firstInteger = Stack.top();
		Stack.pop();
		ctrlStructElement* secondInteger = Stack.top();
		Stack.pop();
		if(firstInteger->dataType == "integer" && secondInteger->dataType == "integer")
		{
			int intOne;
			int intTwo;
			istringstream(firstInteger->nodeName)>>intOne;
			istringstream(secondInteger->nodeName)>>intTwo;
			int mul = intOne*intTwo;
			ctrlStructElement *mulNode = new ctrlStructElement();
			stringstream itos;
			itos<<mul;
			mulNode->nodeName = itos.str();
			mulNode->dataType = "integer";
			Stack.push(mulNode);
		}
		else
		{
			cout<<"Operands to * function must be integers"<<endl;
		}
	}
	else if(functionNode->nodeName == "/")
	{
		Control.pop();
		ctrlStructElement* firstInteger = Stack.top();
		Stack.pop();
		ctrlStructElement* secondInteger = Stack.top();
		Stack.pop();
		if(firstInteger->dataType == "integer" && secondInteger->dataType == "integer")
		{
			int intOne;
			int intTwo;
			istringstream(firstInteger->nodeName)>>intOne;
			istringstream(secondInteger->nodeName)>>intTwo;
			int quo = intOne/intTwo;
			ctrlStructElement *quoNode = new ctrlStructElement();
			stringstream itos;
			itos<<quo;
			quoNode->nodeName = itos.str();
			quoNode->dataType = "integer";
			Stack.push(quoNode);
		}
		else
		{
			cout<<"Operands to / function must be integers"<<endl;
		}
	}
	else if(functionNode->nodeName == "**")
	{
		Control.pop();
		ctrlStructElement* firstInteger = Stack.top();
		Stack.pop();
		ctrlStructElement* secondInteger = Stack.top();
		Stack.pop();
		if(firstInteger->dataType == "integer" && secondInteger->dataType == "integer")
		{
			int intOne;
			int intTwo;
			istringstream(firstInteger->nodeName)>>intOne;
			istringstream(secondInteger->nodeName)>>intTwo;
			int power = pow(intOne,intTwo);
			ctrlStructElement *powerNode = new ctrlStructElement();
			stringstream itos;
			itos<<power;
			powerNode->nodeName = itos.str();
			powerNode->dataType = "integer";
			Stack.push(powerNode);
		}
		else
		{
			cout<<"Operands to pow function must be integers"<<endl;
		}
	}
	else if(functionNode->nodeName == "ls")
	{
		Control.pop();
		ctrlStructElement* firstInteger = Stack.top();
		Stack.pop();
		ctrlStructElement* secondInteger = Stack.top();
		Stack.pop();
		if(firstInteger->dataType == "integer" && secondInteger->dataType == "integer")
		{
			int intOne;
			int intTwo;
			istringstream(firstInteger->nodeName)>>intOne;
			istringstream(secondInteger->nodeName)>>intTwo;
			ctrlStructElement *lsNode = new ctrlStructElement();
			if(intOne < intTwo)
			{
				lsNode->nodeName = "true";
			}
			else
			{
				lsNode->nodeName = "false";
			}
			lsNode->dataType = "truthvalue";
			Stack.push(lsNode);
		}
		else
		{
			cout<<"Operands to ls function must be integers"<<endl;
		}
	}
	else if(functionNode->nodeName == "gr")
	{
		Control.pop();
		ctrlStructElement* firstInteger = Stack.top();
		Stack.pop();
		ctrlStructElement* secondInteger = Stack.top();
		Stack.pop();
		if(firstInteger->dataType == "integer" && secondInteger->dataType == "integer")
		{
			int intOne;
			int intTwo;
			istringstream(firstInteger->nodeName)>>intOne;
			istringstream(secondInteger->nodeName)>>intTwo;
			ctrlStructElement *grNode = new ctrlStructElement();
			if(intOne > intTwo)
			{
				grNode->nodeName = "true";
			}
			else
			{
				grNode->nodeName = "false";
			}
			grNode->dataType = "truthvalue";
			Stack.push(grNode);
		}
		else
		{
			cout<<"Operands to gr function must be integers"<<endl;
		}
	}
	else if(functionNode->nodeName == "le")
	{

		Control.pop();
		ctrlStructElement* firstInteger = Stack.top();
		Stack.pop();
		ctrlStructElement* secondInteger = Stack.top();
		Stack.pop();
		if(firstInteger->dataType == "integer" && secondInteger->dataType == "integer")
		{
			int intOne;
			int intTwo;
			istringstream(firstInteger->nodeName)>>intOne;
			istringstream(secondInteger->nodeName)>>intTwo;
			ctrlStructElement *leNode = new ctrlStructElement();
			if(intOne <= intTwo)
			{
				leNode->nodeName = "true";
			}
			else
			{
				leNode->nodeName = "false";
			}
			leNode->dataType = "truthvalue";
			Stack.push(leNode);
		}
		else
		{
			cout<<"Operands to le function must be integers"<<endl;
		}
	}
	else if(functionNode->nodeName == "ge")
	{

		Control.pop();
		ctrlStructElement* firstInteger = Stack.top();
		Stack.pop();
		ctrlStructElement* secondInteger = Stack.top();
		Stack.pop();
		if(firstInteger->dataType == "integer" && secondInteger->dataType == "integer")
		{
			int intOne;
			int intTwo;
			istringstream(firstInteger->nodeName)>>intOne;
			istringstream(secondInteger->nodeName)>>intTwo;
			ctrlStructElement *geNode = new ctrlStructElement();
			if(intOne >= intTwo)
			{
				geNode->nodeName = "true";
			}
			else
			{
				geNode->nodeName = "false";
			}
			geNode->dataType = "truthvalue";
			Stack.push(geNode);
		}
		else
		{
			cout<<"Operands to ge function must be integers"<<endl;
		}
	}
	else if(functionNode->nodeName == "neg")
	{
		Control.pop();
		ctrlStructElement* negNode = Stack.top();
		Stack.pop();
		if(negNode->dataType == "integer")
		{
			ctrlStructElement* negatedNode = new ctrlStructElement();
			int num;
			istringstream(negNode->nodeName)>>num;
			num = -num;
			stringstream itos;
			itos<<num;
			negatedNode->nodeName = itos.str();
			negatedNode->dataType = "integer";
			Stack.push(negatedNode);
		}
		else
		{
			cout<<"Operand to neg function must be an integer"<<endl;
		}
	}
	else
	{
		//cout<<"Undeclared identifier "<<functionNode->nodeName<<endl;
	}
	//end of Integer Arithmetic functions
}

string CSE::printTuple(ctrlStructElement* toPrintNode,stringstream& tuple)
{
	tuple<<'(';
	vector<ctrlStructElement*>::iterator it = (toPrintNode->tupleContent).begin();
	while(it != (toPrintNode->tupleContent).end())
	{
		if((*it)->dataType == "tuple")
		{
			stringstream newTuple;
			string newTupleContent = printTuple(*it,newTuple);
			tuple<<newTupleContent<<", ";
		}
		else
		{
		tuple<<((*it)->nodeName)<<", ";
		}
		it = it+1;
	}
	string tupleString = tuple.str();
	tuple.str("");
	tupleString.resize(tupleString.size()-2);
	tuple<<tupleString<<')';
	return tuple.str();
}

pair<bool,ctrlStructElement*> CSE::searchEnvTree(string identifier,envNode* env)
{
	if(env == NULL)
	{
		pair<bool,ctrlStructElement*> returnVal;
		returnVal.first = false;
		returnVal.second = NULL;
		return returnVal;
	}
	
	map<string,ctrlStructElement*>::iterator itr;
	itr = (env->envBindings).find(identifier);
	if(itr != (env->envBindings).end())
	{
		pair<bool,ctrlStructElement*> returnVal;
		returnVal.first = true;
		//making copy of found binding
		ctrlStructElement* tempBinding = new ctrlStructElement();
		tempBinding->nodeName = ((itr->second)->nodeName);
		tempBinding->envMarker = ((itr->second)->envMarker);
		tempBinding->lambdaEnvMarker = ((itr->second)->lambdaEnvMarker);
		tempBinding->deltaNum = ((itr->second)->deltaNum);
		tempBinding->numParameters = ((itr->second)->numParameters);
		tempBinding->dataType = ((itr->second)->dataType);
		tempBinding->tupleContent = ((itr->second)->tupleContent);
		tempBinding->isCommaLambda = ((itr->second)->isCommaLambda);
		tempBinding->parameters = ((itr->second)->parameters);
		returnVal.second = tempBinding;
		return returnVal;
	}
	else
	{
		return searchEnvTree(identifier,env->parent);
	}
	
}

bool CSE::isConstant(ctrlStructElement* node)
{
	if(node->nodeName == "<true>" || node->nodeName == "<false>" || node->nodeName == "<nil>" || node->nodeName == "<dummy>")
	{
		stringstream newNodeName;
		for(int i = 1;i<(node->nodeName).length()-1;i++)
		{
			newNodeName<<(node->nodeName)[i];
		}
		node->nodeName = newNodeName.str();
		if(node->nodeName == "dummy")
		{
			node->dataType = "dummy";	
		}
		else if(node->nodeName == "true" || node->nodeName == "false")
		{
			node->dataType = "truthvalue";
		}
		else
		{
			node->dataType = "nil";
		}
		return true;
	}

	else if(((node->nodeName).substr(0,5) == "<INT:") || ((node->nodeName).substr (0,5) == "<STR:"))
	{
		if((node->nodeName).substr(0,5) == "<INT:")
		{
			stringstream newNodeName;
			for(int i = 5;i<(node->nodeName).length()-1;i++)
			{
				newNodeName<<(node->nodeName)[i];
			}
			node->nodeName = newNodeName.str();
			node->dataType = "integer";
		}
		else
		{
			int i;
			stringstream newNodeName;
			for(i = 6;i<(node->nodeName).length()-2;i++)
			{
				newNodeName<<(node->nodeName)[i];
			}
			if(i == 6)
			{
				node->nodeName = "";
			}
			else
			{
				node->nodeName = newNodeName.str();
			}
			node->dataType = "string";
		}
		return true;
	}
	return false;
}

bool CSE::isID(ctrlStructElement* node)
{
	if((node->nodeName).substr(0,4) == "<ID:")
	{
		stringstream newNodeName;
		for(int i = 4;i<(node->nodeName).length()-1;i++)
		{
			newNodeName<<(node->nodeName)[i];
		}
		node->nodeName = newNodeName.str();
		node->dataType = "identifier";
		return true;
	}
	else
	{
		return false;
	}
}

void CSE::toPrint(string printValue)
{
	for(int i=0;i<printValue.size();i++)
	{
		if(printValue[i] == '\\')
		{
			i++;
			if(printValue[i] == 'n')
			{
				cout<<endl;
			}
			else if(printValue[i] == 't')
			{
				cout<<"\t";
			}
			else
			{
				cout<<"\\"<<printValue[i];
			}
		}
		else
		{
			cout<<printValue[i];
		}
	}
}
