#include <iostream>
#include <vector>
#include <cstdlib>
#include <string.h>
#include <algorithm> // std::find
#include <bitset>
#include <map>

using namespace std;

map<char, unsigned int> mapLetterToColumn(bitset<26> setLetters);
bitset<26> countLetter(string input);
bool invalidCharAfter(char inputChar) ;
bool illegalImplication(string &input);
void removeSpace(string &input);
bool illegalSet(string input);
int whoIsFirst(const string &incoming);
bool precedence(const string &incoming, const string &tos);
bool convertToRPN(string input, string& output);
bool getInput(string &line);
void process(string rpn, int sets[]);
unsigned int unionOfTwoSets(string x, string y, string &result);
unsigned int intersectionOfTwoSets(string x, string y, string &result);
unsigned int differenceOfTwoSets(string x, string y, string &result);
unsigned int setCompliment(string x, string &result);
 
int main()
{
    string line, output;            //Create input (line) and output (output) variables for functions to use
    int sets[26];                   //Create a 26 element array of sets
    while(getInput(line))           //As long as there is input from the keyboard
    {        
        if (convertToRPN(line, output)) //See if we can convert infix to postfix notation
            process(output, sets); //process(output,sets); //If we can, process the input
        else //If not, tell the user that there was bad input
            cout << "Illegal expression!!!" << endl;
        //     cout<<"The expression after reducing: " << line << endl;
        //      cout << "--------------" <<endl;    
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
        case '~' : value = 8;   //NOT is the highest
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
            input.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ()=><&|@%^") < size);
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
            case '|':                  //If it is any valid operator
            case '@':                  //we either immediately push it onto the operand stack
            case '%':                  //or push higher precedence operators currently in the stack
            case '^' :  op = input[0]; //onto the output
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
                        if(operatorStack.size() == 0)
                            return false;
                        else
                            operatorStack.pop_back();
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
 
 
void process(string rpn, int sets[])    //Process the RPN on sets
{
    unsigned int result = 0, pos;       //Initialize result to 0 (or create a bitset to be your result holder)
    vector<string> operandStack;        //Create an operand and operator stack
    vector<char> operatorStack;
    string set, x, y, output;           //Create some temporary variables
    cout<<"Translated to RPN: "<<rpn<<endl;
    while(rpn.size() > 0)               //As long as there are inputs available
    {
        if(rpn[0] >= 'A' && rpn[0] <= 'Z') //If a named set, push onto the operand stack
        {
            operandStack.push_back(output = rpn[0]);
            rpn.erase(0,1);
        }
        else                                //Otherwise
        {
            switch(rpn[0])                  //See what the operator is
            {
                case ' ' :  rpn.erase(0,1); //Get rid of spaces
                            break;
                case '~' :  x = operandStack.back();     //If compliment operator
                            operandStack.pop_back();     //Pop an operand and
                            result = setCompliment(x, output); //compliment it
                            operandStack.push_back(output); //Push the result back onto the operand stack
                            rpn.erase(0,1);
                            break;
                case '+' :  x = operandStack.back();    //If it is Union, two operands are required
                            operandStack.pop_back();    //Pop them, then perform the union
                            y = operandStack.back();
                            operandStack.pop_back();
                            result = unionOfTwoSets(x, y, output);
                            operandStack.push_back(output); //Then place the result onto the operand stack
                            rpn.erase(0,1);                 //Delete from input the operand
                            break;
                case '*' :  x = operandStack.back();        //If it is Intersection, two operands are required
                            operandStack.pop_back();        //Pop them, then perform the intersection
                            y = operandStack.back();
                            operandStack.pop_back();
                            result = intersectionOfTwoSets(x, y, output);//The place the result onto the operand stack
                            operandStack.push_back(output); //Then place the result onto the operand stack
                            rpn.erase(0,1);                 //Delete from input the operand
                            break;
               case '\\' :  x = operandStack.back();        //If it is Set Difference, two operands are required
                            operandStack.pop_back();        //Pop them, then perform the set difference
                            y = operandStack.back();
                            operandStack.pop_back();
                            result = differenceOfTwoSets(y, x, output);//The place the result onto the operand stack
                            operandStack.push_back(output); //Delete from input the operand
                            rpn.erase(0,1);
                            break;
            }
        }
    }
}
 
 
//The functions below are for you to complete. You can use bitset or ints, but you will have to
//adjust the function above to work correctly with bitsets
unsigned int unionOfTwoSets(string x, string y, string &result)
{
 
}
 
unsigned int intersectionOfTwoSets(string x, string y, string &result)
{
 
}
 
unsigned int differenceOfTwoSets(string x, string y, string &result)
{
 
}
 
unsigned int setCompliment(string x, string &result)
{
}

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
                    cout << "GOING TO > case" << endl;
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
    cout << "The total letters appear are: " << letters.count() << endl;
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
map<char, unsigned int> mapLetterToColumn(bitset<26> setLetters)
{
    map<char, unsigned int> letterMap;
    unsigned int index = 0;
    for (int i = 0; i < setLetters.size(); ++i)
    {
        //insert key is an index of ON bitset
        //and value is an index of column
        if (setLetters.test(i))
        {
            char c = char(65 + i);
            letterMap.insert(pair<char, unsigned int>(c, index++));
        }
    }

    map<char, unsigned int>::iterator it = letterMap.begin();
    for (it = letterMap.begin(); it != letterMap.end(); ++it)
        cout << it->first << " => " << it->second << '\n';
    return letterMap;
}