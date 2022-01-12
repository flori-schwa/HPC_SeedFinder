#ifndef MCSEEDS_GRAPH2D_HPP
#define MCSEEDS_GRAPH2D_HPP

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <iostream>

struct Pos2 {
    int x;
    int y;

    Pos2(int x, int y) : x(x), y(y) {}

    bool operator==(const Pos2 &other) const;
};

namespace std {
    template<>
    struct hash<Pos2> {
        std::size_t operator()(const Pos2 &pos) const;
    };
}

template<typename T>
struct GraphNode {
    Pos2 pos;
    T value;

    GraphNode<T> *up = nullptr;
    GraphNode<T> *down = nullptr;
    GraphNode<T> *left = nullptr;
    GraphNode<T> *right = nullptr;

    GraphNode(int x, int y, T value) : pos(x, y), value(value) {}
};

template<typename T>
class Graph2D {
private:
    std::unordered_map<Pos2, GraphNode<T> *> node_positions;
    GraphNode<T> *root;

    GraphNode<T> *new_node(int x, int y, T value) {
        Pos2 pos = Pos2(x, y);

        if (node_positions.contains(pos)) {
            return node_positions[pos];
        }

        GraphNode<T> *node = new GraphNode<T>(x, y, value);
        all_nodes.template emplace_back(node);
        node_positions[pos] = node;

        Pos2 top = Pos2(pos.x, pos.y - 1);
        Pos2 left = Pos2(pos.x - 1, pos.y);
        Pos2 right = Pos2(pos.x + 1, pos.y);
        Pos2 down = Pos2(pos.x, pos.y + 1);

        if (node_positions.contains(top)) {
            GraphNode<T> *neighbor = node_positions[top];

            neighbor->down = node;
            node->up = neighbor;
        }

        if (node_positions.contains(left)) {
            GraphNode<T> *neighbor = node_positions[left];

            neighbor->right = node;
            node->left = neighbor;
        }

        if (node_positions.contains(right)) {
            GraphNode<T> *neighbor = node_positions[right];

            neighbor->left = node;
            node->right = neighbor;
        }

        if (node_positions.contains(down)) {
            GraphNode<T> *neighbor = node_positions[down];

            neighbor->up = node;
            node->down = neighbor;
        }

        return node;
    }

    static bool are_equal_nodes(GraphNode<T> *a, GraphNode<T> *b, std::unordered_set<GraphNode<T> *> &visited) {
        visited.insert(a);
        visited.insert(b);

        if (a->value != b->value) {
            return false; // mismatch in value
        }

        if ((a->up != nullptr && b->up == nullptr) || (a->up == nullptr && b->up != nullptr)) {
            return false; // mismatch in shape
        } else if (a->up != nullptr) {
            if (visited.contains(a->up) != visited.contains(b->up)) {
                return false; // mismatch in shape
            } else if (!visited.contains(a->up) && !are_equal_nodes(a->up, b->up, visited)) {
                return false; // mismatch in shape or value
            }
        }

        if ((a->left != nullptr && b->left == nullptr) || (a->left == nullptr && b->left != nullptr)) {
            return false; // mismatch in shape
        } else if (a->left != nullptr) {
            if (visited.contains(a->left) != visited.contains(b->left)) {
                return false; // mismatch in shape
            } else if (!visited.contains(a->left) && !are_equal_nodes(a->left, b->left, visited)) {
                return false; // mismatch in shape or value
            }
        }

        if ((a->right != nullptr && b->right == nullptr) || (a->right == nullptr && b->right != nullptr)) {
            return false; // mismatch in shape
        } else if (a->right != nullptr) {
            if (visited.contains(a->right) != visited.contains(b->right)) {
                return false; // mismatch in shape
            } else if (!visited.contains(a->right) && !are_equal_nodes(a->right, b->right, visited)) {
                return false; // mismatch in shape or value
            }
        }

        if ((a->down != nullptr && b->down == nullptr) || (a->down == nullptr && b->down != nullptr)) {
            return false; // mismatch in shape
        } else if (a->down != nullptr) {
            if (visited.contains(a->down) != visited.contains(b->down)) {
                return false; // mismatch in shape
            } else if (!visited.contains(a->down) && !are_equal_nodes(a->down, b->down, visited)) {
                return false; // mismatch in shape or value
            }
        }

        return true;
    }

public:
    std::vector<GraphNode<T> *> all_nodes;

    Graph2D(int init_x, int init_y, T value) : node_positions(), all_nodes() {
        root = new_node(init_x, init_y, value);
    }

    ~Graph2D() {
        for (const GraphNode<T> *node: all_nodes) {
            delete node;
        }
    }

    GraphNode<T> *get_root() {
        return this->root;
    }

    void promote_to_root(GraphNode<T> *node) {
        this->root = node;
    }

    size_t node_count() {
        return all_nodes.size();
    }

    GraphNode<T> *find_left_topmost() {
        int min_x = all_nodes[0]->pos.x;
        int min_y = all_nodes[0]->pos.y;

        GraphNode<T> *ptr = all_nodes[0];

        for (int i = 1; i < all_nodes.size(); ++i) {
            GraphNode<T> *node = all_nodes[i];

            if (node->pos.x < min_x || (node->pos.x == min_x && node->pos.y < min_y)) {
                min_x = node->pos.x;
                min_y = node->pos.y;

                ptr = node;
            }
        }

        return ptr;
    }

    GraphNode<T> *insert_up(GraphNode<T> *at, T value) {
        return new_node(at->pos.x, at->pos.y + 1, value);
    }

    GraphNode<T> *insert_left(GraphNode<T> *at, T value) {
        return new_node(at->pos.x - 1, at->pos.y, value);
    }

    GraphNode<T> *insert_right(GraphNode<T> *at, T value) {
        return new_node(at->pos.x + 1, at->pos.y, value);
    }

    GraphNode<T> *insert_down(GraphNode<T> *at, T value) {
        return new_node(at->pos.x, at->pos.y + 1, value);
    }

    static bool are_equal(Graph2D<T> &g_a, Graph2D<T> &g_b) {
        GraphNode<T> *a = g_a.find_left_topmost();
        GraphNode<T> *b = g_b.find_left_topmost();

        std::unordered_set<GraphNode<T> *> visited;

        return are_equal_nodes(a, b, visited);
    }

};

#endif //MCSEEDS_GRAPH2D_HPP
