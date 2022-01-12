#include "include/Graph2D.hpp"

bool Pos2::operator==(const Pos2& other) const {
    return this->x == other.x && this->y == other.y;
}

std::size_t std::hash<Pos2>::operator()(const Pos2& pos) const {
    using std::size_t;
    using std::hash;

    size_t h1 = hash<int>{}(pos.x);
    size_t h2 = hash<int>{}(pos.y);

    return h1 ^ (h2 << 1);
}