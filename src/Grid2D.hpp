//
// Created by flori on 10/01/2022.
//

#ifndef MCSEEDS_GRID2D_HPP
#define MCSEEDS_GRID2D_HPP


template<typename T>
class Grid2D {
public:
    T* data;

    const int width;
    const int height;

    Grid2D(int width, int height) : width(width), height(height), data(new T [width * height]) {};

    ~Grid2D() {
        delete[] data;
    }

    T& at(int x, int y) {
        return this->data[y * this->width + x];
    }

    T* ptr_at(int x, int y) {
        return this->data + (y * this->width + x);
    }

};


#endif //MCSEEDS_GRID2D_HPP
