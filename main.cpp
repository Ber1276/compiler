#include <iostream>
#include "lex.h"

using namespace std;

int main()
{
    string source = PreprocessFile("test.c");
    cout << "pre process result:" << endl;
    cout << source << endl
         << endl;

    Lexer lexer(source);

    while (true)
    {
        Token token = lexer.nextToken();
        if (token.code == -1)
        {
            break;
        }
        cout << tokenCategory(token.code)
             << "------" << token.code
             << "--------" << token.text
             << "-------" << token.value
             << endl;
    }
    return 0;
}