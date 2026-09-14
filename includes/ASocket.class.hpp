/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ASocket.class.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dminh <dminh@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 16:47:52 by dminh             #+#    #+#             */
/*   Updated: 2026/09/10 11:41:48 by dminh            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	ASOCKET_CLASS_HPP
# define ASOCKET_CLASS_HPP

# include <iostream>
# include <sys/socket.h>
# include <exception>
# include <netinet/in.h>
# include <cstdlib>
# include <cstring>
# include <unistd.h>

/*	AF_INET: Ipv4
 *	SOCK_STREAM: TCP
 *	PROTOCOL is 0 because usually, only one protocol exists 
 *	LOC_HOST is the default local host ip */

# define SOCKET_DOMAIN AF_INET
# define TYPE SOCK_STREAM
# define PROTOCOL 0
# define LOC_HOST "127.0.0.1"

class	ASocket
{
	protected:
		ASocket(const ASocket &cpy);
		ASocket	&operator=(const ASocket &src);
		const std::string	_host;
		const std::string	_port;
		const std::string	_pw;
		bool				_running;
		struct sockaddr_in	_addr;
	public:
		ASocket(const std::string &port, const std::string &pass);
		~ASocket(void);
		virtual void	establishConnection (void) = 0;
		bool			getRun(void) const;
		void			setRun(bool val);
		virtual void	closeFd(void) = 0;
		
};

#endif

