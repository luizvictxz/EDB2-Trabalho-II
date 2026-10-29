#include "avl_tree.hpp"
#include "rb_tree.hpp"
#include "tree.hpp"

std::unique_ptr<Tree> makeTree(const std::string& kind) {
    if (kind == "avl") return std::make_unique<AVLTree>();
    if (kind == "rb") return std::make_unique<RedBlackTree>();
    return nullptr;
}
