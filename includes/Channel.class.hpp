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

class	Client;

class	Channel
{
	private:
		std::map<int, Client>	_members;
	public:
		Channel(Client	&member);
		Channel(const Channel &cpy);
		~Channel(void);
		Channel	&operator=(const Channel &src);
};

#endif

