#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

// Definição da estrutura Cmd
struct Cmd {
    string command;
    unsigned int value;
};

// Função para analisar os comandos e valores
vector<Cmd> parse_cmds(istream& input) {
    vector<Cmd> cmds;
    string line;

    while (getline(input, line)) {
        // Usando stringstream para dividir a linha em partes
        stringstream ss(line);
        string command;
        unsigned int value;

        // Lendo o comando e o valor separadamente
        if (ss >> command >> value) {
            // Criando um novo Cmd e adicionando ao vetor
            cmds.push_back({command, value});
        }
    }

    return cmds;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cout << "Uso: " << argv[0] << " <arquivo_de_entrada>" << endl;
        return 1;
    }

    ifstream inputFile(argv[1]);

    if (!inputFile) {
        cout << "Erro ao abrir o arquivo de entrada." << endl;
        return 1;
    }

    // Chamando a função parse_cmds para analisar o arquivo
    vector<Cmd> cmds = parse_cmds(inputFile);

    // Exibindo os comandos analisados
    for (const auto& cmd : cmds) {
        cout << "Comando: " << cmd.command << ", Valor: " << cmd.value << endl;
    }

    inputFile.close();

    return 0;
}
