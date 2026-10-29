#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>

#include "tree.hpp"

namespace {

// BST ingênua (iterativa) usada só como baseline para mostrar o problema.
struct PlainBST {
    struct N { int k; N *l = nullptr, *r = nullptr; };
    N* root = nullptr;
    void insert(int k) {
        N** p = &root;
        while (*p) {
            if (k == (*p)->k) return;
            p = k < (*p)->k ? &(*p)->l : &(*p)->r;
        }
        *p = new N{k};
    }
    int height() const {
        int best = 0;
        std::vector<std::pair<const N*, int>> st;
        if (root) st.push_back({root, 1});
        while (!st.empty()) {
            auto [n, d] = st.back();
            st.pop_back();
            best = std::max(best, d);
            if (n->l) st.push_back({n->l, d + 1});
            if (n->r) st.push_back({n->r, d + 1});
        }
        return best;
    }
    ~PlainBST() {
        std::vector<N*> st;
        if (root) st.push_back(root);
        while (!st.empty()) {
            N* n = st.back();
            st.pop_back();
            if (n->l) st.push_back(n->l);
            if (n->r) st.push_back(n->r);
            delete n;
        }
    }
};

std::vector<int> makeDataset(const std::string& kind, int n, std::mt19937& rng) {
    std::vector<int> v(n);
    std::iota(v.begin(), v.end(), 1);
    if (kind == "decrescente") std::reverse(v.begin(), v.end());
    else if (kind == "aleatória") std::shuffle(v.begin(), v.end(), rng);
    else if (kind == "quase ordenada") {
        // IDs de pedidos chegando quase em ordem (5% das posições trocadas),
        // cenário típico de contador sequencial + latência de rede.
        std::uniform_int_distribution<int> pos(0, n - 1);
        for (int i = 0; i < n / 20; ++i) std::swap(v[pos(rng)], v[pos(rng)]);
    }
    return v;
}

double ms(std::chrono::steady_clock::time_point a, std::chrono::steady_clock::time_point b) {
    return std::chrono::duration<double, std::milli>(b - a).count();
}

}  // namespace

int main() {
    const int sizes[] = {1000, 10000, 100000};
    const char* kinds[] = {"crescente", "decrescente", "aleatória", "quase ordenada"};

    std::cout << "# Resultados do experimento (mesmos dados nas três estruturas)\n\n"
              << "Altura: árvore vazia = 0, um nó = 1. Remoção: 50% das chaves, em ordem "
                 "embaralhada (semente fixa).\n"
              << "Limites teóricos: AVL h <= 1,44*log2(n+2); Rubro-Negra h <= 2*log2(n+1).\n\n";

    std::cout << "## Após inserir n chaves\n\n"
              << "| Dataset | n | BST h | AVL h | RN h | AVL rot. | RN rot. | RN recol. | "
                 "AVL ms | RN ms |\n"
              << "|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|\n";

    struct Row { std::string kind; int n; int avlH, rbH; long avlRot, rbRot; };
    std::vector<Row> afterInsert;
    std::cout << std::fixed << std::setprecision(2);

    for (const char* kind : kinds) {
        for (int n : sizes) {
            std::mt19937 rng(12345);
            auto data = makeDataset(kind, n, rng);
            auto avl = makeTree("avl");
            auto rb = makeTree("rb");

            auto t0 = std::chrono::steady_clock::now();
            for (int k : data) avl->insert(k);
            auto t1 = std::chrono::steady_clock::now();
            for (int k : data) rb->insert(k);
            auto t2 = std::chrono::steady_clock::now();

            std::string bst = "n/d";
            if (n <= 10000) {
                PlainBST b;
                for (int k : data) b.insert(k);
                bst = std::to_string(b.height());
            }
            std::cout << "| " << kind << " | " << n << " | " << bst << " | " << avl->height()
                      << " | " << rb->height() << " | " << avl->rotations() << " | "
                      << rb->rotations() << " | " << rb->recolorings() << " | " << ms(t0, t1)
                      << " | " << ms(t1, t2) << " |\n";
        }
    }

    std::cout << "\n## Após remover 50% das chaves (sobre as árvores acima)\n\n"
              << "| Dataset | n | AVL h | RN h | AVL rot. (acum.) | RN rot. (acum.) | "
                 "RN recol. (acum.) |\n"
              << "|---|---:|---:|---:|---:|---:|---:|\n";
    for (const char* kind : kinds) {
        for (int n : sizes) {
            std::mt19937 rng(12345);
            auto data = makeDataset(kind, n, rng);
            auto avl = makeTree("avl");
            auto rb = makeTree("rb");
            for (int k : data) {
                avl->insert(k);
                rb->insert(k);
            }
            auto victims = data;
            std::mt19937 rng2(999);
            std::shuffle(victims.begin(), victims.end(), rng2);
            victims.resize(n / 2);
            for (int k : victims) {
                avl->remove(k);
                rb->remove(k);
            }
            std::string w1, w2;
            bool ok = avl->validate(&w1) && rb->validate(&w2);
            std::cout << "| " << kind << " | " << n << " | " << avl->height() << " | "
                      << rb->height() << " | " << avl->rotations() << " | " << rb->rotations()
                      << " | " << rb->recolorings() << " |" << (ok ? "" : " INVÁLIDA") << "\n";
        }
    }

    std::cout << "\n## Altura teórica de referência\n\n| n | log2(n) | 1,44*log2(n+2) | "
                 "2*log2(n+1) |\n|---:|---:|---:|---:|\n";
    for (int n : sizes)
        std::cout << "| " << n << " | " << std::log2(n) << " | " << 1.44 * std::log2(n + 2)
                  << " | " << 2 * std::log2(n + 1) << " |\n";

    // Busca: mesma carga de consultas nas duas árvores (n = 100000, dados aleatórios).
    {
        const int n = 100000;
        std::mt19937 rng(12345);
        auto data = makeDataset("aleatória", n, rng);
        auto avl = makeTree("avl");
        auto rb = makeTree("rb");
        for (int k : data) {
            avl->insert(k);
            rb->insert(k);
        }
        std::uniform_int_distribution<int> q(1, 2 * n);
        std::vector<int> queries(2000000);
        for (int& x : queries) x = q(rng);
        long hitsA = 0, hitsR = 0;
        auto t0 = std::chrono::steady_clock::now();
        for (int x : queries) hitsA += avl->search(x);
        auto t1 = std::chrono::steady_clock::now();
        for (int x : queries) hitsR += rb->search(x);
        auto t2 = std::chrono::steady_clock::now();
        std::cout << "\n## Busca (n = 100000 aleatório, 2 000 000 consultas)\n\n"
                  << "| Estrutura | Achados | Tempo (ms) |\n|---|---:|---:|\n"
                  << "| AVL | " << hitsA << " | " << ms(t0, t1) << " |\n"
                  << "| Rubro-Negra | " << hitsR << " | " << ms(t1, t2) << " |\n";
    }
    return 0;
}
