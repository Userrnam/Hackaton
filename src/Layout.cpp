#include "Layout.hpp"

#include <iostream>
#include <assert.h>
#include <float.h>


template<typename T>
int max_index(const std::vector<T>& v) {
    int max_ind = 0;
    for (int i = 1; i < v.size(); ++i) {
        if (v[i] > v[max_ind]) {
            max_ind = i;
        }
    }
    return max_ind;
}

int choose_container(const std::vector<int>& placed, const std::vector<Container>& containers) {
    assert(containers.size());

    std::vector<int> values;
    values.resize(containers.size(), INT_MIN);
    for (int i = 0; i < containers.size(); ++i) {
        if (!placed[i]) {
            // calculate K for this node
            values[i] = 0;
            for (auto conn : containers[i].connections) {
                if (placed[conn.index]) {
                    values[i] += conn.weight;
                } else {
                    values[i] -= conn.weight;
                }
            }
        }
    }

    return max_index(values);
}

void sequential_stage(Layout& layout, const std::vector<Container>& containers) {
    assert(containers.size());

    std::vector<int> placed;
    placed.resize(containers.size(), 0);

    placed[0] = 1;
    layout.blocks[0][0] = 0;
    layout.container_coords[0] = { 0, 0 };
    int placed_count = 1;

    int start_col = 1;
    int delta = 1;
    for (int row = 0; row < layout.blocks.size(); ++row) {
        int col = start_col;
        for ( ; col >= 0 && col < layout.blocks[row].size(); col += delta) {
            if (placed_count == containers.size()) {
                return;
            }
            int container_index = choose_container(placed, containers);
            placed_count++;
            placed[container_index] = 1;
            layout.blocks[row][col] = container_index;
            layout.container_coords[container_index] = { row, col };
        }
        if (col < 0) {
            delta = 1;
            start_col = 0;
        } else {
            delta = -1;
            start_col = layout.blocks[row].size()-1;
        }
    }
}

int absi(int a) {
    return a > 0 ? a : -a;
}

float relative_length(const Layout& layout, const Container& container, Coord coord) {
    float l = 0;
    for (auto conn : container.connections) {
        int dx = coord.x - layout.container_coords[conn.index].x;
        int dy = coord.y - layout.container_coords[conn.index].y;

        l += conn.weight * (absi(dx) + absi(dy));
    }
    return l / container.weight;
}

int container_cost(const Layout& layout, const Container& container, Coord coord) {
    int l = 0;
    for (auto conn : container.connections) {
        int dx = coord.x - layout.container_coords[conn.index].x;
        int dy = coord.y - layout.container_coords[conn.index].y;

        l += conn.weight * (absi(dx) + absi(dy));
    }
    return l;
}

float dL(Layout& layout, const std::vector<Container>& containers, Coord coord1, Coord coord2, const std::vector<float>& distances) {
    int v1 = layout.blocks[coord1.x][coord1.y];
    int v2 = layout.blocks[coord2.x][coord2.y];

    // swap containers
    if (v2 != -1) {
        layout.container_coords[v2] = coord1;
    }
    layout.container_coords[v1] = coord2;

    auto l1 = relative_length(layout, containers[v1], coord2);

    // move to new location
    if (v2 == -1) {
        // swap back
        layout.container_coords[v1] = coord1;

        return distances[v1] - l1;
    }

    auto l2 = relative_length(layout, containers[v2], coord1);

    // swap back
    layout.container_coords[v2] = coord2;
    layout.container_coords[v1] = coord1;

    return distances[v1] + distances[v2] - (l1 + l2);
}

