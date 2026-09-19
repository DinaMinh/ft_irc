/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.class.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dminh <dminh@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/21 11:00:47 by dminh             #+#    #+#             */
/*   Updated: 2026/09/19 18:21:13 by dminh            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ASocket.class.hpp"
#include "Client.class.hpp"
#include "Server.class.hpp"
#include "Channel.class.hpp"

Server::Server(const std::string &port, const std::string &pw)
:	ASocket(port, pw)
{
	if ((this->_serv_fd = socket(SOCKET_DOMAIN, TYPE, PROTOCOL)) == 0)
		throw	std::runtime_error("ERROR: Couldn't open the socket.");
	std::cout << "serv fd = " << this->_serv_fd << std::endl;
	
	if (fcntl(this->_serv_fd, F_SETFL, O_NONBLOCK) == -1)
		throw	std::runtime_error("ERROR: Couldn't set the socket flags");
	this->setCmdMap();
}

Server::Server(const Server &cpy)
:	ASocket(cpy._port, cpy._pw)
{}

Server	&Server::operator=(const Server &src)
{
	if (this != &src){}
	return (*this);
}

void	Server::setCmdMap(void)
{
	this->_cmd.insert(std::make_pair("PASS", &Server::cmdPass));
	this->_cmd.insert(std::make_pair("NICK", &Server::cmdNick));
	this->_cmd.insert(std::make_pair("USER", &Server::cmdUser));
	this->_cmd.insert(std::make_pair("JOIN", &Server::cmdJoin));
	this->_cmd.insert(std::make_pair("KICK", &Server::cmdKick));
	this->_cmd.insert(std::make_pair("PART", &Server::cmdPart));
	this->_cmd.insert(std::make_pair("PRIVMSG", &Server::cmdPrivmsg));
	this->_cmd.insert(std::make_pair("TOPIC", &Server::cmdTopic));
	this->_cmd.insert(std::make_pair("INVITE", &Server::cmdInvite));
	this->_cmd.insert(std::make_pair("MODE", &Server::cmdMode));
	this->_cmd.insert(std::make_pair("PING", &Server::cmdPing));
}

void	Server::establishConnection(void)
{
	std::memset(&this->_addr, 0, sizeof(this->_addr));
	this->_addr.sin_family = AF_INET;
	this->_addr.sin_addr.s_addr = INADDR_ANY;
	this->_addr.sin_port = htons(atoi(this->_port.c_str()));
	
	int opt = 1;
	setsockopt(this->_serv_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	if (bind(this->_serv_fd, (sockaddr *)&_addr, sizeof(_addr)) < 0)
		throw std::runtime_error("ERROR: Couldn't bind the socket.");
	if (listen(this->_serv_fd, 1) != 0)
		std::cout << "Couldn't start listening." << std::endl;
	else
		std::cout << "Listening on port " << this->_port << "..." << std::endl;
	struct pollfd server_pollfd;
	server_pollfd.fd = this->_serv_fd;
	server_pollfd.events = POLLIN;
	server_pollfd.revents = 0;
	
	this->_fds.push_back(server_pollfd);
}

void	Server::sendError(Client &client, int code, std::string arg)
{
	std::string	err = ":" + this->_host + " " + numToStr(code) + " ";
	
	if (client.getNickname().empty())
		err += "* ";
	else
		err += client.getNickname() + " ";
	err += arg;
	switch (code)
	{
		case ERR_NOSUCHNICK:
			err += " :No such nick/channel";
			break ;
		case ERR_NOSUCHCHANNEL:
			err += " :No such channel";
			break ;
		case ERR_NOTONCHANNEL:
			err += " :You're not on channel";
			break ;
		case ERR_USERONCHANNEL:
			err += " :is already on channel";
			break ;
		case ERR_NEEDMOREPARAMS:
			err += " :Not enough parameters";
			break ;
		case ERR_NOINVITEONLYCHAN:
			err += " :Cannot join channel";
			break ;
		case ERR_CHANOPRIVSNEEDED:
			err += " :You're not channel operator";
			break ;
		default:
			err += " :Unknown error";
			break ;
	}
	this->sendMessage(client.getFd(), err);
}

void	Server::run(void)
{
	while (this->_running)
	{
		int fds_ready = poll(this->_fds.data(), this->_fds.size(), -1);
		
		if (fds_ready < 0)
		{
			if (errno == EINTR)
				continue;
			throw std::runtime_error("ERROR: poll failed");
		}
		
		for (size_t i = 0; i < this->_fds.size(); i++)
		{
			if (this->_fds[i].revents & POLLIN)
			{
				if (this->_fds[i].fd == this->_serv_fd)
					this->acceptClient();
				else
					this->receiveData(this->_fds[i].fd);
			}
		}
	}
}

void	Server::acceptClient(void)
{
	this->_accept_sock = accept(this->_serv_fd, NULL, NULL);
	if (this->_accept_sock == -1)
		throw std::runtime_error("ERROR: Couldn't accept the connection");
	std::cout << "Connection accepted !" << std::endl;
	int flags = fcntl(this->_accept_sock, F_GETFL, 0);
	if (fcntl(this->_accept_sock, F_SETFL, flags | O_NONBLOCK) == - 1)
		throw std::runtime_error("ERROR: Couldn't set the socket flags");
		
	this->_clients.insert(std::make_pair(this->_accept_sock,
				Client(this->_accept_sock)));
	struct pollfd client_pollfd;
	client_pollfd.fd = this->_accept_sock;
	client_pollfd.events = POLLIN;
	client_pollfd.revents = 0;
	this->_fds.push_back(client_pollfd);
}

void	Server::receiveData(int client_fd)
{
	this->_bytes = recv(client_fd, this->_buf, DATA_SIZE - 1, 0);
	if (this->_bytes <= 0)
	{
		std::cout << "Client disconnected." << std::endl;
		for (mapIt it = this->_clients.begin(); it != this->_clients.end(); ++it)
		{
			if (it->first == client_fd)
			{
				this->_clients.erase(it);
				break ;
			}
		}
		for (std::vector<struct pollfd>::iterator it = this->_fds.begin(); it != this->_fds.end(); ++it)
		{
			if (it->fd == client_fd)
			{
				this->_fds.erase(it);
				break;
			}
		}
		close(client_fd);
	}
	else
	{
		this->_buf[this->_bytes] = '\0';
		mapIt it = this->_clients.find(client_fd);
		if (it != this->_clients.end())
		{
			it->second.appendData(this->_buf);
			while (it->second.isMessageComplete())
			{
				std::string full_message = it->second.extractMessage();
				this->parseAndExecute(full_message, client_fd);
			}
		}
	}
}

void	Server::closeFd(void)
{
	close(this->_serv_fd);
}

void Server::parseAndExecute(std::string message, int client_fd)
{
	if (message.empty())
		return;

	std::istringstream iss(message);
	std::string command;
	iss >> command;

	for (size_t i = 0; i < command.length(); ++i)
		command[i] = std::toupper(command[i]);

	std::vector<std::string> args;
	std::string word;

	while (iss >> word)
	{
		if (word[0] == ':')
		{
			std::string trailing = word.substr(1);
			std::string rest;
			std::getline(iss, rest);
			trailing += rest;
			args.push_back(trailing);
			break;
		}
		else
		{
			args.push_back(word);
		}
	}
	mapIt it = this->_clients.find(client_fd);
	if (it == this->_clients.end())
		return;
		
	Client &client = it->second;
	cmdIt cmd = this->_cmd.find(command);

	if (cmd != this->_cmd.end())
		(this->*(cmd->second))(client, args);
	else
	{
		if (!client.isRegistered())
			this->sendMessage(client_fd, "ERROR :You are not registered yet");
		else
			this->sendMessage(client_fd, "ERROR :Unknown command");
	}
	/* Test parsing (visualiser les arg)
	std::cout << "Client " << client_fd << " a envoyé :" << std::endl;
	std::cout << "  -> COMMANDE : [" << command << "]" << std::endl;
	for (size_t i = 0; i < args.size(); i++)
		std::cout << "  -> ARG " << i + 1 << "    : [" << args[i] << "]" << std::endl; 
	*/
}

