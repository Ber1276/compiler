#include "yacc.h"
#include <functional>
#include <iostream>
#include <sstream>
#include <algorithm>

// 文法整理一遍

// 声明
//  P' -> P
//  P -> D S
//  D -> L id ; D
//  D -> ε
//  L -> int
//  L -> float

// 赋值条件判断
//  S -> S ; Stmt
//  S -> Stmt
//  Stmt -> Assign
//  Stmt -> IfStmt
//  Stmt -> WhileStmt
//  Assign -> id = E ;
//  IfStmt -> if ( C ) Stmt else Stmt
//  IfStmt -> if ( C ) StmtNoElse
//  StmtNoElse -> Assign
//  StmtNoElse -> WhileStmt
//  StmtNoElse -> if ( C ) StmtNoElse else StmtNoElse
//  WhileStmt -> while ( C ) Stmt
// 加法+乘法+表达式
//  C -> E > E
//  C -> E < E
//  C -> E == E
//  E -> E + T
//  E -> E - T
//  E -> T
//  T -> T * F
//  T -> T / F
//  T -> F
//  F -> ( E )
//  F -> id
//  F -> digits

// 在此输入产生式
void Parser::buildGrammar()
{
    productions.clear();
    terminalId.clear();
    nonterminalId.clear();
    firstSets.clear();
    allGrammarSymbols.clear();

    // 1. 产生式
    productions.push_back({"P'", {"P"}});

    productions.push_back({"P", {"D", "S"}});

    productions.push_back({"D", {"L", "id", ";", "D"}});
    productions.push_back({"D", {}}); // ε

    productions.push_back({"L", {"int"}});
    productions.push_back({"L", {"float"}});

    productions.push_back({"S", {"S", "Stmt"}}); // 这里把";"删了,因为有问题
    productions.push_back({"S", {"Stmt"}});

    productions.push_back({"Stmt", {"Assign"}});
    productions.push_back({"Stmt", {"IfStmt"}});
    productions.push_back({"Stmt", {"WhileStmt"}});

    productions.push_back({"Assign", {"id", "=", "E", ";"}});

    productions.push_back({"IfStmt", {"if", "(", "C", ")", "Stmt", "else", "Stmt"}});
    productions.push_back({"IfStmt", {"if", "(", "C", ")", "StmtNoElse"}});

    productions.push_back({"StmtNoElse", {"Assign"}});
    productions.push_back({"StmtNoElse", {"WhileStmt"}});
    productions.push_back({"StmtNoElse", {"if", "(", "C", ")", "StmtNoElse", "else", "StmtNoElse"}});

    productions.push_back({"WhileStmt", {"while", "(", "C", ")", "Stmt"}});

    productions.push_back({"C", {"E", ">", "E"}});
    productions.push_back({"C", {"E", "<", "E"}});
    productions.push_back({"C", {"E", "==", "E"}});

    productions.push_back({"E", {"E", "+", "T"}});
    productions.push_back({"E", {"E", "-", "T"}});
    productions.push_back({"E", {"T"}});

    productions.push_back({"T", {"T", "*", "F"}});
    productions.push_back({"T", {"T", "/", "F"}});
    productions.push_back({"T", {"F"}});

    productions.push_back({"F", {"(", "E", ")"}});
    productions.push_back({"F", {"id"}});
    productions.push_back({"F", {"digits"}});

    // 2. 非终结符集合
    nonterminalId["P'"] = 0;
    nonterminalId["P"] = 1;
    nonterminalId["D"] = 2;
    nonterminalId["L"] = 3;
    nonterminalId["S"] = 4;
    nonterminalId["Stmt"] = 5;
    nonterminalId["Assign"] = 6;
    nonterminalId["IfStmt"] = 7;
    nonterminalId["StmtNoElse"] = 8;
    nonterminalId["WhileStmt"] = 9;
    nonterminalId["C"] = 10;
    nonterminalId["E"] = 11;
    nonterminalId["T"] = 12;
    nonterminalId["F"] = 13;

    // 3. 终结符集合
    terminalId["id"] = 0;
    terminalId["digits"] = 1;
    terminalId["int"] = 2;
    terminalId["float"] = 3;
    terminalId["if"] = 4;
    terminalId["else"] = 5;
    terminalId["while"] = 6;
    terminalId["+"] = 7;
    terminalId["-"] = 8;
    terminalId["*"] = 9;
    terminalId["/"] = 10;
    terminalId["="] = 11;
    terminalId["=="] = 12;
    terminalId["<"] = 13;
    terminalId[">"] = 14;
    terminalId["("] = 15;
    terminalId[")"] = 16;
    terminalId[";"] = 17;
    terminalId["$"] = 18;

    // 4. FIRST 集初始化
    for (const auto &[term, id] : terminalId)
    {
        firstSets[term].insert(term);
    }

    for (const auto &[nonterm, id] : nonterminalId)
    {
        firstSets[nonterm];
    }

    // 5. 构造所有语法符号，用于项目集展开
    allGrammarSymbols = {
        "id", "digits", "int", "float", "if", "else", "while",
        "+", "-", "*", "/", "=", "==", "<", ">",
        "(", ")", ";", "$",
        "P'", "P", "D", "L", "S", "Stmt", "Assign", "IfStmt", "StmtNoElse", "WhileStmt", "C", "E", "T", "F"};
}

