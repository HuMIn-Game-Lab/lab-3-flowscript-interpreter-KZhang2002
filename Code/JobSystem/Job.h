#ifndef LAB_1_MULTITHREADING_KZHANG2002_JOB_H
#define LAB_1_MULTITHREADING_KZHANG2002_JOB_H

#pragma once
#include <iostream>
#include <utility>
#include <vector>
#include <array>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>
#include "json.hpp"

using namespace std;
using json = nlohmann::json; //may be bad practice

class Job {
    friend class JobSystem;
    friend class JobWorkerThread;

public:
    //To be deprecated. Don't use this. Left in for testing purposes.
//    explicit Job(unsigned long jobChannels = 0xFFFFFFFF, string jobName = "N/A") :
//    m_jobChannels(jobChannels), m_jobName(move(jobName)) {
//        static int s_nextJobID = 0;
//        m_jobID = s_nextJobID++;
//    }

    explicit Job(json &input) : m_inputJSON(input) {
        static int s_nextJobID = 0;
        m_jobID = s_nextJobID++;

        if (input.find("jobType") == input.end()) {
            cout << "No job type given. Assigning job default channel \"N/A\"." << endl;
        } else {
            //cout << input["jobType"] << endl;
            m_jobType = input["jobType"];
        }

        if (input.find("jobChannels") == input.end()) {
            cout << "No job channels given. Assigning job default channel \"0xFFFFFFFF\"." << endl;
        } else {
            //cout << input["jobChannels"] << endl;
            m_jobChannels = stoul((string)input["jobChannels"], nullptr, 16);
        }

        if (input.find("jobName") == input.end()) {
            cout << "No name given. Assigning job name \"N/A\"." << endl;
        } else {
            string name = input["jobName"];
            if (name.empty()) cout << "Empty name given. Assigning job name \"N/A\"." << endl;
            m_jobName = name;
        }
    };


    virtual ~Job() {}

    virtual void Execute() = 0; //Pure virtual function
    virtual void JobCompleteCallback(){};
    int GetUniqueID() const { return m_jobID; }
    string GetJobType() const { return m_jobType; }

private:

    int m_jobID                 = -1;
    string m_jobType            = "N/A";

    unsigned long m_jobChannels = 0xFFFFFFFF;
    string m_jobName            = "N/A";

    json m_inputJSON;
};

#endif //LAB_1_MULTITHREADING_KZHANG2002_JOB_H
