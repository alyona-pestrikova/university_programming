// Interpretator.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <algorithm>
using namespace std;

class Expression {
public:
    virtual Expression* eval() {
        return this;
    }
    virtual int get_value() {
        throw "ERROR";
    }
    virtual Expression* get_body() {
        throw "ERROR";
    }
    virtual string get_id() {
        throw "ERROR";
    }
    virtual string get_str() {
        return "";
    }
    virtual list <Expression*> get_list() {
        throw "ERROR";
    }
    virtual bool is_array() {
        return false;
    }
    virtual ~Expression() {}
}; 

class Env {
private:
    unordered_map<string, Expression*> env;
public:
    Expression* from_env(const std::string& id) {
        auto it = env.begin();
        it = env.find(id);
        if (it == env.end()) throw "ERROR";
        return it->second;
    }
    void add(std::string id, Expression* e) {
        env[id] = e;
    }
};

Env env;

class Val : public Expression {
private:
    int value;
public:
    Val(int val_) : value(val_) {}
    Expression* eval() {
        return this;
    }
    int get_value() {
        return value;
    }
    string get_str() {
        return "(val " + to_string(value) + ")";
    }
};
class Var : public Expression {
private:
    string id;
public:
    Var(string id_) : id(id_) {}
    Expression* eval() {
        return env.from_env(id);
    }
    string get_id() {
        return id;
    }
    string get_str() {
        return "(var " + id + ")";
    }
};
class Add : public Expression {
private:
    Expression* sum1;
    Expression* sum2;
public:
    Add(Expression* ex1_, Expression* ex2_) : sum1(ex1_), sum2(ex2_) {}
    Expression* eval() {
        Expression* v = new Val(sum1->eval()->get_value() + sum2->eval()->get_value());
        return v;
    }
    string get_str() {
        return "(add " + sum1->get_str() + " " + sum2->get_str() + ")";
    }
    ~Add() {
        delete sum1;
        delete sum2;
    }
};
class If : public Expression {
private:
    Expression* ex1;
    Expression* ex2;
    Expression* if_ex;
    Expression* else_ex;
public:
    If(Expression* ex1_, Expression* ex2_, Expression* if_ex_, Expression* else_ex_) {
        ex1 = ex1_;
        ex2 = ex2_;
        if_ex = if_ex_;
        else_ex = else_ex_;
    }
    Expression* eval() {
        if (ex1->eval()->get_value() > ex2->eval()->get_value()) {
            Expression* e = if_ex->eval();
            return e;
        }
        Expression* e = else_ex->eval();
        return e;
    }
    string get_str() {
        return "(if " + ex1->get_str() + " " + ex2->get_str() + " then " + if_ex->get_str() + " else " + else_ex->get_str() + ")";
    }
    ~If() {
        delete ex1;
        delete ex2;
        delete if_ex;
        delete else_ex;
    }
};
class Let : public Expression {
private:
    string id;
    Expression* ex1;
    Expression* ex2;
public:
    Let(string id_, Expression* ex1_, Expression* ex2_) {
        id = id_;
        ex1 = ex1_;
        ex2 = ex2_;
    }
    Expression* eval() {
        env.add(id, ex1->eval());
        Expression* e = ex2->eval();
        return e;
    }
    string get_str() {
        return "(let " + id + " = " + ex1->get_str() + " in " + ex2->get_str() + ")";
    }
    ~Let() {
        delete ex1;
        delete ex2;
    }
};
class Function : public Expression {
private:
    string id;
    Expression* ex;
public:
    Function(string id_, Expression* ex_) {
        id = id_;
        ex = ex_;
        env.add(id, ex);
    }
    Expression* eval() {
        return this;
    }
    Expression* get_body () {
        return ex;
    }
    string get_id() {
        return id;
    }
    string get_str() {
        return "(function " + id + " " + ex->get_str() + ")";
    }
    ~Function() {
        delete ex;
    }
};
class Call : public Expression {
private:
    Expression* f_ex;
    Expression* arg_ex;
public:
    Call(Expression* ex1_, Expression* ex2_) : f_ex(ex1_), arg_ex(ex2_) {}
    Expression* eval() {
        Expression* fun = f_ex->eval();
        Expression* body = fun->get_body();
        env.add(fun->get_id(), arg_ex->eval());
        Expression* e = body->eval();
        return e;

    }
    string get_str() {
        return "(call " + f_ex->get_str() + " " + arg_ex->get_str() + ")";
    }
    ~Call() {
        delete f_ex;
        delete arg_ex;
    }
};

class Set : public Expression {
private:
    string id;
    Expression* ex;
public:
    Set(string id_, Expression* ex_) : id(id_), ex(ex_) {}
    Expression* eval() {
        env.add(id, ex->eval());
        return this;
    }
    string get_str() {
        return "(set " + id + " " + ex->get_str() + ")";
    }
    ~Set() {
        delete ex;
    }
};

