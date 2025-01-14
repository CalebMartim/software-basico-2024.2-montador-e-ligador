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

Estamos assumindo as seguintes condições:

1) O usuário sempre passará como argumento no montador um arquivo .pre ou um
arquivo .asm

2) Estamos assumindo que valores inteiros passados nas diretivas CONST e EQU
podem ser representados por um valor do tipo int, isto é, ele pertence ao intervalo 
[-2^{31}, 2^{31} - 1]

3) Estamos assumindo, pelo menos por agora, que erros sintáticos, semânticos não são realizados em diretivas IF e EQU

4) Se for verificado algum erro sintático, a montagem para imediatamnete.

5) Estamos assumindo que o arquivo .pre corretamente divide o programa em SECTION TEXT e SECTION DATA
