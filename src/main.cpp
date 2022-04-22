#include <iostream>

#include "Graph.hpp"
#include "Layout.hpp"

int main() {
    Graph graph;

    graph.load("test20.txt");

    ComposerParams params;
    auto containers = compose(&graph, &params);

    Board board;
    board.width  = 10;
    board.height = 10;

    auto layout = create_layout(containers, board);
}