class Block : public Expression {
private:
    list <Expression*> exp;
public:
    Block(list<Expression*> exp_) : exp(exp_) {}
    Expression* eval() {
        Expression* last = nullptr;
        list<Expression*>::iterator i;
        for (i = exp.begin(); i != exp.end(); i++) {
            last = (*i)->eval();
        }
        return last;
    }
    string get_str() {
        return eval()->get_str();
    }
    ~Block() {
        exp.~list<Expression*>();
    }
};

class Arr : public Expression {
private:
    list <Expression*> exp;
public:
    Arr(list<Expression*> exp_) : exp(exp_) {}
    list <Expression*> get_list() {
        return exp;
    }
    Expression* eval() {
        list <Expression*> eval_res;
        list<Expression*>::iterator i;
        for (i = exp.begin(); i != exp.end(); i++) {
            eval_res.push_back((*i)->eval());
        }
        Expression* e = new Arr(eval_res);
        return e;
    }
    string get_str() {
        list<Expression*>::iterator i;
        list <Expression*> res = eval()->get_list();
        string str = "(array";
        for (i = res.begin(); i != res.end(); i++) {
            str = str + " " + (*i)->eval()->get_str();
        }
        str += ")";
        return str;
    }
    bool is_array() {
        return true;
    }
    ~Arr() {
        exp.~list<Expression*>();
    }
};

class Gen : public Expression {
private:
    Expression* ex1;
    Expression* ex2;
public:
    Gen(Expression* ex1_, Expression* ex2_) : ex1(ex1_), ex2(ex2_) {}
    Expression* eval() {
        list <Expression*> lst;
        for (int i = 0; i < ex1->eval()->get_value(); i++) {
            Expression* fun = new Call(ex2, new Val(i + 1));
            lst.push_back(fun->eval());
        }
        Expression* e = new Arr(lst);
        return e;
    }
    string get_str() {
        return eval()->get_str();
    }
    ~Gen() {
        delete ex1;
        delete ex2;
    }
};

class At : public Expression {
private:
    Expression* ex1;
    Expression* ex2;
public:
    At(Expression* ex1_, Expression* ex2_) : ex1(ex1_), ex2(ex2_) {}
    Expression* eval() {
        Expression* arr = ex1->eval();
        if (!arr->is_array()) {
            throw "ERROR";
        }
        list<Expression*> lst = arr->get_list();
        list<Expression*>::iterator it = lst.begin();
        for (int i = 0; i < ex2->eval()->get_value(); i++) {
            if (it == lst.end()) {
                throw "ERROR";
            }
            it++;
        }
        if (it == lst.end()) {
            throw "ERROR";
        }
        return (*it)->eval();
    }
    string get_str() {
        return eval()->get_str();
    }
    ~At() {
        delete ex1;
        delete ex2;
    }
};

string read_id(string& str, int& i) { //считывание id
    string id = "";
    while (str[i] != ' ' && str[i] != ')') {
        id += str[i++];
    }
    return id;
}

string read_expr(string& str, int& i) { //cчитывание выражения с учетом вложенности
    string exp = "(";
    int co = 1;
    while (co) {
        i++;
        if (str[i] == '(') {
            co++;
        }
        else if (str[i] == ')') {
            co--;
        }
        exp += str[i];
    }
    return exp;
}

