# AGENTS.md

Este arquivo define como agentes (humanos ou IA) devem trabalhar neste repositorio.

## Fontes de verdade

- `docs/project/PROJECT_MEMORY.md`: memoria viva do projeto, decisoes e contexto tecnico.
- `docs/process/AGENT_STATUS.md`: quadro de status, backlog, bloqueios e log de sessoes.
- `docs/hardware/PINMAP.md`: pinagem oficial consolidada.
- `docs/software/IMPLEMENTATION_PHASES.md`: plano de software e fases de implementacao.
- `src/config/pins.h`: pinagem efetiva do firmware ativo.

## Protocolo de handoff entre agentes

1. Ler `docs/project/PROJECT_MEMORY.md` e `docs/process/AGENT_STATUS.md` antes de qualquer mudanca.
2. Registrar no `docs/process/AGENT_STATUS.md`:
- data da sessao
- o que foi feito
- decisoes tomadas
- bloqueios encontrados
- proximo passo claro
3. Se houver decisao que impacta hardware (PCB), atualizar tambem `docs/project/PROJECT_MEMORY.md`.
4. Nunca sobrescrever historico: sempre append no log de sessoes.

## Regra para mudancas que impactam PCB

Sempre marcar a decisao como uma destas classes:
- `LOCKED`: definido para a revisao atual de PCB.
- `CANDIDATE`: proposta forte, ainda aberta.
- `DEFERRED`: adiado para revisao futura.

Toda mudanca em pinagem, sensor, alimentacao ou barramento deve atualizar:
- `docs/project/PROJECT_MEMORY.md`
- `docs/process/AGENT_STATUS.md`
- `docs/hardware/PINMAP.md`
- e, quando aplicavel, `src/config/pins.h`

## Definicao de pronto (documentacao de status)

Uma tarefa so pode ser marcada como concluida se tiver:
- criterio de aceite objetivo
- arquivo(s) alterado(s) listados
- evidencias minimas (build, teste ou inspecao documentada)
