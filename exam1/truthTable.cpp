#include <iostream>
#include <vector>
#include <cstdlib>
#include <string.h>
#include <algorithm> // std::find
#include <bitset>
#include <map>
#include <math.h>
#include <iomanip> // std::setw
#include <fstream>

using namespace std;

void rowEvaluate(string rpn, vector<bool> truthTable, map<string, unsigned int> indexCols, bool &result);
vector<vector<bool>> genTruthTable(unsigned int num);
map<string, unsigned int> mapLetterToColumn(bitset<26> setLetters);
bitset<26> countLetter(string input);
bool invalidCharAfter(char inputChar);
bool illegalImplication(string &input);
void removeSpace(string &input);
bool illegalSet(string input);
int whoIsFirst(const string &incoming);
bool precedence(const string &incoming, const string &tos);
bool convertToRPN(string input, string &output);
bool getInput(string &line);
vector<vector<bool>> process(string rpn, string originExp, string command);
bool conjunctionTwoSets(bool x, bool y, bool &result); //AND
bool disjunctionTwoSets(bool x, bool y, bool &result); //OR
bool notConjunction(bool x, bool y, bool &result);     //NAND
bool notDisjunction(bool x, bool y, bool &result);     //NOR
bool setCompliment(bool x, bool &result);              //NOT
bool exclusiveOr(bool x, bool y, bool &result);        //XOR
bool implication(bool x, bool y, bool &result);        // =>
bool biImplication(bool x, bool y, bool &result);      // <=>
bool commandInput(string &input, vector<vector<string>> &totalExpression);
bool checkCommandValid(string input);
bool newCommand(string input, unsigned int pos, vector<vector<string>> &totalExpression);
bool deleteCommand(string input, unsigned int pos, vector<vector<string>> &totalExpression);
bool tableCommand(string input, unsigned int pos, vector<vector<string>> &totalExpression);
bool listCommand(vector<vector<string>> totalExpression,string input);
void printTruthTable(vector<vector<bool>> truthTable, bitset<26> numLetters, string originExp);
bool compareExpression(string input, unsigned int pos, vector<vector<string>> totalExpression);
unsigned int commandAppear(string input, string command);

int main()
{
    string line, output;                    //Create input (line) and output (output) variables for functions to use
    int sets[26];                           //Create a 26 element array of sets
    vector<vector<string>> totalExpression; //Storage of all input expression and RPN
    while(getInput(line))                   //As long as there is input from the keyboard
    {        
        // if (convertToRPN(line, output)) //See if we can convert infix to postfix notation
        //     process(output, sets); //process(output,sets); //If we can, process the input
        // else //If not, tell the user that there was bad input
        //     cout << "Illegal expression!!!" << endl;
        if(commandInput(line,totalExpression))
        {
            // string rpn = totalExpression[0][1];
            // process(rpn,sets);
        }
        else 
            cout << "Illegal expression!!!" << endl;
        cout <<"--------------"<<endl;
    }
    return 0;
}
 
bool getInput(string &line)
{
    cout<<"In-fix expression: ";
    getline(cin, line);                           //Get infix expression 
    fflush(stdin);                                //Clear input buffer
    for(unsigned int i = 0; i < line.size(); ++i) //standardize set names to uppercase
        line[i] = toupper(line[i]);
    return line != "";                            //see if the line was empty
}
 
int whoIsFirst(const string &incoming) //Convert operator to its precedence value
{
    int value = 0;
    switch(incoming[0])
    {
        //verilog https://class.ece.uw.edu/cadta/verilog/operators.html
        case '~' : value = 8;   //NOT 
                   break;
        case '&' : value = 7;   //AND
                   break;
        case '|' : value = 6;   //OR
                   break;
        case '@' : value = 5;   //NAND
                   break;
        case '%' : value = 4;   //NOR
                   break;
        case '^' : value = 3;   //XOR
                   break;
        case '>' : value = 2;  //Implication =>
                   break;
        case '<' : value = 1;  //Bi-Implication <=>
    }
    return value;
}
 
