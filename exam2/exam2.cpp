/*
  CS45 DISCRETE STRUCTURES WITH COMPUTER SCIENCE APPLICATIONS
  Professor PAUL WILKINSON
  Tom Nguyen
  Test 2
*/
#include <iostream>
#include <vector>
#include <cstdlib>
#include <bitset>
#include <string>
#include <array>
#include <fstream>
#include <algorithm> 
#include <iterator>
#include <map>
#include <utility>
#include <math.h>

using namespace std;
 
bool illegalSet(string input);
int whoIsFirst(const string &incoming);
bool precedence(const string &incoming, const string &tos);
bool convertToRPN(string input, string& output);
bool getInput(string &line);
bool process(string rpn, int sets[], int index,map<string,int> uniSet);
unsigned int unionOfTwoSets(string x, string y, string &output,int sets[]);
unsigned int intersectionOfTwoSets(string x, string y, string &output,int sets[]);
unsigned int differenceOfTwoSets(string x, string y, string &output, int sets[]);
unsigned int setCompliment(string x, string &output,int sets[]);
bool commandInput(string &input, int sets[], map<string, int> uniSet,map<int,string> revSet,bool& hasSaved,bool& checkEmpty);
bool commandMatching(string input,string commandName,unsigned int size);
bool commandHelper(string input,unsigned int& index);
void removeSpace(string& input);
bool helpCommand(string input);
bool checkFileName(string fileName);
bool saveHelper(string fileName, int sets[],bool& checkSaved);
// bool setCommand(string &input, int sets[],bool& isEmpty);
bool setHelper(string& input,unsigned int& setNum);
bool saveCommand(int sets[], string& input,bool& checkSaved,bool& isEmpty);
void loadHelper(int sets[],string fileName,bool& isEmpty,bool& hasSaved);
bool loadCommand(int sets[], string input,bool& isEmpty,bool& hasSaved);
void showHelper(const int &num, map<int, string> uniSet);
bool showCommand(int *sets,string input,map<int,string> uniSet);
void forwardUniverse(map<string,int>& uniSet);
void reverseUniverse(map<int, string> &uniSet);
bool uniHelper(string input,unsigned int& setNum,map<string,int> uniMap);
bool checkSubSet(string setA,string setB,int sets[]);
bool isCommand(int sets[],string input);
bool checkEqualSet(string setA,string setB,int sets[]);
void saveFile(string fileName,int sets[],bool& hasSaved);
void exitCommand(int sets[],bool hasSaved,bool isEmpty);
bool listCommand(int *sets, string input, map<int, string> uniSet);
bool setCommand(string &input, int sets[],map<string,int> uniSet,bool& isEmpty,bool& checkSaved);



