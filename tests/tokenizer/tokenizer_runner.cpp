#include "ConfigParser.hpp"
#include <iostream>
#include <string>

std::string tokenTypeToString(t_file_tokens type) {
    switch (type) {
        case TOKEN_L_BRACE: return "TOKEN_L_BRACE";
        case TOKEN_R_BRACE: return "TOKEN_R_BRACE";
        case TOKEN_SEMICOLON: return "TOKEN_SEMICOLON";
        case TOKEN_NEWLINE: return "TOKEN_NEWLINE";
        case TOKEN_WORD: return "TOKEN_WORD";
        default: return "UNKNOWN";
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return 1;
    }

    ConfigParser parser;
    parser.parse(argv[1]);

    const std::deque< std::pair<t_file_tokens, std::string> >& tokens = parser.getTokens();
    for (std::deque< std::pair<t_file_tokens, std::string> >::const_iterator it = tokens.begin(); it != tokens.end(); ++it) {
        const std::pair<t_file_tokens, std::string>& token = *it;
        std::cout << tokenTypeToString(token.first) << " | " << token.second << std::endl;
    }

    return 0;
}
