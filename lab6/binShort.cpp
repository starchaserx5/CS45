#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <ctime>
#include <cstdlib>
 
using namespace std;
 
bool getInput(string title, int &qty);
void binsort(vector<int> &list);
void generate(vector<int> &list, int qty);
void display(string title, vector<int> list);
 
int main()
{
    vector<int> list;
    int qty;
    srand(time(nullptr));
    while(getInput("How many random numbers? ", qty))
    {
        generate(list, qty);
        display("Unsorted", list);
        binsort(list);
        display("Sorted", list);
    }
    return 0;
}
 
 
void generate(vector<int> &list, int qty)
{
    list.clear();
    for(unsigned int i = 0; i < qty; ++i)
        list.push_back(rand()%1000);
}
 
void display(string title, vector<int> list)
{
    cout<<endl<<title<<" numbers:";
    for(unsigned int i = 0; i < list.size(); ++i)
    {
        if(i%10 == 0)
            cout<<endl;
        cout<<setw(5)<<list[i];
    }
    cout<<endl<<endl;
}
 
bool getInput(string title, int &qty)
{
    string line;
    cout<<title;
    getline(cin, line);
    if(line.empty())
        return false;
    qty = stoi(line);
    return true;
}
 
 
/*  Get a digit started from left to right 
    to determine which bucket to insert the number derived from that digit.
    Insert all numbers to buckets and copy all numbers to the list.
    Remove the bucket for next inserting with next digit determined by power.
    Index of bucket = (number/power) mod 10;
 */
void binsort(vector<int> &list)
{
    vector<int> bins[10];
    int power = 1;
    while(bins[0].size() != list.size())
    {
        for(unsigned int i = 0; i < 10; ++i)
            bins[i].clear();
        for(unsigned int i = 0; i < list.size(); ++i)
            //          11 / 1
            bins[(list[i]/power) % 10].push_back(list[i]);
        list.clear();
        for(unsigned int i = 0; i < 10; ++i)
/*
            for(unsigned int j = 0; j < bins[i].size(); ++j)
                list.push_back(bins[i][j]);
*/
            list.insert(list.end(), bins[i].begin(), bins[i].end());
       power *= 10;
    }
}