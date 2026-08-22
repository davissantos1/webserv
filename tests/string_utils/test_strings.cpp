#include "string_utils.hpp"
#include <iostream>
#include <string>

int	main( void )
{
	std::string	varias_barras("/////////////////////////");
	std::string	dotdot("../../../../../../../../../../../../../..");
	std::string dot("./././././.././././././././././././././././");
	std::string dir("/home/P0dzim/bin/");

	std::string teste1 = normalize_str(varias_barras);
	std::string teste2 = normalize_str(dotdot);
	std::string teste3 = normalize_str(dot);
	std::string teste4 = normalize_str(dir);

	std::cout << "original: " << varias_barras << " | " << "normalizada: " << teste1 << std::endl;
	std::cout << "original: " << dotdot << " | " << "normalizada: " << teste2 << std::endl;
	std::cout << "original: " << dot << " | " << "normalizada: " << teste3 << std::endl;
	std::cout << "original: " << dir << " | " << "normalizada: " << teste4 << std::endl;
	return (0);
}
