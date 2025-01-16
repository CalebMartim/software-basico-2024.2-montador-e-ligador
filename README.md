# Software Básico 2024.2: Montador e Ligador

Autor: Caleb Martim 

Descrição: Um montador e um ligador para o "Assembly inventado" da disciplina.

Sistema operacional utilizado: Arch Linux, versaão do kernel: 6.12.9-arch1-1

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

2) Estamos assumindo que o arquivo .asm corretamente divide o programa em SECTION TEXT e SECTION DATA

## Coisas que não foram possíveis fazer:

1) Implementar o ligador

2) Implementar operações de soma em labels, por exemploe, ```LOAD LABEL + 2```
