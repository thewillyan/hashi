#include <iostream>
#include <fstream>
#include <string>

using namespace std;

void log(const string& message) {
    ofstream logFile("log.txt", ios::app);
    if (logFile.is_open()) {
        logFile << message << endl;
        logFile.close();
    } else {
        cout << "Erro ao abrir o arquivo de log." << endl;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cout << "Uso: " << argv[0] << " <arquivo_de_entrada>" << endl;
        return 1;
    }

    string inputFile = argv[1];

    cout << "Arquivo de entrada recebido: " << inputFile << endl;

   
    log("Arquivo de entrada: " + inputFile);

    return 0;
}