int main(int argc, char* argv[])
{

    string line, output;            //Create input (line) and output (output) variables for functions to use
    int sets[26] = {};              //Create a 26 element array of sets
    map<string, int> uniSet;        //forward Set color to nums
    map<int, string> revSet;        //Numbers to color
    forwardUniverse(uniSet);        //map an array of colors string to an array of number
    reverseUniverse(revSet);
    bool hasSaved = false;          //Check SAVE before exiting program
    bool isEmpty = true;          //Check if the set is empty
    bool inputCheck = false;
    for(int i=0;i<26;++i)          //intial value for each setset 
        sets[i]= -1;

    /*  Using argv and argc to load a file 
        User must be enter the name of program to run
        follow with LOAD command and fileName.
    */
    if(argc>1 && argc <=3)
    {
        //convert LOAD command to upper.
        string loadCommand(argv[1]);
        transform(loadCommand.begin(), loadCommand.end(), loadCommand.begin(), ::toupper);      
        if(!(loadCommand == "LOAD"))
            cout << "LOAD command invalid."<<endl;
        else if (argv[2] == nullptr)
            cout << "Mising the fileName."<<endl;
        else
        {
            string fileName(argv[2]);       //get fileName
            transform(fileName.begin(), fileName.end(), fileName.begin(), ::toupper);      
            loadHelper(sets,fileName,isEmpty,hasSaved);    
        }
    }
    else if(argc>3)
        cout<<"Only accept one file at a time."<<endl;

    do
    {
        inputCheck = getInput(line);
        if(inputCheck)
        {
            //check if the input is invalid or not
            if (commandInput(line, sets, uniSet, revSet, hasSaved, isEmpty)) //See if we can convert infix to postfix notation
                cout << "--------------" << endl;
            else //If not, tell the user that there was bad input
            {
                cout << "Illegal command! Please type HELP for the instructions." << endl;
                cout << "--------------" << endl;
            }
        }
        else             //input is empty line
        {
            //if user forgot to save and already enter one correct set.
            if (!hasSaved && !isEmpty)
                exitCommand(sets, hasSaved, isEmpty);
            else
                exit(0);    //exit without saving
        }                
    }while(inputCheck);           //As long as there is input from the keyboard
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
        case '~' : value = 3;          //Compliment is the highest
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
            input.find("{,") < size ||
            input.find(",}") < size ||
            input.find("{{") < size ||
            input.find("}}") < size ||
            input.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ{,}") < size);
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
                           if(illegalSet(parens)) return false;   //Did they enter numbers or a double comma?
                           output += parens;        //Bad input!!
                           input.erase(0,pos + 1);  //and remove it from the input
                           break;
                case '*' :                          //If it is any valid operator
                case '\\' :                         //we either immediately push it onto the operand stack
                case '+' :                          //or push higher precedence operators currently in the stack
                case '~' : op = input[0];           //onto the output
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

bool process(string rpn, int sets[], int index, map<string, int> uniSet) //Process the RPN on sets
{
    unsigned int result = -1, pos;       //Initialize result to 0 (or create a bitset to be your result holder)
    vector<string> operandStack;        //Create an operand and operator stack
    vector<char> operatorStack;
    string set, x, y, output;           //Create some temporary variables
    cout<<"Translated to RPN: "<< rpn <<endl;
    while(rpn.size() > 0)               //As long as there are inputs available
    {
        if(rpn[0] >= 'A' && rpn[0] <= 'Z') //If a named set, push onto the operand stack
        {
            operandStack.push_back(output = rpn[0]);
            result = sets[((int)(rpn[0])) -65];
            // sets[index] = temp;
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
                            // cout << "num after converted from set: " << std::to_string((setHelper(set).to_ulong())) << endl;
                            if(!uniHelper(set,result,uniSet))
                                return false;   //invalid
                            operandStack.push_back(to_string(result)); //convert bitset<32> to string and push to stack
                            rpn.erase(0, pos+1);         //Then remove it from the RPN input
                            break;
                case '~' :  x = operandStack.back();     //If compliment operator
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
}
 
 
//The functions below are for you to complete. You can use bitset or ints, but you will have to
//adjust the function above to work correctly with bitsets
unsigned int unionOfTwoSets(string x, string y, string &output,int sets[])
{
    bitset<16> setX = 0;
    bitset<16> setY = 0;
    // cout << "String X is: " << x << endl;
    // cout << "String Y is: " << y << endl;

    //x is a set letter
    if(x[0] >= 'A' && x[0] <= 'Z')
    {
        setX = sets[x[0]-'A'];//convert number to bitset
        // cout << "The bitset X is: " << setX << endl;
        // cout << "The sets[" << x[0]-'A' << "] is: " << sets[x[0]-'A'] << endl;
    }
    else
        setX = stoul(x,nullptr,0);//convert string --> ulong -->bitset
    //y is a set letter
    if (y[0] >= 'A' && y[0] <= 'Z')
    {
        setY = sets[y[0]-'A']; //convert number to bitset
        // cout << "The bitset Y is: " << setX << endl;
        // cout << "The bitset sets[" << y[0]-'A' << "] is: " << sets[y[0]-'A'] << endl;
    }
    else
        setY = stoul(y,nullptr,0);
    // cout << "The bitset X is: " << setX << endl;
    // cout << "The bitset Y is: " << setY << endl;
    //OR bitwise
    setX |= setY;
    output = std::to_string(setX.to_ulong());//convert int setX to string
    // cout << "The bitset after OR is: " << setX << endl;
    // cout << "The output after OR is: " << output << endl;
    return setX.to_ulong();
}
 
unsigned int intersectionOfTwoSets(string x, string y, string &output, int sets[])
{
    bitset<16> setX = 0;
    bitset<16> setY = 0;
    // cout << "String X is: " << x << endl;
    // cout << "String Y is: " << y << endl;

    //x is a set letter
    if(x[0] >= 'A' && x[0] <= 'Z')
    {
        setX = sets[x[0]-'A'];//convert number to bitset
        // cout << "The bitset X is: " << setX << endl;
        // cout << "The sets[" << x[0]-'A' << "] is: " << sets[x[0]-'A'] << endl;
    }
    else
        setX = stoul(x,nullptr,0);//convert string --> ulong -->bitset
    //y is a set letter
    if (y[0] >= 'A' && y[0] <= 'Z')
    {
        setY = sets[y[0]-'A']; //convert number to bitset
        // cout << "The bitset Y is: " << setX << endl;
        // cout << "The bitset sets[" << y[0]-'A' << "] is: " << sets[y[0]-'A'] << endl;
    }
    else
        setY = stoul(y,nullptr,0);
    // cout << "The bitset X is: " << setX << endl;
    // cout << "The bitset Y is: " << setY << endl;
    //AND bitwise
    setX &= setY;
    output = std::to_string(setX.to_ulong());
    // cout << "The bitset after AND is: " << setX << endl;
    // cout << "The output after AND is: " << output << endl;
    return setX.to_ulong();
}
 
unsigned int differenceOfTwoSets(string x, string y, string &output, int sets[])
{
    bitset<16> setX = 0;
    bitset<16> setY = 0;
    // cout << "String X is: " << x << endl;
    // cout << "String Y is: " << y << endl;

    //x is a set letter
    if(x[0] >= 'A' && x[0] <= 'Z')
    {
        setX = sets[x[0]-'A'];//convert number to bitset
        // cout << "The bitset X is: " << setX << endl;
        // cout << "The sets[" << x[0]-'A' << "] is: " << sets[x[0]-'A'] << endl;
    }
    else
        setX = stoul(x,nullptr,0);//convert string --> ulong -->bitset
    //y is a set letter
    if (y[0] >= 'A' && y[0] <= 'Z')
    {
        setY = sets[y[0]-'A']; //convert number to bitset
        // cout << "The bitset Y is: " << setX << endl;
        // cout << "The bitset sets[" << y[0]-'A' << "] is: " << sets[y[0]-'A'] << endl;
    }
    else
        setY = stoul(y,nullptr,0);
    // cout << "The bitset X is: " << setX << endl;
    // cout << "The bitset Y is: " << setY << endl;
    //differentiation bitwise
    setY.flip();//flip all bit
    setX &= setY;
    output = std::to_string(setX.to_ulong());
    // cout << "The bitset after \"difference\" is: " << setX << endl;
    // cout << "The output after \"difference\" is: " << output << endl;
    return setX.to_ulong();
}
 
unsigned int setCompliment(string x, string &output, int sets[])
{
    bitset<16> setX = 0;
    // cout << "String X is: " << x << endl;
    //x is a set letter
    if(x[0] >= 'A' && x[0] <= 'Z')
    {
        setX = sets[x[0]-'A'];//convert number to bitset
        // cout << "The bitset X is: " << setX << endl;
        // cout << "The sets[" << x[0]-'A' << "] is: " << sets[x[0]-'A'] << endl;
    }
    else
        setX = stoul(x,nullptr,0);//convert string --> ulong -->bitset
    // cout << "The bitset X is: " << setX << endl;
    //Take COMPLIMENT
    setX = setX.flip();
    output = std::to_string(setX.to_ulong());
    // cout << "The bitset after \"difference\" is: " << setX << endl;
    // cout << "The output after \"difference\" is: " << output << endl;
    return setX.to_ulong();
}


/*  This function will read the command(SET) 
    from the user's input. If it's SET command, get the right expression
    assign to string array. Then get the first letter to determine the index of a set.
    If there is not "SET" command or "equal" sign, print an error message as an invalid input
    @index : index of a set in the sets array sets[26]
    @equalPos: return the position of "=" sign if there is "SET" command
*/

bool setCommand(string &input, int sets[],map<string,int> uniSet,bool& isEmpty,bool& checkSaved)
{
    //remove spaces trailing leading space
    removeSpace(input);    

    unsigned int posSet = input.find("SET");
    unsigned int posEqual = input.find("=");
    unsigned int openBracket = input.find("{");
    unsigned int closeBracket = input.find("}");
    unsigned int cAlternative;                       //check if aternative entry appear
    int index = -1; //index of a set
    string output = ""; //result after process RPN expression
    string strSet = "";
    string expression = input.substr(posEqual+1);
    removeSpace(expression);

    //  get a letter of a SET command 
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
        }  
    }
    else
        return false;   //invalid command
    
    //Aternative entry of sets, NOT include "{"
    if ((openBracket > input.size() || closeBracket > input.size()))
    {
        unsigned int check1 = expression.find_first_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ+*\\");
        unsigned int check2 = expression.find_first_of("0123456789");
        unsigned int setNum;
        if(check1 < expression.size() && check2 < expression.size())
            return false;       //invalid command, numbers and letters can be mixed
          
        //if entry is an alternative of sets
        //only numbers are allowed
        if(check1 > expression.size() && check2 < expression.size())
        {
            try
            {
                setNum = stoi(expression);
            }
            catch (const std::exception &e)
            {
                cout<< "The number is out of range. Only accept up to 16-bit." << '\n';
            }
            // if (setNum > pow(2, 16))
            //     return false; //not beyond 16-bit
            // else
            sets[index] = setNum;
            isEmpty = false; //at least 1 set existed
            checkSaved = false;//need to save before exiting
            return true;     //valid alternative entry of a set
        }
    }

    input = input.substr(posEqual+1); // get the right expression
    if(!convertToRPN(input,output))
        return false;
    process(output,sets,index,uniSet);
    isEmpty = false;                //turn on Check Empty set
    checkSaved = false;//need to save before exiting
    return true;  //valid command
}

bool oldSetCommand(string &input, int sets[], map<string, int> uniSet, bool &isEmpty)
{
    //remove spaces trailing leading space
    removeSpace(input);

    unsigned int posSet = input.find("SET");
    unsigned int posEqual = input.find("=");
    unsigned int openBracket = input.find("{");
    unsigned int closeBracket = input.find("{");
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
            if (index < 0)
            {
                cout << "The name of a set is invalid;" << endl;
                return false;
            }
        }
    }
    else
        return false; //invalid command

    input = input.substr(posEqual + 1); // get the right expression
    if (!convertToRPN(input, output))
        return false;
    process(output, sets, index, uniSet);
    isEmpty = false; //turn on Check Empty set

    return true; //valid command
}

