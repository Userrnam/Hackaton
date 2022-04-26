#pragma once

#include "Composer.hpp"

struct Coord {
    int x = 0;
    int y = 0;
};

struct Board {
    int width = 0;
    int height = 0;
};

struct Layout {
    // uses container indices
    // -1 => place is empty
    std::vector<std::vector<int>> blocks;
    std::vector<Coord> container_coords;

    void print() {
        for (int x = 0; x < blocks.size(); ++x) {
            for (int y = 0; y < blocks[x].size(); ++y) {
                if (blocks[x][y] == -1) {
                    printf("    ");
                } else {
                    printf("%3d ", blocks[x][y]);
                }
            }
            printf("\n");
        }
    }
};

Layout create_layout(const std::vector<Container>& containers);
float cost(const Layout& layout, const std::vector<Container>& containers);
