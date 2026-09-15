/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.class.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dminh <dminh@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/10 11:46:05 by dminh             #+#    #+#             */
/*   Updated: 2026/09/15 14:23:55 by dminh            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	CLIENT_CLASS_HPP
# define CLIENT_CLASS_HPP

# include <iostream>
# include <unistd.h>
# include <string>

class	Client
{
	private:
		std::string	_buf;
		std::string	_username;
		std::string	_nickname;
		std::string	_ip;
		int			_fd;
		bool		_isPassOk;
		bool		_isRegistered;
	public:
		Client(int socket);
		Client(const Client &cpy);
		~Client(void);
		Client		&operator=(const Client &src);

		void		appendData(std::string data);
		bool		isMessageComplete(void) const;
		std::string	extractMessage(void);

		int			getFd(void) const;
		bool		isPassOk(void) const;
		void		setPassOk(bool val);
		bool		isRegistered(void) const;
		void		setRegistered(bool val);
		void		setNickname(std::string nick);
		std::string	getNickname(void) const;
		void		setUsername(std::string user);
		std::string	getUsername(void) const;
		void		setChannel(void);
};

#endif
