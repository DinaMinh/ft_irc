/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.class.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dminh <dminh@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/14 13:59:17 by dminh             #+#    #+#             */
/*   Updated: 2026/09/19 04:57:19 by dminh            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ASocket.class.hpp"
#include "Server.class.hpp"
#include "Client.class.hpp"
#include "Channel.class.hpp"

Channel::Channel(Client &member, std::string name)
:	_name(name), _inviteOnly(false), _restricted(false), _limit(0)

{
	if (this->_members.empty())
		this->_operators.insert(std::make_pair(member.getFd(), member));
	this->_members.insert(std::make_pair(member.getFd(), member));
}

Channel::Channel(const Channel &cpy)
:	_name(cpy._name), _topic(cpy._topic)
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
	if (!this->isOp(kicker))
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

void	Channel::join(Server &serv, Client &client, std::vector<std::string> args)
{
	int fd = client.getFd();

	if (this->_members.find(fd) != this->_members.end())
	{
		serv.sendMessage(fd, "You're already in this channel !");
		return ;
	}
	if (this->_limit != 0 && this->_members.size() >= this->_limit)
	{
		serv.sendMessage(fd, "ERROR: The channel is full");
		return ;
	}
	if (this->_inviteOnly && !this->isInvited(client.getNickname()))
	{
		serv.sendMessage(fd, "ERROR: You're not invited");
		return ;
	}
	if (!this->_password.empty())
	{
		if (args.size() < 2 || args[1] != this->_password)
		{
			serv.sendMessage(fd, "ERROR: Incorrect channel key");
			return ;
		}
	}
	this->addMember(client);
	this->sendChannel(serv, ":" + client.getNickname() + " JOIN :" + this->_name);
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
			if (this->_operators.find(it->first) == this->_operators.end())
				this->_operators.insert(std::make_pair(it->first,
							it->second));
		}
	}
}

void	Channel::sendChannel(Server &serv, std::string msg)
{
	for (memIt it = this->_members.begin(); it != this->_members.end();
			++it)
		serv.sendMessage(it->first, msg);
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

std::string	Channel::getTopic(void) const
{
	return (this->_topic);
}

void	Channel::setTopic(Server &serv, int setter, std::vector<std::string> topic)
{
	std::string	whole;

	if (this->_members.find(setter) == this->_members.end())
		serv.sendMessage(setter, "ERROR: You are not part of this channel");
	if (this->_restricted && !this->isOp(setter))
		serv.sendMessage(setter, "ERROR: The topic is restricted");
	else
	{
		for (std::vector<std::string>::iterator it = topic.begin() + 1;
				it != topic.end(); ++it)
			whole += " "  + *it;
		this->_topic = whole;
		this->sendChannel(serv, ":"
				+ this->_members.find(setter)->second.getNickname()
				+ " TOPIC " +  this->_name + " :" + this->_topic);
	}
}

void	Channel::invite(Server &serv, Client &client, std::string invited, int invitedFd)
{
	if (this->_members.find(client.getFd()) == this->_members.end())
		serv.sendError(client, ERR_NOTONCHANNEL, "INVITE");
	else if (!isOp(client.getFd()))
		serv.sendError(client, ERR_CHANOPRIVSNEEDED, "INVITE");
	else
	{
		if (this->_members.find(invitedFd) != this->_members.end())
			serv.sendError(client, ERR_USERONCHANNEL, invited + " " + this->_name);
		else if (std::find(this->_invited.begin(), this->_invited.end(), invited)
				== this->_invited.end())
		{
			this->_invited.push_back(invited);
			serv.sendMessage(client.getFd(), RPL_INVITING
					+ client.getNickname() + " " + invited + " " + this->_name);
			serv.sendMessage(invitedFd, ":" + client.getNickname()
					+ " INVITE " + invited + " :" + this->_name);
		}
	}
}


bool	Channel::isInvited(std::string nickname) const
{
	return (std::find(this->_invited.begin(), this->_invited.end(), nickname)
			!= this->_invited.end());
}

void	Channel::setInviteOnly(bool choice)
{
	this->_inviteOnly = choice;
}

void	Channel::setTopicRestricted(bool choice)
{
	this->_restricted = choice;
}

void	Channel::setPassword(std::string pw)
{
	this->_password = pw;
}

void	Channel::setLimit(size_t limit)
{
	this->_limit = limit;
}

void	Channel::addOp(Server &serv, Client &member, Client &op)
{
	int	fd = member.getFd();
	if (this->_members.find(fd) == this->_members.end())
		serv.sendError(op, ERR_NOTONCHANNEL, "placeholder");
	else if (this->_operators.find(fd) == this->_operators.end())
		this->_operators.insert(std::make_pair(fd, member));
}

void	Channel::removeOp(Server &serv, Client &member, Client &op)
{
	int	fd = member.getFd();

	if (this->_members.find(fd) == this->_members.end())
		serv.sendError(op, ERR_NOTONCHANNEL, "placeholder");
	else if (this->_operators.find(fd) != this->_operators.end())
		this->_operators.erase(fd);
}

size_t	Channel::getLimit(void) const
{
	return (this->_limit);
}

bool	Channel::getInviteOnly(void) const
{
	return (this->_inviteOnly);
}

Channel::~Channel(void)
{
}

