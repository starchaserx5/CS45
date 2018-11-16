#include <iostream>
#include <string>

using namespace std;

bool getInput(string& sendInput);
void encryptText(string text,string& output);
void decryptText(string text);

int main()
{
    string inputText;
    while(getInput(inputText))
    {
        string decrypMessage;
        encryptText(inputText,decrypMessage);
        decryptText(decrypMessage);
    }
    return 0;
}

bool getInput(string& sendInput)
{
    cout << "Enter a text: ";
    getline(cin,sendInput);
    return !sendInput.empty();
}

char hashText(char letter)
{
    int newChar = letter + 32;
    newChar = newChar + 'J';
    cout << " " <<newChar;
    return newChar;
}

void encryptText(string text,string& output)
{
    for(int i=0;i<text.size();++i)
    {
        output+=hashText(text[i]);
    }
    cout<<"The encrypted message is: "<<output<<endl;
}

void decryptText(string text)
{
    string output;
    for(int i=0;i<text.size();++i)
    {
        char temp = text[i] - 32;
        temp = temp - 'J';
        output += temp;
    }
    cout<<"The decrypted message is: "<<output<<endl;
}