#include "avl_tree.hpp"

#include <algorithm>
#include <cstdlib>

void AVLTree::update(Node* n) {
    n->height = 1 + std::max(h(n->left), h(n->right));
}

AVLTree::Node* AVLTree::rotateRight(Node* y) {
    Node* x = y->left;
    y->left = x->right;
    x->right = y;
    update(y);
    update(x);
    ++rot_;
    return x;
}

// Rotação simples à esquerda (caso Direita-Direita), simétrica à anterior.
AVLTree::Node* AVLTree::rotateLeft(Node* x) {
    Node* y = x->right;
    x->right = y->left;
    y->left = x;
    update(x);
    update(y);
    ++rot_;
    return y;
}

// Restaura o balanceamento do nó n (cujos filhos já são AVL válidas).
// Rotação dupla = duas rotações simples (ambas contabilizadas).
AVLTree::Node* AVLTree::rebalance(Node* n) {
    update(n);
    int bf = balanceFactor(n);
    if (bf > 1) {
        if (balanceFactor(n->left) < 0) n->left = rotateLeft(n->left);  // Esq-Dir
        return rotateRight(n);
    }
    if (bf < -1) {
        if (balanceFactor(n->right) > 0) n->right = rotateRight(n->right);  // Dir-Esq
        return rotateLeft(n);
    }
    return n;
}

AVLTree::Node* AVLTree::insert(Node* n, int key, bool& ok) {
    if (!n) {
        ok = true;
        return new Node(key);
    }
    if (key < n->key)
        n->left = insert(n->left, key, ok);
    else if (key > n->key)
        n->right = insert(n->right, key, ok);
    else {
        ok = false;  // chave repetida: nada muda
        return n;
    }
    return rebalance(n);
}

bool AVLTree::insert(int key) {
    bool ok = false;
    root_ = insert(root_, key, ok);
    if (ok) ++size_;
    return ok;
}

AVLTree::Node* AVLTree::minNode(Node* n) {
    while (n->left) n = n->left;
    return n;
}

AVLTree::Node* AVLTree::remove(Node* n, int key, bool& ok) {
    if (!n) {
        ok = false;  // chave inexistente
        return nullptr;
    }
    if (key < n->key) {
        n->left = remove(n->left, key, ok);
    } else if (key > n->key) {
        n->right = remove(n->right, key, ok);
    } else {
        ok = true;
        if (!n->left || !n->right) {  // 0 ou 1 filho
            Node* child = n->left ? n->left : n->right;
            delete n;
            return child;  // nullptr se folha
        }
        // 2 filhos: substitui pela chave do sucessor em-ordem e o remove da direita.
        Node* succ = minNode(n->right);
        n->key = succ->key;
        bool dummy;
        n->right = remove(n->right, succ->key, dummy);
    }
    return rebalance(n);
}

bool AVLTree::remove(int key) {
    bool ok = false;
    root_ = remove(root_, key, ok);
    if (ok) --size_;
    return ok;
}

bool AVLTree::search(int key) const {
    const Node* n = root_;
    while (n) {
        if (key == n->key) return true;
        n = key < n->key ? n->left : n->right;
    }
    return false;
}

void AVLTree::destroy(Node* n) {
    if (!n) return;
    destroy(n->left);
    destroy(n->right);
    delete n;
}

void AVLTree::clear() {
    destroy(root_);
    root_ = nullptr;
    size_ = 0;
}

void AVLTree::inOrder(const Node* n, std::vector<int>& out) {
    if (!n) return;
    inOrder(n->left, out);
    out.push_back(n->key);
    inOrder(n->right, out);
}

std::vector<int> AVLTree::inOrder() const {
    std::vector<int> out;
    out.reserve(size_);
    inOrder(root_, out);
    return out;
}

void AVLTree::print(const Node* n, std::ostream& os, int depth) {
    if (!n) return;
    print(n->right, os, depth + 1);
    os << std::string(depth * 6, ' ') << n->key << " (fb=" << balanceFactor(n) << ")\n";
    print(n->left, os, depth + 1);
}

void AVLTree::print(std::ostream& os, bool) const {
    if (!root_) {
        os << "(árvore vazia)\n";
        return;
    }
    print(root_, os, 0);
}

int AVLTree::check(const Node* n, const int* lo, const int* hi, bool& ok, std::string* why) {
    if (!n || !ok) return 0;
    auto fail = [&](const std::string& msg) {
        if (ok && why) *why = msg + " (nó " + std::to_string(n->key) + ")";
        ok = false;
    };
    if ((lo && n->key <= *lo) || (hi && n->key >= *hi)) fail("violação da ordem BST");
    int hl = check(n->left, lo, &n->key, ok, why);
    int hr = check(n->right, &n->key, hi, ok, why);
    int real = 1 + std::max(hl, hr);
    if (n->height != real) fail("altura armazenada incorreta");
    if (std::abs(hl - hr) > 1) fail("fator de balanceamento fora de [-1,1]");
    return real;
}

bool AVLTree::validate(std::string* why) const {
    bool ok = true;
    check(root_, nullptr, nullptr, ok, why);
    if (ok && inOrder().size() != size_) {
        if (why) *why = "contador de tamanho inconsistente";
        ok = false;
    }
    return ok;
}
