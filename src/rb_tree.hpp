#ifndef RB_TREE_HPP
#define RB_TREE_HPP

#include "tree.hpp"

// Árvore Rubro-Negra (formulação de Cormen et al., com nó sentinela NIL):
//  1. todo nó é vermelho ou preto;       2. a raiz é preta;
//  3. as folhas (NIL) são pretas;        4. filhos de nó vermelho são pretos;
//  5. todo caminho nó -> NIL tem o mesmo número de nós pretos.
class RedBlackTree : public Tree {
public:
    RedBlackTree();
    ~RedBlackTree() override;
    RedBlackTree(const RedBlackTree&) = delete;
    RedBlackTree& operator=(const RedBlackTree&) = delete;

    bool insert(int key) override;
    bool remove(int key) override;
    bool search(int key) const override { return find(key) != nil_; }
    std::vector<int> inOrder() const override;
    void print(std::ostream& os, bool ansi = false) const override;
    int height() const override { return height(root_); }
    std::size_t size() const override { return size_; }
    void clear() override;
    bool validate(std::string* why = nullptr) const override;
    long rotations() const override { return rot_; }
    long recolorings() const override { return rec_; }
    void resetStats() override { rot_ = rec_ = 0; }
    std::string name() const override { return "Rubro-Negra"; }

private:
    enum Color { RED, BLACK };
    struct Node {
        int key;
        Color color;
        Node* left;
        Node* right;
        Node* parent;
    };

    Node* nil_;
    Node* root_;
    std::size_t size_ = 0;
    long rot_ = 0;
    long rec_ = 0;

    Node* find(int key) const;
    Node* minNode(Node* n) const;
    void paint(Node* n, Color c);  // muda a cor e contabiliza o recoloramento
    void rotateLeft(Node* x);
    void rotateRight(Node* y);
    void transplant(Node* u, Node* v);
    void insertFixup(Node* z);
    void removeFixup(Node* x);

    int height(const Node* n) const;
    void destroy(Node* n);
    void inOrder(const Node* n, std::vector<int>& out) const;
    void print(const Node* n, std::ostream& os, bool ansi, int depth) const;
    // Retorna a altura-negra do nó, ou -1 em caso de violação.
    int check(const Node* n, const int* lo, const int* hi, bool& ok, std::string* why) const;
};

#endif
