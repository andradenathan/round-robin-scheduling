# Escalonamento com Round Robin

Projeto desenvolvido com o objetivo de visualizar o escalonamento de processos com o algoritmo de Round Robin.


### Escopo
Desenvolver um simulador que implementa o algoritmo de escalonamentos de processos com a estratégia de seleção Round Robin com Feedback.

### Premissas
- Limite máximo de processos criados;
- O valor da fatia de tempo dada aos processos em execução;
- Tempos de serviço e de I/O aleatórios para cada processo criado;
- Tempos de duração de cada tipo de I/O (disco, fita magnética e impressora);
- Gerência de processos
    - Definição de PID de cada processo;
    - Informações do PCB - prioridade, PID, PPID, status;
    - Escalonador (3 filas, sendo uma de alta e uma de baixa prioridade para execução na CPU, 1 fila de I/O implementada com filas diferentes para cada tipo de dispositivo)s
- Tipos de I/O
    - Disco: retorna para fila de baixa prioridade;
    - Fita magnética - retorna para fila de alta prioridade;
    - Impressora - retorna para fila de alta prioridade;
- Ordem de entrada na fila de prontos
    - Processos novos: fila de alta prioridade;
    - Processos que retornam de I/O: dependente do tipo de I/O solicitado;
    - Processos que sofreram preempção: retornam na fila de baixa prioridade.
