#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#define sz(v) (int) size(v)
using namespace std;

#ifdef DEBUG
template <typename ...T> void dbg_esp(T... x){
  ((cout << x << ' '), ...);
}
template <typename ...T> void dbg(T... x){
  (dbg_esp(x), ...);
  cout << '\n';
}
#else 
#define dbg(...)
#define dbg_esp(...)
#endif 

unordered_map<string, int> tamanho_da_instrucao = {
  {"ADD", 2},
  {"SUB", 2},
  {"MUL", 2},
  {"MULT", 2},
  {"DIV", 2},
  {"JMP", 2},
  {"JMPN", 2},
  {"JMPP", 2},
  {"JMPZ", 2},
  {"COPY", 3},
  {"LOAD", 2},
  {"STORE", 2},
  {"INPUT", 2},
  {"OUTPUT", 2},
  {"STOP", 1}
};

unordered_map<string, string> opcode_da_instrucao = {
  {"ADD", "1"},
  {"SUB", "2"},
  {"MUL", "3"},
  {"MULT", "3"},
  {"DIV", "4"},
  {"JMP", "5"},
  {"JMPN", "6"},
  {"JMPP", "7"},
  {"JMPZ", "8"},
  {"COPY", "9"},
  {"LOAD", "10"},
  {"STORE", "11"},
  {"INPUT", "12"},
  {"OUTPUT", "13"},
  {"STOP", "14"}
};

unordered_set<string> diretivas_para_ignorar = {
  "END", "SECTION"
};

unordered_map<string, string> mensagem_de_erro = {
  {"rotulo_ausente", "Rótulo não definido."},
  {"rotulo_dobrado", "Dois rótulos definidos na mesma instrução."},
  {"rotulo_redefinido", "Redefinição de rótulo."},
  {"expressao_invalida", "Expressão inválida (número de operandos errado ou formato errado)"},
  {"rotulo_invalido", "Nome inválido para definição de rótulo."},
  {"instrucao_invalida", "Instrução ou diretiva não existente."},
};

const string 
rotulo_ausente = "rotulo_ausente",
rotulo_dobrado = "rotulo_dobrado",
rotulo_redefinido = "rotulo_redefinido",
expressao_invalida = "expressao_invalida",
rotulo_invalido = "rotulo_invalido",
instrucao_invalida = "instrucao_invalida";

vector<string> pegue_tokens(const string & s){
  vector<string> ret;
  string aux;
  for (const char & c : s) {
    if (c == ';') { // optamos por desconsiderar comentários
      break; 
    }
    if (c == ' ') {
      if (aux != "") 
        ret.push_back(aux);
      aux = "";
    } else {
      aux += (char) toupper(c); 
    }
  }
  if (aux != "") 
    ret.push_back(aux);
  return ret;
}

int hexa_para_decimal(const char & c) {
  /* Converte um dígito hexadecimal para seu valor decimal. 
  É assumido que o caractére passado é, de fato, um dígito hexadecimal. */
  if (c >= '0' and c <= '9') return c - '0';
  return c - 'A' + 10;
}

string hexa_para_binario(const char & c) {
  /* Converte um dígito hexadecimal para seu valor em binario de 4 bits.*/
  int valor = hexa_para_decimal(c);
  string valor_binario(4, '0');
  for (int i = 0; i < 4; ++i)
    /* Verifica se o i-ésimo bit está ligado ou não */
    valor_binario[i] = (((1 << i) & valor) != 0) + '0';
  reverse(valor_binario.begin(), valor_binario.end()); 
  return valor_binario;
}

string hexa_para_decimal(const string & s) {
  /* Converte um valor hexadecimal em complemento de dois 
  para o seu valor decimal. É assumido que o valor passado
  é um valor hexadecimal válido que pode ser representado 
  por um int. */
  string digitos_em_binario;
  for (int i = 2; i < sz(s); ++i) 
    digitos_em_binario += hexa_para_binario(s[i]);
  int valor = 0, potencia_de_dois = 1;
  for (int i = sz(digitos_em_binario) - 1; i > 0; --i) {
    valor += potencia_de_dois * (digitos_em_binario[i] - '0');
    potencia_de_dois *= 2;
  }
  valor += -(potencia_de_dois * (digitos_em_binario[0] - '0'));
  return to_string(valor);
}

bool rotulo_valido(const string & s) {
  if (s.front() >= '0' and s.front() <= '9') return false;
  for (const char & c : s) 
    if (not (c >= '0' and c <= '9') and 
        not (c >= 'A' and c <= 'Z') and 
        not (c == '_')) 
          return false;
  return true;
}

void erro(int numero_da_linha, string tipo_do_erro) {
  cout << "Erro na linha " << numero_da_linha << ":\n"
  << mensagem_de_erro[tipo_do_erro] << '\n';
}

bool existe_instrucao(const string & s) {
  return opcode_da_instrucao.count(s) > 0;
}