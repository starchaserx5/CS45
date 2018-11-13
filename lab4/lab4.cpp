#include <iostream>
#include <vector>
#include <cstdlib>
#include <bitset>
#include <string>
#include <array>

using namespace std;
 
 
bool illegalSet(string input);
int whoIsFirst(const string &incoming);
bool precedence(const string &incoming, const string &tos);
bool convertToRPN(string input, string& output);
bool getInput(string &line);
void process(string rpn, int sets[], int index);
unsigned int unionOfTwoSets(string x, string y, string &output,int sets[]);
unsigned int intersectionOfTwoSets(string x, string y, string &output,int sets[]);
unsigned int differenceOfTwoSets(string x, string y, string &output, int sets[]);
unsigned int setCompliment(string x, string &output,int sets[]);
bool setCommand(string &input, int sets[]);
bitset<16> setHelper(string &input);
bool commandInput(string& input,int sets[]);
bool commandMatching(string input,string commandName,unsigned int size);
bool commandHelper(string input,unsigned int& index);
void removeSpace(string& input);


int main()
{
    string line, output;            //Create input (line) and output (output) variables for functions to use
    int sets[26] = {};              //Create a 26 element array of sets


    while(getInput(line))           //As long as there is input from the keyboard
    {

        //check if the input is invalid or not
        if(commandInput(line,sets)) //See if we can convert infix to postfix notation
            cout <<"--------------"<<endl;
        else                           //If not, tell the user that there was bad input
        {
            cout<<"Illegal command! Please type HELP for the instructions."<<endl;
            cout <<"--------------"<<endl;
        }
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
        case '!' : value = 3;          //Compliment is the highest
                   break;
        case '*' : value = 2;
                   break;
        case '+' :
        case '\\' : value = 1;          //Union and set difference are the lowest
    }
    return value;
}
 
bool illegalSet(string input)          //See if the user entered a double comma or something
{                                      //like {, or ;}
    unsigned int pos, size = input.size();
    while((pos = input.find(' ')) < size) //Find spaces
        input.erase(pos,1);
    return (input.find(",,") < size ||
            input.find("{,")  < size ||
            input.find(",}") < size ||
            input.find_first_not_of("0123456789{,}") < size);
}
 
bool precedence(const string &incoming, const string &tos) //Return TRUE is incoming operator
{
     return whoIsFirst(incoming) < whoIsFirst(tos);  //is less than what is on the top of the operator stack
}
 
