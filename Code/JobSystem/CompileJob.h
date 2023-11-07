#ifndef LAB_1_MULTITHREADING_KZHANG2002_COMPILEJOB_H
#define LAB_1_MULTITHREADING_KZHANG2002_COMPILEJOB_H

#include "Job.h"
#include <string>
#include <utility>
#include "json.hpp"
using namespace std;

class CompileJob : public Job {
public:
    //To be deprecated. Don't use this. Left in for testing purposes.
//    CompileJob(unsigned long jobChannels, string makefileContents, string outputFileName)
//            : Job(jobChannels, "compile"){
//        this->makefileContents = move(makefileContents);
//        this->outputFileName = move(outputFileName);
//    };

    explicit CompileJob(json input);

    ~CompileJob(){};

    string makefileContents;
    string outputFileName;
    string output;
    int returnCode = 0;

    void Execute();
    void JobCompleteCallback();

private:
    void Compile();
    void ParseError();
    void OutputToJSON();

    string m_jobType = "compile";

    nlohmann::json compilationErrors;
};


#endif //LAB_1_MULTITHREADING_KZHANG2002_COMPILEJOB_H
