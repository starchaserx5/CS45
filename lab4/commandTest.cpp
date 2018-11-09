#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <bitset>
#include <string.h>
#include <map>
#include <regex>

using namespace std;

bool commandMatching(string input,string commandName,unsigned int size);
bool commandHelper(string input,unsigned int& index);

//compare user's command input to default command
//return true if matching
bool commandMatching(string input,string commandName,unsigned int size)
{
    cout << "Substring is: "<<input.substr(0,size)<<endl;
    return (input.substr(0,size) == commandName);
}

// "SIZE","NEW","DELETE","LIST","LOAD","IS","QUIT","EDIT","EXIT","WEXIT","WQUIT","HELP","TABLE","STORE"
bool commandHelper(string input,unsigned int& index)
{
    while(input.size()>0)
    {
        switch(input[0])
        {
            case ' ' :  input.erase(0,1);
                        break;
            case 'N' :  if(commandMatching(input,"NEW",3))
                        {
                            index = 0;          //NEW
                            cout << "Input is : " << input << endl;
                            return true;                           
                        }
                        else return false;
            case 'D' :  if(commandMatching(input,"DELETE",6))
                        {
                            index = 1;          //DELETE
                            return true;                           
                        }
                        else return false;
            case 'L' :  if(commandMatching(input,"LIST",4))
                        {
                            index = 2;          //LIST
                            return true;                           
                        }
                        else if(commandMatching(input,"LOAD",4))
                        {
                            index = 3;          //LOAD
                            return true;                           
                        }
                        return false;           //invalid command
            case 'I' :  if(commandMatching(input,"IS",2))
                        {
                            index = 4;          //IS
                            return true;                           
                        }
                        else return false;
            case 'Q' :  if(commandMatching(input,"QUIT",4))
                        {
                            index = 5;          //QUIT
                            return true;                           
                        }
                        else return false;                        
            case 'E' :  if(commandMatching(input,"EDIT",4))
                        {
                            index = 6;          //EDIT
                            return true;                           
                        }
                        else if(commandMatching(input,"EXIT",4))
                        {
                            index = 5;          //EXIT
                            return true;                           
                        }
                        return false;
            case 'W' :  if(commandMatching(input,"WEXIT",5) || commandMatching(input,"WQUIT",5))
                        {
                            index = 7;          //WEXIT WQUIT
                            return true;                           
                        }
                        else return false;
            case 'H' :  if(commandMatching(input,"HELP",4))
                        {
                            index = 8;          //HELP
                            return true;                           
                        }
                        else return false;
            case 'T' :  if(commandMatching(input,"TABLE",5))
                        {
                            index = 9;          //TABLE
                            return true;                           
                        }
                        else return false;
            case 'S' :  if(commandMatching(input,"STORE",5))
                        {
                            index = 10;          //STORE
                            return true;                           
                        }
                        else return false;                                                                                                                                                                                                
            default:    return false;   //invalid command                                            
        }
    }
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

int main()
{
    string input = "";
    while(getInput(input))
    {
        unsigned int index = -1;
        if(commandHelper(input,index))
            cout << "Expression using command "<<index << " ." <<endl;
        else
            cout << "Invalid exp" <<endl;
    }

    return 0;
}
