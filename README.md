# Software Básico 2024.2: Montador e Ligador

Autor: Caleb Martim 

Descrição: Um montador e um ligador para o "Assembly inventado" da disciplina.

<!-- Sistema Operacional: Arch Linux, Kernel: 6.12.6-arch1-1 -->

Como compilar:

```g++ -std=c++17 montador.cpp -o montador```

```g++ -std=c++17 ligador.cpp -o ligador```

Como usar:

```./montador a.asm``` 

```./montador a.pre```

```./ligador a.obj b.obj```

## Considerações:

1) Estamos assumindo que o usuário sempre passará como argumento no montador um arquivo .pre ou um
arquivo .asm

2) Estamos assumindo que valores inteiros passados nas diretivas CONST e EQU
podem ser representados por um valor do tipo int, isto é, ele pertence ao intervalo 
[-2^{31}, 2^{31} - 1] 

4) Se for verificado algum erro sintático, a montagem para imediatamnete.

5) Estamos assumindo que o arquivo .pre corretamente divide o programa em SECTION TEXT e SECTION DATA

## Coisas que não foram possíveis fazer:

1) Implementar o ligador

2) Implementar operações de soma em labels, por exemploe, ```LOAD LABEL + 2```

3) Implementar a funcionalidade da diretiva ```IF```
