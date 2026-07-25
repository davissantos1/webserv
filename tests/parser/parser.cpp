#include "ConfigParser.hpp"
#include <iostream>

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Uso: " << argv[0] << " <arquivo.conf>" << std::endl;
		return 1;
	}

	ConfigParser parser;

	// O parsing começa aqui chamando o método público que você criou
	std::vector<VirtualHostConfig> servers = parser.parse(argv[1]);

	// Se a flag de erro no ConfigParser for setada, você limpa o vetor lá dentro,
	// então sabemos que falhou se estiver vazio.
	if (servers.empty())
	{
		std::cerr << "Erro: Arquivo de configuracao invalido ou vazio." << std::endl;
		return 1;
	}

	std::cout << "\n[ PARSEAMENTO CONCLUÍDO COM SUCESSO ]\n\n";

	for (std::size_t i = 0; i < servers.size(); ++i)
	{
		std::cout << "========================================\n";
		std::cout << " SERVIDOR #" << (i + 1) << "\n";
		std::cout << "========================================\n";
		std::cout << servers[i] << "\n";
	}

	return 0;
}
