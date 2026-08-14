#include "ConfigParser.hpp"
#include <exception>
#include <iostream>
#include <string>

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Uso: " << argv[0] << " <arquivo.conf>" << std::endl;
		return 1;
	}

	ConfigParser parser;

	// O parsing começa aqui chamando o método público que você criou
	try {
		std::vector<VirtualHostConfig> servers = parser.parse(argv[1]);
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

		std::set<std::pair<std::string, std::string> > tuples = ConfigParser::extractLinten(servers);
		std::set<std::pair<std::string, std::string> >::iterator it;


		std::cout << "====HOSTS====" << std::endl;
		for (it = tuples.begin(); it != tuples.end(); ++it)
			std::cout << "Host: " << it->first << " | Port: " << it->second << std::endl;
	}
	catch( std::exception& e )
	{
		std::cout << e.what() << std::endl;
	}
	return 0;
}
