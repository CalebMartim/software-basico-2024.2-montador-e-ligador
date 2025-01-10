#include <vector>
#include <fstream>
#include <iostream>
#include <unordered_map>

/* A diretiva a seguir é feita para que não seja necessário 
insrir o prefixo "std::" em toda instância de um
objeto da biblioteca padrão. */ 
using namespace std;   

#warning falta implementar macros e outras diretivas (BEGIN, END)
#warning falta implmentar operadores nas labels

unordered_map<string, int> tamanho_da_instrucao = {
  {"ADD", 2},
  {"SUB", 2},
  {"MUL", 2},
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

// -- Funções auxiliares, início:
vector<string> pegue_tokens(const string & s){
  vector<string> ret;
  string aux;
  for (const char & c : s) {
    #warning isso aqui está errado, só podemos tirar isso aqui no EQU e IF
    if (c == ';') // Desconsideramos o comentário do comando
      break;
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

int valor_do_caractere_hexa(const char & c) {
  /* Converte um dígito hexadecimal para seu valor decimal*/
  if (c >= '0' and c <= '9') return c - '0';
  return c - 'A' + 10;
}

#warning mudar para complemento de 2 
string hexa_para_decimal(const string & s) {
  int valor = 0, potencia_de_16 = 1;
  for (int i = (int) s.length() - 1; s[i] != 'x'; --i) {
    valor += valor_do_caractere_hexa(s[i]) * potencia_de_16;
    potencia_de_16 *= 16; 
  }
  return to_string(valor);
}

bool rotulo_invalido(const string & s) {
  if (s.front() >= '0' and s.front() <= '9') return true;
  for (const char & c : s) 
    if (not (c >= '0' and c <= '9') and 
        not (c >= 'A' and c <= 'Z') and 
        not (c == '_')) 
          return true;
  return false;
}

unordered_map<string, string> mensagem_de_erro = {
  {"rotulo_ausente", "Rótulo não definido."},
  {"rotulo_dobrado", "Dois rótulos definidos na mesma instrução."},
  {"rotulo_redefinido", "Redefinição de rótulo."},
  {"expressao_invalida", "Quantidade de operandos inválidos para instrução ou diretiva."},
  {"rotulo_invalido", "Nome inválido para definição de rótulo."},
  {"instrucao_invalida", "Instrução ou diretiva não existente."},
};

void erro(int numero_da_linha, string tipo_do_erro) {
  cout << "Erro na linha " << numero_da_linha << ":\n"
  << mensagem_de_erro[tipo_do_erro] << '\n';
}

bool existe_instrucao(const string & s) {
  return opcode_da_instrucao.count(s) > 0;
}
// -- Funções auxiliares, fim.

int main(int argc, char* argv[]){
  string arquivo_recebido;
  for (int i = 0; argv[1][i] != '\0'; ++i)
    arquivo_recebido += argv[1][i];
  string nome_do_arquivo;
  for (int i = 0; arquivo_recebido[i] != '.'; ++i) 
    nome_do_arquivo += arquivo_recebido[i];

  /* Processo de leitura do arquivo. */
  ifstream arquivo(argv[1]);
  vector<string> linhas_do_programa;
  string aux;
  while (getline(arquivo, aux)) 
    linhas_do_programa.push_back(aux);    
  arquivo.close(); 

  /* Tendo lido nosso arquivo, vamos construir os tokens dos comandos,
  remover seus comentários e garantir que a ordem
  da seção TEXT e DATA esteja correta */
  vector<vector<string>> tokens_data, tokens_text;
  bool lendo_text = false;
  for (const string & linha : linhas_do_programa) {
    vector<string> tokens = pegue_tokens(linha);
    if (tokens.empty()) continue; // Linha vazia ou composta apenas por comentário
    if (tokens[0] == "SECTION")
      lendo_text = (tokens[1] == "TEXT");
    if (lendo_text)
      tokens_text.push_back(tokens);
    else
      tokens_data.push_back(tokens);
  }

  if (arquivo_recebido.substr((int) arquivo_recebido.length() - 4, 4) == ".pre") {
                              
    // Pré-processamento do arquivo //

    ofstream saida(nome_do_arquivo + ".asm");
    for (const vector<string> & operacao : tokens_text) 
      if (operacao[0] == "COPY") {
        saida << operacao[0];
        if ((int) operacao.size() > 1) saida << ' '; 
        for (int i = 1; i < (int) operacao.size(); ++i)
          saida << operacao[1] << ",\n"[i == (int) operacao.size() - 1];
      } else 
          for (int i = 0; i < (int) operacao.size(); ++i) 
            saida << operacao[i] << " \n"[i == (int) operacao.size() - 1];
    for (const vector<string> & operacao : tokens_data) 
      if (operacao[1] == "CONST") {
        #warning estamos assumindo que tudo está com certa com esta operação
        string valor = operacao[2];
        if ((valor.length() >= 2 and valor.substr(0, 2) == "0X"))
          valor = hexa_para_decimal(valor);
        saida << operacao[0] << ' ' << operacao[1] << ' ' << valor << '\n';
      } else 
          for (int i = 0; i < (int) operacao.size(); ++i) 
            saida << operacao[i] << " \n"[i == (int) operacao.size() - 1];
    saida.close();
  } else {

    // Montagem do arquivo //

    /* Primeira passagem: */
    unordered_map<string, string> tabela_de_simbolos;
    int contador_de_instrucao = 0, contador_de_linha = 1;
    for (const vector<string> & operacao : tokens_text) {
      if (operacao[0] == "SECTION") continue;
      if (operacao[0].back() == ':') { // É a definição de uma rotulo
        string rotulo = operacao[0]; 
        if (rotulo_invalido(rotulo)) {
          erro(contador_de_linha, "rotulo_invalido");
          return 1;
        }  
        rotulo.pop_back(); // Remove o ':'
        if (tabela_de_simbolos.count(rotulo) > 0) { // Verifica se já não foi definido
          erro(contador_de_linha, "rotulo_redefinido");
          return 1;
        } 
        tabela_de_simbolos[rotulo] = to_string(contador_de_instrucao);
        if (operacao.size() > 1) {
          if (operacao[1].back() == ':') { // Verifica se há uma definição duplicada aqui
            erro(contador_de_linha, "rotulo_dobrado");
            return 1; 
          }
          if (existe_instrucao(operacao[1])) {
            contador_de_instrucao += tamanho_da_instrucao[operacao[1]];
          } else {
            erro(contador_de_linha, "instrucao_invalida");
            return 1;
          }
        }
      } else {
        if (existe_instrucao(operacao[0])) {
          if ((int) operacao.size() != tamanho_da_instrucao[operacao[0]]) { // Número de operandos inválido
            erro(contador_de_linha, "expressao_invalida");
            return 1;
          }
          contador_de_instrucao += tamanho_da_instrucao[operacao[1]];
        } else {
          erro(contador_de_linha, "instrucao_invalida");
          return 1;
        }
      }
      contador_de_linha += 1;
    } 
    for (const vector<string> & operacao : tokens_data) {
      string rotulo = operacao[0];
      rotulo.pop_back(); // Remove o ':'
      if (tabela_de_simbolos.count(rotulo) > 0) {
        erro(contador_de_linha, "rotulo_redefinido");
        return 1;
      }      
      tabela_de_simbolos[rotulo] = to_string(contador_de_instrucao);
      if (operacao[1] == "CONST") {
        if ((int) operacao.size() != 3) {
          erro(contador_de_linha, "expressao_invalida");
          return 1;
        }
        contador_de_instrucao += 2;
      } else if (operacao[1] == "SPACE") {
        if ((int) operacao.size() > 3) {
          erro(contador_de_linha, "expressao_invalida");
          return 1;
        }
        int espaco_utlizado = 1;
        if (operacao.size() > 2) 
          espaco_utlizado = stoi(operacao[2]);
        contador_de_instrucao += espaco_utlizado;
      } else {
        erro(contador_de_linha, "instrucao_invalida");
        return 1;
      }
      contador_de_linha += 1;
    }

    /* Segunda passagem */
    contador_de_linha = 1;
    unordered_map<string,string> valores_de_rotulos;
    string codigo_objeto;
    for (const vector<string> & operacao : tokens_text) {
      #warning lembrar que isso aqui tá errado e que provavelmente tem mais coisas para cima que estão erradas
      if (operacao[0] == "SECTION \\ BEGIN \\ END") continue; 
      int pulo = 0;
      if (operacao[0].back() == ':') { // É a definição de um rotulo
        if (operacao.size() > 1) 
          pulo = 1;
        else 
          continue;
      } 
      codigo_objeto += opcode_da_instrucao[operacao[0 + pulo]] + " ";
      if ((int) operacao.size() > 1 + pulo) {
        if (operacao[0 + pulo] == "COPY") {
          string argumento_um, argumento_dois;
          int i = 0;
          while (operacao[1 + pulo][i] != ',') {
            argumento_um += operacao[1 + pulo][i];
            i += 1;
          }
          i += 1;
          while (i < (int) operacao[1 + pulo].length()) {
            argumento_dois += operacao[1 + pulo][i];
            i += 1;
          }
          if (tabela_de_simbolos.count(argumento_um) == 0 or
              tabela_de_simbolos.count(argumento_dois) == 0) {
                erro(contador_de_linha, "rotulo_ausente");
                return 1;
              }
          codigo_objeto += tabela_de_simbolos[argumento_um] + " " + tabela_de_simbolos[argumento_dois] + " ";
        } else {
          if (tabela_de_simbolos.count(operacao[1 + pulo]) == 0) {
            erro(contador_de_linha, "rotulo_ausente");
            return 1;
          }
          codigo_objeto += tabela_de_simbolos[operacao[1 + pulo]] + " ";
        }
      }
      contador_de_linha += 1;
    }
    for (const vector<string> & operacao : tokens_data) {
      #warning considerar quando a label é separada por enter
      #warning considerar um if
      int pulo = 0;
      if (operacao[0].back() == ':') {  
        if ((int) operacao.size() > 1) 
          pulo = 1;
        else 
          continue;
      }
      if (operacao[0 + pulo] == "EQU") {

      }
      else if (operacao[0 + pulo] == "CONST")
        #warning aqui temos que mudar para quando tratamos de uma label
        codigo_objeto += operacao[1 + pulo] + " ";
      else 
        #warning aqui temos que mudar para quando tratamos de uma label
        for (int i = 0; i < stoi(operacao[1 + pulo]); ++i)
          codigo_objeto += (string) "0" + " ";
    }
    if (not codigo_objeto.empty())
      codigo_objeto.pop_back(); // remove o ' ' que está no final
    ofstream saida(nome_do_arquivo + ".obj");
    saida << codigo_objeto;
    saida.close();
  }
  return 0;
}

