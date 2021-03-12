/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parcer.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhogg <mhogg@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/04 11:11:04 by mhogg             #+#    #+#             */
/*   Updated: 2021/03/12 15:50:37 by mhogg            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../cub.h"

void	skip_spaces(const char **str)
{
	while (**str == ' ' || **str == '\t' || **str == ',')
		(*str)++;
}

int		count_params(char const *s)
{
	int	i;
	int	count;

	i = 0;
	count = 0;
	while (s[i])
	{
		if ((s[i] != ' ' && s[i] != ',' && s[i] != '\t') &&
			(s[i + 1] == ' ' || s[i + 1] == ',' || s[i] == '\t'
			|| s[i + 1] == '\0'))
			count++;
		i++;
	}
	return (count);
}

int		check_params(const char *str, const char *check)
{
	int	i;

	i = 0;
	while (str[i] != '\0')
	{
		if (!ft_strchr(check, str[i]))
			return (1);
		i++;
	}
	return (0);
}

void	parce_r(const char *line, t_all *all)
{
	if (check_params(line, "0123456789 	,") || count_params(line) != 2)
		ft_error(ERR_CODE_2);
	skip_spaces(&line);
	all->scene->i_width = ft_atoi_move(&line);
	skip_spaces(&line);
	all->scene->i_height = ft_atoi_move(&line);
	all->flags->r++;
}

void	parce_s(char *line, t_all *all)
{
	if (*line == ' ')
	{
		++line;
		all->scene->sprite_file = ft_strtrim(line, " 	");
		all->flags->s++;
	}
	if (*line == 'O')
	{
		++line;
		all->scene->tex_south_file = ft_strtrim(line, " 	");
		all->flags->so++;
	}
}

void	parce_nwe(char *line, t_all *all)
{
	if (*line == 'N' && *(++line) == 'O')
	{
		++line;
		all->scene->tex_north_file = ft_strtrim(line, " 	");
		all->flags->no++;
	}
	else if (*line == 'W' && *(++line) == 'E')
	{
		++line;
		all->scene->tex_west_file = ft_strtrim(line, " 	");
		all->flags->we++;
	}
	else if (*line == 'E' && *(++line) == 'A')
	{
		++line;
		all->scene->tex_east_file = ft_strtrim(line, " 	");
		all->flags->ea++;
	}
	else
		ft_error(ERR_CODE_3);
}

int		parce_col(const char *line, t_all *all)
{
	int	r;
	int	g;
	int	b;

	skip_spaces(&line);
	r = ft_atoi_move(&line);
	skip_spaces(&line);
	g = ft_atoi_move(&line);
	skip_spaces(&line);
	b = ft_atoi_move(&line);
	if (r > 255 || g > 255 || b > 255)
		ft_error(ERR_CODE_4);
	return (r << 16 | g << 8 | b);
}

void	parce_color(const char *line, t_all *all)
{
	if (*line == 'F')
	{
		all->scene->floor_color = parce_col(++line, all);
		all->flags->f++;
	}
	if (*line == 'C')
	{
		all->scene->ceill_color = parce_col(++line, all);
		all->flags->c++;
	}
}

void	parce_player1(t_all *all, char c, int x, int y)
{
	all->var->pos_x = (double)y + 0.5;
	all->var->pos_y = (double)x + 0.5;
	printf("parcer pos_x = %.2f, pos_y = %.2f", all->scene->pos_x, all->scene->pos_x);
	
	all->flags->player++;
	all->scene->map[x][y] = '0';
	if (c == 'N')
	{
		all->var->dir_x = -1;
		all->var->dir_y = 0;
		all->var->plane_x = 0;
		all->var->plane_y = 0.66;
	}
	if (c == 'S')
	{
		all->var->dir_x = 1;
		all->var->dir_y = 0;
		all->var->plane_x = 0;
		all->var->plane_y = -0.66;
	}
}

void	parce_player(t_all *all, char c, int x, int y)
{
	parce_player1(all, c, x, y);
	printf("parcer pos_x = %.2f, pos_y = %.2f", all->scene->pos_x, all->scene->pos_x);

	if (c == 'W')
	{
		all->var->dir_x = 0;
		all->var->dir_y = -1;
		all->var->plane_x = -0.66;
		all->var->plane_y = 0;
	}
	if (c == 'E')
	{
		all->var->dir_x = 0;
		all->var->dir_y = 1;
		all->var->plane_x = 0.66;
		all->var->plane_y = 0;
	}
}

void	check_map(t_all *all, int x, int y)
{
	if (all->scene->map[y + 1][x] == ' ' ||
		all->scene->map[y - 1][x] == ' ' ||
		all->scene->map[y][x + 1] == ' ' ||
		all->scene->map[y][x - 1] == ' ' ||
		x == 0 || y == 0 ||
		all->scene->map[y][x + 1] == '\0' ||
		all->scene->map[y + 1][x] == '\0')
		ft_error(ERR_CODE_6);
}

void	parce_sprite(t_all *all)
{
	int		x;
	int		y;
	int		i;
	char	c;
	int		spr;

	y = -1;
	i = 0;
	spr = all->scene->spr_num;
	if (!(all->sprite = malloc(sizeof(t_sprite) * spr)))
		ft_error(ERR_CODE_0);
	while (all->scene->map[++y])
	{
		x = -1;
		while ((c = all->scene->map[y][++x]) != '\0')
		{
			if (c == '2')
			{
				all->sprite[i].x = (double)y + 0.5;
				all->sprite[i].y = (double)x + 0.5;
				i++;
			}
		}
	}
}

void	parce_map(t_all *all)
{
	int		x;
	int		y;
	char	c;

	all->scene->spr_num = 0;
	y = -1;
	while (all->scene->map[++y])
	{
		if (check_params(all->scene->map[y], "012 NWSE"))
			ft_error(ERR_CODE_5);
		x = -1;
		while ((c = all->scene->map[y][++x]) != '\0')
		{
			if (c == '0' || c == '2' || c == 'N' || c == 'S'
				|| c == 'W' || c == 'E')
				check_map(all, x, y);
			if (c == '2')
				all->scene->spr_num++;
			if (c == 'N' || c == 'S' || c == 'W' || c == 'E')
				parce_player(all, c, x, y);
		}
	}
	if (all->flags->player)
		ft_error(ERR_CODE_7);
	parce_sprite(all);
}

void	parce_params(char **params, t_all *all, int size)
{
	int	i;

	i = -1;
	while (params[++i])
	{
		if (params[i][0] == 'R')
			parce_r(++params[i], all);
		else if (ft_strchr("FC", params[i][0]) && count_params(params[i]) == 4)
			parce_color(params[i], all);
		else if (params[i][0] == 'S' && count_params(params[i]) == 2)
			parce_s(++params[i], all);
		else if (ft_strchr("NWE", params[i][0]) && count_params(params[i]) == 2)
			parce_nwe(params[i], all);
		else if (params[i][0] == '\n' || params[i][0] == '\0')
			;
		else if (ft_strchr("1 ", params[i][0]))
		{
			all->scene->map = params + i;
			all->scene->m_width = size - i;
			parce_map(all);
			break ;
		}
		else
			ft_error(ERR_CODE_1);
	}
}

char	**make_map(t_list **head, int size)
{
	char	**params;
	int		i;
	t_list	*tmp;

	if (!(params = malloc((size + 1) * sizeof(char *))))
		ft_error(ERR_CODE_0);
	i = -1;
	tmp = *head;
	while (tmp)
	{
		params[++i] = tmp->content;
		tmp = tmp->next;
	}
	return (params);
}

void	parcer(int fd, t_all *all)
{
	char	*line;
	t_list	*head;
	t_list	*tmp;
	char	**params;
	int		i;

	line = NULL;
	head = NULL;
	while (get_next_line(fd, &line))
		ft_lstadd_back(&head, ft_lstnew(line));
	ft_lstadd_back(&head, ft_lstnew(line));
	if (!(params = malloc((ft_lstsize(head) + 1) * sizeof(char *))))
		ft_error(ERR_CODE_0);
	i = -1;
	tmp = head;
	while (tmp)
	{
		params[++i] = tmp->content;
		tmp = tmp->next;
	}
	parce_params(params, all, ft_lstsize(head));
	if (all->flags->r || all->flags->c || all->flags->f || all->flags->s ||
	all->flags->no || all->flags->so || all->flags->we || all->flags->ea)
		ft_error(ERR_CODE_1);
}

void	struct_flags_init(t_all *all)
{
	all->flags->r = -1;
	all->flags->c = -1;
	all->flags->f = -1;
	all->flags->s = -1;
	all->flags->no = -1;
	all->flags->so = -1;
	all->flags->we = -1;
	all->flags->ea = -1;
	all->flags->player = -1;
}
