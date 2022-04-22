#include "Layout.hpp"

#include <iostream>
#include <assert.h>


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
    int placed_count = 0;
    placed.resize(containers.size(), 0);

    placed[0] = 1;
    layout.blocks[0][0] = 0;
    layout.container_coords[0] = { 0, 0 };

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

float relative_length(Layout& layout, const Container& container, int cx, int cy) {
    float l = 0;
    for (auto conn : container.connections) {
        int dx = cx - layout.container_coords[conn.index].x;
        int dy = cy - layout.container_coords[conn.index].y;

        l += conn.weight * (absi(dx) + absi(dy));
    }
    return l / container.weight;
}

float dL(Layout& layout, const std::vector<Container>& containers, int v1, int v2, const std::vector<float>& distances) {
    auto coords1 = layout.container_coords[v1];
    auto coords2 = layout.container_coords[v2];
    auto l1 = relative_length(layout, containers[v1], coords2.x, coords2.y);

    // move to new location
    if (v2 == -1) {
        return distances[v1] - l1;
    }

    auto l2 = relative_length(layout, containers[v2], coords1.x, coords1.y);
    return distances[v1] + distances[v2] - (l1 + l2);
}

bool iterate(Layout& layout, const std::vector<Container>& containers) {
    std::vector<float> distances;
    distances.resize(containers.size(), 0);
    for (int i = 0; i < containers.size(); ++i) {
        distances[i] = relative_length(layout, containers[i], layout.container_coords[i].x, layout.container_coords[i].y);
    }

    float container_index = max_index(distances);

    // calculate center of mass
    int xc = 0;
    int yc = 0;
    for (auto conn : containers[container_index].connections) {
        xc += conn.weight * (layout.container_coords[container_index].x - layout.container_coords[conn.index].x);
        yc += conn.weight * (layout.container_coords[container_index].y - layout.container_coords[conn.index].y);
    }
    xc /= containers[container_index].weight;
    yc /= containers[container_index].weight;

    // center of mass in global coordinate system
    xc += layout.container_coords[container_index].x;
    yc += layout.container_coords[container_index].y;

    Coord target;
    float dL_max = INT_MIN;

    // [xc, yc]
    {
        float t = dL(layout, containers, container_index, layout.blocks[xc][yc], distances);
        if (t > dL_max) {
            dL_max = t;
            target.x = xc;
            target.y = yc;
        }
    }

    // [xc+1, yc]
    if (xc+1 < layout.blocks.size()) {
        float t = dL(layout, containers, container_index, layout.blocks[xc+1][yc], distances);
        if (t > dL_max) {
            dL_max = t;
            target.x = xc+1;
            target.y = yc;
        }
    }

    // [xc, yc+1]
    if (yc+1 < layout.blocks[0].size()) {
        float t = dL(layout, containers, container_index, layout.blocks[xc][yc+1], distances);
        if (t > dL_max) {
            dL_max = t;
            target.x = xc;
            target.y = yc+1;
        }
    }

    // [xc+1], yc+1]
    if (xc+1 < layout.blocks.size() && yc+1 < layout.blocks[0].size()) {
        float t = dL(layout, containers, container_index, layout.blocks[xc+1][yc+1], distances);
        if (t > dL_max) {
            dL_max = t;
            target.x = xc+1;
            target.y = yc+1;
        }
    }

    if (dL_max > 0) {
        auto coord1 = layout.container_coords[container_index];
        int id = layout.blocks[target.x][target.y];

        layout.blocks[coord1.x][coord1.y] = layout.blocks[target.x][target.y];
        layout.blocks[target.x][target.y] = container_index;

        if (id != -1) {
            layout.container_coords[id] = coord1;
        }
        layout.container_coords[container_index] = target;

        return true;
    }

    return false;
}

Layout create_layout(const std::vector<Container>& containers, const Board& board) {
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

    sequential_stage(layout, containers);

    while (iterate(layout, containers)) {  }

    return layout;
}