bool convertToRPN(string input, string &output)
{
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
                case ' ' : input.erase(0,1);        //Dump any space that has been read from the input
                           break;
                case '{' : pos = input.find('}');   //If it is a open curly brace, we have an unnamed set
                           pos2 = input.find('{', pos + 1);  //So see if it is a valid unnamed set
                           if(pos > input.size() || pos2 < pos)
                               return false;        //If not, signify that an invalid unnamed set was read in
                           parens = (input.substr(0,pos + 1)) + " "; //It is is valid, pull it out of the input
                           if(illegalSet(parens))   //Did they enter a double comma?
                               return false;
                           output += parens;        //Bad input!!
                           input.erase(0,pos + 1);  //and remove it from the input
                           break;
                case '*' :                          //If it is any valid operator
                case '\\' :                         //we either immediately push it onto the operand stack
                case '+' :                          //or push higher precedence operators currently in the stack
                case '!' : op = input[0];           //onto the output
                           while((operatorStack.size() > 0) && precedence(op, operatorStack[operatorStack.size()-1]))
                           {
                                output += operatorStack.back() + " ";
                                operatorStack.pop_back();
                           }
                           operatorStack.push_back(op); //Insert current operator onto operator stack
                           input.erase(0,1);            //Remove the current operator from input
                           break;
                case '(' : operatorStack.push_back("("); //Parenthesis are a "special case"
                           input.erase(0,1);             //Push the opening onto the operand stack and wait till
                           break;                        //a closing parentheses is found
                case ')' : while(operatorStack.size() > 0 && operatorStack.back() != "(")
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
                default  : return false;
            }
          }
        }
        while(operatorStack.size() > 0)  //If there are any additional operators left on the stack
        {                                //we push them onto output unless we find a mis-matched paren
            string op = operatorStack.back();
            if(op[0] == '(')
                return false;
            output += op + " ";
            operatorStack.pop_back();
        }
        return true;                      //Signify a successful conversion to RPN
}
 
 
void process(string rpn, int sets[], int index)    //Process the RPN on sets
{
    unsigned int result = 0, pos;       //Initialize result to 0 (or create a bitset to be your result holder)
    vector<string> operandStack;        //Create an operand and operator stack
    vector<char> operatorStack;
    string set, x, y, output;           //Create some temporary variables
    cout<<"Translated to RPN: "<< rpn <<endl;
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
                case '{' :  pos = rpn.find('}');//If curly braces, get the unnamed set
                            set = (rpn.substr(0, pos + 1));
                            // operandStack.push_back(set); //and push it onto the operand stack                                                        
                            cout << "num after converted from set: " << std::to_string((setHelper(set).to_ulong())) << endl;
                            result = setHelper(set).to_ulong();//set number to a set.
                            operandStack.push_back(std::to_string((setHelper(set).to_ulong()))); //convert bitset<32> to string and push to stack
                            rpn.erase(0, pos+1);         //Then remove it from the RPN input
                            break;
                case '!' :  x = operandStack.back();     //If compliment operator
                            operandStack.pop_back();     //Pop an operand and
                            result = setCompliment(x, output,sets); //compliment it
                            operandStack.push_back(output); //Push the result back onto the operand stack
                            rpn.erase(0,1);
                            break;
                case '+' :  x = operandStack.back();    //If it is Union, two operands are required
                            operandStack.pop_back();    //Pop them, then perform the union
                            y = operandStack.back();
                            operandStack.pop_back();
                            result = unionOfTwoSets(y, x, output,sets);
                            operandStack.push_back(output); //Then place the result onto the operand stack
                            rpn.erase(0,1);                 //Delete from input the operand
                            break;
                case '*' :  x = operandStack.back();        //If it is Intersection, two operands are required
                            operandStack.pop_back();        //Pop them, then perform the intersection
                            y = operandStack.back();
                            operandStack.pop_back();
                            result = intersectionOfTwoSets(y, x, output,sets);//The place the result onto the operand stack
                            operandStack.push_back(output); //Then place the result onto the operand stack
                            rpn.erase(0,1);                 //Delete from input the operand
                            break;
               case '\\' :  x = operandStack.back();        //If it is Set Difference, two operands are required
                            operandStack.pop_back();        //Pop them, then perform the set difference
                            y = operandStack.back();
                            operandStack.pop_back();
                            result = differenceOfTwoSets(y, x, output,sets);//The place the result onto the operand stack
                            operandStack.push_back(output); //Delete from input the operand
                            rpn.erase(0,1);
                            break;
            }
        }
    }

    //assign the elements to a set with index
    sets[index] = result;
    cout << "--------------------------------------------" << endl;
}
 
 
//The functions below are for you to complete. You can use bitset or ints, but you will have to
//adjust the function above to work correctly with bitsets
unsigned int unionOfTwoSets(string x, string y, string &output,int sets[])
{
    bitset<16> setX = 0;
    bitset<16> setY = 0;
    cout << "String X is: " << x << endl;
    cout << "String Y is: " << y << endl;

    //x is a set letter
    if(x[0] >= 'A' && x[0] <= 'Z')
    {
        setX = sets[x[0]-'A'];//convert number to bitset
        // cout << "The bitset X is: " << setX << endl;
        cout << "The sets[" << x[0]-'A' << "] is: " << sets[x[0]-'A'] << endl;
    }
    else
        setX = stoul(x,nullptr,0);//convert string --> ulong -->bitset
    //y is a set letter
    if (y[0] >= 'A' && y[0] <= 'Z')
    {
        setY = sets[y[0]-'A']; //convert number to bitset
        // cout << "The bitset Y is: " << setX << endl;
        cout << "The bitset sets[" << y[0]-'A' << "] is: " << sets[y[0]-'A'] << endl;
    }
    else
        setY = stoul(y,nullptr,0);
    cout << "The bitset X is: " << setX << endl;
    cout << "The bitset Y is: " << setY << endl;
    //OR bitwise
    setX |= setY;
    output = std::to_string(setX.to_ulong());//convert int setX to string
    cout << "The bitset after OR is: " << setX << endl;
    cout << "The output after OR is: " << output << endl;
    return setX.to_ulong();
}
 
