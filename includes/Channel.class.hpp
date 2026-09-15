/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.class.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dminh <dminh@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/14 13:39:09 by dminh             #+#    #+#             */
/*   Updated: 2026/09/14 13:39:09 by dminh            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	CHANNEL_CLASS_HPP
# define CHANNEL_CLASS_HPP

# include <map>

typedef std::map<int, Client>::const_iterator	memIt;

class	Server;
class	Client;

class	Channel
{
	private:
		std::map<int, Client>	_members;
		std::map<int, Client>	_operators;
	public:
		Channel(Client	&member);
		Channel(const Channel &cpy);
		~Channel(void);
		Channel	&operator=(const Channel &src);
		void	addMember(Client &member);
		bool	isMember(int fd) const;
		bool	isOp(int fd) const;
		void	kick(Server &serv, int kicker, int kicked);
};

#endif

