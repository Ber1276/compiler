#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cctype>
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

struct Token
{
    int code;    // 种别
    string text; // 文本
    int value;   // 数值

    Token(int c, string t, int v) : code(c), text(t), value(v) {}
}; // token data structure

// // keyword
// unordered_map<string, int> keywordTable = {
//     {"int", 1}, {"float", 2}, {"if", 3}, {"else", 4}, {"while", 5}, {"for", 6}, {"void", 7}, {"return", 8}};

// // operator
// unordered_map<string, int> operatorTable = {
//     {"+", 10}, {"-", 11}, {"*", 12}, {"/", 13}, {"=", 14}, {"==", 15}, {"!=", 16}, {"<", 17}, {">", 18}, {"(", 20}, {")", 21}, {"{", 22}, {"}", 23}, {";", 24}};

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

unordered_map<string, int> keywordTable = buildTable(
    {"int", "float", "if", "else", "while", "for", "void", "return"},
    1);

unordered_map<string, int> operatorTable = buildTable(
    {"+", "-", "*", "/", "=", "==", "!=", "<", ">", "(", ")", "{", "}", ";"},
    10);

string inputBuffer;
int pos = 0;

void skipWhitespace()
{
    while (pos < inputBuffer.size() && isspace(inputBuffer[pos]))
    {
        pos++;
    }
}

string getOperator()
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

vector<Token> analyze()
{
    vector<Token> tokens;
    while (pos < inputBuffer.size())
    {
        skipWhitespace();
        if (pos >= inputBuffer.size())
            break;
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
            tokens.emplace_back(25, num, value); // 25 mean digital
            continue;
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
                tokens.emplace_back(code, word, -1);
            }
            else
            {
                tokens.emplace_back(0, word, -1);
            }
            continue;
        }
        //-----operator--
        string op = getOperator();
        if (operatorTable.count(op))
        {
            int code = operatorTable[op];
            tokens.emplace_back(code, op, -1);
        }
        else if (!isspace(op[0]))
        {
            cout << "error , cant recognize the word" << op << endl;
        }
    }
    return tokens;
}

int main()
{
    // test1--- pre process
    cout << "pre process result" << endl
         << PreprocessFile("test.c") << endl
         << endl;

    inputBuffer = PreprocessFile("test.c");

    vector<Token> tokens = analyze();
    for (const auto &t : tokens)
    {
        cout << t.code << " " << t.text << " " << t.value << endl;
    }
    return 0;
}
