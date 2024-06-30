# Implementação do Algoritmo de Ricart-Agrawala com MPI

Este projeto implementa o algoritmo de Ricart-Agrawala utilizando MPI (Message Passing Interface) em linguagem C. O código foi desenvolvido para coordenar o acesso à seção crítica entre vários processos.

## Descrição do Código

### Funções Implementadas

- **`isFalse`**: Função para verificar se há pelo menos um valor `false` em um vetor de booleanos.
- **`generate_timestamps`**: Função para gerar timestamps aleatórios para cada processo. Diferente do código base original que tem um input.txt com os timestamps gerados, nós implementamos timestamps aleatórios
- **`ricart_agrawala`**: Implementação do algoritmo de Ricart-Agrawala para exclusão mútua.

### Detalhes de Implementação

O algoritmo utiliza troca de mensagens entre processos para coordenar o acesso exclusivo à seção crítica. Cada processo possui um timestamp gerado aleatoriamente, que é usado para determinar a prioridade de acesso à seção crítica. O processo aguarda permissões de todos os outros processos antes de entrar na seção crítica.

### Compilação e Execução

Para compilar o programa, utilize um compilador C com suporte a MPI, como `mpicc`:

```bash
mpicc ricart_agrawala.c -o ricart_agrawala
```
Para executar o programa com mpirun e 4 processos, por exemplo:
```bash
mpirun -np 4 ./ricart_agrawala
```

### Requisitos

Certifique-se de ter MPI instalado e configurado no seu sistema. Você pode instalá-lo utilizando os seguintes comandos (Ubuntu):

```bash
sudo apt update
sudo apt install openmpi-bin openmpi-common libopenmpi-dev
```