/*  Helper will convert the set of numbers to int
    seperate each number by comma and turn on the bit 
    at index where appears in the set.
*/
bool setHelper(string& input,unsigned int& setNum)
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
            if(stoi(strNumber) < 0 || stoi(strNumber) > 15)
            {
                cout << "The universe set only accepts number in range 0-15."<<endl;
                return false;
            }
            else
                num.flip(stoi(strNumber));
            //remove substring
            subSet.erase(0, pos + 1);
            // cout << "The subset after erase: " << subSet << endl;
        }
        //last number
        else
        {
            string strNumber = subSet.substr(); //get last number
            if(stoi(strNumber) < 0 || stoi(strNumber) > 15)
            {
                cout << "The universe set only accepts number in range 0-15."<<endl;
                return false;
            }
            else
                num.flip(stoi(strNumber));
            //remove last substring number in the set
            subSet.erase(0, subSet.size());
            // cout << "The subset after erase: " << subSet << endl;
        }
    }
    // return num.to_ulong();
    setNum = num.to_ulong();
    return true;
}

/* 
    Convert a number to bitset, then loop through the bit array
    Map the index correspond the color, and print that color
 */
void showHelper(const int &num, map<int, string> uniSet)
{
    bitset<16> bitNum = num;            //convert number to bitset
    unsigned int index = 0;
    cout<<"{";
    for(int i=0;i<bitNum.size();++i)
    {
        if(index==0 && bitNum.test(i))
        {
            cout <<uniSet[i];
            index++;
        }
        else if(index!=0 && bitNum.test(i))
        {
            cout <<", "<<uniSet[i];
        }
    }
    cout << "}"<<endl;
}