bool illegalSet(string input)          //See if the user entered a double comma or something
{                                      //like {, or ;}
    unsigned int pos, size = input.size();
    while((pos = input.find(' ')) < size) //Find spaces
        input.erase(pos,1);
    return (input.find("&&") < size || // operator appears twice
            input.find("||") < size ||
            input.find("@@") < size ||
            input.find("%%") < size ||
            input.find("^^") < size ||
            input.find("==") < size ||
            input.find("<<") < size ||
            input.find("(,") < size ||
            input.find(",)") < size ||
            input.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ()=><&|@%^~") < size);
}
 
bool precedence(const string &incoming, const string &tos) //Return TRUE is incoming operator
{
     return whoIsFirst(incoming) < whoIsFirst(tos);  //is less than what is on the top of the operator stack
}
 
bool convertToRPN(string input, string &output)
{
    //reduce '=>' or '<=>' to '>' or '<'
    //check invalid operator & % ^ 
    if(illegalSet(input) || !illegalImplication(input))             
        return false;                             //invalid implication input check
    cout << "The expression after reducing: " << input << endl;
    vector<string>  operatorStack;                //Holds operators
    string  op, operand, parens;                  //Holds the current operator and operand
    output = "";                                  //Initialize output to empty
    unsigned int pos, pos2;                       //Used to find position of substrings within a string
    while(input.size() > 0)                       //As long as there is still input
    {
        if(input[0]>='A' && input[0] <= 'Z')      //Did we read the name of a set?
        {                                         //If so, move it directly to the output
            if(input[1] == '~' || input[1] == '(') 
                return false;                       //missing an operator to perform bitwise A&B(&D)
            operand = input[0];
            output += operand + " ";
            input.erase(0,1);                     //Remove the operand from the input
        }
        else                                      //Otherwise
        {
        switch(input[0])                        //See what the operator "could" be
        {
            case ' ' :  input.erase(0,1);        //Dump any space that has been read from the input
                        break;

            case '>':
            case '<':
            case '~':
            case '&':
            case '|':                           //If it is any valid operator
            case '@':                           //we either immediately push it onto the operand stack
            case '%':                           //or push higher precedence operators currently in the stack
            case '^' :  op = input[0];          //onto the output
                        if(invalidCharAfter(input[1]))  
                            return false;       //invalid two operators next to each other
                        while ((operatorStack.size() > 0) && precedence(op, operatorStack[operatorStack.size() - 1]))
                        {
                            // if(operatorStack.back() == ">")
                            //     output += "=" + operatorStack.back() + " ";
                            // else if (operatorStack.back() == "<")
                            //     output += operatorStack.back() + "=>" + " ";
                            output += operatorStack.back() + " ";
                            operatorStack.pop_back();
                        }
                        operatorStack.push_back(op); //Insert current operator onto operator stack
                        input.erase(0, 1);           //Remove the current operator from input
                        break;
            case '(' :  operatorStack.push_back("("); //Parenthesis are a "special case"
                        input.erase(0,1);             //Push the opening onto the operand stack and wait till
                        break;                        //a closing parentheses is found
            case ')' :  while(operatorStack.size() > 0 && operatorStack.back() != "(")
                        {                             //Once found, keep pushing operators onto output
                            output += operatorStack.back() + " ";
                            operatorStack.pop_back();  //Until we either empty the stack or find a opening paren
                        }
                        if(operatorStack.size() == 0)   //"(" must be exist
                            return false;               //missing "(" or still left operators in stack
                        else
                            operatorStack.pop_back();   //pop back "("
                        input.erase(0,1);
                        break;
            default  :  
                    return false;
        }
        }
    }
    while(operatorStack.size() > 0)  //If there are any additional operators left on the stack
    {                                //we push them onto output unless we find a mis-matched paren
        string op = operatorStack.back();
        if(op[0] == '(')
            return false;
        // if (operatorStack.back() == ">")
        //     output += "=" + operatorStack.back() + " ";
        // else if (operatorStack.back() == "<")
        //     output += operatorStack.back() + "=>" + " ";
        // else 
        output += op + " ";
        operatorStack.pop_back();
    }
    return true;                      //Signify a successful conversion to RPN
}
 
/*  2-dimensional Truthtable is created based on RPN expression
    Evaluate each row of truthtable and assign final value to the last column of each row.
    @countLetter: Number of letters in RPN expression
    @indexCols:   Column number correspond to a letter <letter,its column index>
    @truthTable:  Rows = 2^numLetters, Columns = numLetters + an extra column for a result
    @output:      The result of a row after evaluating  
 */
vector<vector<bool>> process(string rpn, string originExp, string command) //Process the RPN on sets
{
    bitset<26> numLetters = countLetter(rpn);                               //Count number presents in RPN expression
    map<string,unsigned int> indexCols = mapLetterToColumn(numLetters);       //Map each letter to corresponding column in truth table
    vector<vector<bool>> truthTable = genTruthTable(numLetters.count());    //Create a truth table
    cout<<"Translated to RPN: "<<rpn<<endl;
    unsigned int totalRows = pow(2,numLetters.count());
    bool output = false;
    for(int i=0; i<truthTable.size(); ++i)
    {        
        try
        {
            rowEvaluate(rpn,truthTable[i],indexCols,output);      //get output for each row 
            truthTable[i].push_back(output);                      //add result to the last column 
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }               
    }

    //PRINT TRUTH TABLE
    if(command == "print")
        printTruthTable(truthTable,numLetters,originExp);
    // cout << "TRUTH TABLE Rows: "<< totalRows << " Cols: " << (numLetters.count()+1) << endl;
    // for(int i=0; i < truthTable.size(); ++i)
    // {
    //     for (int j = 0; j < numLetters.count()+1; ++j)
    //         cout << truthTable[i][j] << "   ";
    //     cout << endl;
    // }
    return truthTable;
}

/*  Evaluate RPN expression in horizontal
 */
void rowEvaluate(string rpn, vector<bool>truthTable, map<string,unsigned int> indexCols,bool& finalResult)    //Process the RPN on sets
{
    vector<bool> operandStack;        //Create an operand and operator stack
    vector<char> operatorStack;
    // string output,x,y;                      //Create some temporary variables
    bool first, second, output;                  //Index column of letter need to get value


    while(rpn.size() > 0)               //As long as there are inputs available
    {
        if(rpn[0] >= 'A' && rpn[0] <= 'Z') //If a named set, push onto the operand stack
        {
            string letter = string(1,rpn[0]);               //convert char to string(size_t,char)
            unsigned int index = indexCols[letter];         //get index of element need to be evaluated
            operandStack.push_back(truthTable[index]);      //get value at index column matches the letter from RPN
            rpn.erase(0,1);
        }
        else                                //Otherwise
        {
            switch(rpn[0])                  //See what the operator is
            {
                case ' ' :  rpn.erase(0,1); //Get rid of spaces
                            break;
                case '~' :  first = operandStack.back();     //If compliment operator
                            operandStack.pop_back();     //Pop an operand and
                            setCompliment(first, finalResult); //compliment it
                            operandStack.push_back(finalResult); //Push the result back onto the operand stack
                            rpn.erase(0,1);
                            break;
                case '&' :  second = operandStack.back();    //If it is AND, two operands are required                           
                            operandStack.pop_back();    //Pop them, then perform the union
                            first = operandStack.back();
                            operandStack.pop_back();
                            conjunctionTwoSets(first, second,finalResult);
                            operandStack.push_back(finalResult); //Then place the result onto the operand stack
                            rpn.erase(0,1);                 //Delete from input the operand
                            break;
                case '|' :  second = operandStack.back();        //If it is OR, two operands are required
                            operandStack.pop_back();        //Pop them, then perform the intersection
                            first = operandStack.back();
                            operandStack.pop_back();
                            disjunctionTwoSets(first, second, finalResult);//The place the result onto the operand stack
                            operandStack.push_back(finalResult); //Then place the result onto the operand stack
                            rpn.erase(0,1);                 //Delete from input the operand
                            break;
                case '@' :  second = operandStack.back();        //If it is NAND, two operands are required
                            operandStack.pop_back();        //Pop them, then perform the NAND
                            first = operandStack.back();
                            operandStack.pop_back();
                            notConjunction(first, second, finalResult);//The place the result onto the operand stack
                            operandStack.push_back(finalResult); //Delete from input the operand
                            rpn.erase(0,1);
                            break;
                case '%':   second = operandStack.back();        //If it is NOR, two operands are required
                            operandStack.pop_back();        //Pop them, then perform the NOR
                            first = operandStack.back();
                            operandStack.pop_back();
                            notDisjunction(first, second, finalResult);   //The place the result onto the operand stack
                            operandStack.push_back(finalResult); //Delete from input the operand
                            rpn.erase(0, 1);
                            break;
                case '^':   second = operandStack.back();        //If it is XOR, two operands are required
                            operandStack.pop_back();        //Pop them, then perform the XOR
                            first = operandStack.back();
                            operandStack.pop_back();
                            exclusiveOr(first, second, finalResult);      //The place the result onto the operand stack
                            operandStack.push_back(finalResult); //Delete from input the operand
                            rpn.erase(0, 1);
                            break;
                case '>':   second = operandStack.back();            //If it is implication, two operands are required
                            operandStack.pop_back();            //Pop them, then perform the implication
                            first = operandStack.back();
                            operandStack.pop_back();
                            implication(first, second, finalResult);      //The place the result onto the operand stack
                            operandStack.push_back(finalResult); //Delete from input the operand
                            rpn.erase(0, 1);
                            break;
                case '<':   second = operandStack.back();            //If it is Bi-implication, two operands are required
                            operandStack.pop_back();                //Pop them, then perform the bi-implication
                            first = operandStack.back();
                            operandStack.pop_back();
                            biImplication(first, second, finalResult);      //The place the result onto the operand stack
                            operandStack.push_back(finalResult); //Delete from input the operand
                            rpn.erase(0, 1);
                            break;
            }
        }
    }
}
 

 
//The functions below are for you to complete. You can use bitset or ints, but you will have to
//adjust the function above to work correctly with bitsets
bool conjunctionTwoSets(bool x, bool y,bool& result)
{
    result = (x && y);      //AND
    return result;
}
 
bool disjunctionTwoSets(bool x, bool y, bool &result)
{
    result = (x || y);      //OR
    return result;
}
 
bool notConjunction(bool x, bool y, bool &result)
{
    result = !(x && y);     //NAND
    return result;
}

bool notDisjunction(bool x, bool y, bool &result)
{
    result = !(x || y);     //NOR
    return result;
}

bool exclusiveOr(bool x, bool y, bool &result)
{
    result = (x ^ y);     //NOR
    return result;
}

bool implication(bool x, bool y, bool &result)
{
    ((x == true) && (y == false)) ? (result=false) : (result=true);  //only false if left false, right true
    // if(x == true && y == false)          
    //     result = false;
    // else 
    //     result = true;
    return result;
}

bool biImplication(bool x, bool y, bool &result)
{
    (x == y) ? (result=true) : (result=false);    
    return result;
}

bool setCompliment(bool x, bool &result)
{
    result = !x;        //NOT
    return result;
}


/* Remove all spaces from user's input */
void removeSpace(string &input)
{
    int pos = 0;
    while (input.find(" ") < (input.size()))
    {
        int pos = input.find(" ");
        input.erase(pos, 1);
    }
    cout << "Input after remove space : " << input << endl;
}

// Find illegal input of implication or bi-immplication
// Reduce the implication from "=>" to '>' if it's valid
// Reduce the bi-implication from "<=>" to '<' if it's valid
bool illegalImplication(string &input)
{
    removeSpace(input); //remove space before converting
    for(int i=0; i < input.size();++i)
    {
        switch (input[i])
        {
            //Implication
            case '=':   
                {                    
                    if(input[i+1] != '>')       //invalid implication "=>" format
                        return false;
                    else // (input[i + 1] == '>')
                    {
                        if(invalidCharAfter(input[i + 2])) //found illegal char after '='
                            return false;
                    }
                    input.erase(i, 1); // reduce "=>" to ">"
                    break;
                }            
            //bi-implication                          
            case '<':   
                {
                    if (input[i + 1] != '=') //invalid if the char after is not "="
                        return false;
                    else
                    {
                        if (input[i + 2] != '>') //invalid if the char after is not "="
                            return false;
                        else
                        {
                            if (invalidCharAfter(input[i + 3])) //found illegal char after '>'
                                return false;
                            else
                                input.erase(i + 1, 2); // reduce "<=>" to "<"
                        }
                    }
                    break;
                }
            case '>':   
                {
                    // cout << "GOING TO > case" << endl;
                    if (input[i - 1] != '=')
                        return false; // "=" doesn't go after ">"
                    if (invalidCharAfter(input[i + 1]))
                        return false; //found illegal char after '>'
                }
            default:    break;
        }
    }
    return true;   //if no found any invalid implication
}


//check if the character after => or <=> is not aletter, "(", or "~"
bool invalidCharAfter(char inputChar)
{
    char illegalChar[] = ">=)<&|@%^";
    if (strchr(illegalChar, inputChar) != NULL) 
        return true;
}

/*  Created a 2-dimensional dynamic array. Take n-th index of a row and convert to bitset.
    Convert the index number to bitset(number start 0,1,2,3... -> 2^n).
    Map each single bit of a bitset to each array element in horizontal .
    @num: number of letters A-Z
    @numRows: total number of array's rows 2^n
    @binTable: 2d-array 
    @bitNum: binary string used to assign each element of array in a row.
 */
vector<vector<bool>> genTruthTable(unsigned int num)
{
    vector<vector<bool>> binTable = {{}};
    int numRows = pow(2,num);
    binTable.resize(numRows);           //get an actual size of the array
    bitset<27> bitNum = 0;              //initalize the first number
    for(int i=0; i < numRows;++i)
    {
        bitNum = i; //assign a N-th row
        for(int j=num-1; j >=0;j--)
        {
            (bitNum.test(j)) ? (binTable[i].push_back(true)) : (binTable[i].push_back(false)); // push true if bitset is 1, otherwise 0
        }
    }
    //print the truth table
    // cout << "TRUTH TABLE Rows: "<< numRows << " Cols: " << num << endl;
    // for(int i=0; i < binTable.size(); ++i)
    // {
    //     for (int j = 0; j < num; ++j)
    //         cout << binTable[i][j] << " ";
    //     cout << endl;
    // }
    return binTable;    
}

/*  Count the number of letters present in the valid RPN input
    to determine how many number of columns in the truthtable
    and which letters present in the string input.
 */
bitset<26> countLetter(string input)
{
    bitset<26> letters = 0; // each bit represent the appearance of alphabet letter
    for (int i = 0; i < input.size(); ++i)
    {
        if (input[i] >= 'A' && input[i] <= 'Z')
        {
            unsigned int index = input[i] - 'A'; //determine the index in bitset
            if (!letters.test(index))
                letters.set(index); //turn on the bit
        }
    }
    // cout << "The total letters appear are: " << letters.count() << endl;
    return letters;
}

/*  Create a map<index of letter,index of column>
    To determine which letter correspond to a truthtable's index column
    Each ON bitset will map to index of corresponding column.
    @letterMap: the cotainer store <key,value> with key is index of bitset,
                value is index of the column
    @index: the index of column correspond to the letter in the RPN expression                
    @setLetters: bitset<26> represents the letters appear in RPN expression
 */
map<string, unsigned int> mapLetterToColumn(bitset<26> setLetters)
{
    map<string, unsigned int> letterMap;
    unsigned int index = 0;
    for (int i = 0; i < setLetters.size(); ++i)
    {
        //insert key is an index of ON bitset
        //and value is an index of column
        if (setLetters.test(i))
        {
            string c = string(1,char(65 + i));
            letterMap.insert(pair<string, unsigned int>(c, index++));
        }
    }

    // map<string, unsigned int>::iterator it = letterMap.begin();
    // for (it = letterMap.begin(); it != letterMap.end(); ++it)
    //     cout << it->first << " => " << it->second << '\n';
    return letterMap;
}


/*  Create a set of COMMANDs to determine which action need to pass
    when the user enter a command.
 */

bool commandInput(string &input,vector<vector<string>>& totalExpression)
{ 
    unsigned int pos = -1;  //pos of command
    if(!checkCommandValid(input))
    {
        // cout << "Command is ambiguous." << endl;
        return false;
    }
    //NEW COMMAND
    if((pos = input.find("NEW")) < input.size())
    {
        return newCommand(input,pos,totalExpression);       //return false if invalid expression
    }

    //DELETE COMMAND
    if((pos=input.find("DELETE")) < input.size())
    {
        return deleteCommand(input,pos,totalExpression);
    }

    //TABLE COMMAND print out truth table
    if((pos=input.find("TABLE")) < input.size())
    {
        return tableCommand(input,pos,totalExpression);
    }

    //IS COMMAND compare two expressions
    if((pos=input.find("IS")) < input.size() && (pos=input.find("LIST")) > input.size())
    {
        return compareExpression(input, pos, totalExpression);
    }

    if((pos=input.find("SIZE")) < input.size())
    {
        cout << "Size of vector expressions: " << totalExpression.size() << endl;
    }

    //LIST COMMAND 
    if((pos=input.find("LIST")) < input.size())
    {
        return listCommand(totalExpression,input);
    }

    return true;    //command valid
}

/*  Only accept one command at once
    Return false if more than one command appears.
*/
bool checkCommandValid(string input)
{
    string command[12] = {"SIZE","NEW","DELETE","LIST","IS","LOAD","EDIT","QUIT","WEXIT","WQUIT","HELP","TABLE"};
    int count = 0;  //count number of commands appear in the input
    int pos = 0;    //position of first occurence of the sequence
    for (int i = 0; i < sizeof(command) / sizeof(command[0]); ++i)
    {
        //number of times a command appears
        if(commandAppear(input,command[i]) >= 2)
        {
            cout << "Command is ambiguous." << endl;
            return false;
        }        

        if((pos = input.find(command[i])) < input.size())
            count++;
        if(count >=2)
        {
            cout << "Only accept one command at once." << endl;
            return false;                                   //too many commands
        }
    }
    if (count == 0)
    {
        cout << "Invalid command. Please type LIST to see instructions." << endl;
        return false;
    }

    return true;    //input valid count == 1
}

// Count how many times a command appears in input
unsigned int commandAppear(string input,string command)
{
    int count = 0;
    int pos = 0;
    
    while ((pos = input.find(command, pos)) < input.size())
    {
        input = input.substr(pos + command.size());
        ++count;
        if(count>=2)
            return count;
    }
    return count;   //valid if appears once
}


bool newCommand(string input, unsigned int pos,vector<vector<string>>& totalExpression)
{
    string originExp = input.substr(pos+3);    //get originExp after the command
    string rpn = "";
    if(convertToRPN(originExp,rpn))
    {
        //add a vector include an original express and RPN originExp
        //1 row and 2 column
        vector<string> expression;
        expression.push_back(originExp);
        expression.push_back(rpn);
        totalExpression.push_back(expression);
        cout << "EXPRESSION "<<totalExpression.size()<<" entered." <<endl;
        return true;
    }
    else 
        return false;   //illegal expression
}

bool deleteCommand(string input,unsigned int pos,vector<vector<string>>& totalExpression)
{
    unsigned int checkNum = 0;
    string indexExp = input.substr(pos+6);      //get index of expression need to be deleted
    removeSpace(indexExp);  
    if(totalExpression.empty() && ((indexExp.find_first_not_of("0123456789")) > indexExp.size()))
    {
        cout<< "There doesn't have any expression to delete." << endl;
        return true;
    }
    if((indexExp.find_first_not_of("0123456789")) < indexExp.size())
    {
        cout << "The index expression is invalid." << endl;
        return false;
    }
    else if(totalExpression.size()>0 && stoi(indexExp) <= totalExpression.size())
    {
        
        try
        {
            unsigned int index = stoi(indexExp);
            totalExpression.erase(totalExpression.begin()+index-1);     //first index-0
            cout << "DELETED "<< index;
            cout << ". Size after deleting " << totalExpression.size() << endl;
            return true;    //Delete fulfil
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }        
    }
    else
    {
        cout << "The number of expressions is less than index." << endl;
        return true;
    }
    return false;//invalid input
}

bool tableCommand(string input,unsigned int pos,vector<vector<string>>& totalExpression)
{
    unsigned int checkNum = 0;
    string indexExp = input.substr(pos+5);      //get expression need to be deleted
    removeSpace(indexExp);
    if(totalExpression.empty() && ((indexExp.find_first_not_of("0123456789")) > indexExp.size()))
    {
        cout<< "There doesn't have any expression to print." << endl;
        return true;
    }
    if((indexExp.find_first_not_of("0123456789")) < indexExp.size())
    {
        cout << "The index expression is invalid." << endl;
        return false;
    }
    else if(totalExpression.size()>0 && stoi(indexExp) <= totalExpression.size())
    {
        
        try
        {
            unsigned int index = stoi(indexExp);                                    //get index of truth table
            process(totalExpression[index-1][1],totalExpression[index-1][0],"print");         //print out truth table with rpn exp
            return true;    //Delete fulfil
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }        
    }
    else
    {
        cout << "The number of expressions is less than index." << endl;
        return true;
    }
    return false;//invalid input
}

void printTruthTable(vector<vector<bool>> truthTable, bitset<26> numLetters,string originExp)
{
    unsigned int columns = numLetters.count()+1;    //plus result column
    removeSpace(originExp);
    cout << "TRUTH TABLE -- Rows: "<< truthTable.size() << "-- Cols: " << columns << endl;
    //Print out signature
    cout <<setw(5);
    for(int i=0;i < numLetters.size();++i)
    {
        if(numLetters.test(i))
        {   
            cout<<char(65+i);
            cout << setw(5);
        }
    }
    cout << setw(3+originExp.size());
    cout << originExp << endl;
    //print out TruthTable
    cout << setw(5);
    for(int i=0; i < truthTable.size(); ++i)
    {
        for (int j = 0; j < columns; ++j)
        {
            char binChar;
            (truthTable[i][j] == true) ?(binChar = 'T'):(binChar='F');
            cout << binChar;
            cout << setw(5);
        }
        cout << endl;
    }
}

/*  Compare two expressions after evaluating each of them
    Only if two expressions have same number of letters.
 */
bool compareExpression(string input, unsigned int pos, vector<vector<string>> totalExpression)
{
    string compareExp = input.substr(pos+2);    //get comparison expressions
    unsigned int equalPos ;                     //find position "="
    unsigned int first = 0;                        //left expression number 
    unsigned int second = 0;                       //right expression number
    vector<vector<bool>> leftTable ;            //left truth table
    vector<vector<bool>> rightTable;            //right truth table    
    unsigned int c1 = 0;       //indexs of last columns from two tables to evaluate
    unsigned int c2 = 0;
    removeSpace(compareExp);
    //check empty storage of expression
    if (totalExpression.empty() && (compareExp.find_first_not_of("0123456789= ")) > input.size())
    {
        cout << "There is no any expression to evaluate." << endl;
        return false;
    }
    else if ((compareExp.find_first_not_of("0123456789= ")) < input.size() )
    {
        cout << "Invalid Comparison."<<endl;
        return false;       //invalid input
    }
    else
    {
        //check invalid 
        int count = 0;
        int equalPos = 0;
        for(int i=0;i<compareExp.size();++i)
        {
            if(compareExp[i] == '=') count++;
            if(count > 1) 
            {
                cout << "Comparison is ambiguous." <<endl;
                return false;
            }
        }
    }

    //if the command is valid
    if(totalExpression.size()>0)
    {
        equalPos = input.find("=");    //find position "="
        first = stoi(input.substr(pos+2,equalPos-pos-2));
        second = stoi(input.substr(equalPos+1));        
    }
    //first or second is out of range of expression number
    if(first > totalExpression.size() || second > totalExpression.size())
    {
        cout << "The expression number is out of range" << endl;
        return false;
    }
    else
    {
        //get truth tables
        leftTable = process(totalExpression[first-1][1],totalExpression[first-1][0],"print");
        rightTable = process(totalExpression[second-1][1], totalExpression[second-1][0], "print");
    }
    
    if(leftTable.size() != rightTable.size())
    {
        cout << "The number letters in two expressions are not equal." << endl;
        return false;
    }
    else
    {
        //get indexs of last columns from two tables to evaluate
        c1 = leftTable[0].size()-1;
        c2 = rightTable[0].size()-1;
        //compare two expressions
        for(int i=0;i<leftTable.size();++i)
        {
            if(leftTable[i][c1] != rightTable[i][c2])
            {
                cout << "The expression " << first << " is not equivalent to " << second << endl;
                return false;
            }
        }
        // two expressions are the same
        cout << "The expression " << first << " is equivalent to " << second << endl;
        return true;
    }
    
}


//print list of expressions
bool listCommand(vector<vector<string>> totalExpression,string input)
{
    cout << "GOING LIST COMMAND"<<endl;
    int pos = 0;                           //get LIST index in the input string  
    removeSpace(input);
    pos = input.find("LIST");
    string getList = input.substr(pos);    
    //no other parameter after or before LIST
    if(getList != "LIST" && getList.size() != 4)
    {
        cout<<"Command LIST is ambiguous."<<endl;
        return false;
    }
    else if(totalExpression.empty())
    {
        cout<<"The list is empty." << endl;
        return false;
    }
    else
    {
        for(int i=0;i<totalExpression.size();++i)
        {
            string originExp = totalExpression[i][0];
            removeSpace(originExp);
            cout<<"Expression "<<i+1<<" : "<<originExp<<endl;
        }
    }
    return true;    //command valid
}


//check if file name exist or not
bool storeHelper(string fileName)
{
    if(ifstream(fileName))
    {
        string ans = "";
        cout << "File already exists" <<endl;
        cout << "Do you want to erase or change another filename (Y/N): ";
        getline(cin,ans);
        while (ans == 'N')
        {
            cout << "Do you want to erase or change another filename (Y/N): ";
        }
    }
}