// 创建first集
void Parser::buildFirstSets()
{
    bool changed = true;
    while (changed)
    {
        changed = false; // 使用或等于是因为只要改动一次就算发生了变化,证明没有收敛
        for (const auto &production : productions)
        {
            const string &A = production.lhs;
            if (production.rhs.empty())
            {
                changed |= firstSets[A].insert("ε").second;
                continue;
            }
            bool allNullable = true;
            for (const string &symbol : production.rhs)
            {
                for (const string &item : firstSets[symbol])
                {
                    if (item != "ε")
                    {
                        changed |= firstSets[A].insert(item).second;
                    }
                }
                if (firstSets[symbol].count("ε") == 0)
                {
                    allNullable = false; // 如果中间出现了一个非空的,则不是nullable
                    break;               // 可能是ε才会继续后面的匹配
                }
            }
            if (allNullable)
            {
                changed |= firstSets[A].insert(("ε")).second;
            }
        }
    }
}
// 产生一个first_s集
set<string> Parser::firstOfSequence(const vector<string> &symbols) const
{
    set<string> result;
    bool allNullable = true;

    for (const string &symbol : symbols)
    {
        auto it = firstSets.find(symbol);
        if (it == firstSets.end())
        {
            result.insert(symbol);
            allNullable = false;
            break;
        }
        for (const string &item : it->second)
        {
            if (item != "ε")
            {
                result.insert(item);
            }
        }
        if (it->second.count("ε") == 0)
        {
            allNullable = false;
            break;
        }
    }
    if (allNullable)
    {
        result.insert("ε");
    }
    return result;
}

// closure,就是把右侧的非终结符的所有产生式加进去
set<LR1Item> Parser::closure(const set<LR1Item> &items) const
{
    set<LR1Item> result = items;
    bool changed = true;

    while (changed)
    {
        changed = false;
        vector<LR1Item> current(result.begin(), result.end()); // 深拷贝一份
        for (const LR1Item &item : current)
        {
            const Production &prod = productions[item.productionIndex];
            if (item.dotPosition >= prod.rhs.size())
            {
                continue;
            }
            const string &symbolAfterDot = prod.rhs[item.dotPosition]; // 点后符号
            if (nonterminalId.count(symbolAfterDot) == 0)
            {
                continue;
            }

            vector<string> betaA; // 后缀+前向符号
            for (size_t i = item.dotPosition + 1; i < prod.rhs.size(); ++i)
            {
                betaA.push_back(prod.rhs[i]);
            }
            betaA.push_back(item.lookahead);                 // 包含前向符号
            set<string> lookaheads = firstOfSequence(betaA); // 通过前向符号得到的新项目的前向符号
            for (int i = 0; i < productions.size(); i++)
            {
                if (productions[i].lhs != symbolAfterDot)
                {
                    continue;
                }

                for (const string &la : lookaheads)
                {
                    if (la == "ε")
                    {
                        continue;
                    }
                    LR1Item nextItem{i, 0, la}; // 新项目i指向产生式,0是点的位置,la是前向符号
                    if (result.insert(nextItem).second)
                    {
                        changed = true;
                    }
                }
            }
        }
    }
    return result;
}

// goto从一个图输入symbol产生下一个图(闭包)
set<LR1Item> Parser::goTo(const set<LR1Item> &items, const string &symbol) const
{
    set<LR1Item> moved;

    for (const LR1Item &item : items)
    {
        const Production &prod = productions[item.productionIndex];
        if (item.dotPosition < prod.rhs.size() && prod.rhs[item.dotPosition] == symbol)
        {
            moved.insert(LR1Item{item.productionIndex, item.dotPosition + 1, item.lookahead});
        }
    }
    return closure(moved);
}

