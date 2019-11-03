//============================================================================
// Name        : Calculator.cpp
// Author      : Andrew Rubinstein
// Version     : v1
// Copyright   :
// Description : An infix expression interpreter, and calculator in C++
//============================================================================

#include <unordered_map>
#include <chrono>
#include <ctime>
#include <string>
#include <iostream>

#include "stack.h"
#include "setting.h"
#include "largeint.h"


/////////////////////////////
//char/string interpreters
/////////////////////////////
int getPriority(char ator);
bool isOperator(char s);
bool isNonParentheticalOperator(char s);
bool isNumeric(char c);
bool cmpstr(const std::string &s1,const std::string &s2);
bool hasChar(const std::string &dat,const char &c);
/////////////////////////////
//Logic
/////////////////////////////
template <class t>
t calculateExpression(std::string exp);
template <class t>
t calc(char op,t and1,t and2);
template <class t>
t processStack(linkedStack<t> &operands,linkedStack<char> &operators);
//string interpreters/Logic
/////////////////////////////
template <class t>
t getNextInt(std::string data,int &index);
double getNextDouble(std::string data,int &index);
/////////////////////////////
//UI
/////////////////////////////
void interpretParam(std::string &p);
void calcWithOptions();
/////////////////////////////
/////////////////////////////
//list of previous expressions for u command in interpretParam fn
linkedStack<std::string> lastExp;
//list of previous undone expressions for r command in interpretParam fn
linkedStack<std::string> undoneExp;
//variable to hold expression for parsing
std::string expr = "";
std::unordered_map<std::string,setting<bool> > boolsettings;

int main(int argc,char* argv[])
{
	std::cout << std::endl;
  /*
   * Initializing values in settings hashmap
   * */
  {
    setting<bool> set(
    		/*name*/
    			"Show Operations",
    		/*command line command*/
    			"o",
    		/*variable*/
    			false);

    	boolsettings[set.getCommand()] = set;
    	set = setting<bool>(
    		/*name*/
    			"Debug Mode",
    		/*command line command*/
    			"d",
    		/*variable*/
    			false);
    	boolsettings[set.getCommand()] = set;
    	set = setting<bool>(
    		/*name*/
    			"Loop calculator",
    		/*command line command*/
    			"l",
    		/*variable*/
    			true);
    	boolsettings[set.getCommand()] = set;
    	set = setting<bool>(
    		/*name*/
    			"Print current Date, and time taken to run calculation",
    		/*command line command*/
    			"t",
    		/*variable*/
    			false);
    	boolsettings[set.getCommand()] = set;
  	  }//bracket to remove set variable from program memory
    	  /*
    	   * End of initialization values in settings hashmap
    	   * */
  //Beginning of section interpreting program parameters from command line
  if(argc > 1)
  {
	//setting for main program loop makes use of overloaded operator = on setting class
	boolsettings["l"] = false;
	//Set Expression to be calculated with console supplied parameter
    expr = argv[1];
    std::string arg = "";


    for(int i = 2;i<argc;i++)
    {
    	arg = argv[i];
    	interpretParam(arg);
    }

	 if(!isOperator(expr[0]) && !isNumeric(expr[0]) && expr[0] != '.'){
		  interpretParam(expr);
	  }
  }
  //End of section interpreting program parameters from command line
  //beginning of main program loop, if only one parameter is given in the
  //command line by the user loop will not run, unless that param was the loop command
  //by default the loop runs
  do
  {
	  //push expression to stack of previous entries for u command
	  lastExp.push(expr);
	  //runs the calculation using the global variable expr
	  //this function also infers the datatype of the expression based on
	  //if there is a . present in the string it will use doubles else long
	  calcWithOptions();

	  if(*boolsettings["l"])
	  {
		  //Interpreter prompt to let user know program is expecting a command/expression
		  std::cout<<std::endl<<">>";
		  getline(std::cin, expr);
		  if(!isOperator(expr[0]) && !isNumeric(expr[0]) && expr[0] != '.'){
			  interpretParam(expr);
		  }
		  //While to interpret for multiple commands for a single expression
		  //Exits when a valid expression is entered
		  while(*boolsettings["l"] && !isOperator(expr[0]) && !isNumeric(expr[0]) && expr[0] != '.')
		  {

			  std::cout<<std::endl<<">>";
			  getline(std::cin, expr);

			  if(!isOperator(expr[0]) && !isNumeric(expr[0]) && expr[0] != '.'){
				  interpretParam(expr);
			  }
		  }
	  }
  } while(*boolsettings["l"]);
  if(undoneExp.length() > 1 || lastExp.length()>1)
  {
	  std::cout<<std::endl<<"Calculator exited"<<std::endl;
  }
  return calculateExpression<int>(expr);
}
void interpretParam(std::string &p)
{
	if(p.length() == 0) {}
	else if(boolsettings.count(p)>0)
	{
		setting<bool> set = boolsettings[p];
		//inverts setting value via operator overloads of = and *
		bool data = !*set;
		setting<bool> newSetting(set.getName(),set.getCommand(),data);
		boolsettings.erase(p);
		boolsettings[p] = newSetting;
	}
	else if (cmpstr(p,"u") || cmpstr(p,"-u"))//undo
	{
		if(!lastExp.isEmpty()){
			std::string last = "";
			lastExp.top(last);
			lastExp.pop();
			undoneExp.push(last);
			expr = last;
			std::cout<<last<<std::endl;
			calcWithOptions();
		}
		else
			std::cout<<"No previous statements"<<std::endl;
	}
	else if (cmpstr(p,"r") || cmpstr(p,"-r"))//redo
	{
		if(!undoneExp.isEmpty()){
			std::string last = "";
			undoneExp.top(last);
			undoneExp.pop();
			lastExp.push(last);
			expr = last;
			std::cout<<last<<std::endl;
			calcWithOptions();
		}
		else
			std::cout<<"No statements can be redone"<<std::endl;
	}
	else
	{
		std::cout<<"Error Invalid Parameter \""<<p<<"\""<<std::endl;
		std::cout<<"Enter a mathematical expression, or you can also use \nparameters to choose between the following options"<<
				"\nvalid params are:\nt to show time taken to interpret,";
		std::cout<<" and calculate expression";
		std::cout<<"\no to show operations in order of execution in console";
		std::cout<<"\nd to show debug information in console\n";
		std::cout<<"u to show execute previous statement in console or \"undo\"\n";
		std::cout<<"r to show \"redo\"\n";
	}

	p = "";
}

