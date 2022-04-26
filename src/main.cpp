#include <iostream>
#include <stdio.h>
#include <time.h>

#include "Graph.hpp"
#include "Layout.hpp"

int main() {
    Graph graph;

    graph.load("/Users/antonkondratuk/Desktop/uni/Berchun/hackaton3/test250.txt");

    auto start = time(NULL);

    ComposerParams params;
    params.container_sizes = {
        30,45,55,70,80
    };
    auto containers = compose(&graph, &params);
    std::cout << "Time: " << time(NULL) - start << std::endl;
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

    std::cout << "Time: " << time(NULL) - start << std::endl;
    std::cout << "Optimal container layout:\n";
    layout.print();
}
