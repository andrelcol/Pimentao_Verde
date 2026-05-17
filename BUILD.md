# Compilar Pimentao_Verde (equipa + lib)

## Um comando

```bash
./script/build_all.sh
```

Compila `lib/` → `./pimentao_verde_lib/` e a equipa → `./build/src/`.

## Pacote de competição

```bash
./script/make_competition_package.sh
```

Gera `Pimentao_Verde.tar.gz` na raiz do projeto.

## Dependências do sistema

CMake, g++, Boost (system), zlib, git.

A librcsc **não** precisa de estar instalada em `$HOME/local` — vem em `lib/` neste repositório.

## Caminhos e portabilidade

- Tudo é **relativo à raiz do clone**; não precisas de `$HOME/local/...` se correres `build_all.sh`.
- A lib instala-se em `./pimentao_verde_lib/` dentro do projeto.
- Scripts (`start.sh`, `github.sh`) procuram `./lib` ou sobem diretórios até achar `pimentao_verde_lib/`.
- Evita clonar para pastas com **espaços ou parênteses** no caminho se o rcssserver falhar nos logs.
- Se mudares o projeto de sítio: volta a correr `./script/build_all.sh`.