void calcWithOptions()
{

	//Determines whether integer, or double math is used
	bool intMath = false;

	if(hasChar(expr,'.'))
	{
		intMath = false;
	}
	else
	{
		intMath = true;
	}

	bool timeInstruction = *boolsettings["t"];

	std::cout<<"";
	if(intMath)
	{
		  std::chrono::system_clock::time_point start,end;
		  if(timeInstruction){
		  start = std::chrono::system_clock::now();
		  }
		  //beginning of calculation
		  //------------------------
		  long result = calculateExpression<long>(expr);
		  //------------------------

		  if(timeInstruction){
			  end = std::chrono::system_clock::now();

			  std::chrono::duration<double> elapsed_seconds = end-start;
			  std::time_t end_time = std::chrono::system_clock::to_time_t(end);

			  std::cout << "finished computation at " << std::ctime(&end_time)
			            << "elapsed time: " << elapsed_seconds.count() << "s\n";
		  }

		  std::cout<<"Final Answer: "<<result<<std::endl;
	}
	else
	{
		  std::chrono::system_clock::time_point start,end;
		  if(timeInstruction){
		  start = std::chrono::system_clock::now();
		  }

		  //alternate beginning of calculation for doubles
		  //-------------------------
		  double result = calculateExpression<double>(expr);
		  //------------------------
		  if(timeInstruction){
			  end = std::chrono::system_clock::now();

			  std::chrono::duration<double> elapsed_seconds = end-start;
			  std::time_t end_time = std::chrono::system_clock::to_time_t(end);

			  std::cout << "finished computation at " << std::ctime(&end_time)
			            << "elapsed time: " << elapsed_seconds.count() << "s\n";
		  }
		  std::cout<<"Final Answer: "<<result<<std::endl;
	}

}
bool hasChar(const std::string &dat,const char &c)
{
	bool contains = false;
	for(int i = 0;!contains && i<dat.length();i++)
	{
		if(dat[i] == c)
		{
			contains = true;
		}
	}
	return contains;
}

