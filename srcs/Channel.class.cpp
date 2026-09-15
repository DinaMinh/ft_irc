/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.class.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dminh <dminh@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/14 13:59:17 by dminh             #+#    #+#             */
/*   Updated: 2026/09/15 15:49:08 by dminh            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ASocket.class.hpp"
#include "Server.class.hpp"
#include "Client.class.hpp"
#include "Channel.class.hpp"

Channel::Channel(Client &member)
{
	if (this->_members.empty())
		this->_operators.insert(std::make_pair(member.getFd(), member));
	this->_members.insert(std::make_pair(member.getFd(), member));
}

Channel::Channel(const Channel &cpy)
{
		this->_members.insert(cpy._members.begin(), cpy._members.end());
		this->_operators.insert(cpy._operators.begin(), cpy._operators.end());
}

Channel	&Channel::operator=(const Channel &src)
{
	if (this != &src)
	{
	}
	return (*this);
}

void	Channel::addMember(Client &member)
{
	this->_members.insert(std::make_pair(member.getFd(), member));
}

bool	Channel::isMember(int fd) const
{
	for (memIt it = this->_members.begin(); it != this->_members.end();
			++it)
		if (it->first == fd)
			return (true);
	return (false);
}

bool	Channel::isOp(int fd) const
{
	for (memIt it = this->_operators.begin(); it != this->_operators.end();
			++it)
		if (it->first == fd)
			return (true);
	return (false);
}

void	Channel::kick(Server &serv, int kicker, int kicked)
{
	if (this->_operators.find(kicker) == this->_operators.end())
		serv.sendMessage(kicker, "ERROR: You are not an operator in this channel");
	(void)kicked;
}

Channel::~Channel(void)
{
}

