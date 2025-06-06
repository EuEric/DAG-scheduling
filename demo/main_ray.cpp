#include <iostream>
#include <filesystem>
#include <vector>
#include <map>
#include "dagSched/DAGTask.h"
#include "dagSched/Taskset.h"
#include "dagSched/tests.h"
#include "dagSched/priority_parse.h"

namespace fs = std::filesystem;

// Function to process a single taskset with its corresponding priority file
void process_taskset(const fs::path& taskset_path, std::map<int, int>& priorities, std::ostream* report = nullptr, int indent = 0) {
    const int n_proc = 4;
    const std::vector<int> typed_proc = {4, 4};
    
    dagSched::Taskset taskset;
    
    if (taskset_path.extension() == ".yaml" || taskset_path.extension() == ".yml") {
        taskset.readTasksetFromYaml(taskset_path.string());
    } else {
        taskset.readTasksetFromDOT(taskset_path.string());
    }

    std::string indent_spaces(indent, ' ');

    std::cout << "\nProcessing taskset: " << taskset_path.filename() << "\n";

    std::cout << "Assignment: " << dagSched::WorstFitProcessorsAssignment(taskset, n_proc) << "\n";

    std::cout << "Taskset utilization: " << taskset.getUtilization() << "\n";
    std::cout << "Taskset Hyper-Period: " << taskset.getHyperPeriod() << "\n";
    std::cout << "Taskset max Density: " << taskset.getMaxDensity() << "\n\n";

    // Report header for each taskset (Ray DAG)
    if (report) {
        (*report) << indent_spaces << "- taskset: " << taskset_path.filename().string() << "\n";
    }

    //single DAG tests
    std::cout<<"Single task tests: \n";
    bool constrained_taskset = true;
    bool implicit_taskset = true;

    for(int i=0; i<taskset.tasks.size();++i){
        std::cout<<"\tTask "<<i<<std::endl;
        //constrained
        // if(taskset.tasks[i].getDeadline() < taskset.tasks[i].getPeriod()){
        //     std::cout<< "\t\tBaruah 2012 constrained (GP-FP-EDF): " <<dagSched::GP_FP_EDF_Baruah2012_C(taskset.tasks[i], n_proc)<<std::endl;
        //     implicit_taskset = false;
        // }

        if(taskset.tasks[i].getDeadline() <= taskset.tasks[i].getPeriod()){
            //std::cout<< "\t\tHan 2019 constrained typed(GP-FP): " <<dagSched::GP_FP_Han2019_C_1(taskset.tasks[i], typed_proc)<<std::endl;
            if (report) {
                (*report) << indent_spaces << "  he_2019:\n";
                (*report) << indent_spaces << "    bounds:\n";
            }
            bool schedulable = dagSched::GP_FP_He2019_C(taskset.tasks[i], n_proc, priorities, report);
            std::cout<< "\t\tHe 2019 constrained typed(GP-FP): " <<schedulable<<std::endl;
            if (report) {
                (*report) << indent_spaces << "    schedulable: " << std::boolalpha << schedulable << "\n";
            }
        }
       // std::cout<< "\t\tBaruah 2012 arbitrary (GP-FP-EDF): "   <<dagSched::GP_FP_EDF_Baruah2012_A(taskset.tasks[i], n_proc)<<std::endl;
       if (report) {
           (*report) << indent_spaces << "  graham_1969:\n";
           (*report) << indent_spaces << "    bounds:\n";
        }
        bool schedulable = dagSched::Graham1969(taskset.tasks[i], n_proc, report);
        std::cout<< "\t\tGraham 1969 : "   <<schedulable<<std::endl;
        if (report) {
            (*report) << indent_spaces << "    schedulable: " << std::boolalpha << schedulable << "\n";
        }

        if(taskset.tasks[i].getDeadline() > taskset.tasks[i].getPeriod()){
            constrained_taskset = false;
            implicit_taskset = false;
        }
        
    }

    //taskset tests
    std::cout<<"Taskset tests: \n";


    //arbitrary
    // std::cout<< "\tBonifaci 2013 arbitrary (GP-FP-EDF): "   <<dagSched::GP_FP_EDF_Bonifaci2013_A(taskset, n_proc)<<std::endl;
    // std::cout<< "\tBonifaci 2013 arbitrary (GP-FP-DM): "   <<dagSched::GP_FP_DM_Bonifaci2013_A(taskset, n_proc)<<std::endl;

    //implicit
    // if(implicit_taskset){
    //     std::cout<< "\tLi 2013 implicit (GP-FP-EDF): "   <<dagSched::GP_FP_EDF_Li2013_I(taskset, n_proc)<<std::endl;
    // }

    //constrained
    if(constrained_taskset){
        //global
        // std::cout<< "\tBonifaci 2013 constrained (GP-FP-DM): "   <<dagSched::GP_FP_DM_Bonifaci2013_C(taskset, n_proc)<<std::endl;
        // std::cout<< "\tQamhieh 2013 constrained (GP-FP-EDF): "   <<dagSched::GP_FP_EDF_Qamhieh2013_C(taskset, n_proc)<<std::endl;
        // std::cout<< "\tBaruah 2014 constrained (GP-FP-EDF): "   <<dagSched::GP_FP_EDF_Baruah2014_C(taskset, n_proc)<<std::endl;
        // std::cout<< "\tMelani 2015 constrained (GP-FP-FTP): "   <<dagSched::GP_FP_FTP_Melani2015_C(taskset, n_proc)<<std::endl;
        // std::cout<< "\tMelani 2015 constrained (GP-FP-EDF): "   <<dagSched::GP_FP_EDF_Melani2015_C(taskset, n_proc)<<std::endl;
        // std::cout<< "\tPathan 2017 constrained (GP-FP-DM): "   <<dagSched::GP_FP_DM_Pathan2017_C(taskset, n_proc)<<std::endl;
        // std::cout<< "\tFonseca 2017 constrained (GP-FP-FTP): "<<dagSched::GP_FP_FTP_Fonseca2017_C(taskset, n_proc)<<std::endl;
        if (report) {
            (*report) << indent_spaces << "  fonseca_2019:\n";
            (*report) << indent_spaces << "    bounds:\n";
        }
        bool schedulable = dagSched::GP_FP_FTP_Fonseca2019(taskset, n_proc, true, report);
        std::cout<< "\tFonseca 2019 constrained (GP-FP-FTP): "<<schedulable<<std::endl;
        if (report) {
            (*report) << indent_spaces << "    schedulable: " << std::boolalpha << schedulable << "\n";
        }
        //std::cout<< "\tHe 2019 constrained (GP-FP-FTP): "<<dagSched::GP_FP_FTP_He2019_C(taskset, n_proc)<<std::endl;

        // limited preemption        
        // std::cout<< "\tSerrano 2016 constrained (GP-LP-FTP): "   <<dagSched::GP_LP_FTP_Serrano16_C(taskset, n_proc)<<std::endl;
        // std::cout<< "\tNasri 2019 constrained (G-LP-FTP): "<<dagSched::G_LP_FTP_Nasri2019_C(taskset, n_proc)<<std::endl;

        //partitioned
        // std::cout<< "\tFonseca 2016 constrained (P-FP-FTP): "<<dagSched::P_FP_FTP_Fonseca2016_C(taskset, n_proc)<<std::endl;
        // std::cout<< "\tCasini 2018 constrained (P-LP-FTP): "<<dagSched::P_LP_FTP_Casini2018_C(taskset, n_proc)<<std::endl;
        // #ifdef ZAHAF2019
        // std::cout<< "\tZahaf 2020 constrained (P-LP-EDF): "<<dagSched::P_LP_EDF_Zahaf2019_C(taskset, n_proc)<<std::endl;
        // #endif
    }

    std::cout << "Finished taskset tests" << "\n";

    //std::cout<< "\tFonseca 2019 arbitrary (GP-FP-FTP): "<<dagSched::GP_FP_FTP_Fonseca2019(taskset, n_proc, false)<<std::endl;
}

