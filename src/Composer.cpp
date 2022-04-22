#include "Composer.hpp"


void sequential_stage(std::vector<Container>& containers, Graph *graph, std::vector<int> container_sizes) {
    assert(container_sizes.size());

    std::vector<int> nodes_in_container; // index is node, value is container this node is in
    nodes_in_container.resize(graph->nodes.size(), -1);
    int nodes_in_container_count = 0;

    while (nodes_in_container_count != graph->nodes.size()) {
        Container container;

        // chose container
        int min_index  = 0;
        int min_weight = INT_MAX;
        for (int i = 0; i < graph->nodes.size(); ++i) {
            if (nodes_in_container[i] == -1 && graph->nodes[i].weight < min_weight) {
                min_index = i;
                min_weight = graph->nodes[min_index].weight;
            }
        }

        // add nodes
        container.nodes.push_back(min_index);
        nodes_in_container_count++;
        nodes_in_container[min_index] = containers.size();
        for (auto conn : graph->nodes[min_index].connections) {
            if (nodes_in_container[conn.index] == -1) {
                nodes_in_container[conn.index] = containers.size();
                container.nodes.push_back(conn.index);
                nodes_in_container_count++;
            }
        }

        // choose container
        int size_index = container_sizes.size()-1;
        for (int i = 0; i < container_sizes.size(); ++i) {
            if (container_sizes[i] == container.nodes.size()) {
                size_index = i;
            }
        }

        int size = container_sizes[size_index];
        container_sizes.erase(container_sizes.begin() + size_index);

        // add nodes to container
        while (size > container.nodes.size() && nodes_in_container_count < graph->nodes.size()) {
            for (int parent : container.nodes) {
                for (auto conn : graph->nodes[parent].connections) {
                    if (nodes_in_container[conn.index] == -1) {
                        container.nodes.push_back(conn.index);
                        nodes_in_container_count++;
                        nodes_in_container[conn.index] = containers.size();
                    }
                }
            }
        }

        if (container.nodes.size() > size) {
            struct DeltaIndex {
                int delta = 0;
                int index = 0;
            };

            // calculate deltas
            std::vector<DeltaIndex> deltas;
            deltas.resize(container.nodes.size(), {});
            for (int i = 0; i < container.nodes.size(); ++i) {
                deltas[i].index = i;
                for (auto conn : graph->nodes[container.nodes[i]].connections) {
                    if (nodes_in_container[conn.index] == -1) {
                        deltas[i].delta += conn.weight;
                    }
                }
            }

            std::sort(deltas.begin(), deltas.end(), [](DeltaIndex a, DeltaIndex b) {
                return a.delta < b.delta;
            });

            // remove nodes from container
            while (container.nodes.size() == size) {
                nodes_in_container[deltas.back().index] = -1;
                container.nodes.erase(container.nodes.begin() + deltas.back().index);
                nodes_in_container_count--;
                deltas.pop_back();
            }
        }

        containers.push_back(container);
    }

    // find connections
    for (auto& container : containers) {
        container.weight = 0;
        for (int node : container.nodes) {
            // find con
            bool conn_found = false;
            for (auto& conn : container.connections) {
                if (conn.index == nodes_in_container[node]) {
                    conn_found = true;
                    conn.weight += graph->nodes[node].weight;
                    break;
                }
            }
            if (!conn_found) {
                Connection conn;
                conn.index = nodes_in_container[node];
                conn.weight = graph->nodes[node].weight;
                container.connections.push_back(conn);
            }
            container.weight += graph->nodes[node].weight;
        }
    }
}

std::vector<Container> compose_iteration(Graph *graph, std::vector<int> container_sizes, int *total_connections) {
    std::vector<Container> containers;
    sequential_stage(containers, graph, container_sizes);

    return containers;
}

std::vector<Container> compose(Graph *graph, ComposerParams *params) {
    int total_connections;
    auto res = compose_iteration(graph, params->container_sizes, &total_connections);
    return res;
}
