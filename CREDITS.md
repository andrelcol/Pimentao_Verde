# Créditos e origem

## Pimentao_Verde – Equipe (team) 2D Soccer Simulation

Este projeto é uma **modificação/derivação** do **CYRUS 2D Soccer Simulation Team**.

### Copyright do projeto original (CYRUS)

- **CYRUS 2D Soccer Simulation Team** – código da equipa (player, coach), táticas, roles e utilitários.
- Repositório original: https://github.com/Cyrus2D/cyrus-soccer-simulation-team
- CYRUS é uma equipa de longa data na RoboCup Soccer 2D Simulation League (incluindo Campeonato do Mundo 2021). O código base assenta no CYRUS base e no HELIOS base, usando librcsc e Boost.

### Referências e publicações CYRUS

- Zare, N., Sayareh, A., Sarvmaili, M., Amini, O., Matwin, S., Soares, A., e outros: múltiplos artigos e Team Description Papers do CYRUS (RoboCup 2013–2024). Ver README e referências no repositório original do CYRUS.
- Hidehisa Akiyama, Tomoharu Nakashima: HELIOS Base (RoboCup Soccer 2D Simulation). Springer, 2014.

### Esta derivação

- **Pimentao_Verde**: identidade da equipa (nome Pimentao_Verde, treinador Gordiola), renomeação de pastas e configurações de build.
- **Alterações de lógica** (além do nome): ver **CUSTOMIZATIONS.md** (config base em `data/settings/pimentao_verde.json`, carregamento our/their team, e pequenos ajustes em `Strategy::getNormalDashPower` e em `Setting::SetTeamName`).
- O restante comportamento, táticas e código mantêm o copyright e os créditos do projeto CYRUS e do HELIOS base.
