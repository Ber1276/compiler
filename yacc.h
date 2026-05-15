#include "lex.h"
#include <vector>
#include <set>
#include <unordered_map>
#include <map>
#include <string>
using namespace std;

// 数据结构
struct Production
{
    string lhs;         // left hand side
    vector<string> rhs; // right hand side
};

struct LR1Item
{
    int productionIndex; // 类似于指针,指向而非拷贝
    size_t dotPosition;
    string lookahead;
};
inline bool operator<(const LR1Item &a, const LR1Item &b)
{
    if (a.productionIndex != b.productionIndex)
        return a.productionIndex < b.productionIndex;
    if (a.dotPosition != b.dotPosition)
        return a.dotPosition < b.dotPosition;
    return a.lookahead < b.lookahead;
}

enum class ActionType // 枚举可读性好一些的
{
    Shift,  // 移进
    Reduce, // 规约
    Accept, // 接受
    Error   // 错误
};

struct ActionEntry
{
    ActionType type;
    int value;
};

struct SymbolInfo
{
    string name;
    string type;
    bool declared; // 这个属于拓展的,用于检查先声明后使用,后面有时间实现
};
static string terminalOf(const Token &token);

class Parser
{
public:
    explicit Parser(Lexer &lexer);
    bool parse();
    // void dumpTrace() const;

private:
    // 数据结构
    Lexer &lexer;
    Token lookahead;
    vector<int> stateStack;
    vector<string> symbolStack;

    // 自动构造用的
    vector<Production> productions;
    vector<set<LR1Item>> states;
    unordered_map<string, set<string>> firstSets;
    unordered_map<string, int> terminalId;
    unordered_map<string, int> nonterminalId;
    map<pair<int, string>, ActionEntry> actionTable;
    map<pair<int, string>, int> gotoTable;
    unordered_map<string, SymbolInfo> symbolTable;
    vector<string> traceLines; // 分析guocheng保留
    string currentDeclType;
    vector<string> allGrammarSymbols;

    // 做表用的
    void advance();
    void buildGrammar();
    void buildFirstSets();
    string signature(const set<LR1Item> &items) const;
    set<string> firstOfSequence(const vector<string> &symbols) const;
    set<LR1Item> closure(const set<LR1Item> &items) const;
    set<LR1Item> goTo(const set<LR1Item> &items, const string &symbol) const;
    void buildCanonicalCollection();
    void buildTables();
    int stateId(const set<LR1Item> &items) const;
    ActionEntry action(int state, const string &terminal) const;
    int goTo(int state, const string &nonterminal) const;
};