//remove all spaces in input string
void removeSpace(string& input)
{
    int pos = 0;
    while((pos = input.find(' ')) < input.size())
    {
        input.erase(pos,1);
    }
}

//compare user's command input to default command
//return true if matching
bool commandMatching(string input,string commandName,unsigned int size)
{
    // cout << "Substring is: "<<input.substr(0,size)<<endl;
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
                            return true;                           
                        }
                        else if(commandMatching(input,"SAVE",4))
                        {
                            index = 1;          //SAVE
                            return true;                           
                        }
                        else if(commandMatching(input, "SHOW", 4))
                        {
                          index = 3; //LIST
                          return true;
                        }
                        else return false;
            case 'H' :  if(commandMatching(input,"HELP",4))
                        {
                            index = 2;          //HELP
                            return true;                           
                        }
                        else return false;
            case 'L' :  if(commandMatching(input,"LOAD",4))
                        {
                            index = 4;          //LOAD
                            return true;                           
                        }
                        else if(commandMatching(input, "LIST", 4))
                        {
                            index = 6; //LIST
                            return true;
                        }
            case 'I' :  if(commandMatching(input,"IS",2))
                        {
                            index = 5;          //LOAD
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
bool commandInput(string &input, int sets[], map<string, int> uniSet,map<int,string> revSet,bool& hasSaved,bool& isEmpty)
{
    unsigned int index = -1;        //index of command
    //check valid command
    if(!commandHelper(input,index))
        return false;
    
    
    switch (index)
    {
        case 0:
            return setCommand(input,sets,uniSet,isEmpty,hasSaved);
            break;
        case 1:
            return saveCommand(sets,input,hasSaved,isEmpty);
            break;
        case 2:
            return helpCommand(input);
            break;
        case 3:
            return showCommand(sets,input,revSet);
            break;
        case 4:
            return loadCommand(sets,input,isEmpty,hasSaved);
            break;
        case 5:
            return isCommand(sets,input);
            break;
        case 6:
            return listCommand(sets, input,revSet);
            break;
        default:
            return false;   //invalid command
    }
}

bool helpCommand(string input)
{
    string fileName = "exam2.help";
    if(checkFileName(fileName))
    {
        ifstream opFile(fileName);
        unsigned int pos = 0;
        string token = "";
        if(opFile.is_open())
        {
            while(getline(opFile,token,';'))
                cout << token;
            cout << endl;
            opFile.close();
        }
        return true;
    }
    return false;//invalid command
}


//check if File Name already exist 
//return true if exists ; otherwise false
bool checkFileName(string fileName)
{
    ifstream file(fileName);
    return (bool)file; 
}


//store all expressions if file doesn't exist
//otherwise rename or remove that file
bool saveCommand(int sets[],string& input,bool& checkSaved,bool& isEmpty)
{
    unsigned int pos = input.find("SAVE");
    string fileName = input.substr(pos+4);          //get fileName
    removeSpace(fileName);
    if(isEmpty)
    {
        cout << "The list of expressions is empty." << endl;
        return true;
    }
    else if(fileName == "")
    {
        cout << "Missing the file name." <<endl;
        return true;
    }
    return saveHelper(fileName,sets,checkSaved);
    // return true;
}


//Create a new file 
//if file already exists, overwrite it or give another file name
//return true if checkFileName
bool saveHelper(string fileName, int sets[],bool& hasSaved)
{
    string ans = "";
    removeSpace(fileName);
    //add extension to file if missing
    //if exist lowercase extensions
    //if fileName exists
    if(checkFileName(fileName))
    {
        cout << "File already exists" << endl;
        //erase file
        cout << "Would you like to erase the file(Y/N): ";
        getline(cin,ans);
        transform(ans.begin(),ans.end(),ans.begin(),::toupper);         //convert to upper case
        if(ans == "Y" || ans == "YES")
        {
            remove(fileName.c_str());                                   //remove the file
            cout << "Old file is removed succesfully." << endl;
            saveFile(fileName,sets,hasSaved);                                    //write a new file
            return true;
        }
        else
        {
            cout << "Would you like to give another file name(Y/N): ";
            getline(cin, ans);
            transform(ans.begin(), ans.end(), ans.begin(), ::toupper);
            if (ans == "Y" || ans == "YES")
            {
                cout << "Enter new file name: " << endl;
                getline(cin, ans);
                removeSpace(ans);
                transform(ans.begin(), ans.end(), ans.begin(), ::toupper);
                //check extension missing
                while(checkFileName(ans))
                {
                    cout << "File already exists. Please enter another name:" << endl;
                    getline(cin, ans);
                }                               
                rename(fileName.c_str(), ans.c_str());
                cout << "File successfully renamed." << endl;
                saveFile(fileName,sets,hasSaved);                        //write a new file
                return true;
            }
            else
            {
                //remove old file and write a new file with samename
                remove(fileName.c_str());
                saveFile(fileName,sets,hasSaved);                        //write a new file
                return true;
            }
        }
    }
    else
    {
        saveFile(fileName,sets,hasSaved);
        return true;
    }
    return false;//failed to save the file
}


//LOAD command
bool loadCommand(int sets[], string input,bool& isEmpty,bool& hasSaved)
{
    unsigned int pos = input.find("LOAD");
    string fileName = input.substr(pos + 4); //get fileName
    removeSpace(fileName);
    if(fileName == "")
    {
        cout << "Missing the file name." << endl;
        return true;//invalid command
    }
    loadHelper(sets,fileName,isEmpty,hasSaved);
    return true;
}

//check the file name
//if  the file exist read through the text file.
//token it by ";" delimeter and assign each number to an array element of sets[]
void loadHelper(int sets[],string fileName,bool& isEmpty,bool& hasSaved)
{
    string ans = "";
    removeSpace(fileName);
    //add extension to file if missing
    //if exist lowercase extensions

    //check file exist
    if(!checkFileName(fileName))
        cout << "The file name doesn't exist." << endl;
    else if(fileName.size() == 0)
        cout << "Missing file name." <<endl;
    else
    {
        string line;
        ifstream opFile(fileName);
        unsigned int index = 0;
        string rpn = "";
        if (opFile.is_open())
        {
            while (getline(opFile, line,';'))
            {
                sets[index++] = stoi(line);
            }
            isEmpty = false;                        //Turn off isEmpty
            hasSaved = true;                        //Saved the current state
            cout<<"File already is already loaded."<<endl;
            opFile.close();
        }
        else
            cout << "Unable to open file" << endl;
    }
}


//print a particular set
bool showCommand(int *sets, string input, map<int, string> uniSet)
{
    int pos = 0;                           //get LIST index in the input string  
    removeSpace(input);
    pos = input.find("SHOW");
    string command = input.substr(pos,4);   //get "SHOW" string
    string setIndex = input.substr(pos+4);  //get set number want to see the content   
    removeSpace(setIndex);
    if(setIndex == "")
    {
        cout<<"Missing the name of a set."<<endl;
        return true;
    }
    else 
    {
        int setName = int(setIndex[0]); //set name in char
        if(setName<65 || setName>90)        
            cout<<"Index of the set is invalid or out of range (A-Z)."<<endl;
        else
        {            
            if(sets[setName-65] != -1)
            {
                cout << char(setName) << " = ";
                showHelper(sets[setName-65],uniSet);
            }
            else
                cout<<"The set "<<char(setName)<<" is empty."<<endl;
        }
        return true;
    }
    return false;    //command valid
}

//print list of expressions
bool listCommand(int *sets, string input, map<int, string> uniSet)
{
    int pos = 0; //get LIST index in the input string
    removeSpace(input);
    pos = input.find("LIST");
    string getList = input.substr(pos);
    //no other parameter after or before LIST
    if (getList != "LIST" && getList.size() != 4)
    {
        cout << "Command LIST is ambiguous." << endl;
        return true;
    }
    else
    {
        unsigned int setName = 65; //set name in char
        for (int i = 0; i < 26; ++i)
        {
            if (sets[i] != -1)
            {
                cout << char(setName + i) << " = ";
                showHelper(sets[i], uniSet);
            }
        }
        return true;
    }
    return false; //command valid
}

//map a set of colors to their corresponding number
void forwardUniverse(map<string, int> &uniSet)
{
  uniSet["WHITE"]   = 0;
  uniSet["GOLD"]    = 1;
  uniSet["GRAY"]    = 2;
  uniSet["RED"]     = 3;
  uniSet["GREEN"]   = 4;
  uniSet["BLUE"]    = 5;
  uniSet["ORANGE"]  = 6;
  uniSet["PURPLE"]  = 7;
  uniSet["YELLOW"]  = 8;
  uniSet["VIOLET"]  = 9;
  uniSet["MAGENTA"] = 10;
  uniSet["CYAN"]    = 11; 
  uniSet["RUST"]    = 12;
  uniSet["NAVY"]    = 13;
  uniSet["BURGUNDY"]= 14;
  uniSet["BLACK"]   = 15;
}

//map a set of colors to their corresponding number
void reverseUniverse(map<int, string> &revSet)
{
  revSet[0]   = "WHITE";
  revSet[1]   = "GOLD";
  revSet[2]   = "GRAY";
  revSet[3]   = "RED";
  revSet[4]   = "GREEN";
  revSet[5]   = "BLUE";
  revSet[6]   = "ORANGE";
  revSet[7]   = "PURPLE";
  revSet[8]   = "YELLOW";
  revSet[9]   = "VIOLET";
  revSet[10]  = "MAGENTA";
  revSet[11]  = "CYAN"; 
  revSet[12]  = "RUST";
  revSet[13]  = "NAVY";
  revSet[14]  = "BURGUNDY";
  revSet[15]  = "BLACK";
}


/*  Helper will token the set of colors
    Map each color to get a corresponding number
    Turn on the bit at index matching to that number.
    Return a number is a value of that bitset.
    @setNum: the value of bitset after flipping all bit presents in the color set
    @uniMap: set of colors and its equivalent number
 */
bool uniHelper(string input, unsigned int &setNum, map<string, int> uniMap)
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
      string color = subSet.substr(0, pos);
      int uniNum = uniMap[color];               //size of map will increase if the key doesn't match
      if (uniMap.size()>16)
      {
        cout << "Invalid Color in the universe. Please type HELP for more information." << endl;
        return false;
      }
      else
        num.flip(uniNum);
      //remove substring
      subSet.erase(0, pos + 1);
      // cout << "The subset after erase: " << subSet << endl;
    }
    //last number
    else
    {
      string color = subSet.substr(); //get last number
      int uniNum = uniMap[color];
      if (uniNum < 0 || uniNum > 15)
      {
        cout << "Invalid Color in the universe" << endl;
        return false;
      }
      else
        num.flip(uniNum);
      //remove last substring number in the set
      subSet.erase(0, subSet.size());
      // cout << "The subset after erase: " << subSet << endl;
    }
  }
  setNum = num.to_ulong(); //get the number represent color indexes.
  return true;
}