// Function to find matching priority file for a taskset file
fs::path find_priority_file(const fs::path& taskset_path, const fs::path& priority_dir) {
    std::string stem = taskset_path.stem().string();

    // Handle different naming patterns
    if (stem.find("dag_") != std::string::npos) {
        stem = "priorities_" + stem.substr(stem.find("dag_"));
    }
    
    for (const auto& entry : fs::directory_iterator(priority_dir)) {
        if (entry.is_regular_file()) {
            std::string priority_stem = entry.path().stem().string();
            if (priority_stem == stem || 
                priority_stem.find(stem) != std::string::npos) {
                return entry.path();
            }
        }
    }
    return {}; // empty path if not found
}

// Function to load priorities based on taskset path and priority path
std::map<int, int> load_priorities(const fs::path& taskset_path, 
                                   const fs::path& priority_path) {
    std::map<int, int> priorities;
    
    if (!priority_path.empty()) {
        if (fs::is_regular_file(priority_path)) {
            // Case: Single priority file provided
            if (priority_path.extension() != taskset_path.extension()) {
                throw std::runtime_error("Taskset and priority files must have same format");
            }
            readPrioritiesFromYAML(priority_path.string(), priorities);
        } else {
            // Case: Priority directory provided - find matching file
            auto matching_priority = find_priority_file(taskset_path, priority_path);
            if (!matching_priority.empty()) {
                std::cout << "matching priority is NOT EMPTY" << "\n";
                readPrioritiesFromYAML(matching_priority.string(), priorities);
            } else {

                std::cout << "matching priority is EMPTY" << "\n";
            }
        }
    }
    
    return priorities;
}

