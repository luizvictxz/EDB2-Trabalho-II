#ifndef AVL_TREE_HPP
#define AVL_TREE_HPP

#include "tree.hpp"

// Árvore AVL: para todo nó, |altura(esq) - altura(dir)| <= 1.
class AVLTree : public Tree {
public:
    AVLTree() = default;
    ~AVLTree() override { clear(); }
    AVLTree(const AVLTree&) = delete;
    AVLTree& operator=(const AVLTree&) = delete;

    bool insert(int key) override;
    bool remove(int key) override;
    bool search(int key) const override;
    std::vector<int> inOrder() const override;
    void print(std::ostream& os, bool ansi = false) const override;
    int height() const override { return h(root_); }
    std::size_t size() const override { return size_; }
    void clear() override;
    bool validate(std::string* why = nullptr) const override;
    long rotations() const override { return rot_; }
    void resetStats() override { rot_ = 0; }
    std::string name() const override { return "AVL"; }

private:
    struct Node {
        int key;
        int height;  // altura do nó (folha = 1)
        Node* left = nullptr;
        Node* right = nullptr;
        explicit Node(int k) : key(k), height(1) {}
    };

    Node* root_ = nullptr;
    std::size_t size_ = 0;
    long rot_ = 0;

    static int h(const Node* n) { return n ? n->height : 0; }
    static int balanceFactor(const Node* n) { return h(n->left) - h(n->right); }
    static void update(Node* n);

    Node* rotateRight(Node* y);
    Node* rotateLeft(Node* x);
    Node* rebalance(Node* n);

    Node* insert(Node* n, int key, bool& ok);
    Node* remove(Node* n, int key, bool& ok);
    static Node* minNode(Node* n);

    static void destroy(Node* n);
    static void inOrder(const Node* n, std::vector<int>& out);
    static void print(const Node* n, std::ostream& os, int depth);
    // Retorna a altura real; false em *ok indica violação (motivo em why).
    static int check(const Node* n, const int* lo, const int* hi, bool& ok, std::string* why);
};

#endif
