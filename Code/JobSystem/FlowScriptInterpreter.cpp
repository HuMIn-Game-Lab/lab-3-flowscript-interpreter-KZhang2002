//
// Created by Kenny on 11/7/2023.
//

#include "FlowScriptInterpreter.h"

ostream &operator<<(ostream &os, const token &token) {
    os << "<" << tokenTypeNames[token.type];
    if (token.type == WORD) {
        os << " : " << token.value;
    }
    os  << ">";
    return os;
}

bool token::operator==(tokenType other) const {
    return (type == other);
}


FlowScriptInterpreter::FlowScriptInterpreter(const string& relativePath) {
    ifstream input(relativePath);
    if (input.good()) Interpret(input);
    else cout << "ERROR: Given path could not be found." << endl;
    input.close();
}

void FlowScriptInterpreter::Interpret(ifstream &input) {
    l_Lexer(input);
    if(e_Flag) return;
	cout << endl;
    printLexResult();
	cout << endl;
	s_ParseSyntax();
	cout << endl;
	printMap();

	Execute();
}

bool FlowScriptInterpreter::Execute() {
	if(e_Flag) return false;

	js -> CreateWorkerThread("Thread1", 0xFFFFFFFF);
	js -> CreateWorkerThread("Thread2", 0xFFFFFFFF);
	js -> CreateWorkerThread("Thread3", 0xFFFFFFFF);
	js -> CreateWorkerThread("Thread4", 0xFFFFFFFF);
	js -> CreateWorkerThread("Thread5", 0xFFFFFFFF);
	js -> CreateWorkerThread("Thread6", 0xFFFFFFFF);
	js -> CreateWorkerThread("Thread7", 0xFFFFFFFF);
	js -> CreateWorkerThread("Thread8", 0xFFFFFFFF);
	js -> CreateWorkerThread("Thread9", 0xFFFFFFFF);
	js -> CreateWorkerThread("Thread10", 0xFFFFFFFF);

	RunJobs();

	return true;
}

void FlowScriptInterpreter::RunJobs() {
	unordered_set<string> completedJobs;

	// run all jobs with no dependencies
	for (auto it = existingJobs.begin(); it != existingJobs.end(); ++it) {
		if (dependencyMap[*it].empty()) {
			ifstream jobInput("../Data/jobJSONs/" + *it + ".json");
			if (jobInput.good()) {
				js -> CreateAndQueueJob(JobSystem::ReadFile(jobInput));
				completedJobs.insert(*it);
			}
			else cout << "File read error" << endl;
			jobInput.close();
		}
	}

	while (completedJobs.size() != existingJobs.size()) {
		for (auto it = existingJobs.begin(); it != existingJobs.end(); ++it) {
			if (completedJobs.find(*it) == completedJobs.end()) {
				bool dependenciesFulfilled = false;
				bool conditionFailed = false;
				auto depMapIt = dependencyMap[*it].begin();
				for (auto dep : dependencyMap[*it]) {
					auto res = js->GetJobStatusByName(dep.first);
					if (!res.first) break;
					if (!dep.second.empty() && (dep.second != res.second)) {
						conditionFailed = true;
						completedJobs.insert(*it);
						cout << "Conditions failed for " << *it << endl;
						break;
					}
					if (++depMapIt == dependencyMap[*it].end()) {
						dependenciesFulfilled = true;
					}
				}

				if (dependenciesFulfilled && !conditionFailed) {
					ifstream jobInput("../Data/jobJSONs/" + *it + ".json");
					if (jobInput.good()) {
						js -> CreateAndQueueJob(JobSystem::ReadFile(jobInput));
						completedJobs.insert(*it);
					}
					else cout << "File read error" << endl;
					jobInput.close();
				}
			}
		}
	}
}

void FlowScriptInterpreter::l_Lexer(ifstream &input) {
    int currentRow = 0;
    while(!input.eof() && !e_Flag) {
		string buffer;
        getline(input, buffer);
        cout << "Lexing line: " << buffer << endl;
		rawCodeText.push_back(buffer);
        lexResult.push_back(l_LexLine(buffer, currentRow));
        //cout << endl << endl;

        //cout << "Line read" << endl;

		currentRow++;
    }
}

