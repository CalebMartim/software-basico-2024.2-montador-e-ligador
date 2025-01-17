#include "auxiliar.hpp"

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

  if (arquivo_recebido.substr(sz(arquivo_recebido) - 4, 4) == ".asm") {
                              
    // Pré-processamento do arquivo //

    ofstream saida(nome_do_arquivo + ".pre");
    for (const vector<string> & operacao : tokens_text) 
      for (int i = 0; i < sz(operacao); ++i) 
        saida << operacao[i] << " \n"[i == sz(operacao) - 1];
    for (vector<string> & operacao : tokens_data) {
      if (sz(operacao) > 1 and operacao[1] == "CONST") {
        if (sz(operacao) > 2 and 
            sz(operacao[2]) >= 2 and 
            operacao[2].substr(0, 2) == "0X")
          operacao[2] = hexa_para_decimal(operacao[2]);
      }  
      for (int i = 0; i < sz(operacao); ++i) 
        saida << operacao[i] << " \n"[i == sz(operacao) - 1];
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
      if (diretivas_para_ignorar.count(operacao[0])) {
        contador_de_linha += 1; 
        continue;
      }
      if (operacao[0].back() == ':') { // É a definição de um rótulo
        string rotulo = operacao[0]; 
        rotulo.pop_back(); // Remove o ':' no fim 
        if (rotulo == "BEGIN") {
          if (sz(operacao) > 1) {
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
        if (sz(operacao) > 1) {
          if (operacao[1].back() == ':') // Verifica se há uma definição duplicada aqui
            erro(contador_de_linha, rotulo_dobrado);
          if (representa_modulo and operacao[1] == "EXTERN")  {
            tabela_de_uso[rotulo] = {}; // Insere este rótulo na tabela de uso
            contador_de_linha += 1;
            continue;
          }
          if (existe_instrucao(operacao[1])) { 
            if (sz(operacao) - 1 != tamanho_da_instrucao[operacao[1]]) // Número de operandos inválido
              erro(contador_de_linha, expressao_invalida);
            contador_de_instrucao += tamanho_da_instrucao[operacao[1]];
            if (operacao[1] == "COPY" and operacao[2].back() != ',')
              erro(contador_de_linha, expressao_invalida);
          } else 
            erro(contador_de_linha, instrucao_invalida);
        }
      } else if (representa_modulo and operacao[0] == "PUBLIC") {
        if (sz(operacao) == 2) {
          contador_de_linha += 1;
          continue; // Vamos botar na tabela de definições apenas na segunda passagem 
        } else 
          erro(contador_de_linha, expressao_invalida);
      }
      else {
        if (existe_instrucao(operacao[0])) {
          if (sz(operacao) != tamanho_da_instrucao[operacao[0]]) // Número de operandos inválido
            erro(contador_de_linha, expressao_invalida);
          contador_de_instrucao += tamanho_da_instrucao[operacao[0]];
          if (operacao[0] == "COPY" and operacao[1].back() != ',')
            erro(contador_de_linha, expressao_invalida);
        } else 
          erro(contador_de_linha, instrucao_invalida);
      }
      contador_de_linha += 1;
    } 
    for (vector<string> & operacao : tokens_data) {
      if (diretivas_para_ignorar.count(operacao[0])) {
        contador_de_linha += 1; 
        continue;
      }
      string rotulo = operacao[0];
      rotulo.pop_back(); // Remove o ':'
      if (tabela_de_simbolos.count(rotulo) > 0)
        erro(contador_de_linha, rotulo_redefinido);
      tabela_de_simbolos[rotulo] = to_string(contador_de_instrucao);
      if (operacao[1] == "CONST") {
        if (sz(operacao) != 3) 
          erro(contador_de_linha, expressao_invalida);
        contador_de_instrucao += 1;
      } else if (operacao[1] == "SPACE") {
        if (sz(operacao) != 2 and sz(operacao) != 3) 
          erro(contador_de_linha, expressao_invalida);
        int espaco_utlizado = 1;
        if (sz(operacao) > 2) 
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
      if (diretivas_para_ignorar.count(operacao[0])) {
        contador_de_linha += 1;
        continue; 
      }
      int rotulo = 0; // Esta variável serve como uma deslocação para tratarmos instruções na mesma linha que rótulos
      if (operacao[0].back() == ':') { // É a definição de um rotulo
        if (sz(operacao) > 1 and operacao[0] != "BEGIN:") {
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
      if (sz(operacao) > 1 + rotulo) {
        if (operacao[0 + rotulo] == "COPY") {
          string argumento_um, argumento_dois;
          for (int i = 0; i < sz(operacao[1 + rotulo]) and operacao[1 + rotulo][i] != ','; ++i)
            argumento_um += operacao[1 + rotulo][i];
          argumento_dois = operacao[2 + rotulo];
          if (tabela_de_simbolos.count(argumento_um) == 0 or tabela_de_simbolos.count(argumento_dois) == 0)
            erro(contador_de_linha, rotulo_ausente);
          codigo_objeto += tabela_de_simbolos[argumento_um] + " " + tabela_de_simbolos[argumento_dois] + " ";
          uso_relativo += " 1 1";
          if (representa_modulo) {
            if (tabela_de_uso.count(argumento_um)) 
              tabela_de_uso[argumento_um].push_back(to_string(contador_de_instrucao + 1));
            if (tabela_de_uso.count(argumento_dois)) 
              tabela_de_uso[argumento_dois].push_back(to_string(contador_de_instrucao + 2));
          }
        } else {
          if (tabela_de_simbolos.count(operacao[1 + rotulo]) == 0)
            erro(contador_de_linha, rotulo_ausente);
          codigo_objeto += tabela_de_simbolos[operacao[1 + rotulo]] + " ";
          uso_relativo += " 1";
          if (tabela_de_uso.count(operacao[1 + rotulo])) 
            tabela_de_uso[operacao[1 + rotulo]].push_back(to_string(contador_de_instrucao + 1)); 
        }
      }
      contador_de_instrucao += tamanho_da_instrucao[operacao[0 + rotulo]];
      contador_de_linha += 1;
    }
    for (const vector<string> & operacao : tokens_data) {
      if (diretivas_para_ignorar.count(operacao[0])) continue; 
      if (operacao[1] == "CONST") {
        codigo_objeto += operacao[2] + " ";
        if (representa_modulo)
          uso_relativo += " 0";
      }
      else {
        if (sz(operacao) > 2) 
          for (int i = 0; i < stoi(operacao[2]); ++i) {
            codigo_objeto += "0 ";
            if (representa_modulo)
              uso_relativo += " 0";
          }
        else {
          codigo_objeto += "0 ";
          if (representa_modulo)
            uso_relativo += " 0";
        }
      }
    }

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
