#include <bits/stdc++.h>
#include <regex.h>
using namespace std;

namespace SIT {
    class String {
        public:
        string value;
        String() {}
        String(const char *s) { this->value = s; }
        String(string s) { this->value = s; }
        String(bool b) { this->value = b ? "True" : "False"; }
        String(int i) { this->value = to_string(i); }
        int ToInt() const { return stoi(value); }
        bool ToBool() const { return value == "True"; }
    };
    class Expression;
    class Leaf;
    class Expression {
        public:
        virtual Leaf Evaluate() = 0;
        virtual void Debug() = 0;
    };
    class Leaf : public Expression {
        public:
        String value;
        Leaf() {}
        Leaf(String s) { this->value = s; }
        Leaf Evaluate() override { return *this; }
        void Debug() override { cerr << "Leaf(\"" << value.value << "\")"; }
    };
    map<string, Leaf> variables;
    class Variable : public Expression {
        public:
        String name;
        Variable(String name) { this->name = name; }
        Leaf Evaluate() override { return variables[name.value]; }
        void Debug() override { cerr << "Variable(" << name.value << ")"; }
    };
    class BinaryOperation : public Expression {
        public:
        String op;
        Expression *left;
        Expression *right;
        BinaryOperation(String op, Expression *left, Expression *right) {
            this->op = op, this->left = left, this->right = right;
        }
        Leaf Evaluate() override {
            String left = this->left->Evaluate().value;
            String right = this->right->Evaluate().value;
            if (this->op.value == "+") return Leaf(left.value + right.value);
            if (this->op.value == "==") return Leaf(left.value == right.value);
            if (this->op.value == "!=") return Leaf(left.value != right.value);
            if (this->op.value == "<") return Leaf(left.value < right.value);
            if (this->op.value == ">") return Leaf(left.value > right.value);
            if (this->op.value == "<=") return Leaf(left.value <= right.value);
            if (this->op.value == ">=") return Leaf(left.value >= right.value);
            if (this->op.value == "and") return Leaf(String(left.ToBool() && right.ToBool()));
            if (this->op.value == "or") return Leaf(String(left.ToBool() || right.ToBool()));
            if (this->op.value == "xor") return Leaf(String(left.ToBool() ^ right.ToBool()));
            if (this->op.value == "add") return Leaf(String(left.ToInt() + right.ToInt()));
            if (this->op.value == "sub") return Leaf(String(left.ToInt() - right.ToInt()));
            if (this->op.value == "mul") return Leaf(String(left.ToInt() * right.ToInt()));
            if (this->op.value == "div") return Leaf(String(left.ToInt() / right.ToInt()));
            if (this->op.value == "mod") return Leaf(String(left.ToInt() % right.ToInt()));
            if (this->op.value == "lt") return Leaf(String(left.ToInt() < right.ToInt()));
            if (this->op.value == "gt") return Leaf(String(left.ToInt() > right.ToInt()));
            if (this->op.value == "le") return Leaf(String(left.ToInt() <= right.ToInt()));
            if (this->op.value == "ge") return Leaf(String(left.ToInt() >= right.ToInt()));
            return Leaf();
        }
        void Debug() override {
            cerr << "BinaryOperation(\"" << op.value << "\", ";
            left->Debug();
            cerr << ", ";
            right->Debug();
            cerr << ")";
        }
    };
    class UnaryOperation : public Expression {
        public:
        String op;
        Expression *expr;
        UnaryOperation(String op, Expression *expr) { this->op = op, this->expr = expr; }
        Leaf Evaluate() override {
            String value = this->expr->Evaluate().value;
            if (this->op.value == "not") return Leaf(String(!value.ToBool()));
            if (this->op.value == "neg") return Leaf(String(-value.ToInt()));
            return Leaf();
        }
        void Debug() override {
            cerr << "UnaryOperation(\"" << op.value << "\", ";
            expr->Debug();
            cerr << ")";
        }
    };
    class Assignment : public Expression {
        public:
        String name;
        Expression *value;
        Assignment(String name, Expression *value) { this->name = name, this->value = value; }
        Leaf Evaluate() override { return variables[name.value] = value->Evaluate(); }
        void Debug() override {
            cerr << "Assignment(" << name.value << ", ";
            value->Debug();
            cerr << ")";
        }
    };
    class Sequence : public Expression {
        public:
        Expression *left;
        Expression *right;
        Sequence(Expression *left, Expression *right) { this->left = left, this->right = right; }
        Leaf Evaluate() override {
            left->Evaluate();
            return right->Evaluate();
        }
        void Debug() override {
            cerr << "Sequence(";
            left->Debug();
            cerr << ", ";
            right->Debug();
            cerr << ")";
        }
    };
    class IfPair {
        public:
        Expression *condition;
        Expression *body;
        IfPair() {}
        IfPair(Expression *condition, Expression *body) {
            this->condition = condition, this->body = body;
        }
        bool Execute() {
            if (condition->Evaluate().value.value == "True") {
                body->Evaluate();
                return true;
            } else {
                return false;
            }
        }
        void Debug() {
            cerr << "    IfPair(";
            condition->Debug();
            cerr << ", ";
            body->Debug();
            cerr << ")";
        }
    };
    vector<IfPair> ifPairs;
    class Pop : public Expression {
        public:
        Pop() {}
        Leaf Evaluate() override {
            if (ifPairs.size() > 0) {
                ifPairs.pop_back();
                return Leaf(String("True"));
            } else {
                return Leaf(String("False"));
            }
        }
        void Debug() override { cerr << "Pop()"; }
    };
    class Push : public Expression {
        public:
        IfPair ifPair;
        Push(IfPair ifPair) { this->ifPair = ifPair; }
        Leaf Evaluate() override {
            ifPairs.push_back(ifPair);
            return Leaf(String("True"));
        }
        void Debug() override {
            cerr << "Push(";
            ifPair.Debug();
            cerr << ")";
        }
    };
    class Erase : public Expression {
        public:
        Expression *num;
        Erase(Expression *num) { this->num = num; }
        Leaf Evaluate() override {
            int n = num->Evaluate().value.ToInt();
            if (0 <= n && n < (int) ifPairs.size()) {
                ifPairs.erase(ifPairs.begin() + n);
                return Leaf(String("True"));
            } else {
                return Leaf(String("False"));
            }
        }
        void Debug() override {
            cerr << "Erase(";
            num->Debug();
            cerr << ")";
        }
    };
    class Insert : public Expression {
        public:
        Expression *num;
        IfPair ifPair;
        Insert(Expression *num, IfPair ifPair) { this->num = num, this->ifPair = ifPair; }
        Leaf Evaluate() override {
            int n = num->Evaluate().value.ToInt();
            if (0 <= n && n <= (int) ifPairs.size()) {
                ifPairs.insert(ifPairs.begin() + n, ifPair);
                return Leaf(String("True"));
            } else {
                return Leaf(String("False"));
            }
        }
        void Debug() override {
            cerr << "Insert(";
            num->Debug();
            cerr << ", ";
            ifPair.Debug();
            cerr << ")";
        }
    };
    class Print : public Expression {
        public:
        Expression *expression;
        Print(Expression *expression) { this->expression = expression; }
        Leaf Evaluate() override {
            cout << expression->Evaluate().value.value << endl;
            return Leaf(String("True"));
        }
        void Debug() override {
            cerr << "Print(";
            expression->Debug();
            cerr << ")";
        }
    };
    class Read : public Expression {
        public:
        String name;
        Read(String name) { this->name = name; }
        Leaf Evaluate() override {
            string input;
            getline(cin, input);
            return variables[name.value] = Leaf(String(input));
        }
        void Debug() override { cerr << "Read(" << name.value << ")"; }
    };

