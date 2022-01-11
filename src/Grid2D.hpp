#ifndef MCSEEDS_GRID2D_HPP
#define MCSEEDS_GRID2D_HPP

#include <cstdlib>

template<typename T>
class Grid2D {
private:
    inline size_t indexOf(int x, int y) {
        return y * this->width + x;
    }

public:
    T *data;

    const int width;
    const int height;

    const int length;

    Grid2D(int width, int height) : width(width), height(height), length(width * height) {
        this->data = new T[this->length];
    };

    ~Grid2D() {
        delete[] data;
    }

    T get(int x, int y) {
        size_t idx = this->indexOf(x, y);

        if (idx < 0 || idx > length) {
            exit(-1);
        }

        return this->data[idx];
    }

    void set(int x, int y, T value) {
        size_t idx = this->indexOf(x, y);

        if (idx < 0 || idx > length) {
            exit(-1);
        }

        this->data[idx] = value;
    }
};


#endif //MCSEEDS_GRID2D_HPP