void Server::sendMessage(int client_fd, std::string message)
{
	std::string full_message = message + "\r\n";
	send(client_fd, full_message.c_str(), full_message.length(), 0);
}

void Server::cmdPass(Client &client, std::vector<std::string> args)
{
	if (args.empty())
	{
		this->sendMessage(client.getFd(), "ERROR :Missing password");
		return;
	}
	if (args[0] == this->_pw)
	{
		client.setPassOk(true);
	}
	else
	{
		this->sendMessage(client.getFd(), "ERROR :Invalid password");
	}
}

bool	Server::checkRequirements(Client &client)
{
	if (!client.isPassOk())
	{
		this->sendMessage(client.getFd(), "ERROR :You must send PASS first");
		return (false);
	}
	else if (!client.isRegistered())
	{
		this->sendMessage(client.getFd(),
				"ERROR: You must set a nickname AND a username first");
		return (false);
	}
	return (true);
}

void Server::cmdNick(Client &client, std::vector<std::string> args)
{
	if (!client.isPassOk())
	{
		this->sendMessage(client.getFd(), "ERROR :You must send PASS first");
		return;
	}
	if (args.empty())
	{
		this->sendMessage(client.getFd(), "ERROR :Missing nickname");
		return;
	}
	int	exists = this->findClient(args.front());
	if (exists != 0 && exists != client.getFd())
		this->sendMessage(client.getFd(), "ERROR: Nickname is already in use");
	else
		client.setNickname(args[0]);
}

