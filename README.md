# Hashi 🍣

![GitHub Issues](https://img.shields.io/github/issues/thewillyan/hashi)
![GitHub Pull Requests](https://img.shields.io/github/issues-pr/thewillyan/hashi)

Implementação de um índice Hash Extensível para a disciplina de SGBD (CK0114)
da Universidade Federal do Ceará.

# Executar

Primeiramente para a execução do programa é necessário atender ao seguintes
requisitos:

- Build: CMake 3.27+ e GNU Make 4.4+
- Testes (opcional): Catch2 3.4+

Após se certificar que as devidas depêndencias listadas estão instalada
[clone o repositório](https://docs.github.com/en/repositories/creating-and-managing-repositories/cloning-a-repository)
e execute o seguinte comando no diretório do projeto:

```shell
$ cmake build
$ cmake .
```

se nada ocorrer errado você está pronto para executar o projeto! 🚀 

---

Segue a seguir as opções de execução e seus respectivos comandos:

- Compilando o programa: `$ make hashi` 
- Compilando e rodando o programa: `$ make run`
- Executando os testes: `$ make test`
- Executando um teste em particular: `$ make <nome_do_teste>`

todos os binários são armazenados no diretório `bin` do projeto.
