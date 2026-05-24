#include "lex.h"
#include <memory>
#include <vector>
#include <set>
#include <unordered_map>
#include <map>
#include <utility>
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

struct TacEntry // 三地址码缩写,three address code
{
    string op;
    string arg1;
    string arg2;
    string result;
};

struct SemanticValue // 语义分析要用
{
    string name;
    string type;
    string place;
    string relop;
    string leftPlace;
    string rightPlace;
    vector<TacEntry> code;
};

struct SymbolInfo
{
    string name;
    string type;
    bool declared; // 这个属于拓展的,用于检查先声明后使用,后面有时间实现
};

struct AstNode
{
    string label;
    vector<shared_ptr<AstNode>> children;

    explicit AstNode(string value) : label(std::move(value)) {}
};

static string terminalOf(const Token &token);

class Parser
{
public:
    explicit Parser(Lexer &lexer);
    bool parse(bool printAstTree = false);
    void printSymbolTable() const;
    void printTacTable() const;
    // void dumpTrace() const;

private:
    // 数据结构
    Lexer &lexer;
    Token lookahead;
    vector<int> stateStack;
    vector<string> symbolStack;
    vector<shared_ptr<AstNode>> astStack;
    vector<SemanticValue> semanticStack;
    shared_ptr<AstNode> astRoot;
    bool printAstTreeEnabled;

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
    vector<TacEntry> tacTable;
    int tempCounter;
    int labelCounter;

    // 做表用的
    void advance();
    void buildGrammar();
    void buildFirstSets();
    string qianming(const set<LR1Item> &items) const;
    set<string> firstOfSequence(const vector<string> &symbols) const;
    set<LR1Item> closure(const set<LR1Item> &items) const;
    set<LR1Item> goTo(const set<LR1Item> &items, const string &symbol) const;
    void buildStateGraph();
    void buildTables();
    int stateId(const set<LR1Item> &items) const;
    ActionEntry action(int state, const string &terminal) const;
    int goTo(int state, const string &nonterminal) const;
    shared_ptr<AstNode> buildAstNode(const Production &prod, const vector<shared_ptr<AstNode>> &children) const;
    SemanticValue buildSemanticValue(const Production &prod, const vector<SemanticValue> &children); // 产生语义动作
    void printAst(const shared_ptr<AstNode> &node) const;
    bool shouldKeepAstChild(const string &label) const;
    string makeLeafLabel(const Token &token) const;
    string newTemp(); // 由于三地址码的特性,需要创建临时变量
    string newLabel();
    static vector<TacEntry> mergeCode(const vector<TacEntry> &left, const vector<TacEntry> &right);                             // 代码合并
    void emitTac(vector<TacEntry> &code, const string &op, const string &arg1, const string &arg2, const string &result) const; // 新增具体指令
};