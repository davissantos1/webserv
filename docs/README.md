*Este projeto foi criado como parte do currículo da 42 por dasimoes, dde-sou2 e vitosant.*

# Webserv

## Descrição

Este projeto tem como objetivo implementar um servidor HTTP em C++ 98, seguindo os princípios básicos do protocolo HTTP e a arquitetura de servidores web modernos. O programa é responsável por receber requisições de clientes, interpretar os métodos HTTP, localizar e servir arquivos, tratar erros e executar scripts CGI quando necessário.

A ideia central do projeto é compreender na prática como um navegador conversa com um servidor, como uma requisição é processada, como o servidor responde com headers e status codes corretos, e como a configuração do ambiente pode influenciar o comportamento do serviço.

Ao longo do desenvolvimento, o servidor foi estruturado para lidar com:

- múltiplas conexões e sockets;
- parsing de requisições HTTP;
- arquivos estáticos e diretórios;
- tratamento de erros HTTP;
- execução de CGI;
- leitura de configurações por arquivo;
- suporte a métodos como GET, POST e DELETE.

## Instruções

### Compilação

Na raiz do projeto, execute:

```bash
make
```

Esse comando compila o projeto e gera o binário `webserv`.

Para limpar os arquivos compilados:

```bash
make clean
make fclean
```

### Execução

```bash
./webserv [arquivo_de_configuracao.conf]
```

Exemplo:

```bash
./webserv tests/config_files/simple.conf
```

### Testes auxiliares

O projeto inclui testes específicos para validação do parser e de utilitários:

```bash
make test_config_parser
make test_tokenizer
make test_request_parser
make test_string_utils
```

## Estrutura do projeto

```text
.
├── Makefile
├── README.md
├── docs/
│   ├── PARSER_DOC.md
│   └── README.md
├── eval_tests/
│   ├── cgi_tester
│   ├── e2e.sh
│   ├── tester
│   ├── urls.txt
│   ├── valgrind.sh
│   └── YoupiBanane/
│       ├── nop/
│       └── Yeah/
├── include/
│   ├── CgiEnvironment.hpp
│   ├── CgiHandler.hpp
│   ├── CgiParser.hpp
│   ├── Client.hpp
│   ├── ConfigParser.hpp
│   ├── HttpRequest.hpp
│   ├── HttpRequestParser.hpp
│   ├── HttpResponse.hpp
│   ├── HttpResponseBuilder.hpp
│   ├── Location.hpp
│   ├── Multiplexer.hpp
│   ├── Server.hpp
│   ├── Session.hpp
│   ├── StaticFileHandler.hpp
│   ├── VirtualHostConfig.hpp
│   ├── color.hpp
│   ├── string_utils.hpp
│   └── webserv.hpp
├── src/
│   ├── CgiEnvironment.cpp
│   ├── CgiHandler.cpp
│   ├── CgiParser.cpp
│   ├── Client.cpp
│   ├── ConfigParser.cpp
│   ├── HttpRequest.cpp
│   ├── HttpRequestParser.cpp
│   ├── HttpResponse.cpp
│   ├── HttpResponseBuilder.cpp
│   ├── Location.cpp
│   ├── Multiplexer.cpp
│   ├── Server.cpp
│   ├── Session.cpp
│   ├── StaticFileHandler.cpp
│   ├── string_utils.cpp
│   ├── VirtualHostConfig.cpp
│   └── webserv.cpp
├── tests/
│   ├── config_files/
│   ├── http_request/
│   ├── parser/
│   ├── string_utils/
│   └── tokenizer/
├── www/
│   ├── cgi-bin/
│   ├── css/
│   ├── error_pages/
│   ├── test_autoindex/
│   ├── test_pages/
│   └── index.html
├── .gitignore
└── webserv
```

## Recursos

Abaixo estão algumas referências clássicas e materiais que ajudaram no desenvolvimento do projeto:

- RFC 7230, RFC 7231, RFC 7232 e RFC 7235 — documentação oficial do protocolo HTTP;
- Documentação do NGINX — referência prática sobre configuração e comportamento de servidores web;
- Beej's Guide to Network Programming — estudo de sockets, redes e comunicação cliente-servidor;
- Manuais do Linux e POSIX sobre `socket`, `bind`, `listen`, `accept`, `poll`, `select`, `fcntl` e manipulação de descritores de arquivo;
- exemplos de implementação de CGI e processamento de requisições HTTP.

### Uso da IA

A IA foi utilizada de forma crítica como apoio em algumas etapas do projeto, principalmente para:

- revisar conceitos de HTTP, CGI e arquitetura de servidores web;
- auxiliar na estruturação de classes e organização do código;
- gerar ideias para testes e validação de comportamento;
- revisar trechos de código e identificar pontos de melhoria antes da implementação final;
- apoiar a criação e refinamento de arquivos de configuração e testes iniciais.

Todo o código foi revisado, testado e ajustado manualmente para garantir que a lógica fosse compreendida e que o comportamento do servidor estivesse alinhado com os requisitos do projeto.

## Observações finais

- O projeto deve ser compilado com C++ 98 e flags como `-Wall -Wextra -Werror`.
- O servidor deve ser robusto e não deve travar ou encerrar de forma inesperada.
- A implementação foi desenvolvida com foco em aprender redes, sockets, HTTP e servidores web de forma prática.

## Autores

- dasimoes
- dde-sou2
- vitosant

