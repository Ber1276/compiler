#include <iostream>
#include <fstream>
#include <cctype>
#include <string>

#include "lex.h"
#include "yacc.h"

using namespace std;

static void printTokens(Lexer &lexer, ostream &consoleOut, ostream &fileOut)
{
    while (true)
    {
        Token token = lexer.nextToken();
        if (token.code == -1)
        {
            break;
        }

        consoleOut << tokenCategory(token.code)
                   << "------" << token.code
                   << "------" << token.text
                   << "------" << token.value
                   << endl;

        fileOut << tokenCategory(token.code)
                << "------" << token.code
                << "------" << token.text
                << "------" << token.value
                << endl;
    }
}

int main(int argc, char *argv[])
{
    string source;

    if (argc < 2)
    {
        cerr << "no file provide, input and type Ctrl+Z and Enter to continue" << endl;
        source = preprocessInput(cin);
    }
    else
    {
        ifstream sourceFile(argv[1]);
        if (!sourceFile.is_open())
        {
            cerr << "cant open file " << argv[1] << endl;
            return 1;
        }
        source = preprocessInput(sourceFile);
    }

    if (source.empty())
    {
        cerr << "empty source input" << endl;
        return 1;
    }

    cout << "pre process result:" << endl;
    cout << source << endl
         << endl;

    Lexer lexer(source);

    cout << "lexical tokens:" << endl;

    ofstream tokenFile("fuhaotable.txt");
    if (!tokenFile)
    {
        cerr << "failed to open fuhaotable.txt for writing" << endl;
        return 1;
    }

    printTokens(lexer, cout, tokenFile);

    lexer.reset(source);

    cout << endl
         << "syntax analysis:" << endl;
    try
    {
        Parser parser(lexer);
        bool ok = parser.parse(true);
        if (ok) // 因为这里用的语法制导翻译,所以语法分析的同时也做了语义分析
        {
            parser.printSymbolTable();
            parser.printTacTable();
        }
        cout << (ok ? "syntax analysis passed" : "syntax analysis failed") << endl;
    }
    catch (const exception &ex)
    {
        cerr << "syntax analysis exception: " << ex.what() << endl;
        return 1;
    }

    return 0;
}