bool cmpstr(const std::string &s1,const std::string &s2)
{
	bool isEqual = true;
	int i = 0;
	while(isEqual && s1[i] && s2[i])
	{
		if(s1[i] != s2[i])
		{
			isEqual = false;
		}
		i++;
	}

	if(s1[i] != s2[i])
	{
		isEqual = false;
	}
	return isEqual;
}
template <class t>
t calculateExpression(std::string exp)
{
	//get debug setting from boolsettings hashmap,
	//then using * operator to get setting data
	//which is an overloaded operator in the settings class
	const bool debug = *boolsettings["d"];
	//variable that is always a copy
	  char peeker;
	  //variables to hold the operands
	  t and1,and2;
	  //c is a variable to store each of the characters in the expression
	  //in the for loop
	  char currentChar;
	  linkedStack<t> initialOperands;
	  linkedStack<char> initialOperators;
	  if(debug){
	  std::cout<<"Calculating expression: "<<exp<<std::endl;
	  }
	if(debug){
	  std::cout<<"Intial Processing: "<<std::endl;
	}
	//This loop handles parsing the numbers, and adding the data from the expression
	//to the stacks
	for(int i = 0;i <= exp.length();i++)
	{
	 currentChar = exp[i];
	 if(debug){
	    std::cout<<currentChar;
	 }
	 initialOperators.top(peeker);

	    if (currentChar==')')
	    {
	      if(debug){
	        std::cout<<std::endl<<"Parentheses Process:"<<std::endl;
	      }
	      initialOperators.top(peeker);
	      linkedStack<t> inParenthesesOperands;
	      linkedStack<char> inParenthesesOperators;
	      while(!initialOperators.isEmpty() && peeker != '(')
	      {
	        initialOperands.top(and1);
	        initialOperands.pop();
	        inParenthesesOperands.push(and1);
	        inParenthesesOperators.push(peeker);
	        initialOperators.pop();
	        initialOperators.top(peeker);
	      }
	      if(initialOperands.isEmpty())
	      {
	        std::cout<<"\nInvalid Syntax\n";
	      }

	      initialOperands.top(and2);
	      initialOperands.pop();
	      inParenthesesOperands.push(and2);
	      initialOperators.pop();
	      //Send expression in parentheses to processStack for evaluation
	      and2 = processStack(inParenthesesOperands,inParenthesesOperators);
	      initialOperands.push(and2);
	      if(isNumeric(exp[i+1]) || exp[i+1] == '(')
	      {
	        initialOperators.push('*');
	      }

	if(debug){
	  std::cout<<"Back to initial processing:"<<and2<<std::endl;
	}
	    }
	    //Section to parse numeric values from expression as a string to be inserted into
	    //initialOperands stack
	    if(isNumeric(currentChar) ||
	    ((currentChar == '-' || currentChar == '.')&& (i == 0 || isNonParentheticalOperator(exp[i-1]) || exp[i-1] =='(')
	    ))
	    {
	    	//This comment block must be enabled to use Integer instead of long
	    	//as the number's datatype
	    	/*if(cmpstr(typeid(and1).name(),"i"))
	    	{
	  	      int nextInt = getNextInt<int>(exp,i);
	  	      initialOperands.push(nextInt);
	    	}
	    	else */
	    	if (cmpstr(typeid(and1).name(),"l"))
			{
	  	      long nextInt = getNextInt<long>(exp,i);
	  	      initialOperands.push(nextInt);
			}
	    	else
	    	{
	    		double nextDouble = getNextDouble(exp,i);
	    		initialOperands.push(nextDouble);
	    	}
	    }
	    //This is to support multiplication when expressions look like 2(4)
	    else if(currentChar == '(' && isNumeric(exp[i-1]))

	    {
	      initialOperators.push('*');
	      initialOperators.push(currentChar);
	    }

	    //Section to parse operator's values from expression as a string to be inserted into
	    //initialOperators stack
	    else if(currentChar == 'x' || currentChar == 'X')
	    {
	    	initialOperators.push('*');
	    }
	    else if(isOperator(currentChar) && currentChar != ')')
	    {

	      initialOperators.push(currentChar);
	    }
	  }
	//Finally pop all values off initial stack onto final stacks for processing
	  linkedStack<t> finalOperands;
	  linkedStack<char> finalOperators;
	while(!initialOperands.isEmpty() || !initialOperators.isEmpty())
	{
	  if(!initialOperands.isEmpty()){
	  initialOperands.top(and1);
	  finalOperands.push(and1);
	  initialOperands.pop();
	  }
	  if(!initialOperators.isEmpty()){
	  initialOperators.top(peeker);
	  finalOperators.push(peeker);
	  initialOperators.pop();
	  }
	}

	if(debug){
	  std::cout<<"\nFinal Process:"<<std::endl;
	}
	//process values in stacks, and return final solution
	return processStack(finalOperands,finalOperators);
}
template <class t>
t processStack(linkedStack<t> &operands,linkedStack<char> &operators)
{
  t result = 0,and1 = 0,and2 = 0, and3 = 0,and4 = 0;
  char firstOperator,nextOperator,finalOperator;
  while(!operators.isEmpty() && operands.length()>1)
{
//initialize values so we can make sure no garbage, or previous values is in the variables
//so top function can work properly
  nextOperator = 'a',finalOperator ='a',firstOperator = 'a';

  operands.top(and1);
  operands.pop();

  operands.top(and2);
  operands.pop();

  operators.top(firstOperator);
  operators.pop();

  operators.top(nextOperator);
  if(getPriority(nextOperator)>getPriority(firstOperator))
  {
    operators.pop();
    operators.top(finalOperator);

    if(getPriority(finalOperator)>getPriority(nextOperator))
    {
      operators.pop();
      operators.push(nextOperator);
      operators.push(firstOperator);

      operands.top(and3);
      operands.pop();

      operands.top(and4);
      operands.pop();

      operands.push(calc(finalOperator,and3,and4));

      operands.push(and2);
      operands.push(and1);


    }
    else
    {
      operands.top(and3);
      operands.pop();

      operands.push(calc(nextOperator,and2,and3));

      operands.push(and1);
      operators.push(firstOperator);
    }

  }
  else
  {
    operands.push(calc(firstOperator,and1,and2));
  }
}

operands.top(result);
  return result;
}

