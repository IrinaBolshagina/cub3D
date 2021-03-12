/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhogg <mhogg@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/25 20:06:15 by mhogg             #+#    #+#             */
/*   Updated: 2021/03/11 10:00:07 by mhogg            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../cub.h"

int		close_func(void)
{
	exit(0);
}

void	calculate_ray(t_all all, int x)
{
 	double camera_x;
	camera_x = 2 * x / (double)all.scene->i_width - 1;
 	all.var->ray_dir_x = all.var->dir_x + all.var->plane_x * camera_x;
 	all.var->ray_dir_y = all.var->dir_y + all.var->plane_y * camera_x;
 	all.var->map_x = (int)all.var->pos_x;
 	all.var->map_y = (int)all.var->pos_y;
	all.var->delta_distx = sqrt(1 + (all.var->ray_dir_y * all.var->ray_dir_y) /
							(all.var->ray_dir_x * all.var->ray_dir_x));
	all.var->delta_disty = sqrt(1 + (all.var->ray_dir_x * all.var->ray_dir_x) /
							(all.var->ray_dir_y * all.var->ray_dir_y));
	all.var->hit = 0; 
		
}

void	calculate_step_dist(t_all all, int x)
{
	if(all.var->ray_dir_x < 0)
	{
		all.var->step_x = -1;
		all.var->side_distx = (all.var->pos_x - all.var->map_x) * all.var->delta_distx;
	}
	else
	{
		all.var->step_x = 1;
		all.var->side_distx = (all.var->map_x + 1.0 - all.var->pos_x) * all.var->delta_distx;
	}
	if(all.var->ray_dir_y < 0)
	{
		all.var->step_y = -1;
		all.var->side_disty = (all.var->pos_y - all.var->map_y) * all.var->delta_disty;
	}
	else
	{
		all.var->step_y = 1;
		all.var->side_disty = (all.var->map_y + 1.0 - all.var->pos_y) * all.var->delta_disty;
	}
}

void	perform_dda(t_all all, int x)
{
	while (all.var->hit == 0)
	{
		if(all.var->side_distx < all.var->side_disty)
		{
			all.var->side_distx += all.var->delta_distx;
			all.var->map_x += all.var->step_x;
			all.var->side = 0;
		}
		else
		{
			all.var->side_disty += all.var->delta_disty;
			all.var->map_y += all.var->step_y;
			all.var->side = 1;
		}
		if(all.scene->map[all.var->map_x][all.var->map_y] == '1')			// запомните твари 
			all.var->hit = 1;
	}
}

void	texture_start(t_all all)
{
	double wallX;
	if (all.var->side == 0) 
		wallX = all.var->pos_y + all.var->perp_wall_dist * all.var->ray_dir_y;
    else	wallX = all.var->pos_x + all.var->perp_wall_dist * all.var->ray_dir_x;
    	wallX -= floor((wallX));
    all.var->tex_x = (int)(wallX * (double)(tex_width));
    if(all.var->side == 0 && all.var->ray_dir_x > 0) 
		all.var->tex_x = tex_width - all.var->tex_x - 1;
    if(all.var->side == 1 && all.var->ray_dir_y < 0)
		all.var->tex_x = tex_width - all.var->tex_x - 1;
    all.var->step = 1.0 * tex_height / all.var->line_height;
	all.var->tex_pos = (all.var->draw_start - all.scene->i_height / 2 + all.var->line_height / 2) * all.var->step;
}

void	calculate_sides(t_all all, int x)
{
	if(all.var->side == 0)
		all.var->perp_wall_dist = (all.var->map_x - all.var->pos_x +
		(1 - all.var->step_x) / 2) / all.var->ray_dir_x;
	else
		all.var->perp_wall_dist = (all.var->map_y - all.var->pos_y +
		(1 - all.var->step_y) / 2) / all.var->ray_dir_y;
	all.var->z_buffer[x] = all.var->perp_wall_dist;
	all.var->asp_ratio = 0.75 * all.scene->i_width / all.scene->i_height;
	all.var->line_height = (int)(all.scene->i_height /
							all.var->perp_wall_dist * all.var->asp_ratio);
	all.var->draw_start = -all.var->line_height / 2 + all.scene->i_height / 2;
	if(all.var->draw_start < 0)
		all.var->draw_start = 0;
	all.var->draw_end = all.var->line_height / 2 + all.scene->i_height / 2;
	if(all.var->draw_end >= all.scene->i_height)
		all.var->draw_end = all.scene->i_height - 1;
}

void	put_west_east(t_all all, int x, int y)
{
	int color;
	
	if (all.var->side == 1)
	{
		if (all.var->step_y > 0)
		{
			color = my_mlx_pixel_take(all.texeast_img, all.var->tex_x, all.var->tex_y);
			my_mlx_pixel_put(all.data, x, y, color);
		}
		else if (all.var->step_y < 0)
		{
			color = my_mlx_pixel_take(all.texwest_img, all.var->tex_x, all.var->tex_y);
			my_mlx_pixel_put(all.data, x, y, color);
		}
	}
}

void	put_north_south(t_all all, int x, int y)
{
	int color;
	
	if (all.var->side == 0)
	{
		if (all.var->step_x > 0)
		{
			color = my_mlx_pixel_take(all.texsouth_img, all.var->tex_x, all.var->tex_y);
			my_mlx_pixel_put(all.data, x, y, color);
		}
		else	if (all.var->step_x < 0)
		{
			color = my_mlx_pixel_take(all.texnorth_img, all.var->tex_x, all.var->tex_y);
			my_mlx_pixel_put(all.data, x, y, color);
		}
	}
}

void	put_scene(t_all all)
{
	int x = -1;

	while (++x < all.scene->i_width)
    {
 		calculate_ray(all, x);
		calculate_step_dist(all, x);
		perform_dda(all, x);
		calculate_sides(all, x);
		texture_start(all);
		int	y = -1;
		int color;
		while (++y < all.scene->i_height)
		{
			if (y < all.var->draw_start)
				my_mlx_pixel_put(all.data, x, y, all.scene->floor_color);
			if (y > all.var->draw_end)
				my_mlx_pixel_put(all.data, x, y, all.scene->ceill_color);
			if (y >= all.var->draw_start && y <= all.var->draw_end)
			{
				all.var->tex_y = (int)all.var->tex_pos & (tex_height - 1);
				all.var->tex_pos += all.var->step;
				put_north_south(all, x, y);
				put_west_east(all, x, y);
			}
		}
	}
}

int			main(void)
{
	t_mlx		mlx;
	t_data		data;
	t_data		texnorth_img;
	t_data		texsouth_img;
	t_data		texwest_img;
	t_data		texeast_img;
	t_data		sprite_img;
	t_sprite	*sprite;
	t_scene		scene; // = {.i_width = 640, .i_height = 480};
	t_parce		flags;
	t_var		var = {.pos_x = 5, .pos_y = 5, .dir_x = -1, .dir_y = 1, .plane_x = 0, .plane_y = 0.66, .move_speed = 0.3, .rot_speed = 0.3};
	t_all		all;
	int			tex_w;
	int			tex_h;
	
	all.sprite_img = &sprite_img;
	all.sprite = sprite;
	all.mlx = &mlx;
	all.data = &data;
	all.var = &var;
	all.texnorth_img = &texnorth_img;
	all.texsouth_img = &texsouth_img;
	all.texwest_img = &texwest_img;
	all.texeast_img = &texeast_img;
	all.scene = &scene;
	all.flags = &flags;
	
	int		fd = open("scene.cub", O_RDONLY);
	struct_flags_init(&all);
	parcer(fd, &all);
	printf("west: %s\n", all.scene->tex_west_file);
	if(!(all.var->z_buffer = malloc(sizeof(double) * all.scene->i_width)))
		ft_error(ERR_CODE_0);
	printf("i_width = %d\n", all.scene->i_width);
	
	all.mlx->mlx_ptr = mlx_init();
	all.mlx->win_ptr = mlx_new_window(all.mlx->mlx_ptr, all.scene->i_width, all.scene->i_height, "cub3D");
	all.data->img = mlx_new_image(all.mlx->mlx_ptr, all.scene->i_width, all.scene->i_height);
	all.data->addr = mlx_get_data_addr(data.img, &data.bits_per_pixel, &data.line_length, &data.endian);
	all.texnorth_img->img = mlx_xpm_file_to_image(all.mlx->mlx_ptr, all.scene->tex_north_file, &tex_w, &tex_h);
	all.texnorth_img->addr = mlx_get_data_addr(texnorth_img.img, &texnorth_img.bits_per_pixel, &texnorth_img.line_length, &texnorth_img.endian);
	all.texsouth_img->img = mlx_xpm_file_to_image(all.mlx->mlx_ptr, all.scene->tex_south_file, &tex_w, &tex_h);
	all.texsouth_img->addr = mlx_get_data_addr(texsouth_img.img, &texsouth_img.bits_per_pixel, &texsouth_img.line_length, &texsouth_img.endian);
	all.texwest_img->img = mlx_xpm_file_to_image(all.mlx->mlx_ptr, all.scene->tex_west_file, &tex_w, &tex_h);
	all.texwest_img->addr = mlx_get_data_addr(texwest_img.img, &texwest_img.bits_per_pixel, &texwest_img.line_length, &texwest_img.endian);
	if(!(all.texeast_img->img = mlx_xpm_file_to_image(all.mlx->mlx_ptr, all.scene->tex_east_file, &tex_w, &tex_h)))
		ft_error(ERR_CODE_3);
	all.texeast_img->addr = mlx_get_data_addr(texeast_img.img, &texeast_img.bits_per_pixel, &texeast_img.line_length, &texeast_img.endian);
	all.sprite_img->img = mlx_xpm_file_to_image(all.mlx->mlx_ptr, all.scene->sprite_file, &tex_w, &tex_h);
	all.sprite_img->addr = mlx_get_data_addr(sprite_img.img, &sprite_img.bits_per_pixel, &sprite_img.line_length, &sprite_img.endian);
	
	put_scene(all);
	mlx_put_image_to_window(all.mlx->mlx_ptr, all.mlx->win_ptr, all.data->img, 0, 0);
	mlx_hook(all.mlx->win_ptr, 2, 1L<<0, key_hook, &all);
	//mlx_key_hook(all.mlx->win_ptr, key_hook, &all); 
	mlx_hook(all.mlx->win_ptr, 17, 1L<<0, close_func, 0); //exit on close window
	write(1, "open\n", 5);
	mlx_loop(all.mlx->mlx_ptr);
}