#include "CompileJob.h"
#include <array>
#include <iostream>
#include <ostream>

//needed for parse job
#include <regex>
#include "json.hpp"
#include <fstream>

CompileJob::CompileJob(json input) : Job(input) {
    if (input.find("input") == input.end()) {
        cout << "No inputs given. Aborting job." << endl;
        return;
    }

    if (input["input"].find("makefileContents") != input["input"].end()) {
        makefileContents = input["input"].at("makefileContents");
        cout << "Makefile contents set as: " << makefileContents << endl;
    } else {
        cout << "No makefile commands given. Aborting job." << endl;
        return;
    }

    if (input["input"].find("outputFileName") != input["input"].end()) {
        outputFileName = input["input"].at("outputFileName");
        cout << "Output file name set as: " << outputFileName << endl;
    } else {
        outputFileName = "output";
        cout << "No output file name given. Output will have the default name \"output\"." << endl;
    }
}

void CompileJob::Execute() {
    cout << "Executing compile job" << endl;
    //cout << "Running compile" << endl;
    Compile();
    //cout << "Parsing errors from compile" << endl;
    ParseError();
    //cout << "Outputting error to JSON" << endl;
    OutputToJSON();

    std::cout << "Job: " << this->GetUniqueID() << " has been executed"
              << std::endl;
}

void CompileJob::JobCompleteCallback() {
    std::cout << "Compile Job " << this->GetUniqueID() << " | Return code " << this->returnCode << " | Output: ";
    if (this->output.empty()) {
        cout << "No output. Code compilation successful." << endl;
    } else {
        std::cout << "\n\n" << this->output << std::endl;
    }
}

void CompileJob::Compile() {
    std::array<char, 128> buffer;
    std::string command = makefileContents;
    //command += this->makefileContents;

    // Redirect err to cout
    command.append(" 2>&1");

    //cout << "Running command: " << command << endl;

    // open pipe and run command
    FILE *pipe = popen(command.c_str(), "r");
    if(!pipe) {
        std::cout << "popen Failed: Error: failed to open pipe" << std::endl;
        return;
    }

    // read till end of process:
    while(fgets(buffer.data(), 128, pipe) != nullptr) {
        this->output.append(buffer.data());
    }

    // close pipe and get return code
    this->returnCode = pclose(pipe);

    std::cout << "Compiling for job " << this->GetUniqueID() << " has been completed" << std::endl;
}

void CompileJob::ParseError() {
    regex error_pattern(R"(([^:\n]+):(\d+):(\d+): (error|note|warning): (.+))");

    vector<std::map<std::string, std::string>> errors;
    map<std::string, std::vector<std::map<std::string, std::string>>> errors_by_file;

    // Use std::sregex_iterator to iterate over matches in the error messages
    sregex_iterator iter(output.begin(), output.end(), error_pattern);
    sregex_iterator end;

    for (; iter != end; ++iter) {
        std::smatch match = *iter;

        // Extract matches
        string file_path = match[1];
        string line_number = match[2];
        string column_number = match[3];
        string error = match[4];
        string description = match[5];

        // Create and fill error object
        map<string, string> error_data;
        error_data["description"] = description;
        error_data["file_path"] = file_path;
        error_data["line_number"] = line_number;
        error_data["column_number"] = column_number;
        error_data["error_type"] = error;

        // Store error data in a vector
        errors.push_back(error_data);

        // Group errors by file path
        errors_by_file[file_path].push_back(error_data);
    }

    nlohmann::json jsonOutput;

    // Convert error data to JSON using nlohmann/json
    for (const auto &file_errors : errors_by_file) {
        nlohmann::json json_errors;
        for (const auto &error : file_errors.second) {
            nlohmann::json json_error;
            if (error.find("description") != error.end()) {
                json_error["description"] = error.at("description");
            }
            if (error.find("line_number") != error.end()) {
                json_error["line_number"] = error.at("line_number");
            }
            if (error.find("column_number") != error.end()) {
                json_error["column_number"] = error.at("column_number");
            }
            if (error.find("error_type") != error.end()) {
                json_error["error_type"] = error.at("error_type");
            }
            if (error.find("code_snippet") != error.end()) {
                json_error["code_snippet"] = error.at("code_snippet");
            }

            ifstream sourceFileStream(file_errors.first);
            nlohmann::json contextLines = nlohmann::json::array();
            string sourceLine;
            int currentLineNumber = 1;
            int targetLineNumber = stoi(error.at("line_number"));

            while (getline(sourceFileStream, sourceLine)) {
                if (currentLineNumber > (targetLineNumber + 2)) {
                    break;
                }

                if (currentLineNumber >= (targetLineNumber - 2)) {
                    contextLines.push_back(sourceLine);
                }

                currentLineNumber++;
            }

            json_error["code_snippet"] = contextLines;

            json_errors.push_back(json_error);
        }
        jsonOutput[file_errors.first] = json_errors;
    }

    compilationErrors = jsonOutput;
}

void CompileJob::OutputToJSON() {
    // Serialize JSON data to a string
    string json_str = compilationErrors.dump(4); // Pretty-print with 4 spaces

    if (json_str == "null") {
        json_str = "Files compiled successfuly. No errors found.";
    }
    // Write JSON data to a file
    string outputDirectory = "../Data/output/";
    string fileName = outputFileName + ".json";
    string outputFile = outputDirectory + fileName; // Specify the relative path to the output file
    ofstream json_file(outputFile);
    json_file << json_str;
    json_file.close();

    std::cout << "g++ error and warning messages converted to JSON and saved to \"" << outputDirectory << "\"." << std::endl;
}
