#include "dagSched/tests.h"
#include <iomanip>
#include <iostream>

// Qingqiang He et al. “Intra-task priority assignmentin real-time scheduling of dag tasks on multi-cores”. (IEEE Transactions on Parallel and Distributed Systems 2019)

namespace dagSched{

std::set<int> computeInferenceSet( const DAGTask & task, const int i){
    // definition 3
    std::vector<SubTask *> V = task.getVertices();
    auto ancst = task.getSubTaskAncestors(i);
    auto desc = task.getSubTaskDescendants(i);

    std::set<int> int_set;

    for(int j=0; j<V.size(); ++j){
        if( j != i &&  // not i
            V[j]->prio < V[i]->prio && // greater prio
            std::find(ancst.begin(), ancst.end(), V[j]) == ancst.end() && // not an ancestor
            std::find(desc.begin(), desc.end(), V[j]) == desc.end() ) // not a descendant
            int_set.insert(j);
    }

    return int_set;

}

std::set<int> computePathInferenceSet( const std::set<int>& path, const std::vector<std::set<int>>& int_sets){

    std::set<int> path_int_set;
    for(const auto&p: path){
        for(const auto i: int_sets[p])
            path_int_set.insert(i);
    }
    return path_int_set;
}

float computePathLen(const std::set<int>& path, const std::vector<SubTask *>& V){
    float len = 0;
    for(const auto& p:path){
        len += V[p]->c;
    }
    return len;
}

float computeR(const int i, const std::set<int>& path, const std::vector<SubTask *>& V, const std::vector<std::set<int>>& int_sets, const int m){

    auto path_int = computePathInferenceSet(path, int_sets);
    
    std::set<int> union_int;
    union_int.insert(int_sets[i].begin(), int_sets[i].end());
    union_int.insert(path_int.begin(), path_int.end());

    float vol = 0;
    for(const auto& ui:union_int)
        if(ui != i)
            vol += V[ui]->c;

    float R = computePathLen(path, V) + V[i]->c + 1. / m * vol;
    return R;

}

float computeResponseTimeBound(DAGTask& task, const int m, std::map<int, int>& priorities){
    //algorithm 1

    std::vector<SubTask *> V = task.getVertices();
    std::vector<float> R(V.size());
    std::vector<std::set<int>> paths(V.size());
    std::vector<int> ordIDs = task.getTopologicalOrder();

    if (priorities.empty()) {
        std::cout << "Empty riorities" << " \n";
        // assign based on topological order
        for (int idx = 0, i; idx < ordIDs.size(); ++idx) {
            // Test same priorities
            // V[idx]->prio = 1;
            i = ordIDs[idx]; // vertex index
            V[i]->prio = idx;
        }
    } else {
        // assign from input vector
        for (int i = 0; i < V.size(); ++i) {
            V[i]->prio = priorities[i];
        }
    }

    std::vector<std::set<int>> int_sets(V.size());
    for(int i = 0; i < V.size() ; ++i ){
        int_sets[i] = computeInferenceSet(task, i);
    }

    //compute response time
    for(int idx = 0, i; idx < ordIDs.size() ; ++idx ){
        i = ordIDs[idx]; // vertex index
        
        if(V[i]->pred.size() != 0){
            int max_id = V[i]->pred[0]->id;
            float max_R = R[V[i]->pred[0]->id];
            for(int j=1; j<V[i]->pred.size(); ++j){
                if(R[V[i]->pred[j]->id] > max_R){
                    max_R = R[V[i]->pred[j]->id];
                    max_id = V[i]->pred[j]->id;
                }
            }
            paths[i].insert(paths[max_id].begin(), paths[max_id].end());
        }

        R[i] = computeR(i, paths[i],V, int_sets, m);
        paths[i].insert(i);

    }

    float responseTime = R[ordIDs[ordIDs.size() -1]];

    if(priorities.empty()) {
        std::cout << "Response time with no input priority vector: " << responseTime << "\n";
    } else {
        std::cout << "Response time with priority vector: " << responseTime << "\n";
    }


    return responseTime;
}

bool GP_FP_He2019_C(DAGTask task, const int m, std::map<int, int>& priorities, std::ostream* output_file){
    float responseTimeBound = computeResponseTimeBound(task, m, priorities);
    // YAML output
    if (output_file) {
        (*output_file) << "    - " << responseTimeBound << "\n";
    }
    if (responseTimeBound <= task.getDeadline())
        return true;
    return false;
}

bool GP_FP_FTP_He2019_C(Taskset taskset, const int m){
    std::sort(taskset.tasks.begin(), taskset.tasks.end(), deadlineMonotonicSorting);


    std::vector<float> R_old (taskset.tasks.size(), 0);
    std::vector<float> R (taskset.tasks.size(), 0);
    for(int i=0; i<taskset.tasks.size(); ++i){
        taskset.tasks[i].R = taskset.tasks[i].getLength();
        R_old[i] = taskset.tasks[i].getLength();
    }

    for(int i=0; i<taskset.tasks.size(); ++i){

        if(R_old[i] > taskset.tasks[i].getDeadline())
            return false;

        bool init = true;
        while(!areEqual<float>(R[i], R_old[i]) && R[i] <= taskset.tasks[i].getDeadline()){
            if(!init){
                R_old[i] = R[i];
                R[i] = 0;
            }

            std::map<int, int> empty_priorities;

            R[i] = computeResponseTimeBound(taskset.tasks[i], m, empty_priorities);
            for(int j=0; j<i; ++j)
                R[i] = R[i] + (1. / m) * workloadUpperBound(taskset.tasks[j], R_old[i], m);
                
            init = false;
        }

        if( areEqual<float>(R[i], R_old[i]))
            taskset.tasks[i].R = R[i];
        else if (R[i] > taskset.tasks[i].getDeadline())
            return false;
    }

    return true;

}

}