bool iterate(Layout& layout, const std::vector<Container>& containers) {
    std::vector<float> distances;
    distances.resize(containers.size(), 0);
    for (int i = 0; i < containers.size(); ++i) {
        distances[i] = relative_length(layout, containers[i], layout.container_coords[i]);
    }

    int container_index = max_index(distances);
    Coord container_coord = layout.container_coords[container_index];

    // calculate center of mass
    int xc = 0;
    int yc = 0;
    for (auto conn : containers[container_index].connections) {
        xc += conn.weight * (layout.container_coords[conn.index].x - container_coord.x);
        yc += conn.weight * (layout.container_coords[conn.index].y - container_coord.y);
    }
    xc /= containers[container_index].weight;
    yc /= containers[container_index].weight;

    // center of mass in global coordinate system
    xc += container_coord.x;
    yc += container_coord.y;

    assert(xc >= 0 && xc < layout.blocks.size());
    assert(yc >= 0 && yc < layout.blocks[0].size());

    Coord target;
    float dL_max = INT_MIN;

    // [xc, yc]
    {
        float t = dL(layout, containers, container_coord, { xc, yc }, distances);
        if (t > dL_max) {
            dL_max = t;
            target.x = xc;
            target.y = yc;
        }
    }

    // [xc+1, yc]
    if (xc+1 < layout.blocks.size()) {
        float t = dL(layout, containers, container_coord, { xc+1, yc }, distances);
        if (t > dL_max) {
            dL_max = t;
            target.x = xc+1;
            target.y = yc;
        }
    }

    // [xc, yc+1]
    if (yc+1 < layout.blocks[0].size()) {
        float t = dL(layout, containers, container_coord, { xc, yc+1 }, distances);
        if (t > dL_max) {
            dL_max = t;
            target.x = xc;
            target.y = yc+1;
        }
    }

    // [xc+1], yc+1]
    if (xc+1 < layout.blocks.size() && yc+1 < layout.blocks[0].size()) {
        float t = dL(layout, containers, container_coord, { xc+1, yc+1 }, distances);
        if (t > dL_max) {
            dL_max = t;
            target.x = xc+1;
            target.y = yc+1;
        }
    }

    if (dL_max > 0) {
        auto coord1 = container_coord;
        int id = layout.blocks[target.x][target.y];

        layout.blocks[coord1.x][coord1.y] = layout.blocks[target.x][target.y];
        layout.blocks[target.x][target.y] = container_index;

        if (id != -1) {
            layout.container_coords[id] = coord1;
        } else {
            if (coord1.x == 0) {
                bool empty = true;
                for (int x = 0; x < layout.blocks.size(); ++x) {
                    if (layout.blocks[x][0] != -1) {
                        empty = false;
                        break;
                    }
                }
                // move all containers left
                if (empty) {
                    for (int x = 0; x < layout.blocks.size(); ++x) {
                        for (int y = 1; y < layout.blocks[0].size(); ++y) {
                            layout.blocks[x][y-1] = layout.blocks[x][y];
                            layout.blocks[x][y] = -1;
                            int id = layout.blocks[x][y-1];
                            if (id != -1) {
                                layout.container_coords[id] = { x, y-1 };
                            }
                        }
                    }
                }
            }
            if (coord1.y == 0) {
                bool empty = true;
                for (int y = 0; y < layout.blocks[0].size(); ++y) {
                    if (layout.blocks[0][y] != -1) {
                        empty = false;
                        break;
                    }
                }
                // move all containers up
                if (empty) {
                    for (int x = 1; x < layout.blocks.size(); ++x) {
                        for (int y = 0; y < layout.blocks[0].size(); ++y) {
                            layout.blocks[x-1][y] = layout.blocks[x][y];
                            layout.blocks[x][y] = -1;
                            int id = layout.blocks[x-1][y];
                            if (id != -1) {
                                layout.container_coords[id] = { x-1, y };
                            }
                        }
                    }
                }
            }
        }
        layout.container_coords[container_index] = target;

        return true;
    }

    return false;
}

Layout layout_iterate(const std::vector<Container>& containers, const Board& board) {
    assert(board.width  > 0);
    assert(board.height > 0);

    if (board.width * board.height < containers.size()) {
        std::cout << "Can't put " << containers.size() << " containers in " << board.width * board.height << " positions!\n";
        return {};
    }

    Layout layout;

    layout.container_coords.resize(containers.size());
    layout.blocks.resize(board.height);
    for (int i = 0; i < layout.blocks.size(); ++i) {
        layout.blocks[i].resize(board.width, -1);
    }

    if (containers.size() == 1) {
        layout.container_coords[0] = { 0, 0 };
        layout.blocks[0][0] = 0;
        return layout;
    }

    sequential_stage(layout, containers);

    while (iterate(layout, containers)) { }

    return layout;
}

Layout create_layout(const std::vector<Container>& containers) {
    Layout best_layout;
    float min_cost = FLT_MAX;

    Board board;
    board.width = containers.size();
    board.height = 1;

    while (board.width > 0) {
        auto layout = layout_iterate(containers, board);
        float layout_cost = cost(layout, containers);
        if (layout_cost < min_cost) {
            min_cost = layout_cost;
            best_layout = layout;
        }
        board.width--;
        board.height++;
    }

    for (int i = best_layout.blocks.size()-1; i >= 0; --i) {
        bool empty = true;
        for (int j = 0; j < best_layout.blocks[i].size(); ++j) {
            if (best_layout.blocks[i][j] != -1) {
                empty = false;
                break;
            }
        }
        if (empty) {
            best_layout.blocks.pop_back();
        } else {
            break;
        }
    }

    return best_layout;
}

int cost(const Layout& layout, const std::vector<Container>& containers) {
    int sum = 0;
    for (int i = 0; i < containers.size(); ++i) {
        sum += container_cost(layout, containers[i], layout.container_coords[i]);
    }
    assert(sum % 2 == 0);
    return sum / 2;
}
