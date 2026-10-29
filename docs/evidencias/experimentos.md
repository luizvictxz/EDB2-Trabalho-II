# Resultados do experimento (mesmos dados nas três estruturas)

Altura: árvore vazia = 0, um nó = 1. Remoção: 50% das chaves, em ordem embaralhada (semente fixa).
Limites teóricos: AVL h <= 1,44*log2(n+2); Rubro-Negra h <= 2*log2(n+1).

## Após inserir n chaves

| Dataset | n | BST h | AVL h | RN h | AVL rot. | RN rot. | RN recol. | AVL ms | RN ms |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| crescente | 1000 | 1000 | 10 | 17 | 990 | 983 | 4909 | 0.07 | 0.08 |
| crescente | 10000 | 10000 | 14 | 24 | 9986 | 9976 | 49877 | 0.92 | 0.88 |
| crescente | 100000 | n/d | 17 | 31 | 99983 | 99969 | 499846 | 13.84 | 12.04 |
| decrescente | 1000 | 1000 | 10 | 17 | 990 | 983 | 4909 | 0.05 | 0.04 |
| decrescente | 10000 | 10000 | 14 | 24 | 9986 | 9976 | 49877 | 0.67 | 0.61 |
| decrescente | 100000 | n/d | 17 | 31 | 99983 | 99969 | 499846 | 11.62 | 13.23 |
| aleatória | 1000 | 22 | 12 | 12 | 694 | 556 | 2297 | 0.09 | 0.06 |
| aleatória | 10000 | 30 | 16 | 16 | 6970 | 5812 | 23200 | 1.43 | 0.81 |
| aleatória | 100000 | n/d | 20 | 20 | 70286 | 58511 | 231941 | 29.61 | 22.67 |
| quase ordenada | 1000 | 171 | 12 | 17 | 1173 | 1176 | 4329 | 0.08 | 0.05 |
| quase ordenada | 10000 | 369 | 15 | 23 | 12036 | 12708 | 43777 | 0.94 | 0.75 |
| quase ordenada | 100000 | n/d | 19 | 30 | 119711 | 126056 | 438138 | 10.87 | 8.72 |

## Após remover 50% das chaves (sobre as árvores acima)

| Dataset | n | AVL h | RN h | AVL rot. (acum.) | RN rot. (acum.) | RN recol. (acum.) |
|---|---:|---:|---:|---:|---:|---:|
| crescente | 1000 | 10 | 10 | 1089 | 1099 | 6129 |
| crescente | 10000 | 14 | 14 | 10827 | 10965 | 62095 |
| crescente | 100000 | 17 | 18 | 108372 | 109112 | 620595 |
| decrescente | 1000 | 10 | 10 | 1055 | 1068 | 6100 |
| decrescente | 10000 | 14 | 15 | 10816 | 10922 | 61990 |
| decrescente | 100000 | 17 | 19 | 108380 | 109259 | 620841 |
| aleatória | 1000 | 11 | 11 | 886 | 732 | 3170 |
| aleatória | 10000 | 15 | 16 | 8815 | 7589 | 31973 |
| aleatória | 100000 | 19 | 20 | 88964 | 76669 | 320321 |
| quase ordenada | 1000 | 11 | 12 | 1343 | 1339 | 5508 |
| quase ordenada | 10000 | 15 | 15 | 13724 | 14240 | 55387 |
| quase ordenada | 100000 | 19 | 20 | 136214 | 141733 | 554946 |

## Altura teórica de referência

| n | log2(n) | 1,44*log2(n+2) | 2*log2(n+1) |
|---:|---:|---:|---:|
| 1000 | 9.97 | 14.35 | 19.93 |
| 10000 | 13.29 | 19.13 | 26.58 |
| 100000 | 16.61 | 23.92 | 33.22 |

## Busca (n = 100000 aleatório, 2 000 000 consultas)

| Estrutura | Achados | Tempo (ms) |
|---|---:|---:|
| AVL | 999422 | 338.70 |
| Rubro-Negra | 999422 | 326.70 |
