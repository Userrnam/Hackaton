#include "Graph.hpp"


struct Container {
    std::vector<int> nodes;
};

struct ComposerParams {

};

std::vector<Container> compose(Graph *graph, ComposerParams *params);