// 去重,算是高级功能,c++还是太难了
string Parser::signature(const set<LR1Item> &items) const
{
    vector<string> parts;
    parts.reserve(items.size());

    for (const auto &item : items)
    {
        const Production &prod = productions[item.productionIndex];

        string text = prod.lhs + "->";

        for (size_t i = 0; i < prod.rhs.size(); ++i)
        {
            if (i == item.dotPosition)
            {
                text += ".";
            }
            text += prod.rhs[i];
            text += " ";
        }

        if (item.dotPosition == prod.rhs.size())
        {
            text += ".";
        }

        text += "," + item.lookahead;
        parts.push_back(text);
    }

    sort(parts.begin(), parts.end());

    string result;
    for (const auto &part : parts)
    {
        result += part;
        result += "|";
    }
    return result;
}

// 图
void Parser::buildCanonicalCollection()
{
    set<LR1Item> startItems;
    startItems.insert(LR1Item{0, 0, "$"});
    set<LR1Item> startState = closure(startItems);

    states.clear();

    vector<set<LR1Item>> pending;

    unordered_map<string, int> stateIndex;

    states.push_back(startState);
    pending.push_back(startState);
    stateIndex[signature(startState)] = 0;

    while (!pending.empty())
    {
        set<LR1Item> current = pending.back();
        pending.pop_back();

        for (const string &symbol : allGrammarSymbols)
        {
            set<LR1Item> next = goTo(current, symbol);
            if (next.empty())
            {
                continue;
            }
            string sig = signature(next);
            if (stateIndex.count(sig) == 0)
            {
                int newIndex = static_cast<int>(states.size());
                states.push_back(next);
                pending.push_back(next);
                stateIndex[sig] = newIndex;
            }
        }
    }
}

// 辅助获取stateid
int Parser::stateId(const set<LR1Item> &items) const
{
    string target = signature(items);
    for (size_t i = 0; i < states.size(); ++i)
    {
        if (signature(states[i]) == target)
        {
            return static_cast<int>(i);
        }
    }
    return -1;
}

// 生成表的核心函数
void Parser::buildTables()
{
    for (size_t i = 0; i < states.size(); ++i)
    {
        const auto &state = states[i];

        for (const LR1Item &item : state)
        {
            const Production &prod = productions[item.productionIndex];

            if (item.dotPosition < prod.rhs.size())
            {
                const string &symbol = prod.rhs[item.dotPosition];
                set<LR1Item> next = goTo(state, symbol);
                int j = stateId(next);

                if (terminalId.count(symbol))
                {
                    actionTable[{static_cast<int>(i), symbol}] = ActionEntry{ActionType::Shift, j};
                }
                else if (nonterminalId.count(symbol))
                {
                    gotoTable[{static_cast<int>(i), symbol}] = j;
                }
            }
            else
            {
                if (prod.lhs == "P'" && item.lookahead == "$")
                {
                    actionTable[{static_cast<int>(i), "$"}] = ActionEntry{ActionType::Accept, 0};
                }
                else
                {
                    actionTable[{static_cast<int>(i), item.lookahead}] =
                        ActionEntry{ActionType::Reduce, item.productionIndex};
                }
            }
        }
    }
}

// 解析函数
bool Parser::parse(bool printAstTree)
{
    printAstTreeEnabled = printAstTree;
    stateStack.clear();
    symbolStack.clear();
    astStack.clear();
    astRoot.reset();
    stateStack.push_back(0);
    symbolStack.push_back("$");

    advance();

    while (true)
    {
        int state = stateStack.back();
        string terminal = terminalOf(lookahead);
        ActionEntry act = action(state, terminal);

        if (act.type == ActionType::Shift)
        {
            symbolStack.push_back(terminal);
            stateStack.push_back(act.value);

            if (terminal != "$")
            {
                astStack.push_back(make_shared<AstNode>(makeLeafLabel(lookahead)));
            }

            advance();
        }
        else if (act.type == ActionType::Reduce)
        {
            const Production &prod = productions[act.value];
            size_t popCount = prod.rhs.empty() ? 0 : prod.rhs.size();
            vector<shared_ptr<AstNode>> children;
            children.reserve(popCount);

            for (size_t i = 0; i < popCount; ++i)
            {
                if (!symbolStack.empty())
                    symbolStack.pop_back();
                if (!stateStack.empty())
                    stateStack.pop_back();

                if (!astStack.empty())
                {
                    children.push_back(astStack.back());
                    astStack.pop_back();
                }
            }

            reverse(children.begin(), children.end());

            string lhs = prod.lhs;
            symbolStack.push_back(lhs);

            astStack.push_back(buildAstNode(prod, children));

            int prevState = stateStack.back();
            int nextState = goTo(prevState, lhs);
            stateStack.push_back(nextState);
        }
        else if (act.type == ActionType::Accept)
        {
            if (!astStack.empty())
            {
                astRoot = astStack.back();
            }

            if (printAstTreeEnabled && astRoot)
            {
                cout << "syntax tree:" << endl;
                printAst(astRoot);
            }

            return true;
        }
        else
        {
            return false;
        }
    }
}

