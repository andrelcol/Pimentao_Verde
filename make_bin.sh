#!/bin/bash
# Gera o binário e a pasta 'bin' para levar à máquina da organização.
# Uso: ./make_bin.sh
# Resultado: pasta ./bin com sample_player, sample_coach, start.sh, formations, data, team_logo.xpm, etc.

set -e
DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$DIR"
BIN_DIR="$DIR/bin"
BUILD_DIR="$DIR/build"

echo "=== Build Release ==="
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"
if [ ! -f Makefile ]; then
  cmake -DCMAKE_BUILD_TYPE=Release ..
fi
make -j$(nproc 2>/dev/null || echo 2)

echo "=== Criar pasta bin ==="
rm -rf "$BIN_DIR"
mkdir -p "$BIN_DIR"
cp -a "$BUILD_DIR/src/"* "$BIN_DIR/"
[ -f "$DIR/README_BIN_ORGANIZACAO.txt" ] && cp "$DIR/README_BIN_ORGANIZACAO.txt" "$BIN_DIR/README_ORGANIZACAO.txt"

echo "=== Conteúdo de bin ==="
ls -la "$BIN_DIR"

echo ""
echo "Pronto. Pasta gerada: $BIN_DIR"
echo "Para levar à organização: comprime a pasta 'bin' (zip ou tar.gz) e copia para a máquina."
echo "Na máquina da organização: a pasta deve ter librcsc e Boost instalados (ou em \$HOME/local ou path indicado pela organização)."
echo "Executar: cd bin && ./start.sh -i   (ou o comando que a organização indicar)"
