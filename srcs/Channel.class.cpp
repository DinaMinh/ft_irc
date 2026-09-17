/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.class.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dminh <dminh@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/14 13:59:17 by dminh             #+#    #+#             */
/*   Updated: 2026/09/17 14:52:20 by dminh            ###   ########.fr       */
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
	else if (kicked == kicker)
		serv.sendMessage(kicker, "ERROR: You cannot kick yourself");
	else
	{
		if (this->_members.find(kicked) == this->_members.end())
			serv.sendMessage(kicker, "ERROR: This user is not part of this channel");
		else
		{
			this->_members.erase(kicked);
			if (this->_operators.find(kicked) != this->_operators.end())
				this->_operators.erase(kicked);
		}

	}

}

void	Channel::part(Server &serv, int member)
{
	if (this->_members.find(member) == this->_members.end())
		serv.sendMessage(member, "ERROR: You are not part of this channel");
	else
	{
			this->_members.erase(member);
			if (this->_operators.find(member) != this->_operators.end())
				this->_operators.erase(member);
			if (this->_members.size() == 1)
			{
				memIt it = this->_members.begin();
				if (this->_operators.find(it->first) != this->_operators.end())
					this->_operators.insert(std::make_pair(it->first,
								it->second));
			}
	}
}

void	Channel::msg(Server &serv, int sender, std::vector<std::string> args)
{
	if (this->_members.find(sender) == this->_members.end())
		serv.sendMessage(sender, "ERROR: You are not part of this channel");
	else
	{
		std::string announce = this->_members.find(sender)->second.getNickname() + ": ";

		for (std::vector<std::string>::iterator it = args.begin() + 1;
				it != args.end();
				++it)
			announce += " " + *it;
		for (memIt it = this->_members.begin(); it != this->_members.end();
				++it)
			if (it->first != sender)
				serv.sendMessage(it->first, announce);
	}
}

size_t	Channel::getSize(void) const
{
	return (this->_members.size());
}

Channel::~Channel(void)
{
}

