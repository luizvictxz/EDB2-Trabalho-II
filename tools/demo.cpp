#include <iostream>
#include <vector>

#include "tree.hpp"

namespace {

void show(const Tree& t, const std::string& title) {
    std::cout << "--- " << title << " (altura " << t.height() << ", rotações " << t.rotations();
    if (t.name() != "AVL") std::cout << ", recolorações " << t.recolorings();
    std::cout << ") ---\n";
    t.print(std::cout, false);
    std::cout << "\n";
}

void insertAll(Tree& t, const std::vector<int>& keys) {
    for (int k : keys) t.insert(k);
}

void basicOps(const std::string& kind) {
    auto t = makeTree(kind);
    std::cout << "################ Operações em " << t->name() << " ################\n\n";
    insertAll(*t, {50, 30, 70, 20, 40, 60, 80, 10, 25, 35});
    show(*t, "Após inserir 50 30 70 20 40 60 80 10 25 35");

    std::cout << "inserir 40 (repetido)  -> " << (t->insert(40) ? "inserido" : "rejeitado: já existe") << "\n";
    std::cout << "buscar 35              -> " << (t->search(35) ? "encontrado" : "não encontrado") << "\n";
    std::cout << "buscar 99              -> " << (t->search(99) ? "encontrado" : "não encontrado") << "\n";
    std::cout << "remover 99             -> " << (t->remove(99) ? "removido" : "não encontrado") << "\n";
    std::cout << "remover 10 (folha)     -> " << (t->remove(10) ? "removido" : "não encontrado") << "\n";
    std::cout << "remover 30 (2 filhos)  -> " << (t->remove(30) ? "removido" : "não encontrado") << "\n";
    std::cout << "remover 50 (raiz)      -> " << (t->remove(50) ? "removido" : "não encontrado") << "\n\n";
    show(*t, "Após as remoções");

    std::cout << "Em ordem: ";
    for (int k : t->inOrder()) std::cout << k << " ";
    std::string why;
    std::cout << "\nInvariantes: " << (t->validate(&why) ? "OK" : why) << "\n\n";
}

void avlRotations() {
    std::cout << "################ AVL: exemplos de rotação ################\n\n";
    auto t = makeTree("avl");
    insertAll(*t, {30, 20});
    show(*t, "ANTES: 30, 20 (balanceada)");
    // O próximo nó (10) desbalanceia a raiz (fb=+2): caso Esquerda-Esquerda.
    std::cout << ">> inserindo 10 -> raiz 30 com fb=+2 (caso Esq-Esq): rotação simples à direita\n\n";
    t->insert(10);
    show(*t, "DEPOIS: rotação simples à direita em 30");

    auto d = makeTree("avl");
    insertAll(*d, {10, 30});
    show(*d, "ANTES: 10, 30");
    std::cout << ">> inserindo 20 -> raiz 10 com fb=-2, filho direito com fb=+1 (caso Dir-Esq): "
                 "rotação dupla\n\n";
    d->insert(20);
    show(*d, "DEPOIS: rotação dupla (direita em 30, esquerda em 10)");
}

void rbAdjustments() {
    std::cout << "################ Rubro-Negra: exemplos de ajuste ################\n\n";
    {
        auto t = makeTree("rb");
        insertAll(*t, {10, 5, 15});
        show(*t, "ANTES: 10(B) com filhos 5(R) e 15(R)");
        std::cout << ">> inserindo 1 -> pai (5) e tio (15) vermelhos: caso 1, só RECOLORAÇÃO\n\n";
        t->insert(1);
        show(*t, "DEPOIS: 5 e 15 pretos, 10 vermelho -> raiz repintada de preto");
    }
    {
        auto t = makeTree("rb");
        insertAll(*t, {10, 20});
        show(*t, "ANTES: 10(B) -> 20(R)");
        std::cout << ">> inserindo 30 -> pai vermelho, tio preto (NIL), alinhado: caso 3, "
                     "ROTAÇÃO à esquerda + recoloração\n\n";
        t->insert(30);
        show(*t, "DEPOIS: 20(B) na raiz, 10(R) e 30(R)");
    }
    {
        auto t = makeTree("rb");
        insertAll(*t, {10, 30});
        show(*t, "ANTES: 10(B) -> 30(R)");
        std::cout << ">> inserindo 20 -> tio preto, nó \"dobrado\": caso 2, ROTAÇÃO DUPLA\n\n";
        t->insert(20);
        show(*t, "DEPOIS: 20(B) na raiz");
    }
}

}  // namespace

int main() {
    basicOps("avl");
    basicOps("rb");
    avlRotations();
    rbAdjustments();
    return 0;
}
