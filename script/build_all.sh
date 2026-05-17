#!/bin/sh
# Compila librcsc (lib/) + equipa (raiz) e deixa tudo pronto para make_competition_package.sh
set -e

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
LIB_SRC="${ROOT}/lib"
LIB_INSTALL="${ROOT}/pimentao_verde_lib"

case "${ROOT}" in
  *" "*|*"("*|*")"*)
    echo "Aviso: o caminho do projeto tem espaços ou parênteses:"
    echo "  ${ROOT}"
    echo "  Isto costuma funcionar com build_all.sh; evite espaços se tiver erros no rcssserver."
    ;;
esac
TEAM_BUILD="${ROOT}/build"
JOBS="${JOBS:-$(nproc 2>/dev/null || echo 4)}"

if [ ! -f "${LIB_SRC}/CMakeLists.txt" ]; then
  echo "Erro: pasta lib/ não encontrada em ${ROOT}"
  echo "O repositório deve incluir o código de pimentao_verde_lib em lib/"
  exit 1
fi

echo "=== 1/2 librcsc -> ${LIB_INSTALL} ==="
mkdir -p "${LIB_SRC}/build"
cd "${LIB_SRC}/build"
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="${LIB_INSTALL}"
make -j"${JOBS}"
make install

if [ ! -f "${LIB_INSTALL}/lib/librcsc.so" ] && ! ls "${LIB_INSTALL}/lib"/librcsc.so* >/dev/null 2>&1; then
  echo "Erro: librcsc não instalada em ${LIB_INSTALL}/lib"
  exit 1
fi

echo ""
echo "=== 2/2 equipa (sample_player, sample_coach) ==="
mkdir -p "${TEAM_BUILD}"
cd "${TEAM_BUILD}"
rm -f CMakeCache.txt
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DTEAM_DIR_LIB="${LIB_INSTALL}" \
  -DLIBRCSC_INSTALL_DIR="${LIB_INSTALL}"
make -j"${JOBS}"

echo ""
echo "Pronto."
echo "  Lib:    ${LIB_INSTALL}"
echo "  Binários: ${TEAM_BUILD}/src/sample_player ${TEAM_BUILD}/src/sample_coach"
echo "  Pacote:   ${ROOT}/script/make_competition_package.sh"
