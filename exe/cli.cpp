#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char *argv[]) {
    string command;

    if (argc != 2) {
        cout << "Uso: " << argv[0] << " <arquivo_de_entrada>" << endl;
        return 1;
    }

    ifstream inputFile(argv[1]);

    if (!inputFile) {
        cout << "Erro ao abrir o arquivo de entrada." << endl;
        return 1;
    }

    while (getline(inputFile, command)) {
        cout << "Comando lido do arquivo: " << command << endl;
    }

    inputFile.close();

    return 0;
}
