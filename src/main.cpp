#include <unistd.h>

#include <iostream>
#include <limits>
#include <random>
#include <sstream>

#include "tree.hpp"

namespace {

bool readInt(const std::string& prompt, int& out) {
    std::cout << prompt;
    std::string line;
    if (!std::getline(std::cin, line)) return false;  // EOF
    std::istringstream iss(line);
    std::string extra;
    if (!(iss >> out) || (iss >> extra)) {
        out = std::numeric_limits<int>::min();  // sinaliza entrada inválida
    }
    return true;
}

bool valid(int v) { return v != std::numeric_limits<int>::min(); }

void showStats(const Tree& t) {
    std::cout << "Estrutura   : " << t.name() << "\n"
              << "Elementos   : " << t.size() << "\n"
              << "Altura      : " << t.height() << "\n"
              << "Rotações    : " << t.rotations() << "\n";
    if (t.name() != "AVL") std::cout << "Recolorações: " << t.recolorings() << "\n";
    std::string why;
    std::cout << "Invariantes : " << (t.validate(&why) ? "OK" : "VIOLADAS - " + why) << "\n";
}

void menu(const Tree& t) {
    std::cout << "\n=== Árvore " << t.name() << " (" << t.size() << " elementos) ===\n"
              << " 1) Inserir        2) Remover      3) Buscar\n"
              << " 4) Percurso em ordem              5) Imprimir árvore\n"
              << " 6) Estatísticas   7) Inserir N valores aleatórios\n"
              << " 8) Inserir sequência crescente 1..N\n"
              << " 9) Limpar         0) Sair\n";
}

}  // namespace

int main() {
    std::unique_ptr<Tree> tree;
    while (!tree) {
        std::cout << "Escolha a estrutura:\n 1) Árvore AVL\n 2) Árvore Rubro-Negra\n";
        int c;
        if (!readInt("> ", c)) return 0;
        if (c == 1) tree = makeTree("avl");
        else if (c == 2) tree = makeTree("rb");
        else std::cout << "Opção inválida.\n";
    }

    const bool ansi = isatty(STDOUT_FILENO);
    std::mt19937 rng(std::random_device{}());

    for (;;) {
        menu(*tree);
        int op;
        if (!readInt("> ", op)) return 0;
        int v;
        switch (op) {
            case 1:
                if (!readInt("ID a inserir: ", v)) return 0;
                if (!valid(v)) std::cout << "Entrada inválida: informe um inteiro.\n";
                else if (tree->insert(v)) std::cout << "ID " << v << " inserido.\n";
                else std::cout << "ID " << v << " já existe: inserção ignorada.\n";
                break;
            case 2:
                if (!readInt("ID a remover: ", v)) return 0;
                if (!valid(v)) std::cout << "Entrada inválida: informe um inteiro.\n";
                else if (tree->remove(v)) std::cout << "ID " << v << " removido.\n";
                else std::cout << "ID " << v << " não encontrado: nada foi removido.\n";
                break;
            case 3:
                if (!readInt("ID a buscar: ", v)) return 0;
                if (!valid(v)) std::cout << "Entrada inválida: informe um inteiro.\n";
                else std::cout << "ID " << v << (tree->search(v) ? " encontrado.\n" : " NÃO encontrado.\n");
                break;
            case 4: {
                auto keys = tree->inOrder();
                if (keys.empty()) std::cout << "(árvore vazia)\n";
                for (std::size_t i = 0; i < keys.size(); ++i)
                    std::cout << keys[i] << (i + 1 < keys.size() ? " " : "\n");
                break;
            }
            case 5:
                tree->print(std::cout, ansi);
                break;
            case 6:
                showStats(*tree);
                break;
            case 7:
                if (!readInt("Quantidade N: ", v)) return 0;
                if (!valid(v) || v < 0) std::cout << "Entrada inválida.\n";
                else {
                    std::uniform_int_distribution<int> dist(1, 10 * std::max(v, 1));
                    int added = 0;
                    for (int i = 0; i < v; ++i) added += tree->insert(dist(rng));
                    std::cout << added << " valores inseridos (" << v - added << " repetidos ignorados).\n";
                }
                break;
            case 8:
                if (!readInt("Quantidade N: ", v)) return 0;
                if (!valid(v) || v < 0) std::cout << "Entrada inválida.\n";
                else {
                    int added = 0;
                    for (int i = 1; i <= v; ++i) added += tree->insert(i);
                    std::cout << added << " valores inseridos (" << v - added << " repetidos ignorados).\n";
                }
                break;
            case 9:
                tree->clear();
                tree->resetStats();
                std::cout << "Árvore limpa.\n";
                break;
            case 0:
                return 0;
            default:
                std::cout << "Opção inválida.\n";
        }
    }
}
