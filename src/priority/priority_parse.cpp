#include "dagSched/priority_parse.h"


void readPrioritiesFromYAML(const std::string& file_path, std::vector<int>& priorities) {
    YAML::Node config   = YAML::LoadFile(file_path);
    YAML::Node priorities_node = config["priorities"];

    for(int i=0; i<priorities_node.size(); i++) {
        int id = priorities_node["id"] ? priorities_node["id"].as<int>() : 0;
        int priority = priorities_node[priority] ? priorities_node[priority].as<int>() : 0;

        priorities[id] = priority;
    }
}