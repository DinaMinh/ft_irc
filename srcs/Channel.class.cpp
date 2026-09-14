/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.class.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dminh <dminh@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/14 13:59:17 by dminh             #+#    #+#             */
/*   Updated: 2026/09/14 17:10:54 by dminh            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
	return (!(this->_members.find(fd) == this->_members.end()));
}

Channel::~Channel(void)
{
}