Parser::Parser(Lexer &lexer) : lexer(lexer), lookahead(-1, "$", -1)
{
    buildGrammar();
    buildFirstSets();
    buildCanonicalCollection();
    buildTables();
}

// 前递
void Parser::advance()
{
    lookahead = lexer.nextToken();
    if (lookahead.code == -2)
    {
        throw runtime_error("词法错误: " + lookahead.text);
    }
}

ActionEntry Parser::action(int state, const string &terminal) const
{
    auto it = actionTable.find({state, terminal});
    if (it != actionTable.end())
    {
        return it->second;
    }
    return ActionEntry{ActionType::Error, -1};
}

int Parser::goTo(int state, const string &nonterminal) const
{
    auto it = gotoTable.find({state, nonterminal});
    if (it != gotoTable.end())
    {
        return it->second;
    }
    return -1;
}

bool Parser::shouldKeepAstChild(const string &label) const
{
    return label != ";" && label != "(" && label != ")" && label != "if" && label != "else" && label != "while" && label != "=" && label != "+" && label != "-" && label != "*" && label != "/" && label != "<" && label != ">" && label != "==";
}

string Parser::makeLeafLabel(const Token &token) const
{
    switch (token.code)
    {
    case 0:
        return "id(" + token.text + ")";
    case 25:
        return "num(" + token.text + ")";
    default:
        return token.text;
    }
}

shared_ptr<AstNode> Parser::buildAstNode(const Production &prod, const vector<shared_ptr<AstNode>> &children) const
{
    vector<shared_ptr<AstNode>> keptChildren;
    keptChildren.reserve(children.size());
    for (const auto &child : children)
    {
        if (child && shouldKeepAstChild(child->label))
        {
            keptChildren.push_back(child);
        }
    }

    auto makeNode = [&](const string &label, const vector<shared_ptr<AstNode>> &nodeChildren) -> shared_ptr<AstNode>
    {
        auto node = make_shared<AstNode>(label);
        for (const auto &child : nodeChildren)
        {
            if (child)
            {
                node->children.push_back(child);
            }
        }
        return node;
    };

    if (prod.lhs == "P'")
    {
        return keptChildren.empty() ? makeNode("Program", {}) : keptChildren.front();
    }

    if (prod.lhs == "P")
    {
        return makeNode("Program", keptChildren);
    }

    if (prod.lhs == "D")
    {
        return makeNode("DeclList", keptChildren);
    }

    if (prod.lhs == "L")
    {
        return makeNode("Type(" + prod.rhs.front() + ")", {});
    }

    if (prod.lhs == "S")
    {
        return makeNode("StmtList", keptChildren);
    }

    if (prod.lhs == "Stmt")
    {
        return keptChildren.empty() ? makeNode("Stmt", {}) : keptChildren.front();
    }

    if (prod.lhs == "StmtNoElse")
    {
        return keptChildren.empty() ? makeNode("Stmt", {}) : keptChildren.front();
    }

    if (prod.lhs == "Assign")
    {
        return makeNode("Assign", keptChildren);
    }

    if (prod.lhs == "IfStmt")
    {
        return (prod.rhs.size() == 7) ? makeNode("IfElse", keptChildren) : makeNode("If", keptChildren);
    }

    if (prod.lhs == "WhileStmt")
    {
        return makeNode("While", keptChildren);
    }

    if (prod.lhs == "C")
    {
        string op = prod.rhs.size() >= 2 ? prod.rhs[1] : "?";
        return makeNode("Cond(" + op + ")", keptChildren);
    }

    if (prod.lhs == "E")
    {
        if (prod.rhs.size() == 3)
        {
            return makeNode(prod.rhs[1] == "+" ? "Add" : "Sub", keptChildren);
        }
        return keptChildren.empty() ? makeNode("Expr", {}) : keptChildren.front();
    }

    if (prod.lhs == "T")
    {
        if (prod.rhs.size() == 3)
        {
            return makeNode(prod.rhs[1] == "*" ? "Mul" : "Div", keptChildren);
        }
        return keptChildren.empty() ? makeNode("Term", {}) : keptChildren.front();
    }

    if (prod.lhs == "F")
    {
        if (prod.rhs.size() == 3)
        {
            return keptChildren.empty() ? makeNode("Expr", {}) : keptChildren.front();
        }
        return keptChildren.empty() ? makeNode("Factor", {}) : keptChildren.front();
    }

    if (keptChildren.size() == 1)
    {
        return keptChildren.front();
    }

    return makeNode(prod.lhs, keptChildren);
}