Expression* create(string str) {
    //считывание названия выражения
    string exp = "";
    int i = 1;
    while (str[i] != ' ' && i < str.length()) {
        exp += str[i++];
    }
    i++;
    //обработка
    if (exp == "let") { //(let K = (val 10) in (add (val 5) (var K)))
        string id = read_id(str, i);
        i += 3;
        string exp1 = read_expr(str, i);
        i += 5;
        Expression* ex1 = create(exp1);
        string exp2 = read_expr(str, i);
        Expression* ex2 = create(exp2);
        Expression* e = new Let(id, ex1, ex2);
        return e;
    }
    else if (exp == "set") {
        string id = read_id(str, i);
        i++;
        string exp = read_expr(str, i);
        Expression* ex = create(exp);
        Expression* e = new Set(id, ex);
        return e;
    }
    else if (exp == "block") {
        list<Expression*> lst;
        int co_out = 1;
        while (co_out) {
            string exp = read_expr(str, i);
            lst.push_back(create(exp));
            i++;
            if (str[i] == ')') {
                co_out--;
            }
            i++;
        }
        Expression* e = new Block(lst);
        return e;
    }
    else if (exp == "arr") {
        list<Expression*> lst;
        int co_out = 1;
        while (co_out) {
            string exp = read_expr(str, i);
            lst.push_back(create(exp));
            i++;
            if (str[i] == ')') {
                co_out--;
            }
            i++;
        }
        Expression* e = new Arr(lst);
        return e;
    }
    else if (exp == "val") { //(val 10)
        string num = "";
        while (str[i] != ')') {
            num += str[i];
            i++;
        }
        Expression* e = new Val(stoi(num));
        return e;
    }
    else if (exp == "var") { //(var K)
        string id = read_id(str, i);
        Expression* e = new Var(id);
        return e;
    }
    else if (exp == "at") {
        string exp1 = read_expr(str, i);
        Expression* ex1 = create(exp1);
        i += 2;
        string exp2 = read_expr(str, i);
        Expression* ex2 = create(exp2);
        Expression* e = new At(ex1, ex2);
        return e;
    }
    else if (exp == "gen") {
        string exp1 = read_expr(str, i);
        Expression* ex1 = create(exp1);
        i += 2;
        string exp2 = read_expr(str, i);
        Expression* ex2 = create(exp2);
        Expression* e = new Gen(ex1, ex2);
        return e;
    }
    else if (exp == "add") { //(add (val 5) (var K))
        string exp1 = read_expr(str, i);
        Expression* ex1 = create(exp1);
        i += 2;
        string exp2 = read_expr(str, i);
        Expression* ex2 = create(exp2);
        Expression* e = new Add(ex1, ex2);
        return e;
    }
    else if (exp == "if") { //(if (var A) (add (var B) (val 3)) then (val 10) else (add (var B) (val 1)))))
        string exp1 = read_expr(str, i);
        Expression* ex1 = create(exp1);
        i += 2;
        string exp2 = read_expr(str, i);
        Expression* ex2 = create(exp2);
        i += 7;
        string exp_if = read_expr(str, i);
        Expression* ex_if = create(exp_if);
        i += 7;
        string exp_else = read_expr(str, i);
        Expression* ex_else = create(exp_else);
        Expression* e = new If(ex1, ex2, ex_if, ex_else);
        return e;
    } else if (exp == "function") { // (function arg (add (var arg) (val 1)))
        string id = read_id(str, i);
        i++;
        string exp1 = read_expr(str, i);
        Expression* ex1 = create(exp1);
        Expression* e = new Function(id, ex1);
        return e;
    }
    else if (exp == "call") { //(call (var F) (var V))
        string exp1 = read_expr(str, i);
        Expression* ex1 = create(exp1);
        i += 2;
        string exp2 = read_expr(str, i);
        Expression* ex2 = create(exp2);
        Expression* e = new Call(ex1, ex2);
        return e;
    }
}

//приведение любой строки к строке вида (let K = (val 10) in (add (val 5) (var K))),
//в которой отсутсвуют лишние пробелы, табы, переносы строк
string build(string str) { 
    string good = "(";
    for (int i = 1; i < str.length() - 1; i++) {
        if (str[i - 1] == '(') {
            while (str[i] == ' ' || str[i] == '\t' || str[i] == '\n') {
                i++;
            }
        }
        if (str[i] == '(') {
            if (i > 0 && good[good.length() - 1] != ' ' && str[i - 1] != '=') {
                good += ' ';
            }
            i++;
            while (str[i] == ' ' || str[i] == '\n' || str[i] == '\t') {
                i++;
            }
            i--;
            int co = 1;
            string sub_good = "(";
            while (co) {
                i++;
                if (str[i] == '(') {
                    co++;
                }
                else if (str[i] == ')') {
                    co--;
                }
                sub_good += str[i];
            }
            good += build(sub_good);
            
            while (str[i + 1] == ' ' || str[i + 1] == '\n' || str[i + 1] == '\t') {
                i++;
            }
            if (str[i + 1] != ')') {
                good += ' ';
            }
        }
        else if (str[i] == '\n' || str[i] == '\t' || str[i] == ' ') {
            while (str[i] == ' ' || str[i] == '\n' || str[i] == '\t') {
                i++;
            }
            if (str[i] != ')') {
                good += ' ';
                
            }
            i--;
        }
        else if (str[i] == '=') {
            if (good[good.length() - 1] != ' ') {
                good += ' ';
            }
            good += str[i];
            if (str[i + 1] != ' ' && str[i + 1] != '\t' && str[i + 1] != '\n') {
                good += ' ';
            }
        }
        else {
            good += str[i];
        }
    }
    good += ')';
    return good;
}

int main()
{
    ifstream fin("input.txt");
    ofstream fout("output.txt");
    try
    {
        if (!fin.is_open()) {
            throw "File opening failed";
        }
        if (fin.peek() == ifstream::traits_type::eof()) {
            throw "File is empty";
        }
        string exp = "";
        string str;
        while (getline(fin, str)) {
            exp += str;
        }
        //убираем лишние пробелы, табы, переносы строк в начале и конце
        string good = "(";
        int i = 0;
        while (exp[i] != '(') {
            i++;
            if (i == exp.length() - 1) {
                throw "Syntax ERROR";
            }
        }
        int co = 1;
        while (co) {
            i++;
            if (exp[i] == '(') {
                co++;
            }
            else if (exp[i] == ')') {
                co--;
            }
            else if (i == exp.length() - 1) {
                throw "Syntax ERROR";
            }
            good += exp[i];
            
        }
        cout << create(build(good))->eval()->get_str();
        fout << create(build(good))->eval()->get_str();
    }
    catch (const char* ex)
    {
        cout << ex;
        fout << ex;
        fin.close();
        fout.close();
    }
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
