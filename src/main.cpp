#include <iostream>
#include <stdio.h>

#include "Graph.hpp"
#include "Layout.hpp"

int main() {
    Graph graph;

    graph.load("test.txt");

    ComposerParams params;
    params.container_sizes = {
        2, 3, 4
    };
    auto containers = compose(&graph, &params);
    std::cout << "Nodes in containers:\n";
    for (int i = 0; i < containers.size(); ++i) {
        printf("Container %3d: [ ", i);
        for (auto& node : containers[i].nodes) {
            printf("%3d ", node);
        }
        printf("];\n");
    }

    // show value of cost function
    std::cout << "Cost: " << cost(containers) << std::endl;

    Board board;
    board.width  = 3;
    board.height = 3;

    auto layout = create_layout(containers, board);

    std::cout << "Optimal container layout:\n";
    layout.print();
}