    void Debug() {
        cerr << "{" << endl;
        for (int i = 0; i < (int) ifPairs.size(); i++) {
            ifPairs[i].Debug();
            if (i != (int) ifPairs.size() - 1) cerr << ",";
            cerr << endl;
        }
        cerr << "}" << endl;
    }
    bool Execute() {
        for (IfPair ifPair : ifPairs) {
            if (ifPair.Execute()) {
                return true;
            }
        }
        return false;
    }
    bool debug = false, failExit = false;
    void Run() {
        while (ifPairs.size() > 0) {
            bool result = Execute();
            if (debug) Debug();
            if (failExit && !result) break;
        }
    }

    vector<string> Tokenize(string input) {
        string idt = "[a-zA-Z_][a-zA-Z0-9_]*";
        string num = "[0-9]+";
        string op =
            "\\+|==|\\!=|\\%|\\=|\\!|\\<|\\>|\\<=|\\>=|\\&\\&|\\|\\||\\^\\^|\\(|\\)|\\,|\\?";
        string str = "\\\"(\\\\.|[^\\\"])*\\\"";
        string regex = "(" + idt + ")|(" + num + ")|(" + op + ")|(" + str + ")";
        regex_t regex_compiled;
        regmatch_t match[1];
        regcomp(&regex_compiled, regex.c_str(), REG_EXTENDED);
        vector<string> tokens;
        while (input.length() > 0) {
            if (regexec(&regex_compiled, input.c_str(), 1, match, 0) == 0) {
                tokens.push_back(input.substr(match[0].rm_so, match[0].rm_eo - match[0].rm_so));
                input = input.substr(match[0].rm_eo);
            } else {
                cerr << "Error: Invalid token" << endl;
                return vector<string>();
            }
        }
        return tokens;
    }
    IfPair TokenToIfPair(vector<string> token);
    pair<int, string> Find(vector<string> key, vector<string> tokens) {
        int dep = 0;
        for (int i = 0; i < (int) tokens.size(); i++) {
            if (tokens[i] == "(") dep++;
            if (tokens[i] == ")") dep--;
            if (dep == 0) {
                for (int j = 0; j < (int) key.size(); j++) {
                    if (tokens[i] == key[j]) {
                        return pair<int, string>(i, key[j]);
                    }
                }
            }
        }
        return pair<int, string>(-1, "");
    }
    Expression *TokenToExpression(vector<string> token) {
        if (token.size() == 0) return nullptr;
        pair<int, string> s;
        if ((s = Find({","}, token)).first != -1) {
            vector<string> left = vector<string>(token.begin(), token.begin() + s.first);
            vector<string> right = vector<string>(token.begin() + s.first + 1, token.end());
            return new Sequence(TokenToExpression(left), TokenToExpression(right));
        }
        // Statement
        if (token[0] == "push") {
            return new Push(TokenToIfPair(vector<string>(token.begin() + 2, token.end() - 1)));
        }
        if (token[0] == "pop") {
            return new Pop();
        }
        if (token[0] == "insert") {
            s = Find({","}, token);
            return new Insert(
                TokenToExpression(vector<string>(token.begin() + 2, token.begin() + s.first)),
                TokenToIfPair(vector<string>(token.begin() + s.first + 1, token.end() - 1)));
        }
        if (token[0] == "erase") {
            return new Erase(TokenToExpression(vector<string>(token.begin() + 2, token.end() - 1)));
        }
        if (token[0] == "print") {
            return new Print(TokenToExpression(vector<string>(token.begin() + 2, token.end() - 1)));
        }
        if (token[0] == "read") {
            return new Read(String(token[2]));
        }
        // Expression
        if ((s = Find({"="}, token)).first != -1) {
            return new Assignment(
                String(token[s.first - 1]),
                TokenToExpression(vector<string>(token.begin() + s.first + 1, token.end())));
        }
        if ((s = Find({"||", "or"}, token)).first != -1) {
            return new BinaryOperation(
                String("or"),
                TokenToExpression(vector<string>(token.begin(), token.begin() + s.first)),
                TokenToExpression(vector<string>(token.begin() + s.first + 1, token.end())));
        }
        if ((s = Find({"^^", "xor"}, token)).first != -1) {
            return new BinaryOperation(
                String("xor"),
                TokenToExpression(vector<string>(token.begin(), token.begin() + s.first)),
                TokenToExpression(vector<string>(token.begin() + s.first + 1, token.end())));
        }
        if ((s = Find({"&&", "and"}, token)).first != -1) {
            return new BinaryOperation(
                String("and"),
                TokenToExpression(vector<string>(token.begin(), token.begin() + s.first)),
                TokenToExpression(vector<string>(token.begin() + s.first + 1, token.end())));
        }
        if ((s = Find({"==", "!="}, token)).first != -1) {
            return new BinaryOperation(
                String(s.second),
                TokenToExpression(vector<string>(token.begin(), token.begin() + s.first)),
                TokenToExpression(vector<string>(token.begin() + s.first + 1, token.end())));
        }
        if ((s = Find({"<", ">", "<=", ">=", "lt", "gt", "le", "ge"}, token)).first != -1) {
            return new BinaryOperation(
                String(s.second),
                TokenToExpression(vector<string>(token.begin(), token.begin() + s.first)),
                TokenToExpression(vector<string>(token.begin() + s.first + 1, token.end())));
        }
        if ((s = Find({"+", "add", "sub"}, token)).first != -1) {
            return new BinaryOperation(
                String(s.second),
                TokenToExpression(vector<string>(token.begin(), token.begin() + s.first)),
                TokenToExpression(vector<string>(token.begin() + s.first + 2, token.end())));
        }
        if ((s = Find({"mul", "div", "mod"}, token)).first != -1) {
            return new BinaryOperation(
                String(s.second),
                TokenToExpression(vector<string>(token.begin(), token.begin() + s.first)),
                TokenToExpression(vector<string>(token.begin() + s.first + 1, token.end())));
        }
        if ((s = Find({"neg", "not"}, token)).first != -1) {
            return new UnaryOperation(
                String(s.second),
                TokenToExpression(vector<string>(token.begin() + s.first + 1, token.end())));
        }
        // Parentheses
        if (token[0] == "(") {
            return TokenToExpression(vector<string>(token.begin() + 1, token.end() - 1));
        }
        // Literals
        if (token[0][0] == '\"') {
            return new Leaf(String(token[0].substr(1, token[0].size() - 2)));
        }
        if (token[0] == "True" || token[0] == "False") {
            return new Leaf(String(token[0]));
        }
        // Numbers
        if (isdigit(token[0][0])) {
            return new Leaf(String(token[0]));
        }
        // Variables
        return new Variable(String(token[0]));
    }
    IfPair TokenToIfPair(vector<string> token) {
        int s = Find({"?"}, token).first;
        return IfPair(TokenToExpression(vector<string>(token.begin(), token.begin() + s)),
                      TokenToExpression(vector<string>(token.begin() + s + 1, token.end())));
    }
    void Parse(vector<string> s) {
        for (string line : s) {
            if (line == "#DEBUG") debug = true;
            if (line == "#NODEBUG") debug = false;
            if (line == "#FAILEXIT") failExit = true;
            if (line == "#NOFAILEXIT") failExit = false;
            if (line[0] == '#') continue;
            ifPairs.push_back(TokenToIfPair(Tokenize(line)));
        }
    }
    void Interpret(istream &input) {
        vector<string> s;
        string line;
        while (getline(input, line) && line != "?") s.push_back(line);
        Parse(s);
        if (debug) Debug();
        Run();
    }
    void Reset() { ifPairs.clear(), variables.clear(); }
}; // namespace SIT

int main(int argc, char *argv[]) {
    string s = "";
    if (argc > 1) {
        if (argc == 3 && string(argv[1]) == "-d") {
            ifstream file(argv[2]);
            SIT::debug = true;
            SIT::Interpret(file);
        } else {
            ifstream file(argv[1]);
            SIT::Interpret(file);
        }
    } else if (s == "") {
        SIT::Interpret(cin);
    } else {
        stringstream ss(s);
        SIT::Interpret(ss);
    }
}