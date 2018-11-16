/*
CS45 DISCRETE STRUCTURES WITH COMPUTER SCIENCE APPLICATIONS
Professor PAUL WILKINSON
Tom Nguyen
Lab 5 - LCD
*/
#include <iostream>
#include <string>
#include <map>
#include <bitset>

using namespace std;

bool getInput(string& line);
void loadLCD(map<int,int>& lcdMap);
void printLCD(int input);

int main()
{
  string line;
  map<int,int> lcdMap;
  loadLCD(lcdMap);
  while(getInput(line))
  {
    printLCD(lcdMap[stoi(line)]);
  }
  return 0;
}

bool getInput(string& line)
{
  cout << "Enter a number: ";
  getline(cin,line);
  return !line.empty();
}

void loadLCD(map<int,int>& lcdMap)
{
  lcdMap[0] = 63;    //{0,1,2,3,4,5}
  lcdMap[1] = 6;     //{1,2}
  lcdMap[2] = 91;    //{0,1,3,4,6}
  lcdMap[3] = 79;    //{0,1,2,3,6}
  lcdMap[4] = 78;     //{1,2,5,6}
  lcdMap[5] = 109;     //{0,2,3,5,6}
  lcdMap[6] = 125;     //{0,2,3,4,5,6} 
  lcdMap[7] = 7;      //{0,1,2}
  lcdMap[8] = 127;    //{0,1,2,3,4,5,6}
  lcdMap[9] = 123;    //{0,1,2,3,5,6}  
}

void printLCD(int input)
{
  bitset<8> bitNum = input;
  int index = 0;
  cout<<"The number segment : {";
  for(int i=0;i<bitNum.size();i++)
  {
    if (index == 0 && bitNum.test(i))
    {
      cout <<i;
      index++;
    }
    else if (index != 0 && bitNum.test(i))
    {
        cout << "," << i;
    }
  }
  cout <<"}"<<endl;
}
