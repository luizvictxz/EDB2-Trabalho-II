#include "rb_tree.hpp"

#include <algorithm>

RedBlackTree::RedBlackTree() {
    nil_ = new Node{0, BLACK, nullptr, nullptr, nullptr};
    nil_->left = nil_->right = nil_->parent = nil_;
    root_ = nil_;
}

RedBlackTree::~RedBlackTree() {
    destroy(root_);
    delete nil_;
}

void RedBlackTree::destroy(Node* n) {
    if (n == nil_) return;
    destroy(n->left);
    destroy(n->right);
    delete n;
}

void RedBlackTree::clear() {
    destroy(root_);
    root_ = nil_;
    size_ = 0;
}

RedBlackTree::Node* RedBlackTree::find(int key) const {
    Node* n = root_;
    while (n != nil_ && key != n->key) n = key < n->key ? n->left : n->right;
    return n;
}

RedBlackTree::Node* RedBlackTree::minNode(Node* n) const {
    while (n->left != nil_) n = n->left;
    return n;
}

void RedBlackTree::paint(Node* n, Color c) {
    if (n->color != c) {
        n->color = c;
        ++rec_;
    }
}

void RedBlackTree::rotateLeft(Node* x) {
    Node* y = x->right;
    x->right = y->left;
    if (y->left != nil_) y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == nil_)
        root_ = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left = x;
    x->parent = y;
    ++rot_;
}

void RedBlackTree::rotateRight(Node* y) {
    Node* x = y->left;
    y->left = x->right;
    if (x->right != nil_) x->right->parent = y;
    x->parent = y->parent;
    if (y->parent == nil_)
        root_ = x;
    else if (y == y->parent->left)
        y->parent->left = x;
    else
        y->parent->right = x;
    x->right = y;
    y->parent = x;
    ++rot_;
}

// Substitui a subárvore enraizada em u pela enraizada em v.
void RedBlackTree::transplant(Node* u, Node* v) {
    if (u->parent == nil_)
        root_ = v;
    else if (u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;
    v->parent = u->parent;
}

bool RedBlackTree::insert(int key) {
    Node* y = nil_;
    Node* x = root_;
    while (x != nil_) {
        y = x;
        if (key == x->key) return false;  // chave repetida: nada muda
        x = key < x->key ? x->left : x->right;
    }
    Node* z = new Node{key, RED, nil_, nil_, y};  // todo nó novo entra vermelho
    if (y == nil_)
        root_ = z;
    else if (key < y->key)
        y->left = z;
    else
        y->right = z;
    ++size_;
    insertFixup(z);
    return true;
}

// Só a regra 4 (vermelho-vermelho) pode ser violada após a inserção.
//  Caso 1: tio vermelho  -> recolore pai, tio e avô; sobe com z = avô.
//  Caso 2: tio preto, z "dobrado" -> rotação para cair no caso 3.
//  Caso 3: tio preto, z "alinhado" -> recolore e rotaciona o avô.
void RedBlackTree::insertFixup(Node* z) {
    while (z->parent->color == RED) {
        Node* gp = z->parent->parent;
        if (z->parent == gp->left) {
            Node* uncle = gp->right;
            if (uncle->color == RED) {
                paint(z->parent, BLACK);
                paint(uncle, BLACK);
                paint(gp, RED);
                z = gp;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    rotateLeft(z);
                }
                paint(z->parent, BLACK);
                paint(z->parent->parent, RED);
                rotateRight(z->parent->parent);
            }
        } else {
            Node* uncle = gp->left;
            if (uncle->color == RED) {
                paint(z->parent, BLACK);
                paint(uncle, BLACK);
                paint(gp, RED);
                z = gp;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    rotateRight(z);
                }
                paint(z->parent, BLACK);
                paint(z->parent->parent, RED);
                rotateLeft(z->parent->parent);
            }
        }
    }
    paint(root_, BLACK);
}

