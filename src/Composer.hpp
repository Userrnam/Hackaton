#include "Graph.hpp"


struct Container {
    std::vector<int> nodes;
    std::vector<Connection> connections;
    int weight;
};

struct ComposerParams {

};

std::vector<Container> compose(Graph *graph, ComposerParams *params);