unsigned int intersectionOfTwoSets(string x, string y, string &output, int sets[])
{
    bitset<16> setX = 0;
    bitset<16> setY = 0;
    cout << "String X is: " << x << endl;
    cout << "String Y is: " << y << endl;

    //x is a set letter
    if(x[0] >= 'A' && x[0] <= 'Z')
    {
        setX = sets[x[0]-'A'];//convert number to bitset
        // cout << "The bitset X is: " << setX << endl;
        cout << "The sets[" << x[0]-'A' << "] is: " << sets[x[0]-'A'] << endl;
    }
    else
        setX = stoul(x,nullptr,0);//convert string --> ulong -->bitset
    //y is a set letter
    if (y[0] >= 'A' && y[0] <= 'Z')
    {
        setY = sets[y[0]-'A']; //convert number to bitset
        // cout << "The bitset Y is: " << setX << endl;
        cout << "The bitset sets[" << y[0]-'A' << "] is: " << sets[y[0]-'A'] << endl;
    }
    else
        setY = stoul(y,nullptr,0);
    cout << "The bitset X is: " << setX << endl;
    cout << "The bitset Y is: " << setY << endl;
    //AND bitwise
    setX &= setY;
    output = std::to_string(setX.to_ulong());
    cout << "The bitset after AND is: " << setX << endl;
    cout << "The output after AND is: " << output << endl;
    return setX.to_ulong();
}
 
unsigned int differenceOfTwoSets(string x, string y, string &output, int sets[])
{
    bitset<16> setX = 0;
    bitset<16> setY = 0;
    cout << "String X is: " << x << endl;
    cout << "String Y is: " << y << endl;

    //x is a set letter
    if(x[0] >= 'A' && x[0] <= 'Z')
    {
        setX = sets[x[0]-'A'];//convert number to bitset
        // cout << "The bitset X is: " << setX << endl;
        cout << "The sets[" << x[0]-'A' << "] is: " << sets[x[0]-'A'] << endl;
    }
    else
        setX = stoul(x,nullptr,0);//convert string --> ulong -->bitset
    //y is a set letter
    if (y[0] >= 'A' && y[0] <= 'Z')
    {
        setY = sets[y[0]-'A']; //convert number to bitset
        // cout << "The bitset Y is: " << setX << endl;
        cout << "The bitset sets[" << y[0]-'A' << "] is: " << sets[y[0]-'A'] << endl;
    }
    else
        setY = stoul(y,nullptr,0);
    cout << "The bitset X is: " << setX << endl;
    cout << "The bitset Y is: " << setY << endl;
    //differentiation bitwise
    setY.flip();//flip all bit
    setX &= setY;
    output = std::to_string(setX.to_ulong());
    cout << "The bitset after \"difference\" is: " << setX << endl;
    cout << "The output after \"difference\" is: " << output << endl;
    return setX.to_ulong();
}
 
unsigned int setCompliment(string x, string &output, int sets[])
{
    bitset<16> setX = 0;
    cout << "String X is: " << x << endl;
    //x is a set letter
    if(x[0] >= 'A' && x[0] <= 'Z')
    {
        setX = sets[x[0]-'A'];//convert number to bitset
        // cout << "The bitset X is: " << setX << endl;
        cout << "The sets[" << x[0]-'A' << "] is: " << sets[x[0]-'A'] << endl;
    }
    else
        setX = stoul(x,nullptr,0);//convert string --> ulong -->bitset
    cout << "The bitset X is: " << setX << endl;
    //Take COMPLIMENT
    setX = setX.flip();
    output = std::to_string(setX.to_ulong());
    cout << "The bitset after \"difference\" is: " << setX << endl;
    cout << "The output after \"difference\" is: " << output << endl;
    return setX.to_ulong();
}


/*  This function will read the command(SET) 
    from the user's input. If it's SET command, get the right expression
    assign to string array. Then get the first letter to determine the index of a set.
    If there is not "SET" command or "equal" sign, print an error message as an invalid input
    @index : index of a set in the sets array sets[26]
    @equalPos: return the position of "=" sign if there is "SET" command
*/

bool setCommand(string &input, int sets[])
{
    //remove spaces trailing leading space
    removeSpace(input);    

    unsigned int posSet = input.find("SET");
    unsigned int posEqual = input.find("=");
    unsigned int posBracket = input.find("{");
    string output = ""; //result after process RPN expression
    string strSet = "";
    int index = -1; //index of a set

    //  SET command
    if (posSet < input.size() && posEqual < input.size())
    {
        // get a string between SET command and "=" sign
        strSet = input.substr(posSet + 3, posEqual + 1);
        while (strSet[0] == ' ')
            strSet.erase(0, 1); //remove leading spaces

        //find letter index
        if ((int)strSet[0] >= 65 || (int)strSet[0] <= 90)
        {
            index = (int(strSet[0]) - 65); //get an index of a set
            if(index<0)
            {
                cout << "The name of a set is invalid;" <<endl;
                return false;
            }
            else
            {
                cout << "index of the set is " << index
                 << "\nChar index is " << strSet[0] << ". "
                 << "Equal sign at pos is " << posEqual << endl;
            }
        }  
    }
    else
        return false;   //invalid command


    input = input.substr(posEqual+1, input.size()); // get the right expression
    cout << "String input: "<<input<<endl;
    //get universe set {1,2,3...}
    if(posBracket < input.size())
    {
        bitset<16> universeNum = setHelper(input);      //convert from set to a num
        if((int)(universeNum.to_ulong()) < 0) 
        {
            cout << "Invalid universe set's numbers;" <<endl;
            return false;       //not accept negative number in universe set
        }
        else
            sets[index] = (int)(universeNum.to_ulong());
    }  
    else
    {
        convertToRPN(input,output);
        process(output,sets,index);
    }

    return true;  //valid command
}

