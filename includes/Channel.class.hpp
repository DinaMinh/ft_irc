/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.class.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dminh <dminh@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/14 13:39:09 by dminh             #+#    #+#             */
/*   Updated: 2026/09/19 03:58:29 by dminh            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	CHANNEL_CLASS_HPP
# define CHANNEL_CLASS_HPP

# include <map>
# include <cstddef>
# include <vector>
# include <algorithm>

typedef std::map<int, Client>::const_iterator	memIt;

class	Server;
class	Client;

class	Channel
{
	private:
		std::map<int, Client>		_members;
		std::map<int, Client>		_operators;
		std::vector<std::string>	_invited;
		std::string					_name;
		std::string					_topic;
		std::string					_password;
		bool						_inviteOnly;
		bool						_restricted;
		size_t						_limit;
	public:
		Channel(Client	&member, std::string name);
		Channel(const Channel &cpy);
		~Channel(void);
		Channel	&operator=(const Channel &src);
		void	addMember(Client &member);
		bool	isMember(int fd) const;
		bool	isOp(int fd) const;
		void	kick(Server &serv, int kicker, int kicked);
		void	join(Server &serv, Client &client, std::vector<std::string> args);
		void	part(Server &serv, int kicker);
		void	sendChannel(Server &serv, std::string msg);
		void	msg(Server &serv, int sender, std::vector<std::string> args);
		size_t	getSize(void) const;
		std::string	getTopic(void) const;
		void	setTopic(Server &serv, int setter, std::vector<std::string> topic);
		void	invite(Server &serv, Client &client, std::string invited, int invitedFd);
		bool	isInvited(std::string nickname) const;
		void	setInviteOnly(bool choice);
		void	setTopicRestricted(bool choice);
		void	setPassword(std::string pw);
		void	setLimit(size_t limit);
		void	addOp(Server &serv, Client &member, Client &op);
		void	removeOp(Server &serv, Client &member, Client &op);
		size_t	getLimit(void) const;
		bool	getInviteOnly(void) const;
};

#endif

