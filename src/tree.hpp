
#ifndef TREE_HPP
#define TREE_HPP

#include <cstddef>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

class Tree {
public:
    virtual ~Tree() = default;

    // Retorna false (sem alterar a árvore) se a chave já existir.
    virtual bool insert(int key) = 0;
    // Retorna false (sem alterar a árvore) se a chave não existir.
    virtual bool remove(int key) = 0;
    virtual bool search(int key) const = 0;

    // Chaves em ordem crescente.
    virtual std::vector<int> inOrder() const = 0;
    // Visualização "deitada": subárvore direita no topo, esquerda embaixo.
    // Se ansi == true, nós vermelhos são coloridos no terminal.
    virtual void print(std::ostream& os, bool ansi = false) const = 0;

    virtual int height() const = 0;
    virtual std::size_t size() const = 0;
    virtual void clear() = 0;

    // Verifica todas as invariantes da estrutura (ordem BST + balanceamento).
    // Em caso de falha, escreve o motivo em *why (se não nulo).
    virtual bool validate(std::string* why = nullptr) const = 0;

    // Contadores acumulados desde a criação (ou desde resetStats()).
    virtual long rotations() const = 0;
    virtual long recolorings() const { return 0; }
    virtual void resetStats() = 0;

    virtual std::string name() const = 0;
};

// "avl" ou "rb"; retorna nullptr para nome desconhecido.
std::unique_ptr<Tree> makeTree(const std::string& kind);

#endif
