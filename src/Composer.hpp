#include "Graph.hpp"


struct Container {
    std::vector<int> nodes;
    std::vector<Connection> connections;
    int weight = 0;
};

struct ComposerParams {
    std::vector<int> container_sizes;
};

std::vector<Container> compose(Graph *graph, ComposerParams *params);
