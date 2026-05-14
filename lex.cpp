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
    string processed;            // save resule
    bool inBlockComment = false; // block comment
    bool lastWasSpace = false;   // is white flag

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
            if (!inBlockComment && i < len - 1 && line[i] == '/' && line[i + 1] == '/')
            {
                break; // ignore
            } // meet //

            if (!inBlockComment && i < len - 1 && line[i] == '/' && line[i + 1] == '*')
            {
                inBlockComment = true;
                i += 2;
                continue;
            } // meet /* (block comment start)

            if (inBlockComment && i < len - 1 && line[i] == '*' && line[i + 1] == '/')
            {
                inBlockComment = false;
                i += 2;
                continue;
            } // meet */ (block comment end)

            if (inBlockComment)
            {
                i++;
                continue;
            }

            if (isspace(line[i]))
            {
                if (!lastWasSpace)
                {
                    processed += ' ';
                    lastWasSpace = true;
                }
            }
            else
            {
                processed += line[i];
                lastWasSpace = false;
            }
            i++;
        }
    }

    file.close();
    return processed;
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

Lexer::Lexer(const string &source) : inputBuffer(source), pos(0)
{
    initTables();
}

void Lexer::reset(const string &source)
{
    inputBuffer = source;
    pos = 0;
}

bool Lexer::isEnd() const
{
    return pos >= inputBuffer.size();
}
void Lexer::initTables()
{

    keywordTable = buildTable(
        {"int", "float", "if", "else", "while", "for", "void", "return"},
        1);

    operatorTable = buildTable(
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
    while (pos < inputBuffer.size() && isspace(inputBuffer[pos]))
    {
        pos++;
    }
}

string Lexer::getOperator()
{
    string op;
    char c = inputBuffer[pos];
    if (pos + 1 < inputBuffer.size()) // two char operator
    {
        string twoCharOp;
        twoCharOp += c;
        twoCharOp += inputBuffer[pos + 1];
        if (operatorTable.count(twoCharOp))
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
//     while (pos < inputBuffer.size())
//     {
//         skipWhitespace();
//         if (pos >= inputBuffer.size())
//             break;
//         char c = inputBuffer[pos];
//         // ----number----
//         if (isdigit(c))
//         {
//             string num;
//             while (pos < inputBuffer.size() && isdigit(inputBuffer[pos]))
//             {
//                 num += inputBuffer[pos];
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
//             while (pos < inputBuffer.size() && (isalnum(inputBuffer[pos]) || inputBuffer[pos] == '_'))
//             {
//                 word += inputBuffer[pos];
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
    if (pos >= inputBuffer.size())
        return Token(-1, "", -1);
    char c = inputBuffer[pos];
    // ----number----
    if (isdigit(c))
    {
        string num;
        while (pos < inputBuffer.size() && isdigit(inputBuffer[pos]))
        {
            num += inputBuffer[pos];
            pos++;
        }
        int value = stoi(num);
        return Token(25, num, value); // 25 mean digital
    }
    //-----keyword---
    if (isalpha(c) || c == '_')
    {
        string word;
        while (pos < inputBuffer.size() && (isalnum(inputBuffer[pos]) || inputBuffer[pos] == '_'))
        {
            word += inputBuffer[pos];
            pos++;
        }
        if (keywordTable.count(word))
        {
            int code = keywordTable[word];
            return Token(code, word, -1);
        }
        else
        {
            return Token(0, word, -1); // 0 mean id
        }
    }
    //-----operator--
    string op = getOperator();
    if (operatorTable.count(op))
    {
        int code = operatorTable[op];
        return Token(code, op, -1);
    }

    cout << "error , cant recognize the word" << op << endl;
    return Token(-2, op, -1);
}
