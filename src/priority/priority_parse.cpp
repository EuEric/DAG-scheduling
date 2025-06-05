#include "dagSched/priority_parse.h"

void readPrioritiesFromYAML(const std::string& file_path, std::map<int, int>& priorities) {
    YAML::Node config   = YAML::LoadFile(file_path);
    YAML::Node priorities_node = config["priorities"];

    for(int i=0; i<priorities_node.size(); i++) {
        YAML::Node node = priorities_node[i];
        int id = node["id"] ? node["id"].as<int>() : 0;
        int priority = node["priority"] ? node["priority"].as<int>() : 0;
        priorities[id] = priority;
    }
}