void Server::cmdUser(Client &client, std::vector<std::string> args)
{
	if (!client.isPassOk())
	{
		this->sendMessage(client.getFd(), "ERROR :You must send PASS first");
		return;
	}
	if (args.size() < 4)
	{
		this->sendMessage(client.getFd(), "ERROR :Not enough parameters for USER");
		return;
	}
	client.setUsername(args[0]);
	if (!client.getNickname().empty() && !client.isRegistered())
	{
		client.setRegistered(true);
		this->sendMessage(client.getFd(),
				numToStr(001) + " "
				+ client.getNickname() + " :Welcome to the IRC ");
	}
}

void	Server::cmdJoin(Client &client, std::vector<std::string> args)
{
	if (args.size() < 1 || args.size() >  2)
		this->sendMessage(client.getFd(), "ERROR: Join only takes one channel");
	else if (!this->checkRequirements(client))
		return ;
	else if (!this->isChannel(args))
		this->sendError(client, ERR_NOSUCHCHANNEL, args.front());
	else
	{
		if (!this->_channels.empty())
		{
			chanIt	it = this->_channels.find(args.front());

			if (it == this->_channels.end())
				this->createChannel(client, args);
			else
				it->second.join(*this, client, args);
		}
		else
			this->createChannel(client, args);
	}
}

void	Server::sendAll(std::string  announce)
{
	for (mapIt	it = this->_clients.begin();
			it != this->_clients.end();
			++it)
		this->sendMessage(it->second.getFd(), announce);
}

void Server::cmdPing(Client &client, std::vector<std::string> args)
{
	if (args.empty())
	{
		this->sendMessage(client.getFd(), "ERROR :No origin specified");
		return;
	}
	this->sendMessage(client.getFd(), "PONG " + args[0]);
}

void	Server::createChannel(Client &client, std::vector<std::string> args)
{
	Channel	chan(client, args.front());

	this->_channels.insert(std::make_pair(args.front(), chan));
	this->sendMessage(client.getFd(), ":" + client.getNickname() + " JOIN :" + args.front());
}

bool	Server::isChannel(std::vector<std::string> args)
{
	std::string	c = "&#+!";
	if (args.empty())
		return (false);
	if (args.front().size() < 2)
		return (false);
	return (c.find(args.front()[0]) != std::string::npos
			&& args.front().size() < 52);

}

int	Server::findClient(std::string nickname)
{
	for (mapIt it = this->_clients.begin(); it != this->_clients.end();
			++it)
		if (it->second.getNickname() == nickname)
			return (it->first);
	return (0);
}

bool	Server::chanRequirements(Client &client, std::vector<std::string> args)
{
	if (!this->checkRequirements(client))
		return (false);
	else if (!this->isChannel(args))
	{
		this->sendError(client, ERR_NOSUCHCHANNEL, args.front());
		return (false);
	}
	return (true);
}

void	Server::cmdKick(Client &client, std::vector<std::string> args)
{
	if (!this->chanRequirements(client, args))
		return ;
	else
	{
		if (args.size() < 2)
		{
			this->sendMessage(client.getFd(), "ERROR: Not enough parameters");
			return ;
		}
		int	kickFd = this->findClient(args.at(1));
		if (kickFd == 0)
			this->sendMessage(client.getFd(), "ERROR: Client doesn't exist");
		chanIt	it = this->_channels.find(args.front());
		if (it == this->_channels.end())
			this->sendMessage(client.getFd(), "ERROR: Channel doesn't exist");
		else
			it->second.kick(*this, client.getFd(), kickFd);
	}

}

void	Server::cmdPart(Client &client, std::vector<std::string> args)
{
	if (!this->chanRequirements(client, args))
		return ;
	else
	{
		chanIt	it = this->_channels.find(args.front());
		if (it == this->_channels.end())
			this->sendMessage(client.getFd(), "ERROR: Channel doesn't exist");
		else
		{
			it->second.part(*this, client.getFd());
			if (it->second.getSize() == 0)
			{
				this->_channels.erase(it->first);
				this->sendAll("Deleting channel " + args.front());
			}
		}
	}
}

