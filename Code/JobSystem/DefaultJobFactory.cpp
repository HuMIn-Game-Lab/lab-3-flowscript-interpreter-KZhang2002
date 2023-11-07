//
// Created by Kenny on 10/23/2023.
//

#include "DefaultJobFactory.h"

Job *DefaultJobFactory::CreateJob(const json& params) {
    if (params.find("jobType") == params.end()) {
        cout << "ERROR: No job type given. Aborting job creation." << endl;
        return nullptr;
    }

    string jobType = params["jobType"];
    string jobName = params["jobName"];
    cout << "\nCreating job of type \"" << jobType << "\" with name \"" << jobName << "\"." << endl;

    if (jobType.compare("compile") == 0) {
        return new CompileJob(params);
    } else if (jobType.compare("render") == 0) {
        return new RenderJob(params);
    } else {
        cout << "ERROR: invalid jobType given. Returning nullptr." << endl;
        return nullptr;
    }
}
