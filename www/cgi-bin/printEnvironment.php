<?php

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printEnvironment.php                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dasimoes <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 18:49:13 by dasimoes          #+#    #+#             */
/*   Updated: 2026/08/26 18:49:13 by dasimoes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


	echo("Content-Type: text/html\r\n\r\n");

	echo("<h1> Hello from PHP </h1>");
	echo("<p>CGI Environment Variables:</p>");

	foreach ($_SERVER as $key => $value)
	{
		echo sprintf("<p>%-20s = %s</p>", $key, $value);
	}
?>
