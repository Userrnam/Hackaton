#pragma once

#include <vector>
#include <string>

struct Connection {
    int node_index;
    int weight;
};

struct Node {
    std::vector<Connection> connections;
};

struct Graph {
    std::vector<Node> nodes;

    void load(const std::string& path);
};

