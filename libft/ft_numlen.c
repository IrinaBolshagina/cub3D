/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_numlen.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhogg <mhogg@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/20 20:57:50 by mhogg             #+#    #+#             */
/*   Updated: 2021/03/15 21:39:16 by mhogg            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int		ft_numlen(int num, int base)
{
	int	len;

	if (num == 0)
		return (1);
	len = 0;
	while (num > 0)
	{
		num = num / base;
		len++;
	}
	return (len);
}