bool RedBlackTree::remove(int key) {
    Node* z = find(key);
    if (z == nil_) return false;  // chave inexistente

    Node* y = z;
    Color yOriginal = y->color;
    Node* x;
    if (z->left == nil_) {
        x = z->right;
        transplant(z, z->right);
    } else if (z->right == nil_) {
        x = z->left;
        transplant(z, z->left);
    } else {
        y = minNode(z->right);  // sucessor em-ordem
        yOriginal = y->color;
        x = y->right;
        if (y->parent == z) {
            x->parent = y;  // x pode ser nil_: o fixup precisa do pai
        } else {
            transplant(y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        transplant(z, y);
        y->left = z->left;
        y->left->parent = y;
        paint(y, z->color);
    }
    delete z;
    --size_;
    // Se o nó removido/movido era preto, falta um preto em x: "duplo preto".
    if (yOriginal == BLACK) removeFixup(x);
    return true;
}

//  Caso 1: irmão vermelho -> rotação transforma em um dos casos 2-4.
//  Caso 2: irmão preto, sobrinhos pretos -> recolore irmão e sobe.
//  Caso 3: irmão preto, sobrinho distante preto -> rotação no irmão (vira caso 4).
//  Caso 4: irmão preto, sobrinho distante vermelho -> rotação no pai; termina.
void RedBlackTree::removeFixup(Node* x) {
    while (x != root_ && x->color == BLACK) {
        if (x == x->parent->left) {
            Node* w = x->parent->right;
            if (w->color == RED) {
                paint(w, BLACK);
                paint(x->parent, RED);
                rotateLeft(x->parent);
                w = x->parent->right;
            }
            if (w->left->color == BLACK && w->right->color == BLACK) {
                paint(w, RED);
                x = x->parent;
            } else {
                if (w->right->color == BLACK) {
                    paint(w->left, BLACK);
                    paint(w, RED);
                    rotateRight(w);
                    w = x->parent->right;
                }
                paint(w, x->parent->color);
                paint(x->parent, BLACK);
                paint(w->right, BLACK);
                rotateLeft(x->parent);
                x = root_;
            }
        } else {
            Node* w = x->parent->left;
            if (w->color == RED) {
                paint(w, BLACK);
                paint(x->parent, RED);
                rotateRight(x->parent);
                w = x->parent->left;
            }
            if (w->right->color == BLACK && w->left->color == BLACK) {
                paint(w, RED);
                x = x->parent;
            } else {
                if (w->left->color == BLACK) {
                    paint(w->right, BLACK);
                    paint(w, RED);
                    rotateLeft(w);
                    w = x->parent->left;
                }
                paint(w, x->parent->color);
                paint(x->parent, BLACK);
                paint(w->left, BLACK);
                rotateRight(x->parent);
                x = root_;
            }
        }
    }
    paint(x, BLACK);
}

int RedBlackTree::height(const Node* n) const {
    if (n == nil_) return 0;
    return 1 + std::max(height(n->left), height(n->right));
}

void RedBlackTree::inOrder(const Node* n, std::vector<int>& out) const {
    if (n == nil_) return;
    inOrder(n->left, out);
    out.push_back(n->key);
    inOrder(n->right, out);
}

std::vector<int> RedBlackTree::inOrder() const {
    std::vector<int> out;
    out.reserve(size_);
    inOrder(root_, out);
    return out;
}

void RedBlackTree::print(const Node* n, std::ostream& os, bool ansi, int depth) const {
    if (n == nil_) return;
    print(n->right, os, ansi, depth + 1);
    os << std::string(depth * 6, ' ');
    if (ansi && n->color == RED)
        os << "\033[1;31m" << n->key << " (R)\033[0m\n";
    else
        os << n->key << (n->color == RED ? " (R)" : " (B)") << "\n";
    print(n->left, os, ansi, depth + 1);
}

void RedBlackTree::print(std::ostream& os, bool ansi) const {
    if (root_ == nil_) {
        os << "(árvore vazia)\n";
        return;
    }
    print(root_, os, ansi, 0);
}

int RedBlackTree::check(const Node* n, const int* lo, const int* hi, bool& ok,
                        std::string* why) const {
    if (n == nil_) return 1;  // NIL conta como um nó preto
    auto fail = [&](const std::string& msg) {
        if (ok && why) *why = msg + " (nó " + std::to_string(n->key) + ")";
        ok = false;
    };
    if ((lo && n->key <= *lo) || (hi && n->key >= *hi)) fail("violação da ordem BST");
    if (n->color == RED && (n->left->color == RED || n->right->color == RED))
        fail("nó vermelho com filho vermelho");
    if (n->left != nil_ && n->left->parent != n) fail("ponteiro pai inconsistente");
    if (n->right != nil_ && n->right->parent != n) fail("ponteiro pai inconsistente");
    int bl = check(n->left, lo, &n->key, ok, why);
    int br = check(n->right, &n->key, hi, ok, why);
    if (bl != br) fail("alturas-negras diferentes");
    return bl + (n->color == BLACK ? 1 : 0);
}

bool RedBlackTree::validate(std::string* why) const {
    bool ok = true;
    if (nil_->color != BLACK) {
        if (why) *why = "sentinela NIL não é preta";
        return false;
    }
    if (root_ != nil_ && root_->color != BLACK) {
        if (why) *why = "a raiz não é preta";
        return false;
    }
    check(root_, nullptr, nullptr, ok, why);
    if (ok && inOrder().size() != size_) {
        if (why) *why = "contador de tamanho inconsistente";
        ok = false;
    }
    return ok;
}
