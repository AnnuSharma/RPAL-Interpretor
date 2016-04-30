//-------------------------------------------
/**
	File:Lexer.cpp
	System:RPAL Parser
	Component Name: Lexical Analyser
	Language: C++
	Author: Annu Sharma (36731206)
	Description: This class on invoking reads 
	the next token and passes on to the Parser.
**/
//-------------------------------------------

#include "Lexer.hpp"

void Lexer::start(string File)
{

	fileptr.open(File.c_str());
	if(fileptr.fail())
	{
		cout<<"Error in opening file"<<endl;
		exit(0);
	}

}

void Lexer::stop()
{
	fileptr.close();
}

string Lexer::getNextToken()
{
	//Checking for punctuation tokens
	string nextToken;
	char nextChar;
	nextChar = fileptr.get();
	if(fileptr.eof())
	{
		return "";
	}
	if(nextChar == '(')
	{
		return "(";
	}
	if(nextChar == ')')
	{
		return ")";
	}
	if(nextChar == ';')
	{
		return ";";
	}
	if(nextChar == ',')
	{
		return ",";
	}

	//Checking for Identifiers
	if(nextChar == 'A' || nextChar == 'B' || nextChar == 'C' || nextChar == 'D' || nextChar == 'E' || nextChar == 'F' || 
		nextChar == 'G' || nextChar == 'H' || nextChar == 'I' ||nextChar == 'J' || nextChar == 'K' || nextChar == 'L' ||
		nextChar == 'M' || nextChar == 'N' || nextChar == 'O' || nextChar == 'P' || nextChar == 'Q' || nextChar == 'R' ||
		nextChar == 'S' || nextChar == 'T' || nextChar == 'U' ||nextChar == 'V' || nextChar == 'W' || nextChar == 'X' ||
		nextChar == 'Y' || nextChar == 'Z'|| nextChar == 'a' || nextChar == 'b' || nextChar == 'c' || nextChar == 'd' || 
		nextChar == 'e' || nextChar == 'f' || nextChar == 'g' || nextChar == 'h' || nextChar == 'i' || nextChar == 'j' || 
		nextChar == 'k' || nextChar == 'l' || nextChar == 'm' || nextChar == 'n' || nextChar == 'o' || nextChar == 'p' || 
		nextChar == 'q' || nextChar == 'r' || nextChar == 's' || nextChar == 't' || nextChar == 'u' || nextChar == 'v' || 
		nextChar == 'w' || nextChar == 'x' || nextChar == 'y' || nextChar == 'z')
	{
		nextToken.push_back(nextChar);
		nextChar = fileptr.get();

		while(nextChar == 'A' || nextChar == 'B' || nextChar == 'C' || nextChar == 'D' || nextChar == 'E' || nextChar == 'F'|| 
		nextChar == 'G' || nextChar == 'H' || nextChar == 'I' ||nextChar == 'J' || nextChar == 'K' || nextChar == 'L' ||
		nextChar == 'M' || nextChar == 'N' || nextChar == 'O' || nextChar == 'P' || nextChar == 'Q' || nextChar == 'R' ||
		nextChar == 'S' || nextChar == 'T' || nextChar == 'U' ||nextChar == 'V' || nextChar == 'W' || nextChar == 'X' ||
		nextChar == 'Y' || nextChar == 'Z'|| nextChar == 'a' || nextChar == 'b' || nextChar == 'c' || nextChar == 'd' || 
		nextChar == 'e' || nextChar == 'f' || nextChar == 'g' || nextChar == 'h' || nextChar == 'i' || nextChar == 'j' || 
		nextChar == 'k' || nextChar == 'l' || nextChar == 'm' || nextChar == 'n' || nextChar == 'o' || nextChar == 'p' || 
		nextChar == 'q' || nextChar == 'r' || nextChar == 's' || nextChar == 't' || nextChar == 'u' || nextChar == 'v' || 
		nextChar == 'w' || nextChar == 'x' || nextChar == 'y' || nextChar == 'z' || nextChar == '0' || 
		nextChar == '1' || nextChar == '2' || nextChar == '3' || nextChar == '4' || nextChar == '5' || nextChar == '6' || 
		nextChar == '7' || nextChar == '8' || nextChar == '9' || nextChar == '_')
		{
			nextToken.push_back(nextChar);
			nextChar = fileptr.get();
		}
		fileptr.seekg(-1,fileptr.cur);
		return nextToken;
	}

	//Checking for Integers
	if(nextChar == '0' || nextChar == '1' || nextChar == '2' || nextChar == '3' || nextChar == '4' || nextChar == '5' || 
		nextChar == '6' || nextChar == '7' || nextChar == '8' || nextChar == '9' )
	{
		nextToken.push_back(nextChar);
		nextChar = fileptr.get();
		while(nextChar == '0' || nextChar == '1' || nextChar == '2' || nextChar == '3' || nextChar == '4' || nextChar == '5' || 
		nextChar == '6' || nextChar == '7' || nextChar == '8' || nextChar == '9')
		{
			nextToken.push_back(nextChar);
			nextChar = fileptr.get();
		}
		fileptr.seekg(-1,fileptr.cur);
		return nextToken;
	}

	//Checking for Comment 

	if(nextChar == '/')
	{
		nextToken.push_back(nextChar);
		nextChar = fileptr.get();
		if(nextChar == '/')
		{
			//Token is a comment
			nextToken.push_back(nextChar);
			nextChar = fileptr.get();
			while(nextChar == '\'' || nextChar == '(' || nextChar == ')' || nextChar == ';' || nextChar == ',' || 
				nextChar == '\\' || nextChar == ' ' || nextChar == '\t' || nextChar == 'A' || nextChar == 'B' ||
				nextChar == 'C' || nextChar == 'D' || nextChar == 'E' || nextChar == 'F'|| nextChar == 'G' ||
				nextChar == 'H' || nextChar == 'I' || nextChar == 'J' || nextChar == 'K' || nextChar == 'L' ||
				nextChar == 'M' || nextChar == 'N' || nextChar == 'O' || nextChar == 'P' || nextChar == 'Q' ||
				nextChar == 'R' || nextChar == 'S' || nextChar == 'T' || nextChar == 'U' || nextChar == 'V' ||
				nextChar == 'W' || nextChar == 'X' || nextChar == 'Y' || nextChar == 'Z' || nextChar == 'a' ||
				nextChar == 'b' || nextChar == 'c' || nextChar == 'd' || nextChar == 'e' || nextChar == 'f' || 
				nextChar == 'g' || nextChar == 'h' || nextChar == 'i' || nextChar == 'j' || nextChar == 'k' || 
				nextChar == 'l' || nextChar == 'm' || nextChar == 'n' || nextChar == 'o' || nextChar == 'p' ||
				nextChar == 'q' || nextChar == 'r' || nextChar == 's' || nextChar == 't' || nextChar == 'u' || 
				nextChar == 'v' || nextChar == 'w' || nextChar == 'x' || nextChar == 'y' || nextChar == 'z' || 
				nextChar == 'C' || nextChar == '0' || nextChar == '1' || nextChar == '2' || nextChar == '3' || 
				nextChar == '4' || nextChar == '5' || nextChar == '6' || nextChar == '7' || nextChar == '8' || 
				nextChar == '9' || nextChar == '+' || nextChar == '-' || nextChar == '*' || nextChar == '<' ||
				nextChar == '&' || nextChar == '.' || nextChar == '@' || nextChar == '/' || nextChar == ':' ||
				nextChar == '=' || nextChar == '~' || nextChar == '|' || nextChar == '$' || nextChar == '!' || 
				nextChar == '#' || nextChar == '%' || nextChar == '^' || nextChar == '_' || nextChar == '[' ||
				nextChar == ']' || nextChar == '{' || nextChar == '}' || nextChar == '"' || nextChar == '`' || 
				nextChar == '?' || nextChar == '>')
			{
				nextToken.push_back(nextChar);
				nextChar = fileptr.get();
			}
			if(nextChar == '\n')
			{
				nextToken.push_back(nextChar);
				return getNextToken();
			}
			else
			{
				fileptr.seekg(-1,fileptr.cur);
				cout<<"Error in parsing comment\n";
				exit(0);
			}

		}
		else
		// Operator detected
		{
			int count = 0;
			while(nextChar == '+' || nextChar == '-' || nextChar == '*' || nextChar == '<' || nextChar == '>' ||
				nextChar == '&' || nextChar == '.' || nextChar == '@' || nextChar == '/' || nextChar == ':' ||
				nextChar == '=' || nextChar == '~' || nextChar == '|' || nextChar == '$' || nextChar == '!' || 
				nextChar == '#' || nextChar == '%' || nextChar == '^' || nextChar == '_' || nextChar == '[' ||
				nextChar == ']' || nextChar == '{' || nextChar == '}' || nextChar == '"' || nextChar == '`' || 
				nextChar == '?')
			{
				nextToken.push_back(nextChar);
				nextChar = fileptr.get();
				if(nextChar == '/')
				{
					count++;
				}
				else
				{
					count = 0;
				}
				//Comment detected
				if(count == 2)
				{
					nextToken.resize(nextToken.size()-1);
					fileptr.seekg(-2,fileptr.cur);
					return nextToken;
				}

			}
			fileptr.seekg(-1,fileptr.cur);
			return nextToken;
		}
	}

	//Checking for Operator
	if(nextChar == '+' || nextChar == '-' || nextChar == '*' || nextChar == '<' || nextChar == '>' ||
		nextChar == '&' || nextChar == '.' || nextChar == '@' || nextChar == '/' || nextChar == ':' ||
		nextChar == '=' || nextChar == '~' || nextChar == '|' || nextChar == '$' || nextChar == '!' ||
		nextChar == '#' || nextChar == '%' || nextChar == '^' || nextChar == '_' || nextChar == '[' ||
		nextChar == ']' || nextChar == '{' || nextChar == '}' || nextChar == '"' || nextChar == '`' ||
		nextChar == '?')
	{	
		int count = 0;
		nextToken.push_back(nextChar);
		nextChar = fileptr.get();

		while(nextChar == '+' || nextChar == '-' || nextChar == '*' || nextChar == '<' || nextChar == '>' ||
		nextChar == '&' || nextChar == '.' || nextChar == '@' || nextChar == '/' || nextChar == ':' ||
		nextChar == '=' || nextChar == '~' || nextChar == '|' || nextChar == '$' || nextChar == '!' ||
		nextChar == '#' || nextChar == '%' || nextChar == '^' || nextChar == '_' || nextChar == '[' ||
		nextChar == ']' || nextChar == '{' || nextChar == '}' || nextChar == '"' || nextChar == '`' ||
		nextChar == '?')
		{
			nextToken.push_back(nextChar);
			nextChar = fileptr.get();
			if(nextChar == '/')
			{
				count++;
			}
			else
			{
				count = 0;
			}
			//Comment detected
			if(count == 2)
			{
				nextToken.resize(nextToken.size()-1);
				fileptr.seekg(-2,fileptr.cur);
				return nextToken;
			}
		}
		fileptr.seekg(-1,fileptr.cur);
		return nextToken;
	}

	//Checking for String
	if(nextChar == '\'')
	{
		nextToken.push_back(nextChar);
		nextChar = fileptr.get();
		while(nextChar == 'A' || nextChar == 'B' || nextChar == 'C' || nextChar == 'D' || nextChar == 'E' ||
			nextChar == 'F'|| nextChar == 'G' || nextChar == 'H' || nextChar == 'I' || nextChar == 'J' || 
			nextChar == 'K' || nextChar == 'L' || nextChar == 'M' || nextChar == 'N' || nextChar == 'O' || 
			nextChar == 'P' || nextChar == 'Q' || nextChar == 'R' || nextChar == 'S' || nextChar == 'T' || 
			nextChar == 'U' || nextChar == 'V' || nextChar == 'W' || nextChar == 'X' || nextChar == 'Y' || 
			nextChar == 'Z' || nextChar == 'a' || nextChar == 'b' || nextChar == 'c' || nextChar == 'd' || 
			nextChar == 'e' || nextChar == 'f' || nextChar == 'g' || nextChar == 'h' || nextChar == 'i' || 
			nextChar == 'j' || nextChar == 'k' || nextChar == 'l' || nextChar == 'm' || nextChar == 'n' || 
			nextChar == 'o' || nextChar == 'p' || nextChar == 'q' || nextChar == 'r' || nextChar == 's' || 
			nextChar == 't' || nextChar == 'u' || nextChar == 'v' || nextChar == 'w' || nextChar == 'x' || 
			nextChar == 'y' || nextChar == 'z' || nextChar == 'C' || nextChar == '0' || nextChar == '1' || 
			nextChar == '2' || nextChar == '3' || nextChar == '4' || nextChar == '5' || nextChar == '6' || 
			nextChar == '7' || nextChar == '8' || nextChar == '9' || nextChar == '+' || nextChar == '-' ||
			nextChar == '*' || nextChar == '<' || nextChar == '&' || nextChar == '.' || nextChar == '@' || 
			nextChar == '/' || nextChar == ':' || nextChar == '=' || nextChar == '~' || nextChar == '|' || 
			nextChar == '$' || nextChar == '!' || nextChar == '#' || nextChar == '%' || nextChar == '^' ||
			nextChar == '_' || nextChar == '[' || nextChar == ']' || nextChar == '{' || nextChar == '}' || 
			nextChar == '"' || nextChar == '`' || nextChar == '?' || nextChar == '\\' || nextChar == '(' || 
			nextChar == ')' || nextChar == ';' || nextChar == ',' || nextChar == ' ' || nextChar == '>')
		{
			if(nextChar == '\\')
			{
				char nextCharPrev = nextChar;
				nextChar = fileptr.get();
				if(nextChar == 't' || nextChar == 'n' || nextChar == '\\' || nextChar == '\'')
				{
					nextToken.push_back(nextCharPrev);
					nextToken.push_back(nextChar);
				}
				else
				{
					cout<<"Error in parsing string\n";
					exit(0);
				}
			}
			else
			{
				nextToken.push_back(nextChar);
			}

			nextChar = fileptr.get();
			
		}
		if(nextChar == '\'')
		{
			nextToken.push_back(nextChar);
			return nextToken;
		}
		else
		{
			cout<<"Error in parsing string\n";
			exit(0);
		}		
	}

	//Checking for Spaces
	if(nextChar == ' ' || nextChar == '\t' || nextChar == '\n')
	{
		nextToken.push_back(nextChar);
		nextChar = fileptr.get();
		while(nextChar == ' ' || nextChar == '\t' || nextChar == '\n')
		{
			nextToken.push_back(nextChar);
			nextChar = fileptr.get();
		}
		fileptr.seekg(-1,fileptr.cur);
		return getNextToken();
	}
	return "";
}