/*  Convert two sets to bitsets.
    Take an intersection between setB and setA
    Return true if the result of the intersection is the same as setA
    or bitset of result - bitset of setA = 0
    Otherwise, return false.
    Suppose setB is greater than setA
 */
bool checkSubSet(string setA,string setB,int sets[])
{    
    bitset<16> bitA = sets[setA[0]-'A'];
    bitset<16> bitB = sets[setB[0]-'A'];
    // cout << "The bitset of bitA: " <<bitA.to_string()<<endl;
    // cout << "The bitset of bitB: " <<bitB.to_string()<<endl;
    //Take intersection setB and setA
    bitB &= bitA;
    bitset<16> bitC = bitB^bitA;
    if(bitC.to_ulong() == 0)
        return true;
    return false;
}

/* Take XOR operation between two set
   Two sets are equal when they have same element
   or two sets have same of 1-bits.
   Return true if equal.
 */
bool checkEqualSet(string setA,string setB,int sets[])
{
    bitset<16> bitA = sets[setA[0]-'A'];
    bitset<16> bitB = sets[setB[0]-'A'];
    //Take intersection setB and setA
    bitB ^= bitA;
    // bitset<16> bitC = bitB^bitA;
    if(bitB.to_ulong() == 0)
        return true;
    return false;
}


