#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <bitset>
#include <string.h>

using namespace std;

vector<vector<bool>> genTruthTable(unsigned int num);
unsigned int countLetter(string input);

vector<vector<bool>> cart_product(const vector<vector<bool>> &v)
{
    vector<vector<bool>> s = {{}};
    for (auto& u : v) {
        vector<vector<bool>> r;
        for (auto& x : s) {
            for (auto y : u) {
                r.push_back(x);
                r.back().push_back(y);
            }
        }
        s.swap(r);
    }
    return s;
}

int main () {

    // vector<bool> binList = {true,false};
	// vector<vector<bool>> test;
    // for(int i =0; i < 4;++i)
    //     test.push_back(binList);
	// vector<vector<bool> > res = cart_product(test);
	// for(size_t i = 0; i < res.size();i++){
	// 	for (size_t j = 0; j < res[i].size(); j++){
	// 		cout << ((res[i][j] == true) ? ("True") : ("False")) << "\t";
	// 	}
	// 	cout << std::endl;
	// }

    // string a = ">)A%";
    // char input = a[0];
    // char illegalChar[] = ">=)<&|@%^";
    // if (strchr(illegalChar, a[3]) != NULL) //found illegal char at input[2]
    //     cout << "FOUND ILLEGAL" << endl;

    // vector<vector<bool>> truthTable = genTruthTable(3);    
    string foo = "BA&CD&AF=>EHKMP^";
    countLetter(foo);
    return 0;
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
    cout << "TRUTH TABLE Rows: "<< numRows << " Cols: " << num << endl;
    for(int i=0; i < binTable.size(); ++i)
    {
        for (int j = 0; j < num; ++j)
            cout << binTable[i][j] << " ";
        cout << endl;
    }
    return binTable;    
}

unsigned int countLetter(string input)
{
    unsigned int num = 0;   // number of letters
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
    return num;
}