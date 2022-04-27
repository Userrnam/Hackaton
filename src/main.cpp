#include <iostream>
#include <stdio.h>
#include <time.h>

#include "Graph.hpp"
#include "Layout.hpp"

/*
Для размерности 20 размеры контейнеров: 3,4,5,7
Для размерности 50 размеры контейнеров: 10,13,15,17
Для размерности 250 размеры контейнеров: 30,45,55,70,80
Для размерности 1000 размеры контейнеров: 100,125,175,225
Для размерности 10000 размеры контейнеров: 1000,2000,2500,3000

Для размерности 101 размеры контейнеров: 3,4,5,7,11
Для размерности 777 размеры контейнеров: 7,17,27,37,47,57,67,77
*/

int main() {
    Graph graph;

    graph.load("/Users/antonkondratuk/Desktop/uni/Berchun/hackaton3/test10000.txt");

    auto start = time(NULL);

    ComposerParams params;
    params.container_sizes = {
        1000,2000,2500,3000
    };
    auto containers = compose(&graph, &params);
    std::cout << "Time: " << time(NULL) - start << std::endl;
    std::cout << "Container sizes: [ ";
    for (int i = 0; i < containers.size(); ++i) {
        std::cout << containers[i].nodes.size() << " ";
        // printf("Container %3d: [ ", i);
        // for (auto& node : containers[i].nodes) {
        //     printf("%3d ", node);
        // }
        // printf("];\n");
    }
    std::cout << "];\n";

    // show value of cost function
    std::cout << "Composer Cost: " << cost(containers) << std::endl;

    auto layout = create_layout(containers);

    std::cout << "Time: " << time(NULL) - start << std::endl;
    std::cout << "Optimal container layout:\n";
    layout.print();

    std::cout << "Layout Cost: " << cost(layout, containers) << std::endl;
}
