#include <iostream>

#include "Graph.hpp"
#include "Layout.hpp"

int main() {
    Graph graph;

    graph.load("test.txt");

    ComposerParams params;
    params.container_sizes = {
        2, 2, 3
    };
    auto containers = compose(&graph, &params);

    // show value of cost function
    std::cout << "Cost: " << cost(containers) << std::endl;

    Board board;
    board.width  = 3;
    board.height = 3;

    auto layout = create_layout(containers, board);
    std::cout << "Hello\n";
}
