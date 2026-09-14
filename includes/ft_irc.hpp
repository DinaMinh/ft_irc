/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dminh <dminh@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/16 08:56:15 by dminh             #+#    #+#             */
/*   Updated: 2026/09/11 15:24:00 by dminh            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <iostream>

# include <sys/socket.h>
# include <poll.h>
# include <fcntl.h>

# include "ParametersException.class.hpp"
# include "ASocket.class.hpp"
# include "Server.class.hpp"
# include "Client.class.hpp"

# define WRONG_PARAMS "error: Execution requires 2 parameters."
# define MIN_PORT 0
# define MAX_PORT 65535


#endif