/*  Call checkSubSet to determine 
    subset or super set
 */
bool isCommand(int sets[],string input)
{
    string compareExp = input.substr(input.find("IS")+2);   //get comparison expression
    removeSpace(compareExp);                                //remove all spaces betweeen two exps
    unsigned int pos = compareExp.find_first_of("<>=");
    string op = compareExp.substr(pos,1);                   //get operator
    string setA = compareExp.substr(0,pos);
    string setB = compareExp.substr(pos+1);
    switch(op[0])
    {        
        case '<': if(checkSubSet(setA,setB,sets))
                    cout << "Set "<< setA <<" is subset of Set "<< setB<<"."<<endl;
                  else
                    cout << "Set "<< setA <<" is not subset of Set "<< setB<<"."<<endl;
                  return true;
        case '>': if(checkSubSet(setB,setA,sets))
                    cout << "Set "<< setA <<" is super-set of Set "<< setB<<"."<<endl;
                  else
                    cout << "Set "<< setA <<" is not super-set of Set "<< setB<<"."<<endl;
                  return true;   
        case '=': if(checkSubSet(setA,setB,sets))
                    cout << "Set "<<setA<<" and Set "<<setB<<" contains same elements."<<endl;
                  else
                    cout << "Set "<<setA<<" and Set "<<setB<<" NOT contains same elements."<<endl;
                  return true;                   
        default:  return false;
                  break;      
    }          
    return false;   //invalid command
}


