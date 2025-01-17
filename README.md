# Software Básico 2024.2: Montador e Ligador

Autor: Caleb Martim 

Descrição: Um montador e um ligador para o "Assembly Inventado" da disciplina.

Sistema operacional utilizado: Arch Linux, versaão do kernel: 6.12.9-arch1-1

Como compilar:

```
g++ -std=c++17 MONTADOR.cpp -o montador
```

```
g++ -std=c++17 LIGADOR.cpp -o ligador
```

Como usar:

```
./montador a.asm
``` 

```
./montador a.pre
```

```
./ligador a.obj b.obj
```

## Considerações:

1) Estamos assumindo que o usuário sempre passará como argumento no montador um arquivo .pre ou um
arquivo .asm no montador

2) Estamos assumindo que o arquivo .asm corretamente divide o programa em SECTION TEXT e SECTION DATA

3) Estamos assumindo que o SECTION DATA contém apenas rótulos e não instruções

4) Estamos assumindo que na diretiva SPACE sempre é passado um número inteiro válido 

6) Estamos assumindo que quando o arquivo .asm é um módulo, o seu nome é sempre dado após a diretiva BEGIN

7) É importante que o arquivo ```auxiliar.hpp``` esteja baixado e encontrado no mesmo diretório de ```MONTADOR.cpp```
e ```LIGADOR.cpp``` 

8) A diretiva ```using namespace std;``` é feita para que não seja necessário inserir o prefixo ```std::``` na 
instância de objetos da biblioteca padrão 

9) Note que foi usado uma macro, ```sz(v)``` para se obter o tamanho de containers (vector e string)

## Coisas que não foram possíveis fazer:

1) Implementar operações de soma em labels, por exemplo, ```LOAD LABEL + 2```
