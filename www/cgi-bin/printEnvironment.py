# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    printEnvironment.py                                :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: davi <dasimoes@student.42sp.org.br>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/08/04 19:28:50 by davi              #+#    #+#              #
#    Updated: 2026/08/26 18:06:15 by dasimoes         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

import os

def main():
    print("Content-Type: text/html\r\n\r\n", end="")
    print("<h1> Hello from python </h1>")
    print("<p>I am a CGI script to print environment variables:</p>")
    for key, value in os.environ.items():
        print(f"<p>Key: {key} - Value: {value}</p>")

if __name__ == "__main__":
    main()
