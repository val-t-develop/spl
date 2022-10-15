#include "ParserUtils.hpp"

map<string, int> ParserUtils::operatorsPrecedence = {
        {"%", 12},         {"*", 12},  {"/", 12},

        {"-", 11},         {"+", 11},

        {"<<", 10},        {">>", 10}, {">>>", 10},

        {"instanceof", 9}, {">=", 9},  {"<=", 9},   {">", 9},  {"<", 9},

        {"==", 8},         {"!=", 8},

        {"&", 7},

        {"^", 6},

        {"|", 5},

        {"&&", 4},

        {"||", 3},

        {"=", 1},          {"+=", 1},  {"-=", 1},   {"*=", 1}, {"/=", 1},
        {"&=", 1},         {"%=", 1},  {"^=", 1},   {"|=", 1}, {"<<=", 1},
        {">>=", 1},        {">>>=", 1}
    };

int ParserUtils::getBinOpPrecedence(Lexer& lexer) {
    int i = operatorsPrecedence[lexer.getCurrent()->str];
    return i == 0 ? -1 : i;
}

void ParserUtils::skipSemicolons(Lexer &lexer) {
    while(true) {
        if(lexer.getCurrent()->kind == Token::Kind::SEMICOLON) {
            lexer.goForward();
        } else {
            break;
        }
    }
}

bool ParserUtils::isModifier(shared_ptr<Token> token) {
    return token->kind == Token::Kind::PUBLIC ||
           token->kind == Token::Kind::PRIVATE ||
           token->kind == Token::Kind::PROTECTED ||
           token->kind == Token::Kind::STATIC ||
           token->kind == Token::Kind::FINAL ||
           token->kind == Token::Kind::VOLATILE ||
           token->kind == Token::Kind::TRANSIENT ||
           token->kind == Token::Kind::SYNCHRONIZED ||
           token->kind == Token::Kind::ABSTRACT ||
           token->kind == Token::Kind::NATIVE ||
           token->kind == Token::Kind::STRICTFP ||
           token->kind == Token::Kind::CONST;
}

void ParserUtils::skipModifiers(Lexer &lexer) {
    while(true) {
        if(isModifier(lexer.getCurrent())) {
            lexer.goForward();
        } else {
            break;
        }
    }
}

long double ParserUtils::parseDouble(string token) {
    string a = "";
    string b = "";
    bool point = false;
    for(char ch : token) {
        if (ch == '.') {
            point = true;
            continue;
        }
        if (isdigit(ch)) {
            if (point) {
                b.push_back(ch);
            } else {
                a.push_back(ch);
            }
        }
    }
    if (a == "") {
        a = "0";
    }
    int64_t ai = parseLong(a);
    int64_t bi = parseLong(b);
    return ((long double)bi)/(10*b.length()) + ai;
}

int64_t ParserUtils::parseLong(string token) {
    return atoi(token.c_str());
}



ParserUtils::QualifiedName::QualifiedName(Lexer &lexer) {
    while(true) {
        if(lexer.getNext()->kind == Token::Kind::DOT) {
            list.push_back(lexer.getCurrent());
            lexer.goForward();
            lexer.goForward();
        } else {
            break;
        }
    }
    list.push_back(lexer.getCurrent());
    lexer.goForward();
}

string ParserUtils::QualifiedName::getText() {
    string str = "";
    for(shared_ptr<Token> t : list) {
        str.append(t->str).append(".");
    }
    if(!str.empty()) {
        str.pop_back();
    }
    return str;
}

int ParserUtils::QualifiedName::size() {
    return list.size();
}

void ParserUtils::QualifiedName::add(shared_ptr<Token> token) {
    list.push_back(token);
}

shared_ptr<Token> ParserUtils::QualifiedName::get(int index) {
    return list[index];
}

vector<shared_ptr<Token>> ParserUtils::QualifiedName::getList() {
    return list;
}

void ParserUtils::QualifiedName::setList(vector<shared_ptr<Token>> list) {
    this->list = list;
}

vector<string> ParserUtils::QualifiedName::getTextList() {
    vector<string> vec{};

    for (auto t : list) {
        vec.push_back(t->str);
    }
    return vec;
}