vector<token> FlowScriptInterpreter::l_LexLine(const string& str, int currentRow) {
    vector<token> res;
    regex delimiter(R"(\s*(->|\w+\b|\W|\n)\s*)");

	int currentToken = 0;

    if (str.empty()) {
        token temp(EMPTY);
        res.push_back(temp);
        return res;
    }

    for (sregex_iterator it(str.begin(), str.end(), delimiter), end; it != end; ++it) {
        string tokenStr = (*it)[1];
        //cout << "Matching token \"" << tokenStr << "\" -> ";
        tokenType type = l_GetTokenType(tokenStr);
        //cout << "<" << tokenTypeNames[type] << "> " << endl;

        if (type == INVALID) {
            cout << endl << endl;
            cout << "INVALID TOKEN \"" + tokenStr + "\"." << endl;
            cout << "LEXER ABORTED." << endl;
            //TODO: ERROR HANDLING
			token error = token(INVALID, (string &) "", currentRow, currentToken);
			e_HandleError(error, 10); // Lexical error todo: error code
            return res;
        }

        //cout << "TOKEN \"" + tokenStr + "\" OF TYPE \"" << tokenTypeNames[type] << "\" READ." << endl;
        res.emplace_back(type, tokenStr, currentRow, currentToken);

		currentToken++;
    }

    return (res);
}

tokenType FlowScriptInterpreter::l_GetTokenType(string& str) {
    //cout << "Tested: ";

    for(const auto& temp : regexMap) {
        //cout << tokenTypeNames[temp.first] <<  " ";
        regex reg(temp.second);
        if (regex_match(str, reg)) return temp.first;
    }
    //cout << endl;

    return INVALID;
}

//todo: move this method to end
void FlowScriptInterpreter::e_HandleError(const token& token, int errorCode) {
	e_Flag = true;
	//todo: write error handler
	cout << "ERROR CODE " << errorCode << endl;
	cout << "Error occurred on line: " << token.lineIndex + 1 << ", at token: " << token.tokenIndex << endl;
	cout << "Token is of type: " << tokenTypeNames[token.type] << ", with value: " << token.value << endl;
	cout << rawCodeText[token.lineIndex] << endl;
}

