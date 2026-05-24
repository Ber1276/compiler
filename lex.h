#pragma once
#include <string>
#include <unordered_map>
using namespace std;

struct Token
{
    int code;    // 种别
    string text; // 文本
    int value;   // 数值

    Token(int c, string t, int v) : code(c), text(t), value(v) {}
}; // token data structure

string PreprocessFile(const string &filename); // 声明
unordered_map<string, int> buildTable(initializer_list<string> items, int startCode);

// 0 标识符 1-8 关键字 25 常数 10-18 运算符 19-23 分隔符
string tokenCategory(int code);

class Lexer
{
public:
    explicit Lexer(const string &source); // source file

    Token nextToken(); // 核心方法,针对实验二改成逐步调用的,写成类的话方便导出给别人使用一些.
    bool isEnd() const;
    void reset(const string &source);

private:
    string shuruHuancun;
    size_t pos;
    unordered_map<string, int> guanjianbiao;
    unordered_map<string, int> yunsuanzibiao;

    // 辅助函数声明
    void skipWhitespace();

    string getOperator();
    void initTables();
};