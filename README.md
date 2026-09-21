# Implementação 4 - PintOS: Alarm Clock

Implementação do Alarm Clock do PintOS. O objetivo é substituir a espera ocupada de `timer_sleep()` por bloqueio da thread até o prazo solicitado, sem retorno antecipado.

## O que foi implementado

- Espera por semáforo para suspender a thread em `timer_sleep()`.
- Fila de despertares ordenada por prazo.
- Despertar de todas as threads cujo prazo venceu na interrupção do timer.
- Tratamento de duração zero ou negativa.
- Testes de bloqueio, concorrência, prazos extremos e entradas inválidas.

## Estrutura

- `src/`: árvore completa do PintOS, incluindo as alterações da implementação.
- `src/devices/timer.c`: lógica principal do Alarm Clock.
- `src/tests/threads/`: testes adicionais da entrega.
- `evidencias.log`: saídas dos testes e da validação.
- `erros.log`: registros de erros e testes deliberadamente incorretos.
- `Makefile`: comandos de compilação e limpeza na raiz do repositório.

## Compilação

No Ubuntu ou WSL, instale as dependências:

```sh
sudo apt-get update
sudo apt-get install build-essential perl qemu-system-x86
```

Na raiz do repositório:

```sh
make clean
make -j4
```

## Execução de um teste

Após compilar:

```sh
export PATH="$PWD/src/utils:$PATH"
cd src/threads/build
pintos -v -k -T 20 --qemu -- -q run alarm-stress
```

Os testes incluem casos positivos, entradas inválidas e mutações intencionais para verificar se os detectores de erro funcionam.

## Base utilizada

PintOS de [jhu-cs318/pintos](https://github.com/jhu-cs318/pintos), commit `ab699959f0d3eae8e59d8507ae2474a638c3d1d3`.
