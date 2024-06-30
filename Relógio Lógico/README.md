# Implementação do Relógio Lógico de Lamport com MPI

Este projeto implementa o relógio lógico de Lamport utilizando MPI (Message Passing Interface) em linguagem C. O código simula eventos de envio, recebimento e eventos internos em processos distribuídos, utilizando o conceito de relógio lógico para ordenação causal.

## Descrição do Código
  
### Detalhes de Implementação

O código simula processos distribuídos que executam eventos de envio (`SEND`), recebimento (`RECV`) e eventos internos (`INTERNAL`). O relógio de Lamport é atualizado conforme os eventos são processados, garantindo uma ordenação parcial dos eventos baseada na relação causal entre eles.

### Compilação e Execução

Para compilar o programa, utilize um compilador C com suporte a MPI, como `mpicc`:

```bash
mpicc lamport_clock.c -o lamport_clock
```

Para executar o programa com mpirun e 4 processos, por exemplo:
```bash
mpirun -np 4 ./lamport_clock
```

### Requisitos

Certifique-se de ter MPI instalado e configurado no seu sistema. Você pode instalá-lo utilizando os seguintes comandos (Ubuntu):
```bash
sudo apt update
sudo apt install openmpi-bin openmpi-common libopenmpi-dev
```
