#!/bin/sh
# Arranca rcssserver (se necessário) e a equipa. Uso: ./script/run_local.sh [-i]

set -e
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
TEAM_DIR="${ROOT}/build/src"
PORT="${RCSS_PORT:-6000}"

if [ ! -x "${TEAM_DIR}/start.sh" ]; then
  echo "ERRO: compile primeiro: cd ${ROOT} && ./script/build_all.sh" 1>&2
  exit 1
fi

if ! command -v rcssserver >/dev/null 2>&1; then
  echo "ERRO: rcssserver não está no PATH." 1>&2
  exit 1
fi

server_up() {
  if command -v nc >/dev/null 2>&1; then
    nc -z localhost "$PORT" 2>/dev/null
    return $?
  fi
  if command -v ss >/dev/null 2>&1; then
    ss -tln 2>/dev/null | grep -q ":${PORT} "
    return $?
  fi
  return 1
}

if ! server_up; then
  echo "A iniciar rcssserver na porta ${PORT}..."
  rcssserver "server::port=${PORT}" >/tmp/rcssserver.log 2>&1 &
  _i=0
  while ! server_up; do
    _i=$((_i + 1))
    if [ "$_i" -gt 30 ]; then
      echo "ERRO: rcssserver não abriu a porta ${PORT}. Ver /tmp/rcssserver.log" 1>&2
      tail -20 /tmp/rcssserver.log 1>&2
      exit 1
    fi
    sleep 0.2
  done
  echo "rcssserver OK (log: /tmp/rcssserver.log)"
fi

pkill -f "${TEAM_DIR}/sample_player" 2>/dev/null || true
pkill -f "${TEAM_DIR}/sample_coach" 2>/dev/null || true
sleep 1

cd "${TEAM_DIR}"
exec ./start.sh "$@"
