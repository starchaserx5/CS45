#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <bitset>
#include <string.h>
#include <map>


using namespace std;

vector<vector<bool>> genTruthTable(unsigned int num);
bitset<26> countLetter(string input);
map<char,unsigned int> mapLetterToColumn(bitset<26> setLetters);


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
    string foo = "Q^A%Z^%M*L(P";
    bitset<26> letter = countLetter(foo);
    mapLetterToColumn(letter);
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
map<char,unsigned int> mapLetterToColumn(bitset<26> setLetters)
{
    map<char, unsigned int> letterMap;
    unsigned int index = 0;                     
    for(int i=0; i<setLetters.size();++i)
    {
        //insert key is an index of ON bitset 
        //and value is an index of column 
        if(setLetters.test(i))
        {
            char c = char(65+i);
            letterMap.insert(pair<char, unsigned int>(c,index++));        
        }
    }

    map<char, unsigned int>::iterator it = letterMap.begin();
    for(it=letterMap.begin(); it!=letterMap.end();++it)
        cout << it->first << " => " << it->second << '\n';
    return letterMap;
}