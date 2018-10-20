#include <string>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <bitset>

using namespace std;

unsigned long setHelper(string &input);
void printBitSet(unsigned long num);


int main()
{
    string test = "{         0001,  002,  03, 031 } + 265";
    unsigned long num = setHelper(test);
    cout << "DONE" << endl;
    cout << "The set number is: " << num<< endl;
    printBitSet(num);
    return 0;
}


/*  Helper will convert the set of numbers to int
    seperate each number by comma and add them up.
*/

unsigned long setHelper(string &input)
{
    bitset<32> num;
    int firstBracket = input.find_first_of("{");
    int lastBracket = input.find_first_of("}");
    string subSet = input.substr(firstBracket+1,lastBracket-1); // get sub set of the numbers

    while(subSet.size() > 0)
    {
        //comma(s) position
        int pos = subSet.find_first_of(",");
        cout << "Pos of comma is: " << pos << endl;
        //not last number
        if(pos < subSet.size())
        {
            string strNumber = subSet.substr(0,pos);   
            try
            {
                cout << "Bit position is: " << strNumber << endl;
                num.flip(stoi(strNumber));
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }      
            //remove substring
            subSet.erase(0,pos+1); 
            cout << "The subset after erase: " << subSet << endl;     
        }
        //last number
        else 
        {             
            string strNumber = subSet.substr();//get last number   
            try
            {
                cout << "Bit position is: " << strNumber << endl;
                num.flip(stoi(strNumber));
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << " Last number " << '\n';
            }
            //remove last substring number in the set
            subSet.erase(0,subSet.size());
            cout << "The subset after erase: " << subSet << endl;     
        }
    }    
    return num.to_ulong();
}

/* 
    Convert bit to the set of number.
    and print out
 */
void printBitSet(unsigned long num)
{
    bitset<32> bitNum = num;
    int numSet[32];
    int index = 0; //index of numset array
    for(int i = 0; i < bitNum.size(); ++i)
    {
        if(bitNum.test(i))
        {           
            numSet[index] = i;
        }
    }
    for(int i = 0; i < numSet.size(); ++i)
    {
       if(numSet.test(i))
       {
            if(i=0)
                cout << "{" << i << "," ;
            else if(i=numSet.size()-1)//bitset is last bit
                cout << i << "}";
            else 
                cout << i << ",";
       }
       
    }

}


// unsigned long setHelper(string &input)
// {
//     bitset<32> num;   
//     input.erase(0,1);
//     while(input.size() > 0)
//     {
//         //comma(s) position
//         int pos = input.find_first_of(",");
//         cout << "Pos of comma is: " << pos << endl;
//         //not last number
//         if(pos < input.size())
//         {
//             string strNumber = input.substr(0,pos);   
//             try
//             {
//                 cout << "Bit position is: " << strNumber << endl;
//                 num.flip(stoi(strNumber));
//             }
//             catch(const std::exception& e)
//             {
//                 std::cerr << e.what() << '\n';
//             }      
//             //remove substring
//             input.erase(0,pos+1); 
//             cout << input << endl;     
//         }
//         //last number
//         else 
//         {   
//             int lastBracket = input.find_first_of("}");
//             string strNumber = input.substr(0,lastBracket);   
//             try
//             {
//                 cout << "Bit position is: " << strNumber << endl;
//                 num.flip(stoi(strNumber));
//             }
//             catch(const std::exception& e)
//             {
//                 std::cerr << e.what() << " Last number " << '\n';
//             }
//             //remove last substring number in the set
//             input.erase(0,lastBracket+1);
//             cout << input << endl;
//         }
//     }    
//     return num.to_ulong();
// }
