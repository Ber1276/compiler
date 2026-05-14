#include <iostream>
#include "lex.h"

using namespace std;

// 0 标识符 1-8 关键字 25 常数 10-18 运算符 19-23 分隔符
static string tokenCategory(int code)
{
    switch (code)
    {
    case 0:
        return "id";
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
        return "keyword";
    case 25:
        return "const";
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
        return "operator";
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
        return "separator";
    default:
        return "unknown";
    }
}

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