#include "iostream"
#include "fstream"
#include "vector"

using namespace std;

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

int main(int argc, char* argv[]){
  string arquivo_um = argv[1], arquivo_dois = argv[2];
  vector<string> linhas_do_arquivo_um, linhas_do_arquivo_dois;
  ifstream arquivo(arquivo_um);
  string aux;
  while (getline(arquivo, aux))
    linhas_do_arquivo_um.push_back(aux);
  arquivo = ifstream(arquivo_dois);
  while (getline(arquivo, aux))
    linhas_do_arquivo_dois.push_back(aux);
  int inicio_arquivo_dois = pegue_tokens(linhas_do_arquivo_um.back()).size();  
}
