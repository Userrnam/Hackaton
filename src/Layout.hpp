#pragma once

#include "Composer.hpp"

struct Coord {
    int x;
    int y;
};

struct Board {
    int width;
    int height;
};

struct Layout {
    // uses container indices
    // -1 => place is empty
    std::vector<std::vector<int>> blocks;
    std::vector<Coord> container_coords;
};

Layout create_layout(const std::vector<Container>& containers, const Board& board);
