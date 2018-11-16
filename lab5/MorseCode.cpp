/*
CS45 DISCRETE STRUCTURES WITH COMPUTER SCIENCE APPLICATIONS
Professor PAUL WILKINSON
Tom Nguyen
Lab 5 - Morse Code
*/
#include <iostream>
#include <map>
#include <string>
#include <string.h>
#include <cstdlib>
#include <sstream>
using namespace std;
 
bool getInput(string &whatToSend);
void loadMorse(map<string, char> &toMorse);
void process(string line, map<string, char> toMorse);
 
int main()
{
    //  What to send is the char, how to send is the string
    map<string, char> toMorse;
    string input;
    loadMorse(toMorse);
    while(getInput(input))
        process(input, toMorse);
 
    return 0;
}
  
bool getInput(string &whatToSend)
{
    cout<<"To Transmit: ";
    getline(cin, whatToSend);
    return !whatToSend.empty();
} 
 
void loadMorse(map<string, char> &toMorse)
{
    toMorse[" "]    = ' ';
    toMorse[".-"]   = 'A';
    toMorse["-..."] = 'B';
    toMorse["-.-."] = 'C';
    toMorse["-.."]  = 'D';
    toMorse["."]    = 'E';
    toMorse["..-."] = 'F';
    toMorse["--."]  = 'G';
    toMorse["...."] = 'H';
    toMorse[".."]   = 'I';
    toMorse[".---"] = 'J';
    toMorse["-.-"]  = 'K';
    toMorse[".-.."] = 'L';
    toMorse["--"]   = 'M';
    toMorse["-."]   = 'N';
    toMorse["---"]  = 'O';
    toMorse[".--."] = 'P';
    toMorse["--.-"] = 'Q';
    toMorse[".-."]  = 'R';
    toMorse["..."]  = 'S';
    toMorse["-"]    = 'T';
    toMorse["..-"]  = 'U';
    toMorse["...-"] = 'V';
    toMorse[".--"]  = 'W';
    toMorse["-..-"] = 'X';
    toMorse["-.--"] = 'Y';
    toMorse["--.."] = 'Z';
    toMorse["-----"] = '0';
    toMorse[".----"] = '1';
    toMorse["..---"] = '2';
    toMorse["...--"] = '3';
    toMorse["....-"] = '4';
    toMorse["....."] = '5';
    toMorse["-...."] = '6';
    toMorse["--..."] = '7';
    toMorse["---.."] = '8';
    toMorse["----."] = '9';
}
 
void process(string line,  map<string, char> toMorse)
{
    cout<<"Char "<<line<<" converts to: ";
    string morse = "";
    istringstream input(line);
    while(getline(input,morse,' '))
    {
        // for(int i = 0; i < toMorse.size(); ++i)
        cout<<toMorse[morse];
    }
    cout<<endl;
}

