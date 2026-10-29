
#include <algorithm>
#include <iostream>
#include <numeric>
#include <random>
#include <set>
#include <sstream>

#include "tree.hpp"

static int checks = 0, failures = 0;

#define CHECK(cond, msg)                                                         \
    do {                                                                         \
        ++checks;                                                                \
        if (!(cond)) {                                                           \
            ++failures;                                                          \
            std::cout << "    [FALHA] " << msg << "  (" #cond ")\n";             \
        }                                                                        \
    } while (0)

static void checkValid(const Tree& t, const std::string& ctx) {
    std::string why;
    ++checks;
    if (!t.validate(&why)) {
        ++failures;
        std::cout << "    [FALHA] invariantes violadas em " << ctx << ": " << why << "\n";
    }
}

static bool sameAs(const Tree& t, const std::set<int>& oracle) {
    auto v = t.inOrder();
    return v.size() == oracle.size() && std::equal(v.begin(), v.end(), oracle.begin());
}

using TestFn = void (*)(Tree&);

static void insertOrders(Tree& t) {
    const int n = 1000;
    std::vector<int> asc(n), desc(n), rnd(n);
    std::iota(asc.begin(), asc.end(), 1);
    desc.assign(asc.rbegin(), asc.rend());
    rnd = asc;
    std::mt19937 rng(42);
    std::shuffle(rnd.begin(), rnd.end(), rng);

    for (auto* seq : {&asc, &desc, &rnd}) {
        t.clear();
        t.resetStats();
        for (int k : *seq) {
            CHECK(t.insert(k), "insert " << k);
            if (k % 100 == 0) checkValid(t, "inserção em massa");
        }
        checkValid(t, "fim da inserção");
        CHECK(t.size() == (std::size_t)n, "tamanho");
        auto keys = t.inOrder();
        CHECK(std::is_sorted(keys.begin(), keys.end()), "percurso em ordem crescente");
        CHECK(t.height() <= 20, "altura logarítmica (n=1000)");
    }
}

static void searchTests(Tree& t) {
    t.clear();
    for (int k = 2; k <= 200; k += 2) t.insert(k);  // só pares
    bool allFound = true, noneFound = true;
    for (int k = 2; k <= 200; k += 2) allFound &= t.search(k);
    for (int k = 1; k <= 201; k += 2) noneFound &= !t.search(k);
    CHECK(allFound, "busca de existentes");
    CHECK(noneFound, "busca de inexistentes");
    CHECK(!t.search(-5) && !t.search(100000), "busca fora do intervalo");
}

static void duplicates(Tree& t) {
    t.clear();
    CHECK(t.insert(10), "primeira inserção");
    CHECK(!t.insert(10), "repetido deve ser rejeitado");
    CHECK(t.size() == 1, "tamanho não muda com repetido");
    for (int k : {5, 15, 3}) t.insert(k);
    std::size_t before = t.size();
    CHECK(!t.insert(5) && !t.insert(15) && !t.insert(3), "repetidos rejeitados");
    CHECK(t.size() == before, "tamanho estável");
    checkValid(t, "após repetidos");
}

static void removeShapes(Tree& t) {
    // folha, um filho, dois filhos, raiz
    t.clear();
    for (int k : {50, 30, 70, 20, 40, 60, 80, 10}) t.insert(k);
    CHECK(t.remove(10), "remover folha");
    checkValid(t, "após remover folha");
    CHECK(t.remove(20), "remover nó (um filho ou folha após ajustes)");
    checkValid(t, "após remover nó com um filho");
    CHECK(t.remove(30), "remover nó com dois filhos");
    checkValid(t, "após remover nó com dois filhos");
    CHECK(t.remove(50), "remover raiz");
    checkValid(t, "após remover raiz");
    CHECK(!t.search(10) && !t.search(20) && !t.search(30) && !t.search(50), "removidos sumiram");
    CHECK(t.search(40) && t.search(60) && t.search(70) && t.search(80), "demais permanecem");
    CHECK(t.size() == 4, "tamanho após remoções");

    // remover tudo em várias ordens
    for (int mode = 0; mode < 3; ++mode) {
        t.clear();
        std::vector<int> ks(200);
        std::iota(ks.begin(), ks.end(), 1);
        for (int k : ks) t.insert(k);
        if (mode == 1) std::reverse(ks.begin(), ks.end());
        if (mode == 2) std::shuffle(ks.begin(), ks.end(), std::mt19937(7));
        for (int k : ks) {
            CHECK(t.remove(k), "remove " << k);
            std::string why;
            if (!t.validate(&why)) {
                CHECK(false, "invariantes após remover " << k << ": " << why);
                break;
            }
        }
        CHECK(t.size() == 0 && t.height() == 0, "árvore vazia após remover tudo");
    }
}

