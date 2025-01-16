#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

/* A diretiva a seguir é feita para que não seja necessário 
insrir o prefixo "std::" em toda instância de um
objeto da biblioteca padrão. */ 
using namespace std;   

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

unordered_set<string> diretivas_para_ignorar = {
  "END", "SECTION"
};

unordered_map<string, string> mensagem_de_erro = {
  {"rotulo_ausente", "Rótulo não definido."},
  {"rotulo_dobrado", "Dois rótulos definidos na mesma instrução."},
  {"rotulo_redefinido", "Redefinição de rótulo."},
  {"expressao_invalida", "Quantidade de operandos inválidos para instrução ou diretiva."},
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

// -- Funções auxiliares, início:
vector<string> pegue_tokens(const string & s){
  vector<string> ret;
  string aux;
  for (const char & c : s) {
    if (c == ':') { // optamos por desconsiderar comentários
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
  for (int i = 2; i < (int) s.length(); ++i) 
    digitos_em_binario += hexa_para_binario(s[i]);
  int valor = 0, potencia_de_dois = 1;
  for (int i = (int) digitos_em_binario.length() - 1; i > 0; --i) {
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

// -- Funções auxiliares, fim.

int main(int argc, char* argv[]){
  string arquivo_recebido;
  for (int i = 0; argv[1][i] != '\0'; ++i)
    arquivo_recebido += argv[1][i];
  string nome_do_arquivo;
  for (int i = 0; arquivo_recebido[i] != '.'; ++i) 
    nome_do_arquivo += arquivo_recebido[i];

  /* Processo de leitura do arquivo. */
  ifstream stream_do_arquivo(arquivo_recebido);
  vector<string> linhas_do_programa;
  string aux;
  while (getline(stream_do_arquivo, aux)) 
    linhas_do_programa.push_back(aux);    
  stream_do_arquivo.close(); 

  /* Tendo lido nosso arquivo, vamos construir os tokens dos comandos,
  vamos considerar quando o programa se trata de um módulo a 
  ser ligado ou não. Assim como de acordo com a especificação,
  comentários sào removidos nas diretivas IF e EQU */
  vector<vector<string>> tokens_data, tokens_text;
  bool lendo_text = true, representa_modulo = false;
  for (const string & linha : linhas_do_programa) {
    vector<string> tokens = pegue_tokens(linha);
    if (tokens.empty()) continue; // Linha vazia
    else if (tokens[0] == "SECTION")
      lendo_text = (tokens[1] == "TEXT");
    if (lendo_text)
      tokens_text.push_back(tokens);
    else
      tokens_data.push_back(tokens);
    if (tokens[0] == "END") representa_modulo = true; // O arquivo se trata de um módulo 
  }

  if (arquivo_recebido.substr((int) arquivo_recebido.length() - 4, 4) == ".pre") {
                              
    // Pré-processamento do arquivo //

    ofstream saida(nome_do_arquivo + ".asm");
    for (const vector<string> & operacao : tokens_text) 
      if (operacao[0] == "COPY") {
        saida << operacao[0];
        if ((int) operacao.size() > 1) saida << ' '; 
        for (int i = 1; i < (int) operacao.size(); ++i) 
          saida << operacao[i] << ",\n"[i == (int) operacao.size() - 1]; // Separamos argumentos por vírgula
      } else 
          for (int i = 0; i < (int) operacao.size(); ++i) 
            saida << operacao[i] << " \n"[i == (int) operacao.size() - 1];
    for (vector<string> & operacao : tokens_data) {
      if ((int) operacao.size() > 1 and operacao[1] == "CONST") {
        if ((int) operacao.size() > 2 and 
            (int) operacao[2].size() >= 2 and 
            operacao[2].substr(0, 2) == "0X")
          operacao[2] = hexa_para_decimal(operacao[2]);
      }  
      for (int i = 0; i < (int) operacao.size(); ++i) 
        saida << operacao[i] << " \n"[i == (int) operacao.size() - 1];
    }
    saida.close();
  } else {

    // Montagem do arquivo //

    unordered_map<string, string> 
    tabela_de_simbolos, 
    tabela_de_definicoes;
    unordered_map<string, vector<string>> tabela_de_uso;

    /* Primeira passagem e verificação de alguns erros sintáticos: */
    int contador_de_instrucao = 0, contador_de_linha = 1;
    for (vector<string> & operacao : tokens_text) {
      if (diretivas_para_ignorar.count(operacao[0])) continue;
      if (operacao[0].back() == ':') { // É a definição de um rótulo
        string rotulo = operacao[0]; 
        rotulo.pop_back(); // Remove o ':' no fim 
        if (rotulo == "BEGIN") {
          if ((int) operacao.size() > 1) {
            rotulo = operacao[1];
            if (not rotulo_valido(rotulo))
              erro(contador_de_linha, rotulo_invalido);
            tabela_de_simbolos[rotulo] = "0";
          } 
          contador_de_linha += 1;
          continue;
        }
        if (not rotulo_valido(rotulo)) 
          erro(contador_de_linha, rotulo_invalido);
        if (tabela_de_simbolos.count(rotulo)) // Verifica se já não foi definido
          erro(contador_de_linha, rotulo_redefinido);
        tabela_de_simbolos[rotulo] = to_string(contador_de_instrucao);
        if ((int) operacao.size() > 1) {
          if (operacao[1].back() == ':') // Verifica se há uma definição duplicada aqui
            erro(contador_de_linha, rotulo_dobrado);
          if (representa_modulo and operacao[1] == "EXTERN")  {
            tabela_de_uso[rotulo] = {}; // Insere este rótulo na tabela de uso
            continue;
          }
          if (existe_instrucao(operacao[1])) { 
            if ((int) operacao.size() - 1 != tamanho_da_instrucao[operacao[1]]) // Número de operandos inválido
              erro(contador_de_linha, expressao_invalida);
            contador_de_instrucao += tamanho_da_instrucao[operacao[1]];
          } else 
            erro(contador_de_linha, instrucao_invalida);
        }
      } else if (representa_modulo and operacao[0] == "PUBLIC") {
        if (operacao.size() == 2) {
          continue; // Vamos botar na tabela de definições apenas na segunda passagem 
        } else 
          erro(contador_de_linha, expressao_invalida);
      }
      else {
        if (existe_instrucao(operacao[0])) {
          if ((int) operacao.size() != tamanho_da_instrucao[operacao[0]]) // Número de operandos inválido
            erro(contador_de_linha, expressao_invalida);
          contador_de_instrucao += tamanho_da_instrucao[operacao[0]];
        } else 
          erro(contador_de_linha, instrucao_invalida);
      }
      contador_de_linha += 1;
    } 
    for (vector<string> & operacao : tokens_data) {
      string rotulo = operacao[0];
      rotulo.pop_back(); // Remove o ':'
      if (tabela_de_simbolos.count(rotulo) > 0)
        erro(contador_de_linha, rotulo_redefinido);
      tabela_de_simbolos[rotulo] = to_string(contador_de_instrucao);
      if (operacao[1] == "CONST") {
        if ((int) operacao.size() != 3) 
          erro(contador_de_linha, expressao_invalida);
        contador_de_instrucao += 2;
      } else if (operacao[1] == "SPACE") {
        if ((int) operacao.size() != 3) 
          erro(contador_de_linha, expressao_invalida);
        int espaco_utlizado = 1;
        if (operacao.size() > 2) 
          espaco_utlizado = stoi(operacao[2]);
        contador_de_instrucao += espaco_utlizado;
      } else 
        erro(contador_de_linha, instrucao_invalida);
      contador_de_linha += 1;
    }

    contador_de_linha = 1, contador_de_instrucao = 0;
    string codigo_objeto, uso_relativo;

    /* Segunda passagem */
    for (vector<string> & operacao : tokens_text) {
      if (diretivas_para_ignorar.count(operacao[0])) continue; 
      int rotulo = 0; // Esta variável serve como uma deslocação para tratarmos instruções na mesma linha que rótulos
      if (operacao[0].back() == ':') { // É a definição de um rotulo
        if (operacao.size() > 1) {
          if (representa_modulo and operacao[1] == "EXTERN") {
            contador_de_linha += 1;
            continue;
          }
          rotulo = 1;
        } else {
          contador_de_linha += 1;
          continue;          
        }
      } 
      if (representa_modulo and operacao[0] == "PUBLIC") {
        if (tabela_de_simbolos.count(operacao[1]))
          tabela_de_definicoes[operacao[1]] = tabela_de_simbolos[operacao[1]];
        else 
          erro(contador_de_linha, rotulo_ausente);
        contador_de_linha += 1;
        continue;
      }
      codigo_objeto += opcode_da_instrucao[operacao[0 + rotulo]] + " ";
      if (representa_modulo)
        uso_relativo += " 0";
      if ((int) operacao.size() > 1 + rotulo) {
        if (operacao[0 + rotulo] == "COPY") {
          string argumento_um, argumento_dois;
          int i = 0;
          while (operacao[1 + rotulo][i] != ',') {
            argumento_um += operacao[1 + rotulo][i];
            i += 1;
          }
          i += 1;
          while (i < (int) operacao[1 + rotulo].length()) {
            argumento_dois += operacao[1 + rotulo][i];
            i += 1;
          }
          if (tabela_de_simbolos.count(argumento_um) == 0 or tabela_de_simbolos.count(argumento_dois) == 0)
            erro(contador_de_linha, rotulo_ausente);
          codigo_objeto += tabela_de_simbolos[argumento_um] + " " + tabela_de_simbolos[argumento_dois] + " ";
          if (representa_modulo) {
            uso_relativo += " 1 1";
            if (tabela_de_uso.count(argumento_um)) 
              tabela_de_uso[argumento_um].push_back(to_string(contador_de_instrucao + 1));
            if (tabela_de_uso.count(argumento_dois))
              tabela_de_uso[argumento_dois].push_back(to_string(contador_de_instrucao + 2));
          }
        } else {
          if (tabela_de_simbolos.count(operacao[1 + rotulo]) == 0)
            erro(contador_de_linha, rotulo_ausente);
          codigo_objeto += tabela_de_simbolos[operacao[1 + rotulo]] + " ";
          if (representa_modulo)
            uso_relativo += " 1"; 
          if (tabela_de_uso.count(operacao[1 + rotulo]))
            tabela_de_uso[operacao[1 + rotulo]].push_back(to_string(contador_de_instrucao + 1));
        }
      }
      contador_de_instrucao += tamanho_da_instrucao[operacao[0 + rotulo]];
      contador_de_linha += 1;
    }
    for (const vector<string> & operacao : tokens_data)
      if (operacao[1] == "CONST")
          codigo_objeto += operacao[2] + " ";
      else 
        for (int i = 0; i < stoi(operacao[2]); ++i)
          codigo_objeto += "0 ";

    if (not codigo_objeto.empty())
      codigo_objeto.pop_back(); // remove o ' ' que está no final

    ofstream saida(nome_do_arquivo + ".obj");
    if (representa_modulo) {
      // Insere tabela de definiçÕes, de uso e define quem poderá mudar de valor na ligação //
      for (const auto & [x, y] : tabela_de_definicoes) 
        saida << "D, " << x << ' ' << y << '\n'; 
      for (const auto & [x, y] : tabela_de_uso) {
        saida << "U, " << x;
        for (const string & s : y)
          saida << ' ' << s;
        saida << '\n';
      }
      saida << "R," << uso_relativo << '\n';
    }
    saida << codigo_objeto;
    saida.close();
  }
  return 0;
}
