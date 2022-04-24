#include "Composer.hpp"

#include <map>


void sequential_stage(std::vector<Container>& containers, Graph *graph, std::vector<int> container_sizes, std::vector<int>& nodes_in_container) {
    assert(container_sizes.size());
    assert(graph->nodes.size());

    nodes_in_container.resize(graph->nodes.size(), -1);
    int nodes_in_container_count = 0;

    while (nodes_in_container_count != graph->nodes.size()) {
        Container container;

        if (container_sizes.size() == 1) {
            for (int i = 0; i < graph->nodes.size(); ++i) {
                if (nodes_in_container[i] == -1) {
                    container.nodes.push_back(i);
                    nodes_in_container[i] = containers.size();
                }
            }
            containers.push_back(container);
            break;
        }

        // chose node
        int min_index  = 0;
        int min_weight = INT_MAX;
        for (int i = 0; i < graph->nodes.size(); ++i) {
            if (nodes_in_container[i] == -1) {
                int weight = 0;
                for (auto conn : graph->nodes[i].connections) {
                    if (nodes_in_container[conn.index] == -1) {
                        weight += conn.weight;
                    }
                }
                if (weight < min_weight) {
                    min_index = i;
                    min_weight = weight;
                }
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

        // FIXME: use better strategy
        // choose container
        int size_index = container_sizes.size()-1;
        for (int i = 0; i < container_sizes.size(); ++i) {
            if (container_sizes[i] == container.nodes.size()) {
                size_index = i;
                break;
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
                return a.delta > b.delta;
            });

            auto to_remove = std::vector(deltas.begin(), deltas.begin() + container.nodes.size() - size);
            std::sort(to_remove.begin(), to_remove.end(), [](DeltaIndex a, DeltaIndex b) {
                return a.index > b.index;
            });
            for (auto e : to_remove) {
                nodes_in_container[container.nodes[e.index]] = -1;
                container.nodes.erase(container.nodes.begin() + e.index);
                nodes_in_container_count--;
            }
        }

        containers.push_back(container);
    }

}

int external_connections(int m_node, int m_container_index, int e_container_index, Graph *graph, const std::vector<int>& nodes_in_container) {
    assert(nodes_in_container.size());

    int count = 0;
    for (auto conn : graph->nodes[m_node].connections) {
        if (nodes_in_container[conn.index] == m_container_index) {
            count -= conn.weight;
        } else if (nodes_in_container[conn.index] == e_container_index) {
            count += conn.weight;
        }
    }
    return count;
}

void iterative_stage(int mod_container_index, std::vector<Container>& containers, Graph *graph, std::vector<int>& nodes_in_container) {
    assert(nodes_in_container.size());

    auto& m_container = containers[mod_container_index];

    while (true) {
        int max_delta = -1;
        int mod_node_index = 0;
        int ext_node_index  = 0;
        int ext_container_index = 0;

        // find max in R matrix
        for (int m_node_index = 0; m_node_index < m_container.nodes.size(); ++m_node_index) {
            int m_node = m_container.nodes[m_node_index];
            for (int e_container_index = mod_container_index+1; e_container_index < containers.size(); ++e_container_index) {
                int ext1 = external_connections(m_node, mod_container_index, e_container_index, graph, nodes_in_container);
                auto& e_container = containers[e_container_index];
                for (int e_node_index = 0; e_node_index < e_container.nodes.size(); ++e_node_index) {
                    int e_node = e_container.nodes[e_node_index];
                    int ext2 = external_connections(e_node, e_container_index, mod_container_index, graph, nodes_in_container);
                    int same = 0;
                    for (auto conn : graph->nodes[m_node].connections) {
                        if (conn.index == e_node) {
                            same = conn.weight;
                            break;
                        }
                    }

                    int delta = ext1 + ext2 - 2 * same;
                    if (delta > max_delta) {
                        max_delta = delta;
                        mod_node_index = m_node_index;
                        ext_node_index = e_node_index;
                        ext_container_index = e_container_index;
                    }
                }
            }
        }

        // swap nodes
        if (max_delta > 0) {
            auto& e_container = containers[ext_container_index];
            int m_node = m_container.nodes[mod_node_index];
            int e_node = e_container.nodes[ext_node_index];

            // update container nodes
            e_container.nodes[ext_node_index] = m_node;
            m_container.nodes[mod_node_index] = e_node;

            nodes_in_container[e_node] = mod_container_index;
            nodes_in_container[m_node] = ext_container_index;
        } else {
            return;
        }
    }
}

std::vector<Container> compose_iteration(Graph *graph, std::vector<int> container_sizes) {
    std::vector<Container> containers;
    std::vector<int> nodes_in_container; // index is node, value is container this node is in
    sequential_stage(containers, graph, container_sizes, nodes_in_container);

    for (int i = 0; i < containers.size()-1; ++i) {
        iterative_stage(i, containers, graph, nodes_in_container);
    }

    // find connections
    for (int container_index = 0; container_index < containers.size(); ++container_index) {
        auto& container = containers[container_index];
        container.weight = 0;
        for (int node : container.nodes) {
            for (auto node_conn : graph->nodes[node].connections) {
                if (nodes_in_container[node_conn.index] == container_index) {
                    continue;
                }
                // find existing connection
                bool conn_found = false;
                for (auto& conn : container.connections) {
                    if (conn.index == nodes_in_container[node_conn.index]) {
                        conn_found = true;
                        conn.weight += node_conn.weight;
                        break;
                    }
                }
                if (!conn_found) {
                    Connection conn;
                    conn.index = nodes_in_container[node_conn.index];
                    conn.weight = node_conn.weight;
                    container.connections.push_back(conn);
                }
                container.weight += node_conn.weight;
            }
        }
    }

    return containers;
}

struct Iterator {
    std::vector<int> alphabet;
    std::vector<int> last_permutation;
    int max_size = 0;

    bool next(std::vector<int>& container_sizes) {
        container_sizes.clear();

        // this is first permutation
        if (last_permutation.empty()) {
            int size = 0;
            while (size < max_size) {
                size += alphabet[0];
                container_sizes.push_back(alphabet[0]);
                last_permutation.push_back(0);
            }
            return true;
        }

        // next permutation
        bool next_found = false;
        for (int i = last_permutation.size()-1; i >= 0; --i) {
            if (last_permutation[i] != alphabet.size()-1) {
                next_found = true;
                int t = last_permutation[i] + 1;
                for (int k = i; k < last_permutation.size(); ++k) {
                    last_permutation[k] = t;
                }
                break;
            }
        }

        if (!next_found) {
            return false;
        }

        int size = 0;
        for (int i = last_permutation.size()-1; size < max_size; --i) {
            assert(i >= 0);

            int t = alphabet[last_permutation[i]];
            container_sizes.push_back(t);
            size += t;
        }

        return true;
    }
};

std::vector<Container> compose(Graph *graph, ComposerParams *params) {
    int min_cost = INT_MAX;
    std::vector<Container> res;
    std::map<std::vector<int>, bool> seen_previously;

    Iterator it;
    it.alphabet = params->container_sizes;
    it.max_size = graph->nodes.size();

    std::vector<int> container_sizes;
    while (it.next(container_sizes)) {
        // std::cout << container_sizes << std::endl;
        if (seen_previously.find(container_sizes) != seen_previously.end()) {
            break;
        }
        seen_previously[container_sizes] = 1;

        auto t = compose_iteration(graph, container_sizes);
        int t_cost = cost(t);
        if (t_cost < min_cost) {
            min_cost = t_cost;
            res = t;
        }
    }

    return res;
}

int cost(const std::vector<Container>& containers) {
    int c = 0;
    for (int i = 0; i < containers.size(); ++i) {
        c += containers[i].weight;
    }
    assert(!(c & 1));
    return c >> 1;
}
