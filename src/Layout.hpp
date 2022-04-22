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
};

Layout create_layout(const std::vector<Container>& containers, const Board& board);