static void removeMissing(Tree& t) {
    t.clear();
    CHECK(!t.remove(1), "remover em árvore vazia");
    for (int k : {5, 3, 8}) t.insert(k);
    CHECK(!t.remove(99) && !t.remove(4), "remover inexistente");
    CHECK(t.size() == 3, "tamanho inalterado");
    checkValid(t, "após remoção inexistente");
}

static void emptyAndSingle(Tree& t) {
    t.clear();
    CHECK(t.size() == 0 && t.height() == 0, "vazia: tamanho/altura");
    CHECK(!t.search(1), "busca em vazia");
    CHECK(t.inOrder().empty(), "percurso em vazia");
    checkValid(t, "árvore vazia");
    std::ostringstream os;
    t.print(os);
    CHECK(!os.str().empty(), "impressão de árvore vazia");

    CHECK(t.insert(42), "inserir único");
    CHECK(t.size() == 1 && t.height() == 1, "único: tamanho/altura");
    CHECK(t.search(42) && !t.search(41), "busca no único");
    CHECK(t.inOrder() == std::vector<int>{42}, "percurso no único");
    checkValid(t, "árvore com um elemento");
    CHECK(t.remove(42), "remover único");
    CHECK(t.size() == 0 && t.height() == 0, "volta a vazia");
    checkValid(t, "após esvaziar");
}

static void randomStress(Tree& t) {
    // 20 000 operações aleatórias comparadas ao oráculo, validando a cada passo.
    t.clear();
    std::set<int> oracle;
    std::mt19937 rng(2024);
    std::uniform_int_distribution<int> key(1, 500), op(0, 9);
    bool ok = true;
    for (int i = 0; i < 20000 && ok; ++i) {
        int k = key(rng);
        if (op(rng) < 6) {
            bool a = t.insert(k), b = oracle.insert(k).second;
            ok &= (a == b);
        } else {
            bool a = t.remove(k), b = oracle.erase(k) > 0;
            ok &= (a == b);
        }
        ok &= (t.search(k) == (oracle.count(k) > 0));
        std::string why;
        if (!t.validate(&why)) {
            CHECK(false, "stress: invariantes na op " << i << ": " << why);
            ok = false;
        }
    }
    CHECK(ok, "stress: resultados coincidem com o oráculo em todas as operações");
    CHECK(sameAs(t, oracle), "stress: conteúdo final idêntico ao oráculo");
}

static void balanceBounds(Tree& t) {
    // Pior caso para BST comum: sequência ordenada de 100 000 chaves.
    t.clear();
    const int n = 100000;
    for (int k = 1; k <= n; ++k) t.insert(k);
    int h = t.height();
    // AVL: h <= 1.4405*log2(n+2); RB: h <= 2*log2(n+1)  (n=1e5 => 24 e 34)
    CHECK(h <= 34, "altura limitada por 2*log2(n+1) (obtida " << h << ")");
    checkValid(t, "n=100000 ordenado");
}

int main() {
    struct Case { const char* name; TestFn fn; };
    const Case cases[] = {
        {"inserção crescente/decrescente/aleatória", insertOrders},
        {"busca de existentes e inexistentes", searchTests},
        {"inserção de valores repetidos", duplicates},
        {"remoção (folha, um filho, dois filhos, raiz, tudo)", removeShapes},
        {"remoção de elementos inexistentes", removeMissing},
        {"árvore vazia e com um elemento", emptyAndSingle},
        {"estresse aleatório vs. oráculo (invariantes a cada operação)", randomStress},
        {"limite de altura com 100 000 chaves ordenadas", balanceBounds},
    };

    for (const char* kind : {"avl", "rb"}) {
        auto tree = makeTree(kind);
        std::cout << "== " << tree->name() << " ==\n";
        for (const auto& c : cases) {
            int before = failures;
            c.fn(*tree);
            std::cout << "  [" << (failures == before ? "OK" : "FALHOU") << "] " << c.name << "\n";
        }
    }
    std::cout << "\n" << checks << " verificações, " << failures << " falhas.\n";
    return failures ? 1 : 0;
}