void	Server::cmdPrivmsg(Client &client, std::vector<std::string> args)
{
	if (!this->checkRequirements(client))
		return ;
	else
	{
		if (args.size() < 2)
			this->sendMessage(client.getFd(), "ERROR: No message sent");
		else
		{
			chanIt	it = this->_channels.find(args.front());
			if (it == this->_channels.end())
			{
				int	receivee = this->findClient(args.front());
				if (receivee != 0)
				{
					std::string	msg = ":" + client.getNickname() + " PRIVMSG " + args.front();;
					for (std::vector<std::string>::iterator it = args.begin() + 1;
							it != args.end();
							++it)
						msg += " " + *it;
					this->sendMessage(receivee, msg);
				}
				else
					this->sendMessage(client.getFd(), "ERROR: Invalid target");
			}
			else
				it->second.msg(*this, client.getFd(), args);
		}
	}
}

void	Server::cmdTopic(Client &client, std::vector<std::string> args)
{
	if (!this->chanRequirements(client, args))
		return ;
	else
	{
		chanIt it = this->_channels.find(args.front());
		if (it == this->_channels.end())
			this->sendMessage(client.getFd(), "ERROR: Channel doesn't exist");
		else if (args.size() == 1)
		{
			if (!it->second.getTopic().empty())
				this->sendMessage(client.getFd(), RPL_TOPIC
						+ client.getNickname() + " " + args.front() + " :" + it->second.getTopic());
		}
		else
			it->second.setTopic(*this, client.getFd(), args);
	}
}

void	Server::cmdInvite(Client &client, std::vector<std::string> args)
{
	if (!this->checkRequirements(client))
		return ;
	if (args.size() < 2)
	{
		this->sendError(client, ERR_NEEDMOREPARAMS, "INVITE");
		return ;
	}
	std::string nickname = args[0];
	std::string channelName = args[1];

	chanIt it = this->_channels.find(channelName);
	if (it == this->_channels.end())
	{
		this->sendError(client, ERR_NOSUCHCHANNEL, channelName);
	}
	else
	{
		int	inviteId = this->findClient(nickname);
		if (inviteId == 0)
			this->sendError(client, ERR_NOSUCHNICK, nickname);
		else
			it->second.invite(*this, client, nickname, inviteId);
	}
}

void Server::cmdMode(Client &client, std::vector<std::string> args)
{
	if (args.size() < 1)
	{
		this->sendError(client, ERR_NEEDMOREPARAMS, "MODE");
		return ;
	}
	if (this->isChannel(args))
	{
		chanIt it = this->_channels.find(args[0]);
		if (it == this->_channels.end())
		{
			this->sendError(client, ERR_NOSUCHCHANNEL, args[0]);
			return ;
		}
		Channel &chan = it->second;

		if (args.size() == 1)
			return ;
		if (!chan.isOp(client.getFd()))
		{
			this->sendError(client, ERR_CHANOPRIVSNEEDED, args[0]);
			return ;
		}
		std::string modes = args[1];
		bool add = true;
		size_t arg_idx = 2;

		for (size_t i = 0; i < modes.length(); i++)
		{
			char c = modes[i];
			if (c == '+')
				add = true;
			else if (c == '-')
				add = false;
			else if (c == 'i')
				chan.setInviteOnly(add);
			else if (c == 't')
				chan.setTopicRestricted(add);
			else if (c == 'k')
			{
				if (add && arg_idx < args.size())
					chan.setPassword(args[arg_idx++]);
				else if (!add)
					chan.setPassword("");
			}
			else if (c == 'l')
			{
				if (add && arg_idx < args.size())
					chan.setLimit(atoi(args[arg_idx++].c_str()));
				else if (!add)
					chan.setLimit(0);
			}
			else if (c == 'o')
			{
				if (arg_idx < args.size())
				{
					int targetFd = this->findClient(args[arg_idx++]);

					if (targetFd == 0)
						this->sendMessage(client.getFd(), "ERROR: This client doesn't exist");
					else if (add)
						chan.addOp(*this, this->_clients.find(targetFd)->second, client);
					else
						chan.removeOp(*this, this->_clients.find(targetFd)->second, client);
				}
			}
			else
				this->sendMessage(client.getFd(), "472 " + client.getNickname() + " " + c + " :is unknown mode char to me");
		}
		std::string modeMsg = ":" + client.getNickname() + " MODE " + args[0] + " " + modes;

		for (size_t i = 2; i < arg_idx && i < args.size(); i++)
			modeMsg += " " + args[i];
		chan.sendChannel(*this, modeMsg);
	}
}

Server::~Server(void)
{
}
