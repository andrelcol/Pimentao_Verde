#!/bin/sh
# Gera o pacote Pimentao_Verde para envio à competição.
# Executar a partir de: pimentao_verde-soccer-simulation-team/
# Requer: build já feito (./script/build_all.sh) ou librcsc em ${ROOT}/pimentao_verde_lib

set -e
cd "$(dirname "$0")/.."
ROOT="$(pwd)"
BUILD_SRC="${ROOT}/build/src"
PACKAGE_NAME="Pimentao_Verde"

# Onde está librcsc: LIB_DIR (env), TEAM_LIB_DIR (env), ou procurar em paths habituais
if [ -n "${LIB_DIR:-}" ] && [ -d "$LIB_DIR" ] && ( [ -f "$LIB_DIR/librcsc.so" ] || ls "$LIB_DIR"/librcsc.so* >/dev/null 2>&1 ); then
  : # usar LIB_DIR do ambiente
else
  LIB_DIR=""
  if [ -n "$TEAM_LIB_DIR" ]; then
    [ -d "$TEAM_LIB_DIR/lib" ] && ( [ -f "$TEAM_LIB_DIR/lib/librcsc.so" ] || ls "$TEAM_LIB_DIR/lib"/librcsc.so* >/dev/null 2>&1 ) && LIB_DIR="$TEAM_LIB_DIR/lib"
    [ -z "$LIB_DIR" ] && [ -d "$TEAM_LIB_DIR" ] && ( [ -f "$TEAM_LIB_DIR/librcsc.so" ] || ls "$TEAM_LIB_DIR"/librcsc.so* >/dev/null 2>&1 ) && LIB_DIR="$TEAM_LIB_DIR"
  fi
  if [ -z "$LIB_DIR" ]; then
    for d in "${ROOT}/pimentao_verde_lib/lib" \
             "${HOME}/local/pimentao_verde_lib/lib" \
             "${HOME}/.local/pimentao_verde_lib/lib" \
             "${HOME}/local/cyruslib/lib" \
             "/usr/local/lib"; do
      if [ -d "$d" ] && ( [ -f "$d/librcsc.so" ] || ls "$d"/librcsc.so* >/dev/null 2>&1 ); then
        LIB_DIR="$d"
        break
      fi
    done
  fi
fi

if [ ! -d "$BUILD_SRC" ]; then
  echo "Erro: não encontrado ${BUILD_SRC}. Corra primeiro o build (cmake + make)."
  exit 1
fi
if [ ! -f "${BUILD_SRC}/sample_player" ] || [ ! -f "${BUILD_SRC}/sample_coach" ]; then
  echo "Erro: binários não encontrados em ${BUILD_SRC}. Corra 'make' no build."
  exit 1
fi
if [ -z "$LIB_DIR" ] || [ ! -d "$LIB_DIR" ]; then
  echo "Erro: librcsc não encontrada. Corra ./script/build_all.sh ou defina TEAM_LIB_DIR / LIB_DIR."
  exit 1
fi
echo "A usar librcsc em: ${LIB_DIR}"

rm -rf "${ROOT}/dist_tmp" "${ROOT}/${PACKAGE_NAME}"
mkdir -p "${ROOT}/dist_tmp/${PACKAGE_NAME}"
cp -r "${BUILD_SRC}"/* "${ROOT}/dist_tmp/${PACKAGE_NAME}/"
# Remover ficheiros de build desnecessários
rm -rf "${ROOT}/dist_tmp/${PACKAGE_NAME}/CMakeFiles" \
       "${ROOT}/dist_tmp/${PACKAGE_NAME}/Makefile" \
       "${ROOT}/dist_tmp/${PACKAGE_NAME}/cmake_install.cmake" \
       "${ROOT}/dist_tmp/${PACKAGE_NAME}/.o" 2>/dev/null || true
# Incluir librcsc para execução sem instalação no sistema
mkdir -p "${ROOT}/dist_tmp/${PACKAGE_NAME}/lib"
cp "${LIB_DIR}"/librcsc.so* "${ROOT}/dist_tmp/${PACKAGE_NAME}/lib/"
# Scripts de arranque e kill
cp "${ROOT}/script/start" "${ROOT}/script/kill" "${ROOT}/dist_tmp/${PACKAGE_NAME}/"
chmod +x "${ROOT}/dist_tmp/${PACKAGE_NAME}/start" "${ROOT}/dist_tmp/${PACKAGE_NAME}/kill" "${ROOT}/dist_tmp/${PACKAGE_NAME}/"*.sh 2>/dev/null || true

cd "${ROOT}/dist_tmp"
tar -czvf "${ROOT}/${PACKAGE_NAME}.tar.gz" "${PACKAGE_NAME}"
cd "${ROOT}"
rm -rf "${ROOT}/dist_tmp"
echo ""
echo "Pacote criado: ${ROOT}/${PACKAGE_NAME}.tar.gz"
echo "Conteúdo: binários (sample_player, sample_coach), configs, formations-dt, lib, start/kill."
echo "Para testar: descompacte e, dentro da pasta ${PACKAGE_NAME}, execute ./start <host> . <número>"
