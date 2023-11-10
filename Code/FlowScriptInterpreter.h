//
// Created by Kenny on 11/7/2023.
//

#ifndef LAB_3_FLOWSCRIPT_INTERPRETER_KZHANG2002_FLOWSCRIPTINTERPRETER_H
#define LAB_3_FLOWSCRIPT_INTERPRETER_KZHANG2002_FLOWSCRIPTINTERPRETER_H

#include "JobSystem.h"
#include <fstream>
#include <utility>
#include <regex>

enum tokenType {
    SEMICOLON,
    WORD,
    ARROW,
    LBRACE,
    RBRACE,
    LBRACKET,
    RBRACKET,
    EQUALS,
    QUOTE,
    INVALID
};

static const char* tokenTypeNames[] = {
    "SEMICOLON",
    "WORD",
    "ARROW",
    "LBRACE",
    "RBRACE",
    "LBRACKET",
    "RBRACKET",
    "EQUALS",
    "QUOTE",
    "INVALID"
};

static const map<tokenType, string> regexMap = {
        {SEMICOLON, R"(;)"},
        {ARROW,     R"(->)"},
        {LBRACE,    R"(\{)"},
        {RBRACE,    R"(\})"},
        {LBRACKET,  R"(\[)"},
        {RBRACKET,  R"(\])"},
        {EQUALS,    R"(=)"},
        {QUOTE,     R"(")"},
        {WORD,      R"(\w+)"},
        {INVALID,   R"(.*)"}
};

struct token {
public:
    token() = default;
    token(tokenType type, string& value) : type(type), value(value){}

    tokenType type = INVALID;
    string value;

    friend ostream& operator<<(ostream& os, const token& token);
};

class FlowScriptInterpreter {
public:
    FlowScriptInterpreter() = default;
    FlowScriptInterpreter(const string& relativePath);
    FlowScriptInterpreter(ifstream &input);

    void Interpret(ifstream &input);

    void Lexer(ifstream &input);
    vector<token> LexLine(const string& str);
    static tokenType GetTokenType(string &str);

    void printLexResult();

    //first = node, second = node's dependencies
    unordered_map<string, vector<string>> dependencyMap;

    //first = node name, second = node job type
    unordered_map<string, string> jobMap;

    //first = value, second = tokenType type
    vector<vector<token>> lexResult;

private:
    void HandleError(int errorRow, int errorColumn, string& errorLine);

    bool errorFlag = false;
    int errorRow = 0;
    int errorColumn = 0;
};


#endif //LAB_3_FLOWSCRIPT_INTERPRETER_KZHANG2002_FLOWSCRIPTINTERPRETER_H
