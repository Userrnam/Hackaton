#pragma once

#include <vector>
#include <string>

struct Connection {
    int index = 0;
    int weight = 0;
};

struct Node {
    std::vector<Connection> connections;
    int weight = 0;
};

struct Graph {
    std::vector<Node> nodes;

    void load(const std::string& path);
};