/* 
    Create a file with fileName,
    Loop through the sets[], write each element into the file, delimited by ";"
*/
void saveFile(string fileName,int sets[],bool& hasSaved)
{
    ofstream myFile;
    myFile.open(fileName,ios::app); //append to the expression to the end
    for(int i=0;i<26;++i)
    {
        int num= sets[i];
        string temp = to_string(num) + ";" ;
        myFile << temp ;
    }
    hasSaved = true;    //turn SAVED flag on to detect EXIT WITHOUT SAVE
    myFile.close(); //close the file
}

void exitCommand(int sets[],bool hasSaved,bool isEmpty)
{
    string ans = "";
    
    //ask you to save before exit
    if(!hasSaved && !isEmpty)
    {
        cout << "Would you like to save your changes(Y/N): "; 
        getline(cin,ans);
        transform(ans.begin(),ans.end(),ans.begin(),::toupper);         //convert to upper case
        if(ans == "Y" || ans == "YES")
        {
            string fileName = "";
            cout << "Please enter your file name: ";
            getline(cin,fileName);
            transform(fileName.begin(), fileName.end(), fileName.begin(), ::toupper);
            //call saveHelper to process save command
            saveHelper(fileName,sets,hasSaved);           
        }
    }
    else
        exit(0);    //totalExpression is empty
    exit(0);//close program
}

