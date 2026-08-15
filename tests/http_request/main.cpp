#include <iostream>
#include <string>
#include "HttpRequestParser.hpp"
#include "HttpRequest.hpp"

// Função auxiliar para simular o recebimento não-bloqueante
void runTest(const std::string& testName, const std::string& rawRequest) {
    std::cout << "\033[1;34m===== Test: " << testName << " =====\033[0m\n";

    HttpRequestParser parser;
    RequestStatus status = PARSING_REQUEST_LINE;

    // Simulando o recv() lendo apenas 10 bytes por vez da rede
    size_t chunkSize = 10;

    for (size_t i = 0; i < rawRequest.length(); i += chunkSize) {
        std::string chunk = rawRequest.substr(i, chunkSize);

        // Alimenta o parser com o fragmento atual
        status = parser.feed(chunk.c_str(), chunk.length());

        // Se a FSM (Finite State Machine) concluiu ou deu erro, paramos de ler
        if (status == DONE || status == ERROR_BAD_REQUEST || status == ERROR_REQUEST_TOO_LARGE) {
            break;
        }
    }

    // Avaliação do Status Final
    std::cout << "Status do Parser: ";
    switch (status) {
        case DONE: std::cout << "\033[1;32mDONE\033[0m\n"; break;
        case ERROR_BAD_REQUEST: std::cout << "\033[1;31mERROR_BAD_REQUEST\033[0m\n"; break;
        case ERROR_REQUEST_TOO_LARGE: std::cout << "\033[1;31mERROR_REQUEST_TOO_LARGE\033[0m\n"; break;
        default: std::cout << "\033[1;33mINCOMPLETE (Aguardando mais dados)\033[0m\n"; break;
    }

    // Imprime o resultado usando a sobrecarga de operador da sua HttpRequest
    std::cout << parser.getHttpRequest() << std::endl;
}

int main() {
    // 1. Teste de GET padrão
    std::string getReq =
        "GET /index.html?user=marvin HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "User-Agent: curl/7.68.0\r\n"
        "\r\n";

    // 2. Teste de POST para CGI com body (Content-Length)
    std::string postReq =
        "POST /cgi-bin/script.py HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "Content-Length: 15\r\n"
        "\r\n"
        "name=42&lvl=100";

    // 3. Teste de Bad Request (Método inválido)
    std::string badReq =
        "BATATA / HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "\r\n";

    // 4. Teste de POST com Transfer-Encoding: chunked
    // O formato exige o tamanho em Hexa, seguido de \r\n, depois os dados, seguido de \r\n.
    // O final do body é marcado por um chunk de tamanho 0 seguido de \r\n\r\n.
    std::string chunkedReq =
        "POST /upload HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "Transfer-Encoding: chunked\r\n"
        "\r\n"
        "4\r\n"        // 4 bytes em Hex
        "Wiki\r\n"
        "5\r\n"        // 5 bytes em Hex
        "pedia\r\n"
        "F\r\n"        // 15 bytes em Hex
        " in \r\n\r\nchunks.\r\n"
        "0\r\n"        // Zero indica o fim
        "\r\n";

    runTest("Simple GET Request", getReq);
    runTest("POST Request with Body", postReq);
    runTest("Bad Request (Invalid Method)", badReq);
    runTest("POST Request with Chunked Body", chunkedReq);

    return 0;
}
