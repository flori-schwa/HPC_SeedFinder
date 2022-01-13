#ifndef MCSEEDS_GRID2D_HPP
#define MCSEEDS_GRID2D_HPP

#include <cstdlib>
#include <functional>

#include "Graph2D.hpp"

//#define GRID2D_COL_MAJOR

template<typename T>
class Grid2D {
private:
    inline size_t indexOf(int x, int y) const {
#ifndef GRID2D_COL_MAJOR
        return y * this->width + x;
#else
        return x * this->height + y;
#endif
    }

    void
    create_graph_loop(Graph2D<T>* graph, GraphNode<T>* current, std::unordered_set<GraphNode<T>*>* visited) const {
        visited->insert(current);

        if (current->up == nullptr &&
            (current->pos.y > 0 && get(current->pos.x, current->pos.y - 1) == current->value)) {
            create_graph_loop(graph, graph->insert_up(current, current->value), visited);
        }

        if (current->left == nullptr &&
            (current->pos.x > 0 && get(current->pos.x - 1, current->pos.y) == current->value)) {
            create_graph_loop(graph, graph->insert_left(current, current->value), visited);
        }

        if (current->right == nullptr &&
            ((current->pos.x + 1) < width && get(current->pos.x + 1, current->pos.y) == current->value)) {
            create_graph_loop(graph, graph->insert_right(current, current->value), visited);
        }

        if (current->down == nullptr &&
            ((current->pos.y + 1) < height && get(current->pos.x, current->pos.y + 1) == current->value)) {
            create_graph_loop(graph, graph->insert_down(current, current->value), visited);
        }
    }

public:
    T* data;

    const int width;
    const int height;

    const int length;

    const size_t size;

    Grid2D(int width, int height) : width(width), height(height), length(width * height), size(sizeof(T) * width * height) {
        this->data = new T[this->length];
    };

    ~Grid2D() {
        delete[] data;
    }

    T get(int x, int y) const {
        size_t idx = this->indexOf(x, y);

        if (idx < 0 || idx > length) {
            std::cerr << "Reading from Grid out of bounds" << std::endl;
            exit(-1);
        }

        return this->data[idx];
    }

    void set(int x, int y, T value) {
        size_t idx = this->indexOf(x, y);

        if (idx < 0 || idx > length) {
            std::cerr << "Writing to Grid out of bounds" << std::endl;
            exit(-1);
        }

        this->data[idx] = value;
    }

    bool row_matches(int row, int offset, Grid2D<T>* pattern_source, int pattern_row) {
        for (int i = 0; i < pattern_source->width; ++i) {
            if (this->get(offset + i, row) != pattern_source->get(i, pattern_row)) {
                return false;
            }
        }

        return true;
    }

    void find_row_matches(int row, Grid2D<T>* pattern_source, int pattern_row, std::vector<int>& matches) {
        for (int i = 0; i < (width - pattern_source->width); ++i) {
            if (row_matches(row, i, pattern_source, pattern_row)) {
                matches.push_back(i);
            }
        }
    }

    void create_graph(Graph2D<T>* graph) const {
        if (graph->node_count() != 1 ||
            graph->get_root()->value != get(graph->get_root()->pos.x, graph->get_root()->pos.y)) {
            std::cerr << "Graph already contains more than one or value does not match" << std::endl;
            exit(-1);
        }

        std::unordered_set<GraphNode<T>*>* visited = new std::unordered_set<GraphNode<T>*>();
        create_graph_loop(graph, graph->get_root(), visited);
        delete visited;
    }

#ifndef GRID2D_COL_MAJOR

    T* row_pointer(int row) {
        if (row < 0 || row >= height) {
            std::cerr << "row index out of bounds" << std::endl;
            exit(-1);
        }

        return this->data + (row * width);
    }

#endif

    void create_kmp_string(int start_x, int start_z, int width, int height, char* buffer,
                           const std::function<char(T)>& to_char_func) const {
        if (width <= 0) {
            std::cerr << "width <= 0" << std::endl;
            exit(-1);
        }

        if (height <= 0) {
            std::cerr << "height <= 0" << std::endl;
            exit(-1);
        }

        int i = 0;

#ifndef GRID2D_COL_MAJOR
        for (int z = 0; z < height; ++z) {
            for (int x = 0; x < width; ++x) {
#else
                for (int x = 0; x < width; ++x) {
                    for (int z = 0; z < height; ++z) {
#endif
                buffer[i++] = to_char_func(get(start_x + x, start_z + z));
            }
        }

        buffer[width * height] = 0;
    }
};


#endif //MCSEEDS_GRID2D_HPP
