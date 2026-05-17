#include <iostream>
#include <string>

#include "lex.h"
#include "yacc.h"

using namespace std;

static void printTokens(Lexer &lexer)
{
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
}

int main()
{
    string source = PreprocessFile("test.c");
    cout << "pre process result:" << endl;
    cout << source << endl
         << endl;

    Lexer lexer(source);

    cout << "lexical tokens:" << endl;
    printTokens(lexer);

    lexer.reset(source);

    cout << endl
         << "syntax analysis:" << endl;
    try
    {
        Parser parser(lexer);
        bool ok = parser.parse();
        cout << (ok ? "syntax analysis passed" : "syntax analysis failed") << endl;
    }
    catch (const exception &ex)
    {
        cerr << "syntax analysis exception: " << ex.what() << endl;
        return 1;
    }

    return 0;
}