/*  Helper will convert the set of numbers to int
    seperate each number by comma and add them up.
*/
bitset<16> setHelper(string &input)
{
    bitset<16> num;
    int firstBracket = input.find_first_of("{");
    int lastBracket = input.find_first_of("}");
    string subSet = input.substr(firstBracket + 1, lastBracket - 1); // get sub set of the numbers

    while (subSet.size() > 0)
    {
        //comma(s) position
        int pos = subSet.find_first_of(",");
        // cout << "Pos of comma is: " << pos << endl;
        //not last number
        if (pos < subSet.size())
        {
            string strNumber = subSet.substr(0, pos);
            try
            {
                // cout << "Bit position is: " << strNumber << endl;
                num.flip(stoi(strNumber));
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << '\n';
            }
            //remove substring
            subSet.erase(0, pos + 1);
            // cout << "The subset after erase: " << subSet << endl;
        }
        //last number
        else
        {
            string strNumber = subSet.substr(); //get last number
            try
            {
                // cout << "Bit position is: " << strNumber << endl;
                num.flip(stoi(strNumber));
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << " Last number " << '\n';
            }
            //remove last substring number in the set
            subSet.erase(0, subSet.size());
            // cout << "The subset after erase: " << subSet << endl;
        }
    }
    // return num.to_ulong();
    return num;
}

/* 
    Convert bit to the set of number.
    and print out
 */
void printBitSet(unsigned long num)
{
    bitset<32> bitNum = num;
    // int numSet[32];
    array<int, 32> numSet;
    int index = 0; //index of numset array
    for (int i = 0; i < bitNum.size(); ++i)
    {
        if (bitNum.test(i))
        {
            numSet[index] = i;
            index++;
            cout << i << endl;
        }
    }
    for (int i = 0; i < index; ++i)
    {
        if (i == 0)
            cout << "{" << numSet[i] << ", ";
        else if (i == index - 1)
            cout << numSet[i] << "}";
        else
            cout << numSet[i] << ", ";
    }
}

//remove all spaces in input string
void removeSpace(string& input)
{
    int pos = 0;
    while((pos = input.find(' ')) < 0)
        input.erase(pos,1);
}

//compare user's command input to default command
//return true if matching
bool commandMatching(string input,string commandName,unsigned int size)
{
    cout << "Substring is: "<<input.substr(0,size)<<endl;
    return (input.substr(0,size) == commandName);
}
/*  Find a first letter matches to first letter from one of commands in the list(LIST,HELP,SET,LOAD,SAVE)
    Check the rest of letters. If it matches return true
    otherwise return false
*/
bool commandHelper(string input,unsigned int& index)
{
    while(input.size()>0)
    {
        switch(input[0])
        {
            case ' ' :  input.erase(0,1);
                        break;
            case 'S' :  if(commandMatching(input,"SET",3))
                        {
                            index = 0;          //NEW
                            cout << "Input is : " << input << endl;
                            return true;                           
                        }
                        else if(commandMatching(input,"SAVE",4))
                        {
                            index = 1;          //SAVE
                            return true;                           
                        } 
                        else return false;
            case 'H' :  if(commandMatching(input,"HELP",4))
                        {
                            index = 2;          //HELP
                            return true;                           
                        }
                        else return false;
            case 'L' :  if(commandMatching(input,"LIST",4))
                        {
                            index = 3;          //LIST
                            return true;                           
                        }
                        else if(commandMatching(input,"LOAD",4))
                        {
                            index = 4;          //LOAD
                            return true;                           
                        }                                                                                                                                                                        
            default:    return false;   //invalid command                                            
        }
    }
}

/*  Call commandHelper to parse the command
    If the command doesn't match the listed command keyword
    return false.
 */
bool commandInput(string& input,int sets[])
{
    unsigned int index = -1;        //index of command
    //check valid command
    if(!commandHelper(input,index))
        return false;
    
    
    switch (index)
    {
        case 0:
            return setCommand(input,sets);
            break;

        default:
            return false;   //invalid command
    }
}


