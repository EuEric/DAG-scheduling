#pragma once

#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>

// Function used for reading priorities for a single DAG in YAML format
void readPrioritiesFromYAML(const std::string& file_path, std::map<int, int>& priorities);