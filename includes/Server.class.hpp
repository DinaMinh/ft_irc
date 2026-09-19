/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.class.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dminh <dminh@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/21 10:58:04 by dminh             #+#    #+#             */
/*   Updated: 2026/09/19 04:45:56 by dminh            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	SERVER_CLASS_HPP
# define SERVER_CLASS_HPP

# include <iostream>
# include <map>
# include <vector>
# include <fcntl.h>
# include <poll.h>
# include <string>
# include <cstring>
# include <stdexcept>
# include <unistd.h> 
# include <sstream>
# include <cctype>

# define DATA_SIZE 512
# define RPL_TOPIC ":server 332 "
# define RPL_INVITING "341 "

/* Forward declarations of ASocket, Client, Server and Channel */
class	ASocket;
class	Client;
class	Server;
class	Channel;

typedef std::map<int, Client>::iterator	mapIt;
typedef std::map<std::string, Channel>::iterator	chanIt;

typedef std::map<std::string,
		void (Server::*)(Client &, std::vector<std::string>)>::iterator	cmdIt;
enum	Errors
{
	ERR_NOSUCHNICK = 401,
	ERR_NOSUCHCHANNEL = 403,
	ERR_NOTONCHANNEL = 442,
	ERR_USERONCHANNEL = 443,
	ERR_NEEDMOREPARAMS = 461,
	ERR_NOINVITEONLYCHAN = 473,
	ERR_CHANOPRIVSNEEDED = 482
};

class	Server : public ASocket
{
	private:
		std::map<std::string,
			void (Server::*)(Client &,
					std::vector<std::string>)>	_cmd;
		std::map<std::string, Channel>			_channels;
		std::map<int, Client>					_clients;
		std::vector<struct pollfd>				_fds;
		char									_buf[DATA_SIZE];
		int										_serv_fd;
		int										_accept_sock;
		int										_bytes;
	public:
		Server(const std::string &port, const std::string &pass);
		Server(const Server &cpy);
		~Server(void);
		Server	&operator=(const Server &src);
		void	establishConnection(void);
		void	sendError(Client &client, int code, std::string arg);
		void	run(void);
		void	closeFd(void);
		void	acceptClient(void);
		void	receiveData(int client_fd);

		void	parseAndExecute(std::string message, int client_fd);
		void	sendMessage(int client_fd, std::string message);
		bool	checkRequirements(Client &client);
		bool	chanRequirements(Client &client, std::vector<std::string> args);

		void	cmdPass(Client &client, std::vector<std::string> args);
		void	cmdNick(Client &client, std::vector<std::string> args);
		void	cmdUser(Client &client, std::vector<std::string> args);
		void	cmdJoin(Client &client, std::vector<std::string> args);
		void	cmdKick(Client &client, std::vector<std::string> args);
		void	cmdPart(Client &client, std::vector<std::string> args);
		void	cmdPrivmsg(Client &client, std::vector<std::string> args);
		void	cmdTopic(Client &client, std::vector<std::string> args);
		void	cmdInvite(Client &client, std::vector<std::string> args);
		void	cmdMode(Client &client, std::vector<std::string> args);
		void	createChannel(Client &client, std::vector<std::string> args);
		void	setCmdMap(void);
		bool	isChannel(std::vector<std::string> args);
		int		findClient(std::string nickname);
		void	sendAll(std::string  announce);
		template < typename T >
		std::string numToStr(T number)
		{
			std::ostringstream ss;
			if (number < 10)
				ss << "00";
			else if (number < 100)
				ss << "0";
			ss << number << ' ';
			return ss.str();
		}
};

#endif
