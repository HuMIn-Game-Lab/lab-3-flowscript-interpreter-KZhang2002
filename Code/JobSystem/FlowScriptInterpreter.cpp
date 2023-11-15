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
	if(e_Flag) return;

	printMap();

	Execute();
	if(e_Flag) return;
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
	unordered_map<string, bool> triedJobs; // <node name, job succeeded?>

	// run all jobs with no dependencies
	for (auto it = existingJobs.begin(); it != existingJobs.end(); ++it) {
		if (dependencyMap[*it].empty()) {
			ifstream jobInput("../Data/jobJSONs/" + *it + ".json");
			if (jobInput.good()) {
				js -> CreateAndQueueJob(JobSystem::ReadFile(jobInput));
				triedJobs.insert(pair<string, bool> (*it, false));
			}
			else cout << "File read error" << endl;
			jobInput.close();
		}
	}

	// check untried jobs for dependency fulfillment
	while (triedJobs.size() != existingJobs.size()) {
		for (auto it = existingJobs.begin(); it != existingJobs.end(); ++it) {
			// check if job has been tried before
			if (triedJobs.find(*it) == triedJobs.end()) {
				bool dependenciesFulfilled = false;
				bool conditionFailed = false;
				auto depMapIt = dependencyMap[*it].begin();
				for (const auto& dep : dependencyMap[*it]) {
					if (triedJobs.find(dep.first) == triedJobs.end()) break; //if dependency hasn't been tried yet, break
					auto jobStatus = js->GetJobStatusByName(dep.first);

					// checks if dependencies are fulfilled
					// if conditional exists and conditional does not match callback, break
					if (!jobStatus.first || (!dep.second.empty() && (dep.second != jobStatus.second))) {
						conditionFailed = true;
						triedJobs.insert(pair<string, bool> (*it, false));
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
						triedJobs.insert(pair<string, bool> (*it, true));
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
			token error = token(INVALID, (string &) tokenStr, currentRow, currentToken);
			e_HandleError(error, 1); // Lexical error
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
	string path = "../Data/logs/" + graphName + "log.txt";

	cout << "ERROR CODE " << errorCode << endl;
	cout << "Error desc: " << errorMap[errorCode] << endl;
	cout << "Error occurred on line: " << token.lineIndex + 1 << ", at token: " << token.tokenIndex + 1 << endl;
	cout << "Token is of type: " << tokenTypeNames[token.type] << ", with value: " << token.value << endl;
	cout << "Printing line:" << endl;
	cout << rawCodeText[token.lineIndex] << endl;
	cout << "This error text can be found in " << path << endl;
	cout << "Check the documentation for further information about syntax and fixing errors." << endl;

	ofstream output(path);
	if (!output.is_open()) {
		cerr << "Failed to open the file for writing." << endl;
		return;  // or handle the error appropriately
	}
	output << "ERROR CODE " << errorCode << endl;
	output << "Error desc: " << errorMap[errorCode] << endl;
	output << "Error occurred on line: " << token.lineIndex + 1 << ", at token: " << token.tokenIndex + 1 << endl;
	output << "Token is of type: " << tokenTypeNames[token.type] << ", with value: " << token.value << endl;
	output << "Printing line where error occurs:" << endl << endl;
	output << rawCodeText[token.lineIndex] << endl << endl;
	output << "Check the documentation for further information about syntax and fixing errors." << endl;
	output.close();
}

void FlowScriptInterpreter::e_HandleError(int lineNum, int errorCode) {
	e_Flag = true;
	string path = "../Data/logs/" + graphName + "log.txt";

	cout << "ERROR CODE " << errorCode << endl;
	cout << "Error desc: " << errorMap[errorCode] << endl;
	cout << "Error occurred on line: " << lineNum + 1 << endl;
	cout << rawCodeText[lineNum] << endl;
	cout << "This error text can be found in ../Data/logs/log.txt" << endl;

	ofstream output(path);
	if (!output.is_open()) {
		cerr << "Failed to open the file for writing." << endl;
		return;  // or handle the error appropriately
	}
	output << "ERROR CODE " << errorCode << endl;
	output << "Error desc: " << errorMap[errorCode] << endl;
	output << "Error occurred on line: " << lineNum + 1 << endl;
	output << rawCodeText[lineNum] << endl;
	output << "Check the documentation for further information about the error code." << endl;
	output.close();
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
					e_HandleError(curToken, 2); return;
				}
				i_braceFlag = true;
				s_ParseGraphDefinition(command, i_curLine);
				break;
			case RBRACE:
				if (!i_braceFlag) {
					e_HandleError(curToken, 3); return;
				}
				i_braceFlag = false;
				break;
			case SEMICOLON:
				if (!i_braceFlag) {
					e_HandleError(curToken, 4); return;
				}
				if (command.size() > 1) s_ParseNodeStatement(command);
				break;
			case EMPTY:
				break;
			default:
				//cout << "Impossible: " << curToken << " Line num: " <<  endl;
				e_HandleError(curToken, 22); return;
		}

		if(e_Flag) {
			//e_HandleError(i_curToken, i_curLine, 1);
			return;
		}

		curToken = i_GetNext();

		cout << endl;
	}

	if (i_braceFlag) {
		e_HandleError(curToken, 5); return;
	}
}

void FlowScriptInterpreter::s_ParseGraphDefinition(vector<token> command, int lineNum) {
	cout << "Handling graph definition" << endl;

	// Error guard measuring command size
	if (command.size() != 2) {
		e_HandleError(lineNum, 6); return; //
		return;
	}

	int curIndex = 0;
	token curToken = command[curIndex];

	if (curToken.type != WORD) {
		e_HandleError(curToken, 7); // Graph definition error
		return;
	}

	if (!graphType.find(curToken.type)) {
		e_HandleError(curToken, 21); // Graph definition error
		return;
	}

	graphType = curToken.value; cout << "graph type = " << graphType << endl;

	curIndex++;
	curToken = command[curIndex];

	if (curToken.type != WORD) {
		e_HandleError(curToken, 8); // Graph definition error
		return;
	}

	graphName = curToken.value; cout << "graph name = " << graphName << endl;
}

void FlowScriptInterpreter::s_ParseNodeStatement(vector<token> command) {
	int index = 0;
	auto commandIter = command.begin();

	cout << "Handling node statement" << endl;

	// Possible stopping point
	if ((*commandIter).type != WORD) e_HandleError(*commandIter, 9);
	if (e_Flag) return;
	string startNode = (*commandIter).value;
	existingJobs.insert(startNode);
	commandIter++; index++;
	if (commandIter == command.end()) {
		dependencyMap[startNode]; // Add node to map
		cout << "Added " << startNode << endl;
		return;
	}

	if ((*commandIter).type != ARROW) e_HandleError(*commandIter, 10);
	if (e_Flag) return;
	commandIter++; index++;

	// Possible stopping point
	if ((*commandIter).type != WORD) e_HandleError(*commandIter, 11);
	if (e_Flag) return;
	string endNode = (*commandIter).value;
	existingJobs.insert(endNode);
	commandIter++; index++;
	if (commandIter == command.end()) {
		pair<string, string> startNodePair{startNode, ""};
		dependencyMap[endNode].push_back(startNodePair); // Add node to map
		cout << "Added " << startNode << " -> " << endNode << endl;
		return;
	}

	if ((*commandIter).type != LBRACKET) e_HandleError(*commandIter, 12);
	if (e_Flag) return;
	commandIter++; index++;

	if ((*commandIter).type != WORD) e_HandleError(*commandIter, 13);
	if ((*commandIter).value != "condition") e_HandleError(*commandIter, 14);
	if (e_Flag) return;
	commandIter++; index++;

	if ((*commandIter).type != EQUALS) e_HandleError(*commandIter, 15);
	if (e_Flag) return;
	commandIter++; index++;

	if ((*commandIter).type != QUOTE) e_HandleError(*commandIter, 16);
	if (e_Flag) return;
	commandIter++; index++;

	if ((*commandIter).type != WORD) e_HandleError(*commandIter, 17);
	if (e_Flag) return;
	string condition = (*commandIter).value;
	commandIter++; index++;

	if ((*commandIter).type != QUOTE) e_HandleError(*commandIter, 18);
	if (e_Flag) return;
	commandIter++; index++;

	if ((*commandIter).type != RBRACKET) e_HandleError(*commandIter, 19);
	if (e_Flag) return;
	commandIter++; index++;

	pair<string, string> startNodePair{startNode, condition};
	dependencyMap[endNode].push_back(startNodePair); // Add node to map
	cout << "Added " << startNode << " -> " << endNode << " | Condition: " << condition << endl;

	if (commandIter != command.end()) e_HandleError(*commandIter, 20);
	if (e_Flag) return;
}








