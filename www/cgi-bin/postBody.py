# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    postBody.py                                        :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/08/26 19:08:36 by dasimoes          #+#    #+#              #
#    Updated: 2026/08/26 19:20:27 by dasimoes         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

import os, sys

content_length = int(os.environ.get('CONTENT_LENGTH', 0))

body = sys.stdin.read(content_length)

def main():
	print("Content-Type: text/html\r\n\r\n", end="")
	print("<h1> POST Body - Python</h1>")
	print(f"<p>Body size of {len(body)} bytes</p>")
	print(f"{body}")
	

if __name__ == "__main__":
	main()
