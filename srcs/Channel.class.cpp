/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.class.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dminh <dminh@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/14 13:59:17 by dminh             #+#    #+#             */
/*   Updated: 2026/09/14 13:59:17 by dminh            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.class.hpp"
#include "Channel.class.hpp"

Channel::Channel(Client &member)
{
	(void)member;
}

Channel::Channel(const Channel &cpy)
{
	(void)cpy;
}

Channel	&Channel::operator=(const Channel &src)
{
	if (this != &src)
	{
	}
	return (*this);
}

Channel::~Channel(void)
{
}