template <class t>
t getNextInt(std::string data,int &index)//index is a reference so that when we return
//The for loop which passed it's counter variable is updated to look at the next char after
//This number
{
  bool stillReading = true;
  bool isNegative = false;
  char previous;
  t num = 0;
  if(index-1 >= 0)
  {
    previous = data[index-1];
  }
  else
  {
    previous = 'a';
  }
  if(isOperator(data[index]) && (isOperator(previous) || index == 0))
  {
    if(data[index] == '-'){
      isNegative = true;
    }
    index++;
  }
  while(stillReading)
  {

    if(data[index]>=48 && data[index]<58)
    {
      num *= 10;
      num += (int)(data[index]-48);
    }
   if(!isOperator(previous) && index != 0)
  {

    if(data[index]<48 || data[index]>=58)
    {
      stillReading = false;
    }

  }
    previous = data[index++];

  }
  index -= 2;
  if(isNegative)
    num *= -1;
  return num;
}
double getNextDouble(std::string data,int &index)
{
  bool stillReading = true;
  bool isNegative = false;
  bool afterDecimal = false;
  char previous;
  double num = 0;
  int afterDecimalCount = 1;

  if(index-1 >= 0)
  {
    previous = data[index-1];
  }
  else
  {
    previous = '&';
  }
  if(isOperator(data[index]) && (isOperator(previous) || index == 0))
  {
    if(data[index] == '-')
      isNegative = true;
    index++;
  }
  while(stillReading)
  {
    if(data[index]>=48 && data[index]<58)
    {
    	if(!afterDecimal){
    		num *= 10;
    		num += (double)(data[index]-48);
    	}
    	else
    	{
    		num += (double) (data[index]-48)/afterDecimalCount;
    	}
    }
    if(data[index] == '.')
    {
    	afterDecimal = true;
    }
    else if(!isOperator(previous) && index != 0)
  {

    if(data[index]<48 || data[index]>=58)
    {
      stillReading = false;
    }

  }
    if(afterDecimal)
    {
    	afterDecimalCount *= 10;
    }
    previous = data[index++];

  }
  index -= 2;
  if(isNegative)
    num *= -1;
  return num;
}
int getPriority(char ator)
{
  int priority = 0;
  if(ator == '(' || ator == ')')
  {
    priority = 0;
  }
  else if (ator == '^')
  {
    priority = 90;
  }
  else if (ator == '*' || ator == '/' || ator == '%')
  {
    priority = 80;
  }
  else if (ator == '+' || ator == '-'  || ator == '&' || ator == '|' )
  {
    priority = 70;
  }
  return priority;
}

template <class t>
t calc(char op,t and1,t and2)
{
	t result = 0;
        if(op=='+')
        result = (and1+and2);
        else if(op == '-')
        result = (and1-and2);
        else if(op == '*')
        result = (and1*and2);
        else if(op == '/')
        {
        	if(and2 != 0)
        		result = (and1/and2);
        	else
        		result = -400;
        }
        else if(op == '%')
        {
      	if(and2 != 0)
      		result = ((int) and1)%((int)and2);
      	else
      		result = -400;
        }
        else if(op == '^')
        {
          result = 1;
          for(int i = 0;i < and2;i++)
          result *= and1;
        }
        else
          result = -400;
    if(*boolsettings["o"])
      std::cout<<and1<<op<<and2<<" = "<<result<<std::endl;
    return result;
}

bool isOperator(char s)
{
  return s == '&' || s == '|' || s == '+' || s == '-' || s == '*' || s == '/' || s == '^' || s == '(' || s == ')' || s == '%';
}
bool isNonParentheticalOperator(char s)
{
    return s == '&' || s == '|'  || s == '+' || s == '-' || s == '*' || s == '/' || s == '^' || s == '%';
}
bool isNumeric(char c)
{
  return (c >= 48 && c<58);
}
