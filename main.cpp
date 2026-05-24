#include <iostream>
#include <fstream>
#include <iomanip>
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

static void printSymbolTable(const Lexer &lexer, ostream &stdOut, ostream &fileOut)
{
    const vector<SymbolEntry> &table = lexer.getSymbolTable();

    stdOut << endl
           << "symbol table:" << endl;
    stdOut << left << setw(16) << "name"
           << setw(12) << "kind"
           << setw(12) << "value" << endl;

    fileOut << endl
            << "symbol table:" << endl;
    fileOut << left << setw(16) << "name"
            << setw(12) << "kind"
            << setw(12) << "value" << endl;

    if (table.empty())
    {
        stdOut << "(empty)" << endl;
        fileOut << "(empty)" << endl;
        return;
    }

    for (const auto &entry : table)
    {
        stdOut << left << setw(16) << entry.name
               << setw(12) << entry.kind
               << setw(12) << entry.value << endl;
        fileOut << left << setw(16) << entry.name
                << setw(12) << entry.kind
                << setw(12) << entry.value << endl;
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

    cout << "lex tokens:" << endl;

    ofstream tokenFile("tokentable.txt");
    if (!tokenFile)
    {
        cerr << "failed to open tokentable.txt for writing" << endl;
        return 1;
    }

    printTokens(lexer, cout, tokenFile);

    ofstream symbolFile("symboltable.txt");
    if (!symbolFile)
    {
        cerr << "failed to open symboltable.txt" << endl;
        return 1;
    }

    printSymbolTable(lexer, cout, symbolFile);

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