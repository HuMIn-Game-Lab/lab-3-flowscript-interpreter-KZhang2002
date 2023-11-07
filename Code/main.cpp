#include "JobSystem.h"
#include "CustomJob.h"
#include "CustomJobFactory.h"
#include <fstream>

int main (){
    std::cout << "Creating Job System" << std::endl;

    JobSystem* js = JobSystem::CreateOrGet();

    std::cout << "Creating Worker Threads" << std::endl;

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
    js -> DestroyWorkerThread("Thread10");

    js->Stop();

    ifstream cjb("../Data/jobJSONs/compileJobErrorTest.json");
    if (cjb.good()) js->CreateAndQueueJob(JobSystem::ReadFile(cjb));
    else cout << "File read error" << endl;
    cjb.close();

    ifstream cjb2("../Data/jobJSONs/compileJobSuccessTest.json");
    if (cjb2.good()) js->CreateAndQueueJob(JobSystem::ReadFile(cjb2));
    else cout << "File read error" << endl;
    cjb2.close();


    for (int j = 0; j < 10; j++) {
        ifstream rjb("../Data/jobJSONs/renderJob.json");
        if (rjb.good()) js -> CreateAndQueueJob(JobSystem::ReadFile(rjb));
        else cout << "File read error" << endl;
        rjb.close();
    }


    ifstream cmjb("../Data/jobJSONs/customJob.json");
    json customJob;
    if (cmjb.good()) customJob = JobSystem::ReadFile(cmjb);
    else cout << "File read error" << endl;
    JobFactory* cj = new CustomJobFactory();
    js->RegisterJobFactory("custom", cj);
    js->CreateAndQueueJob(customJob);
    cmjb.close();

    ifstream rjb("../Data/jobJSONs/renderJob.json");
    if (rjb.good()) js -> CreateAndQueueJob(JobSystem::ReadFile(rjb));
    else cout << "File read error" << endl;
    rjb.close();

    js->PrintAllJobsStatuses();
    js->Start();

    int running = 1;
    int curJobID = 0;

    while (running) {
        std::string command;
        std::cout << "Enter stop, start, destroy, finish, status, job status, destroyjob, or finishjob\n";
        std::cin >> command;

        if (command == "stop") {
            js -> Stop();
        }
        else if (command == "start") {
            js -> Start();
        }
        else if (command == "destroy") {
            js -> FinishCompletedJobs();
            js -> Destroy();
            running = 0;
        }
        else if (command == "destroyjob") {
            js -> DestroyJob(0);
        }
        else if (command == "jobstatus") {
            js -> PrintJobStatus(0);
        }
        else if (command == "finish"){
            js->FinishCompletedJobs();
            std::cout << "Total jobs completed " << js->totalJobs << std::endl;
        }
        else if (command == "finishjob"){
            std::cout << "Finishing Job " << curJobID << std::endl;
            js->FinishJob(curJobID);
            curJobID++;
        }
        else if(command == "status") {
            js->PrintAllJobsStatuses();
        }
        else {
            std::cout << "Invalid Command" << std::endl;
        }
    }

    return 0;
}


