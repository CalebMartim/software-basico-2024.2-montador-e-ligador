#include <vector>
#include <fstream>
#include <iostream>
#include <unordered_map>

using namespace std;

vector<string> pegue_tokens(const string & s){
  vector<string> ret;
  string aux;
  for (const char & c : s) {
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

int main(int argc, char* argv[]){

  /* Processo de leitura dos arquivos*/
  string arquivo_um = argv[1], arquivo_dois = argv[2];
  vector<string> linhas_do_arquivo_um, linhas_do_arquivo_dois;
  ifstream arquivo(arquivo_um);
  string aux;
  while (getline(arquivo, aux))
    linhas_do_arquivo_um.push_back(aux);
  arquivo = ifstream(arquivo_dois);
  while (getline(arquivo, aux))
    linhas_do_arquivo_dois.push_back(aux);

  /* Constrói a tabela de uso e definição */
  unordered_map<string, vector<int>> 
  tabela_de_uso_arquivo_um, 
  tabela_de_uso_arquivo_dois;
  unordered_map<string,int> 
  tabela_de_definicoes_arquivo_um,
  tabela_de_definicoes_arquivo_dois;
  for (const string & s : linhas_do_arquivo_um) {
    vector<string> tokens = pegue_tokens(s);
    if (tokens[0] == "U,")
      for (int i = 2; i < size(tokens); ++i) 
        tabela_de_uso_arquivo_um[tokens[1]].push_back(stoi(tokens[i]));
    else if (tokens[0] == "D,")
      tabela_de_definicoes_arquivo_um[tokens[1]] = stoi(tokens[2]);
  }
  vector<int> uso_relativo;
  for (const string & s : linhas_do_arquivo_dois) {
    vector<string> tokens = pegue_tokens(s);
    if (tokens[0] == "U,")
      for (int i = 2; i < size(tokens); ++i)
        tabela_de_uso_arquivo_dois[tokens[1]].push_back(stoi(tokens[i]));
    else if (tokens[0] == "D,")
      tabela_de_definicoes_arquivo_dois[tokens[1]] = stoi(tokens[2]);
    else if (tokens[0] == "R, ")
      for (int i = 1; i < size(tokens); ++i)
        uso_relativo.push_back(tokens[i][0] + '0');
  }

  /* Faz a correção */
  vector<int> codigo_objeto_arquivo_um, codigo_objeto_arquivo_dois;
  for (string s : pegue_tokens(linhas_do_arquivo_um.back())) 
    codigo_objeto_arquivo_um.push_back(stoi(s));
  for (string s : pegue_tokens(linhas_do_arquivo_dois.back())) 
    codigo_objeto_arquivo_dois.push_back(stoi(s));
  int fator_de_correcao = size(codigo_objeto_arquivo_um);  
  for (auto &[x, y] : tabela_de_definicoes_arquivo_dois) 
    y += fator_de_correcao;
  for (auto [x, y] : tabela_de_uso_arquivo_um)
    for (int z : y) 
      codigo_objeto_arquivo_um[z] += tabela_de_definicoes_arquivo_dois[x];
  for (auto [x, y] : tabela_de_uso_arquivo_dois)
    for (int z : y)
      codigo_objeto_arquivo_dois[z] += tabela_de_definicoes_arquivo_um[x];
  ofstream saida(arquivo_um.substr(0, size(arquivo_um) - 4) + ".e");
  for (const int & x : codigo_objeto_arquivo_um)
    saida << x << ' ';
  for (int i = 0; i < size(codigo_objeto_arquivo_dois); ++i) {
    if (uso_relativo[i]) 
      codigo_objeto_arquivo_dois[i] += fator_de_correcao;
    saida << codigo_objeto_arquivo_dois[i] << ' ';
  }
  saida.close();  
}
