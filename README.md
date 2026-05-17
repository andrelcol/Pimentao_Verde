# Pimentao_Verde – Soccer Simulation 2D Team

Equipa **Pimentao_Verde** / **Rinobot-Team** para a RoboCup Soccer 2D Simulation League.

Este repositório inclui **equipa + biblioteca librcsc** (`lib/`), para qualquer pessoa clonar, compilar e gerar o pacote de competição.

Créditos ao upstream CYRUS/HELIOS: **CREDITS.md**.

## Requisitos

- Linux, CMake ≥ 3.5, g++ (C++14 equipa, C++17 lib)
- Boost (`libboost-system-dev`), zlib
- [rcssserver](https://github.com/rcsoccersim/rcssserver) para jogar
- Git (para clonar; a pasta `lib/` já vem no repositório)

## Portabilidade (qualquer máquina / pasta)

| Pergunta | Resposta |
|----------|----------|
| Preciso de lib em `$HOME/local`? | **Não.** `build_all.sh` compila `lib/` → `./pimentao_verde_lib/`. |
| Funciona em qualquer diretório? | **Sim**, desde que clones e corras os scripts na **raiz** do repo. |
| Pacote de competição? | Usa `./lib` ao lado dos binários — não depende do PC que compilou. |
| Caminhos com espaços? | Preferir caminhos simples (ex. `~/Pimentao_Verde`). Espaços/`()` podem afetar o **rcssserver** nos logs. |
| Mudei a pasta do projeto? | Correr de novo `./script/build_all.sh`. |

## Compilar tudo (lib + jogador + treinador)

```bash
git clone https://github.com/andrelcol/Pimentao_Verde.git
cd Pimentao_Verde
chmod +x script/build_all.sh script/make_competition_package.sh
./script/build_all.sh
```

Instala a lib em `./pimentao_verde_lib/` e gera binários em `./build/src/`.

## Pacote para competição

```bash
./script/make_competition_package.sh
```

Cria `Pimentao_Verde.tar.gz` com `sample_player`, `sample_coach`, `formations-dt`, `lib/librcsc.so*` e `start`/`kill`.

## Estrutura

| Pasta | Conteúdo |
|-------|----------|
| `src/` | Código da equipa |
| `lib/` | Código-fonte **pimentao_verde_lib** (librcsc) |
| `pimentao_verde_lib/` | Lib instalada pelo build (não versionada) |
| `build/` | Build da equipa (não versionado) |
| `script/` | `build_all.sh`, `make_competition_package.sh`, `start`, `kill` |

## Correr localmente

```bash
# Terminal 1
rcssserver

# Terminal 2 (após build_all.sh)
export LD_LIBRARY_PATH="$(pwd)/pimentao_verde_lib/lib:$LD_LIBRARY_PATH"
./build/src/sample_player -h localhost -p 6000 -t Rinobot-Team -n 9 \
  -f src/data/settings/pimentao_verde.json
```

## Build manual (opcional)

```bash
# Só a lib
mkdir -p lib/build && cd lib/build
cmake .. -DCMAKE_INSTALL_PREFIX=../../pimentao_verde_lib -DCMAKE_BUILD_TYPE=Release
make -j$(nproc) && make install
cd ../..

# Só a equipa
mkdir -p build && cd build
cmake .. -DTEAM_DIR_LIB=../pimentao_verde_lib -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```
