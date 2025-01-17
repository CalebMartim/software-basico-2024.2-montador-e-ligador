#include "auxiliar.hpp"

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
      for (int i = 2; i < sz(tokens); ++i) 
        tabela_de_uso_arquivo_um[tokens[1]].push_back(stoi(tokens[i]));
    else if (tokens[0] == "D,")
      tabela_de_definicoes_arquivo_um[tokens[1]] = stoi(tokens[2]);
  }

  vector<int> uso_relativo;
  for (const string & s : linhas_do_arquivo_dois) {
    vector<string> tokens = pegue_tokens(s);
    if (tokens[0] == "U,")
      for (int i = 2; i < sz(tokens); ++i)
        tabela_de_uso_arquivo_dois[tokens[1]].push_back(stoi(tokens[i]));
    else if (tokens[0] == "D,")
      tabela_de_definicoes_arquivo_dois[tokens[1]] = stoi(tokens[2]);
    else if (tokens[0] == "R,")
      for (int i = 1; i < sz(tokens); ++i)
        uso_relativo.push_back(tokens[i][0] - '0');
  }

  /* Faz a correção */
  vector<int> codigo_objeto_arquivo_um, codigo_objeto_arquivo_dois;
  unordered_set<int> corrigidos;
  
  for (string s : pegue_tokens(linhas_do_arquivo_um.back())) // A última linha contém o arquivo objeto 
    codigo_objeto_arquivo_um.push_back(stoi(s));

  for (string s : pegue_tokens(linhas_do_arquivo_dois.back())) 
    codigo_objeto_arquivo_dois.push_back(stoi(s));
    
  int fator_de_correcao = sz(codigo_objeto_arquivo_um);  
  
  for (auto & [x, y] : tabela_de_definicoes_arquivo_dois)
    y += fator_de_correcao;
    
  for (const auto & [x, y] : tabela_de_uso_arquivo_um) // Corrigindo para o arquivo um
    for (const int &  z : y) 
      codigo_objeto_arquivo_um[z] += tabela_de_definicoes_arquivo_dois[x];

  for (const auto & [x, y] : tabela_de_uso_arquivo_dois) // Corrigi os usos no arquivo dois e marca como corrigido
    for (const int & z : y) {
      corrigidos.insert(z);
      codigo_objeto_arquivo_dois[z] += tabela_de_definicoes_arquivo_um[x];
    }

  string codigo_objeto_final;
  for (const int & x : codigo_objeto_arquivo_um)
    codigo_objeto_final += to_string(x) + " ";
  for (int i = 0; i < sz(codigo_objeto_arquivo_dois); ++i) {
    if (uso_relativo[i] and corrigidos.count(i) == 0) 
      codigo_objeto_arquivo_dois[i] += fator_de_correcao;
    codigo_objeto_final += to_string(codigo_objeto_arquivo_dois[i]) + " ";
  }
  if (not codigo_objeto_final.empty()) codigo_objeto_final.pop_back();
  ofstream saida(arquivo_um.substr(0, sz(arquivo_um) - 4) + ".e");
  saida << codigo_objeto_final;
  saida.close();  
  return 0;
}