// Function to process all tasksets (single file or directory)
void process_all_tasksets(const fs::path& taskset_path, 
                         const fs::path& priority_path,
                         std::ostream* report = nullptr) {
    if (fs::is_regular_file(taskset_path)) {
        // Single file case
        auto priorities = load_priorities(taskset_path, priority_path);
        process_taskset(taskset_path, priorities, report);
    } 
    else if (fs::is_directory(taskset_path)) {
        // Directory case
        if (!priority_path.empty() && fs::is_regular_file(priority_path)) {
            throw std::runtime_error("Cannot pair directory with single priority file");
        }
        
        for (const auto& entry : fs::recursive_directory_iterator(taskset_path)) {  // <-- CHANGED THIS LINE
            if (entry.is_regular_file() && 
                (entry.path().extension() == ".yaml" || 
                 entry.path().extension() == ".yml")) {
                
                auto priorities = load_priorities(entry.path(), priority_path);
                process_taskset(entry.path(), priorities, report);
            }
        }
    }
}


int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] 
                  << " <taskset_file_or_dir> [priority_file_or_dir]\n"
                  << "Examples:\n"
                  << "  " << argv[0] << " taskset.yaml priorities.yaml\n"
                  << "  " << argv[0] << " tasksets/ priorities/\n"
                  << "  " << argv[0] << " taskset.yaml\n";
        return 1;
    }

    // Parse arguments
    fs::path taskset_path(argv[1]);
    fs::path priority_path;
    
    if (argc > 2) {
        priority_path = argv[2];
    }

    // Validate paths
    if (!fs::exists(taskset_path)) {
        std::cerr << "Taskset path does not exist: " << taskset_path << "\n";
        return 1;
    }

    if (!priority_path.empty() && !fs::exists(priority_path)) {
        std::cerr << "Priority path does not exist: " << priority_path << "\n";
        return 1;
    }

    try {
        std::ofstream report_file("report.yaml");  // Open report.yaml

        // If input is a directory, write the directory name as the root key
        if (fs::is_directory(taskset_path)) {
            report_file << taskset_path.filename().string() << ":\n";
        }

        process_all_tasksets(taskset_path, priority_path, &report_file);
        report_file.close();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }


    return 0;
}