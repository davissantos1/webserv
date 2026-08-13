/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printEnvironment.php                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: davi <dasimoes@student.42sp.org.br>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 16:33:47 by davi              #+#    #+#             */
/*   Updated: 2026/08/13 16:33:52 by davi             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

<?php

	header("Content-Type: text/html");

	echo "CGI Environment Variables:";

	foreach ($_SERVER as $key => $value)
	{
		echo sprintf("%-20s = %s\n", $key, $value);
	}
?>