void Parser::printAst(const shared_ptr<AstNode> &node) const
{
    if (!node)
    {
        return;
    }

    struct Placement
    {
        size_t x;
        string label;
    };

    vector<vector<Placement>> rows;
    constexpr size_t gap = 4;

    function<size_t(const shared_ptr<AstNode> &)> measure = [&](const shared_ptr<AstNode> &current) -> size_t
    {
        if (!current)
        {
            return 0;
        }

        size_t labelWidth = current->label.size();
        if (current->children.empty())
        {
            return labelWidth;
        }

        size_t childrenWidth = 0;
        for (size_t i = 0; i < current->children.size(); ++i)
        {
            childrenWidth += measure(current->children[i]);
            if (i + 1 < current->children.size())
            {
                childrenWidth += gap;
            }
        }

        return max(labelWidth, childrenWidth);
    };

    function<void(const shared_ptr<AstNode> &, size_t, size_t)> place = [&](const shared_ptr<AstNode> &current, size_t depth, size_t left) -> void
    {
        if (!current)
        {
            return;
        }

        if (rows.size() <= depth)
        {
            rows.resize(depth + 1);
        }

        size_t subtreeWidth = measure(current);
        size_t labelWidth = current->label.size();
        size_t labelX = left + (subtreeWidth - labelWidth) / 2;
        rows[depth].push_back({labelX, current->label});

        if (current->children.empty())
        {
            return;
        }

        size_t childrenWidth = 0;
        vector<size_t> childWidths;
        childWidths.reserve(current->children.size());
        for (const auto &child : current->children)
        {
            size_t width = measure(child);
            childWidths.push_back(width);
            childrenWidth += width;
        }
        if (current->children.size() > 1)
        {
            childrenWidth += gap * (current->children.size() - 1);
        }

        size_t childLeft = left + (subtreeWidth - childrenWidth) / 2;
        for (size_t i = 0; i < current->children.size(); ++i)
        {
            place(current->children[i], depth + 1, childLeft);
            childLeft += childWidths[i] + gap;
        }
    };

    place(node, 0, 0);

    size_t maxWidth = 0;
    for (const auto &row : rows)
    {
        for (const auto &item : row)
        {
            maxWidth = max(maxWidth, item.x + item.label.size());
        }
    }

    auto trimRight = [](string text) -> string
    {
        while (!text.empty() && text.back() == ' ')
        {
            text.pop_back();
        }
        return text;
    };

    for (const auto &row : rows)
    {
        string line(maxWidth, ' ');
        for (const auto &item : row)
        {
            if (line.size() < item.x + item.label.size())
            {
                line.resize(item.x + item.label.size(), ' ');
            }
            for (size_t i = 0; i < item.label.size(); ++i)
            {
                line[item.x + i] = item.label[i];
            }
        }
        cout << trimRight(line) << endl;
    }
}

static string terminalOf(const Token &token)
{
    switch (token.code)
    {
    case -1:
        return "$";
    case 0:
        return "id";
    case 25:
        return "digits";
    case 1:
        return "int";
    case 2:
        return "float";
    case 3:
        return "if";
    case 4:
        return "else";
    case 5:
        return "while";
    case 10:
        return "+";
    case 11:
        return "-";
    case 12:
        return "*";
    case 13:
        return "/";
    case 14:
        return "=";
    case 15:
        return "==";
    case 17:
        return "<";
    case 18:
        return ">";
    case 19:
        return "(";
    case 20:
        return ")";
    case 23:
        return ";";
    default:
        return "INVALID";
    }
}
