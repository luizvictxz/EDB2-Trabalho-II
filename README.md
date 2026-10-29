# Amazon Engineering Challenge — Árvores AVL e Rubro-Negra

IMD0039 – Estruturas de Dados Básicas II

**Autor:** Luiz Carlos Victor Pinto

Protótipo (C++17, sem bibliotecas de balanceamento) para registrar, localizar e remover
identificadores de encomendas durante a Black Friday. O usuário escolhe, na execução, entre
**árvore AVL** e **árvore Rubro-Negra**.

## Compilar e executar

Requisitos: `g++` com suporte a C++17 e `make`.

```bash
make            # compila tudo em ./bin
make run        # programa interativo (menu)
make test       # suíte de testes (as duas árvores)
make experiments  # análise comparativa (tabelas em Markdown)
make demo       # exemplos de rotação / recoloração e operações básicas
make evidencias # regenera docs/evidencias/*
```

## Funcionalidades (menu do programa)

| Opção                                 | Descrição                                                                                                                                                                   |
| ------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Inserir / Remover / Buscar            | Operações básicas; retorno informa sucesso, repetido ou inexistente                                                                                                         |
| Percurso em ordem                     | Elementos em ordem crescente                                                                                                                                                |
| Imprimir árvore                       | Visualização deitada (raiz à esquerda, subárvore direita no topo). AVL mostra o fator de balanceamento `fb`; Rubro-Negra mostra `(R)`/`(B)` (vermelho colorido em terminal) |
| Estatísticas                          | Tamanho, altura, nº de rotações, nº de recolorações, verificação das invariantes                                                                                            |
| Inserir N aleatórios / sequência 1..N | Para experimentar a ordem de chegada dos dados                                                                                                                              |

Exemplo:

```
$ ./bin/arvores
> 2            (escolhe Rubro-Negra)
> 1, 10        (inserir 10)    > 1, 20    > 1, 30
> 5            (imprimir árvore)
      30 (R)
20 (B)
      10 (R)
```

## Decisões de implementação

- **Chaves:** `int`. **Repetidos:** inserção rejeitada, árvore inalterada (`insert` retorna `false`).
- **Remoção/busca de inexistente:** `remove` retorna `false` e nada muda; `search` retorna `false`.
- **Altura:** árvore vazia = 0; um nó = 1 (mesma convenção nas duas árvores).
- **AVL:** recursiva; `rebalance` aplica rotação simples ou dupla conforme o fator de balanceamento.
  Remoção com dois filhos usa o **sucessor em-ordem**. Rotação dupla conta como **2 rotações**.
- **Rubro-Negra:** formulação de Cormen et al. com **nó sentinela NIL** e ponteiro para o pai
  (inserção em 3 casos, remoção em 4). Remoção com dois filhos usa o sucessor em-ordem.
  Contabiliza **rotações** e **recolorações** (cada mudança efetiva de cor).
- **Validação:** `Tree::validate()` confere ordem BST e todas as invariantes (AVL: |fb| ≤ 1 e alturas
  corretas; RN: raiz preta, sem vermelho-vermelho, mesma altura-negra, ponteiros de pai).
- **Testes:** `std::set` é usado **apenas como oráculo** nos testes, nunca nas árvores.

## Estrutura do repositório

```
src/        tree.hpp (interface), avl_tree.*, rb_tree.*, tree_factory.cpp, main.cpp
tests/      tests.cpp  — suíte de testes
tools/      experiments.cpp (análise comparativa), demo.cpp (evidências de rotação)
docs/       evidencias/ (saídas reais: testes, experimentos e demonstração)
bin/        binários compilados (Linux x86-64; em outro sistema use `make`)
Makefile
```

## Testes cobertos

Inserção crescente/decrescente/aleatória · busca de existentes e inexistentes · valores repetidos ·
remoção de folha, nó com um filho, nó com dois filhos e raiz · remoção de inexistentes · árvore vazia
e com um elemento · 20 000 operações aleatórias comparadas ao oráculo com validação das invariantes
**a cada operação** · limite de altura com 100 000 chaves ordenadas.
Saída: `docs/evidencias/testes.txt` (7372 verificações, 0 falhas).
