#pragma once

#include <vector>
#include <string>

struct Connection {
    int index;
    int weight;
};

struct Node {
    std::vector<Connection> connections;
};

struct Graph {
    std::vector<Node> nodes;

    void load(const std::string& path);
};

