/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.class.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dminh <dminh@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/10 11:46:46 by dminh             #+#    #+#             */
/*   Updated: 2026/09/11 16:03:08 by dminh            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.class.hpp"

Client::Client(int socket)
:	_fd(socket), _isPassOk(false), _isRegistered(false)
{
}

Client::Client(const Client &cpy)
:	_buf(cpy._buf), _username(cpy._username), _nickname(cpy._nickname),
	_ip(cpy._ip), _fd(cpy._fd)
{
}

Client	&Client::operator=(const Client &src)
{
	if (this != &src)
	{
		this->_buf = src._buf;
		this->_username = src._username;
		this->_nickname = src._nickname;
		this->_ip = src._ip;
		this->_fd = src._fd;
	}
	return (*this);
}

Client::~Client(void)
{
}

void Client::appendData(std::string data)
{
	this->_buf += data;
}

bool Client::isMessageComplete(void) const
{
	if (this->_buf.find("\r\n") != std::string::npos || this->_buf.find("\n") != std::string::npos)
		return (true);
	return (false);
}

std::string Client::extractMessage(void)
{
	std::string message;
	size_t pos;

	pos = this->_buf.find("\r\n");
	if (pos != std::string::npos)
	{
		message = this->_buf.substr(0, pos);
		this->_buf.erase(0, pos + 2);
		return (message);
	}
	pos = this->_buf.find("\n");
	if (pos != std::string::npos)
	{
		message = this->_buf.substr(0, pos);
		this->_buf.erase(0, pos + 1);
	}
	return (message);
}

int Client::getFd(void) const { return this->_fd; }

bool Client::isPassOk(void) const { return this->_isPassOk; }
void Client::setPassOk(bool val) { this->_isPassOk = val; }

bool Client::isRegistered(void) const { return this->_isRegistered; }
void Client::setRegistered(bool val) { this->_isRegistered = val; }

void Client::setNickname(std::string nick) { this->_nickname = nick; }
std::string Client::getNickname(void) const { return this->_nickname; }

void Client::setUsername(std::string user) { this->_username = user; }
std::string Client::getUsername(void) const { return this->_username; }

