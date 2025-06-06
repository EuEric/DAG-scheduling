#include "dagSched/tests.h"

// Bounds on Multiprocessing Timing Anomalies, Garham (SIAM Journal on Applied Mathematics 1969)

namespace dagSched{

bool Graham1969(const DAGTask& task, const int m, std::ostream* output_file){

    float bound = task.getLength() + 1. / m * (task.getVolume() - task.getLength());


    // Console output
    std::cout << "Graham 1969 bound: " << bound << '\n';

    // YAML output
    if (output_file) {
        (*output_file) << "    - " << bound << "\n";
    }

    if( bound > task.getDeadline()){
        return false;
    }
    return true;
}

}