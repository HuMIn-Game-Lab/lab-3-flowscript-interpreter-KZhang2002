//
// Created by Kenny on 11/7/2023.
//

#include "FlowScriptInterpreter.h"

ostream &operator<<(ostream &os, const token &token) {
    os << "<" << tokenTypeNames[token.type];
    if (token.type == WORD) {
        os << " | " << token.value;
    }
    os  << ">";
    return os;
}

FlowScriptInterpreter::FlowScriptInterpreter(const string& relativePath) {
    ifstream input(relativePath);
    if (input.good()) Interpret(input);
    else cout << "ERROR: Given path could not be found." << endl;
    input.close();
}

void FlowScriptInterpreter::Interpret(ifstream &input) {
    Lexer(input);
    if(errorFlag) return;
    printLexResult();
}

void FlowScriptInterpreter::Lexer(ifstream &input) {
    errorRow = 0;
    while(!input.eof()) {
        string buffer;
        getline(input, buffer);
        cout << "Lexing line: " << buffer << endl;
        lexResult.push_back(LexLine(buffer));
        cout << endl << endl;

        if (errorFlag) {
            HandleError(errorRow, errorColumn, buffer);
            return;
        }

        //cout << "Line read" << endl;

        errorRow++;
    }
}

vector<token> FlowScriptInterpreter::LexLine(const string& str) {
    vector<token> res;
    regex delimiter(R"(\s*(->|\w+\b|\W)\s*)");

    for (sregex_iterator it(str.begin(), str.end(), delimiter), end; it != end; ++it) {
        string tokenStr = (*it)[1];
        //cout << "Matching token \"" << tokenStr << "\"." << endl;
        tokenType type = GetTokenType(tokenStr);

        if (type == INVALID) {
            cout << endl << endl;
            cout << "INVALID TOKEN \"" + tokenStr + "\"." << endl;
            cout << "LEXER ABORTED." << endl;
            //TODO: ERROR HANDLING
            errorFlag = true;
            return res;
        }

        //cout << "TOKEN \"" + tokenStr + "\" OF TYPE \"" << tokenTypeNames[type] << "\" READ." << endl;
        cout << "<" << tokenTypeNames[type] << "> ";
        res.emplace_back(type, tokenStr);
    }

    return (res);
}

tokenType FlowScriptInterpreter::GetTokenType(string& str) {
    //cout << "Tested: ";

    for(const auto& temp : regexMap) {
        //cout << tokenTypeNames[temp.first] <<  " ";
        try {
            regex reg(temp.second);
            if (regex_match(str, reg)) return temp.first;
        } catch (const regex_error& e) {
            cout << endl;
            cerr << "Regex error: " << e.what() << " (code " << e.code() << ")" << endl;
            // Handle the regex error as needed
            return INVALID;
        }
    }
    //cout << endl;

    return INVALID;
}

//todo: move this method to end
void FlowScriptInterpreter::HandleError(int errorRow, int errorColumn, string& errorLine) {
    //todo: format text/json file with errors
}

void FlowScriptInterpreter::printLexResult() {
    int lineNum = 1;
    for (auto line : lexResult) {
        cout << lineNum << ": ";
        for (auto token : line) {
            cout << token << " ";
        }
        cout << endl;
        lineNum++;
    }
}
