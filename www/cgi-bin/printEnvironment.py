# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    printEnviroment.py                                 :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: davi <dasimoes@student.42sp.org.br>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/08/04 19:28:50 by davi              #+#    #+#              #
#    Updated: 2026/08/13 14:35:44 by davi             ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

import os

def main():
    print("Content-Type: text/html\r\n")
    print("\r\n")
    print("<h1> Hello from python </h1>")
    print("I am a CGI script to print environment variables:\n")
    for key, value in os.environ.items():
        print(f"Key: {key} - Value: {value}")

if __name__ == "__main__":
    main()