void FlowScriptInterpreter::e_HandleError(int lineNum, int errorCode) {
	e_Flag = true;
	//todo: write error handler
	cout << "ERROR CODE " << errorCode << endl;
	cout << "Error occurred on line: " << lineNum + 1 << endl;
	cout << rawCodeText[lineNum] << endl;
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

void FlowScriptInterpreter::printMap() {
	for (const auto& entry : dependencyMap) {
		std::cout << entry.first << " -> ";

		const auto& vec = entry.second;
		for (auto it = vec.begin(); it != vec.end(); ++it) {
			std::cout << "(" << it->first << ", " << it->second << ")";
			if (std::next(it) != vec.end()) {
				std::cout << ", ";
			}
		}

		std::cout << std::endl;
	}
}

void FlowScriptInterpreter::i_ResetIterator() {
	i_braceFlag     = false;
	i_bracketFlag   = false;
	i_quoteFlag     = false;
	i_endFlag       = false;
	i_curLine       = 0;
	i_curToken      = 0;
}

token FlowScriptInterpreter::i_GetCurrent() {
	return lexResult[i_curLine][i_curToken];
}

bool FlowScriptInterpreter::i_Increment() {
	if (i_endFlag) return false;

	// Is current token last in row?
	if (i_curToken == lexResult[i_curLine].size() - 1) {
		// Is current row last in file?
		if (i_curLine == lexResult.size() - 1) {
			i_endFlag = true;
			return false;
		}
		i_curToken = 0;
		i_curLine++;
	} else {
		i_curToken++;
	}

	return true;
}

token FlowScriptInterpreter::i_GetNext() {
	i_Increment();
	return i_GetCurrent();
}

void FlowScriptInterpreter::s_ParseSyntax() {
    // reset flags and iterator ints
    i_ResetIterator();
	token curToken = i_GetCurrent();

	while(!i_endFlag) {
		vector<token> command;

		while (!binary_search(punctuation.begin(), punctuation.end(), curToken.type) && !i_endFlag) {
			//cout << curToken << endl;
			command.push_back(curToken);
			curToken = i_GetNext();
		}

		for(auto token : command) {
			cout << token << " ";
		}
		cout << endl;

		//cout << curToken << endl;
		token endSymbol = curToken;

		switch(endSymbol.type) {
			case LBRACE:
				if (i_braceFlag) {
					e_HandleError(curToken, 1); return; //todo: change error code
				}
				i_braceFlag = true;
				s_ParseGraphDefinition(command, i_curLine);
				//todo: handle command
				break;
			case RBRACE:
				if (!i_braceFlag) {
					e_HandleError(curToken, 1); return; //todo: change error code
				}
				i_braceFlag = false;
				break;
			case SEMICOLON:
				if (!i_braceFlag) {
					e_HandleError(curToken, 1); return; //todo: change error code
				}
				if (command.size() > 1) s_ParseNodeStatement(command);
				//todo: handle case where more commands after bracket end
				break;
			default:
				//todo: handle error
				int a = 3;
		}

		if(e_Flag) {
			//e_HandleError(i_curToken, i_curLine, 1); //todo: change error code
			return;
		}

		curToken = i_GetNext();

		cout << endl;
	}
}

void FlowScriptInterpreter::s_ParseGraphDefinition(vector<token> command, int lineNum) {
	cout << "Handling graph definition" << endl;

	// Error guard measuring command size
	if (command.size() != 2) {
		e_HandleError(lineNum, 1); return; //todo: change error code // Too many/too few arguments for graph definition
		return;
	}

	int curIndex = 0;
	token curToken = command[curIndex];

	if (curToken.type != WORD) {
		e_HandleError(curToken, 2); // Graph definition error
		return;
	}

	graphType = curToken.value; cout << "graph type = " << graphType << endl;

	curIndex++;
	curToken = command[curIndex];

	if (curToken.type != WORD) {
		e_HandleError(curToken, 2); // Graph definition error
		return;
	}

	graphName = curToken.value; cout << "graph name = " << graphName << endl;
}

void FlowScriptInterpreter::s_ParseNodeStatement(vector<token> command) {
	int index = 0;
	auto commandIter = command.begin();

	cout << "Handling node statement" << endl;

	// Possible stopping point
	if ((*commandIter).type != WORD) e_HandleError(*commandIter, 2); //todo: change error code
	if (e_Flag) return;
	string startNode = (*commandIter).value;
	existingJobs.insert(startNode);
	commandIter++; index++;
	if (commandIter == command.end()) {
		dependencyMap[startNode]; // Add node to map
		cout << "Added " << startNode << endl;
		return;
	}

	if ((*commandIter).type != ARROW) e_HandleError(*commandIter, 2); //todo: change error code
	if (e_Flag) return;
	commandIter++; index++;

	// Possible stopping point
	if ((*commandIter).type != WORD) e_HandleError(*commandIter, 2); //todo: change error code
	if (e_Flag) return;
	string endNode = (*commandIter).value;
	existingJobs.insert(endNode);
	commandIter++; index++;
	if (commandIter == command.end()) {
		pair<string, string> startNodePair{startNode, ""};
		dependencyMap[endNode].push_back(startNodePair); // Add node to map todo: doesnt check for duplicates
		cout << "Added " << startNode << " -> " << endNode << endl;
		return;
	}

	if ((*commandIter).type != LBRACKET) e_HandleError(*commandIter, 2); //todo: change error code
	if (e_Flag) return;
	commandIter++; index++;

	if ((*commandIter).type != WORD) e_HandleError(*commandIter, 2); //todo: change error code
	if ((*commandIter).value != "condition") e_HandleError(*commandIter, 2); //todo: change error code
	if (e_Flag) return;
	commandIter++; index++;

	if ((*commandIter).type != EQUALS) e_HandleError(*commandIter, 2); //todo: change error code
	if (e_Flag) return;
	commandIter++; index++;

	if ((*commandIter).type != QUOTE) e_HandleError(*commandIter, 2); //todo: change error code
	if (e_Flag) return;
	commandIter++; index++;

	if ((*commandIter).type != WORD) e_HandleError(*commandIter, 2); //todo: change error code
	if (e_Flag) return;
	string condition = (*commandIter).value;
	commandIter++; index++;

	if ((*commandIter).type != QUOTE) e_HandleError(*commandIter, 2); //todo: change error code
	if (e_Flag) return;
	commandIter++; index++;

	if ((*commandIter).type != RBRACKET) e_HandleError(*commandIter, 2); //todo: change error code
	if (e_Flag) return;
	commandIter++; index++;

	pair<string, string> startNodePair{startNode, condition};
	dependencyMap[endNode].push_back(startNodePair); // Add node to map todo: doesnt check for duplicates
	cout << "Added " << startNode << " -> " << endNode << " | Condition: " << condition << endl;

	if (commandIter != command.end()) e_HandleError(*commandIter, 2); //todo: change error code
	if (e_Flag) return;
}








