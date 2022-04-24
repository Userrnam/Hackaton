#include <iostream>

#include "Graph.hpp"


struct Container {
    std::vector<int> nodes;
    std::vector<Connection> connections;
    int weight = 0;
};

struct ComposerParams {
    std::vector<int> container_sizes;
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
    os << "[ ";
    for (const auto& e : v) {
        os << e << " ";
    }
    os << "]";
    return os;
}

int cost(const std::vector<Container>& containers);

std::vector<Container> compose(Graph *graph, ComposerParams *params);
