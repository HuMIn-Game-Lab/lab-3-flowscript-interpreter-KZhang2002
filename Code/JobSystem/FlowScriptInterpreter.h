//
// Created by Kenny on 11/7/2023.
//

#ifndef LAB_3_FLOWSCRIPT_INTERPRETER_KZHANG2002_FLOWSCRIPTINTERPRETER_H
#define LAB_3_FLOWSCRIPT_INTERPRETER_KZHANG2002_FLOWSCRIPTINTERPRETER_H

#include "JobSystem.h"
#include <fstream>
#include <utility>
#include <regex>
#include <cctype>
#include <unordered_set>
#include <filesystem>

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
	INVALID,
	EMPTY
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
	"INVALID",
	"EMPTY"
};

static const vector<tokenType> punctuation = {
	SEMICOLON,
	LBRACE,
	RBRACE,
	EMPTY
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

static const vector<string> graphTypes = {
	"digraph",
	"graph"
};

static const unordered_map<int, string> eMessageMap = {
		{1,  R"(;)"},
		{2,  R"(;)"},
		{3,  R"(->)"},
		{4,  R"(\{)"},
		{5,  R"(\})"},
		{6,  R"(\[)"},
		{7,  R"(\])"},
		{8,  R"(=)"},
		{9,  R"(")"},
		{10, R"(\w+)"},
		{11, R"(.*)"}
};

static unordered_map<int, string> errorMap = {
		{1, "Invalid token processed. The only valid tokens are ' { ', ' } ', ' [ ', ' ] ', ' = ', ' “ ', and words. Only words can only contain characters a-z, A-Z, 0-9, and/or underscores."},
		{2, "Duplicate ‘{‘ used. Nested brackets are not allowed."},
		{3, "Right brace has no matching left brace."},
		{4, "Semicolons cannot be used outside of braces."},
		{5, "Left brace has no matching right brace."},
		{6, "Too many/too few arguments for graph definition."},
		{7, "Graph type is not a word."},
		{8, "Graph name is invalid. Names can only contain characters a-z, A-Z, 0-9, and/or underscores."},
		{9, "Node definition does not include a valid name for the source node."},
		{10, "Node definition does not include a valid arrow operator."},
		{11, "Node definition does not include a valid name for the end node."},
		{12, "Node definition has too many tokens."},
		{13, "Node attribute is not a word."},
		{14, "Node attribute is invalid."},
		{15, "Invalid attribute operator."},
		{16, "Invalid attribute assignment value. Value must be enclosed with quotation marks."},
		{17, "Attribute assignment value is not a word."},
		{18, "Attribute assignment value is missing end quotes. Value must be fully enclosed with quotation marks."},
		{19, "Bracket pair is not matched."},
		{20, "Node assignment has too many tokens."},
		{21, "Graph type does not exist."},
		{22, "Statement does not end properly. Use brackets, braces, or semicolons to end statements."}
};


struct token {
public:
	token() = default;
	token(tokenType type) : type(type){}
	token(tokenType type, string& value) : type(type), value(value){}
	token(tokenType type, string& value, int lineIndex, int tokenIndex)
		: type(type), value(value), lineIndex(lineIndex), tokenIndex(tokenIndex){}

	tokenType type = INVALID;
	string value;

	int lineIndex = -1;
	int tokenIndex = -1;

	friend ostream& operator<<(ostream& os, const token& token);
	bool operator==(tokenType other) const;
};

class FlowScriptInterpreter {
public:
	FlowScriptInterpreter() = default;
	FlowScriptInterpreter(const string& relativePath);
	FlowScriptInterpreter(ifstream &input);

	//main driver functions
	void Interpret(ifstream &input);
	bool Execute();
	void RunJobs();

	//l_Lexer functions todo: maybe move to private???
	void l_Lexer(ifstream &input);
	vector<token> l_LexLine(const string& str, int currentRow);
	static tokenType l_GetTokenType(string &str);

	//Syntax parser todo: maybe move to private
	void s_ParseSyntax();
	void s_ParseGraphDefinition(vector<token> command, int lineNum); //todo: rename to something better
	void s_ParseNodeStatement(vector<token> command); //todo: ditto

	//printer functions
	void printLexResult();
	void printMap();
	bool isGood() { return e_Flag; }

	string graphName;
	string graphType;
	vector<string> rawCodeText;
	// <node name (end node), <node dependencies (start nodes), conditions (nullptr if does not exist)>>
	unordered_map<string, vector<pair<string, string>>> dependencyMap;
	unordered_set<string> existingJobs;
	vector<vector<token>> lexResult;

private:
	//for error handling
	void e_HandleError(const token& token, int errorCode);
	void e_HandleError(int lineNum, int errorCode); // for lexical

	bool e_Flag      	= false;
	int e_Row       	= -1;
	int e_Token    		= -1;
	string e_Value;

	//for lex iterator
	void i_ResetIterator();
	bool i_Increment();
	token i_GetCurrent();
	token i_GetNext();

	bool i_braceFlag    = false;
	//bool i_bracketFlag  = false;
	//bool i_quoteFlag    = false;
	bool i_endFlag      = false;

	int  i_curLine      = 0;
	int  i_curToken     = 0;

	JobSystem* js = JobSystem::CreateOrGet();
};


#endif //LAB_3_FLOWSCRIPT_INTERPRETER_KZHANG2002_FLOWSCRIPTINTERPRETER_H
