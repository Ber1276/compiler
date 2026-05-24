#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cctype>
#include "lex.h"
using namespace std;

string PreprocessFile(const string &filename)
{
    string yichuliWenben;             // save resule
    bool shifouZaiKuaiZhuShi = false; // block comment
    bool shangciShiKongge = false;    // is white flag

    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "can't open file , what a pity" << filename << endl;
        return "";
    }

    string line;
    while (getline(file, line)) // read line by line
    {
        int i = 0;
        int len = line.length();

        while (i < len)
        {
            if (!shifouZaiKuaiZhuShi && i < len - 1 && line[i] == '/' && line[i + 1] == '/')
            {
                break; // ignore
            } // meet //

            if (!shifouZaiKuaiZhuShi && i < len - 1 && line[i] == '/' && line[i + 1] == '*')
            {
                shifouZaiKuaiZhuShi = true;
                i += 2;
                continue;
            } // meet /* (block comment start)

            if (shifouZaiKuaiZhuShi && i < len - 1 && line[i] == '*' && line[i + 1] == '/')
            {
                shifouZaiKuaiZhuShi = false;
                i += 2;
                continue;
            } // meet */ (block comment end)

            if (shifouZaiKuaiZhuShi)
            {
                i++;
                continue;
            }

            if (isspace(line[i]))
            {
                if (!shangciShiKongge)
                {
                    yichuliWenben += ' ';
                    shangciShiKongge = true;
                }
            }
            else
            {
                yichuliWenben += line[i];
                shangciShiKongge = false;
            }
            i++;
        }
    }

    file.close();
    return yichuliWenben;
}

unordered_map<string, int> buildTable(initializer_list<string> items, int startCode)
{
    unordered_map<string, int> table;
    int code = startCode;
    for (const auto &item : items)
    {
        table[item] = code++;
    }
    return table;
}

Lexer::Lexer(const string &source) : shuruHuancun(source), pos(0)
{
    initTables();
}

void Lexer::reset(const string &source)
{
    shuruHuancun = source;
    pos = 0;
}

bool Lexer::isEnd() const
{
    return pos >= shuruHuancun.size();
}
void Lexer::initTables()
{

    guanjianbiao = buildTable(
        {"int", "float", "if", "else", "while", "for", "void", "return"},
        1);

    yunsuanzibiao = buildTable(
        {"+", "-", "*", "/", "=", "==", "!=", "<", ">", "(", ")", "{", "}", ";"}, // 19-23 mean seprator
        10);
}
// // keyword
// unordered_map<string, int> keywordTable = {
//     {"int", 1}, {"float", 2}, {"if", 3}, {"else", 4}, {"while", 5}, {"for", 6}, {"void", 7}, {"return", 8}};

// // operator
// unordered_map<string, int> operatorTable = {
//     {"+", 10}, {"-", 11}, {"*", 12}, {"/", 13}, {"=", 14}, {"==", 15}, {"!=", 16}, {"<", 17}, {">", 18}, {"(", 20}, {")", 21}, {"{", 22}, {"}", 23}, {";", 24}};
// 改成按照基数自增的,方便后面改

void Lexer::skipWhitespace()
{
    while (pos < shuruHuancun.size() && isspace(shuruHuancun[pos]))
    {
        pos++;
    }
}

string Lexer::getOperator()
{
    string op;
    char c = shuruHuancun[pos];
    if (pos + 1 < shuruHuancun.size()) // two char operator
    {
        string twoCharOp;
        twoCharOp += c;
        twoCharOp += shuruHuancun[pos + 1];
        if (yunsuanzibiao.count(twoCharOp))
        {
            pos += 2;
            return twoCharOp;
        }
    }

    op = c;
    pos++;
    return op;
}

// analyze 是第一版的分析代码,整体分析传入的所有字符串
// vector<Token> analyze()
// {
//     vector<Token> tokens;
//     while (pos < shuruHuancun.size())
//     {
//         skipWhitespace();
//         if (pos >= shuruHuancun.size())
//             break;
//         char c = shuruHuancun[pos];
//         // ----number----
//         if (isdigit(c))
//         {
//             string num;
//             while (pos < shuruHuancun.size() && isdigit(shuruHuancun[pos]))
//             {
//                 num += shuruHuancun[pos];
//                 pos++;
//             }
//             int value = stoi(num);
//             tokens.emplace_back(25, num, value); // 25 mean digital
//             continue;
//         }
//         //-----keyword---
//         if (isalpha(c) || c == '_')
//         {
//             string word;
//             while (pos < shuruHuancun.size() && (isalnum(shuruHuancun[pos]) || shuruHuancun[pos] == '_'))
//             {
//                 word += shuruHuancun[pos];
//                 pos++;
//             }
//             if (keywordTable.count(word))
//             {
//                 int code = keywordTable[word];
//                 tokens.emplace_back(code, word, -1);
//             }
//             else
//             {
//                 tokens.emplace_back(0, word, -1); // 0 mean id
//             }
//             continue;
//         }
//         //-----operator--
//         string op = getOperator();
//         if (operatorTable.count(op))
//         {
//             int code = operatorTable[op];
//             tokens.emplace_back(code, op, -1);
//         }
//         else if (!isspace(op[0]))
//         {
//             cout << "error , cant recognize the word" << op << endl;
//         }
//     }
//     return tokens;
// }

Token Lexer::nextToken()
{

    skipWhitespace();
    if (pos >= shuruHuancun.size())
        return Token(-1, "", -1);
    char c = shuruHuancun[pos];
    // ----number----
    if (isdigit(c))
    {
        string num;
        while (pos < shuruHuancun.size() && isdigit(shuruHuancun[pos]))
        {
            num += shuruHuancun[pos];
            pos++;
        }
        int value = stoi(num);
        return Token(25, num, value); // 25 mean digital
    }
    //-----keyword---
    if (isalpha(c) || c == '_')
    {
        string word;
        while (pos < shuruHuancun.size() && (isalnum(shuruHuancun[pos]) || shuruHuancun[pos] == '_'))
        {
            word += shuruHuancun[pos];
            pos++;
        }
        if (guanjianbiao.count(word))
        {
            int code = guanjianbiao[word];
            return Token(code, word, -1);
        }
        else
        {
            return Token(0, word, -1); // 0 mean id
        }
    }
    //-----operator--
    string op = getOperator();
    if (yunsuanzibiao.count(op))
    {
        int code = yunsuanzibiao[op];
        return Token(code, op, -1);
    }

    cout << "error , cant recognize the word" << op << endl;
    return Token(-2, op, -1);
}

// 0 标识符 1-8 关键字 25 常数 10-18 运算符 19-23 分隔符
string tokenCategory(int code)
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
        return "separator";
    default:
        return "unknown";
    }
}
