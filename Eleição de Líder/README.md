# Algoritmo de Eleição de Líder de Franklin

## Funcionamento

O algoritmo de Eleição de Líder de Franklin pode ser utilizado em sistemas que organizam todos os processos em um anel.

Inicialmente, todos os processos são considerados *ativos*, e cada processo possui um *ID* único. Processos *ativos* são aqueles que se comunicam ativamente com seus vizinhos, enquanto que processos *passivos* apenas retransmitem as mensagens de seus vizinhos da esquerda para os seus vizinhos da direita e vice-versa.

Quando uma eleição é iniciada, cada processo se comunica com seus vizinhos imediatos da esquerda e da direita, informando-os do seu *ID* e recebendo de volta os *IDs* deles.

Quando um processo compara o seu *ID* com os *IDs* de seus vizinhos e percebe que o seu é o menor deles, ele se torna um processo *passivo*.

Ao final, uma nova rodada de comunicações é iniciada e o processo se repete até que reste apenas um nó *ativo* no anel. Isto é percebido quando um nó detecta que o seu *ID* é idêntico ao de ambos os seus vizinhos (i.e., quando um processo detecta que ele é o próprio vizinho tanto à esquerda quanto à direita).

O processo restante é o processo com maior *ID* do anel, e ele é escolhido para ser o novo líder.

## Como compilar?

`mpicc main.c -o main`

## Como rodar?

`mpirun --hostfile hostfile -np [X] main`
- Onde X é um inteiro entre 1 e 10

## Referências

[https://github.com/tasmay/Franklin/tree/master](https://github.com/tasmay/Franklin/tree/master)
