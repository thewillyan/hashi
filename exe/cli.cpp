#include <iostream>
#include <string>

using namespace std;

int main() {
    string command;

    cout << "Bem-vindo à CLI simples! Digite 'enter' para entrar na CLI e 'exit' para sair." << endl;

    while (true) {
        cout << "> ";
        cin >> command;

        if (command == "enter") {
            cout << "Você entrou na CLI. Para sair, digite 'exit'." << endl;
           
        } else if (command == "exit") {
            cout << "Saindo da CLI. Até mais!" << endl;
            break;
        } else {
            cout << "Comando inválido. Tente novamente." << endl;
        }
    }

    return 0;
}
