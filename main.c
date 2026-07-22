
#include "codexion.h"

bool	has_to_stop(t_arguments *args)
{
	bool	stop;

	pthread_mutex_lock(&args->stop_lock);
	stop = args->stop;
	pthread_mutex_unlock(&args->stop_lock);
	return (stop);
}

void	log_message(t_arguments *args, int coder_id, char *message)
{
	long	elapsed_ms;

	pthread_mutex_lock(&args->log_lock);
	elapsed_ms = get_ms_time() - args->start_time;
	printf("%ld %d %s\n", elapsed_ms, coder_id, message);
	pthread_mutex_unlock(&args->log_lock);
}

bool	cooldown_elapsed(t_arguments *args, t_dongle *dongle)
{
	if (dongle->released_at == -1)
		return (true);
	if (dongle->released_at + args->dongle_cooldown < get_ms_time())
		return (true);
	return (false);
}

void	register_waiter(t_coder *coder, t_dongle *dongle)
{
	int	i;

	i = 0;
	while (i < 2)
	{
		if (dongle->queue[i].active == false)
		{
			dongle->queue[i].coder_id = coder->id;
			dongle->queue[i].arrival_time = get_ms_time();
			dongle->queue[i].deadline = coder->last_compile_start
				+ coder->arguments->time_to_burnout;
			dongle->queue[i].active = true;
			return ;
		}
		i++;
	}
}

void	unregister_waiter(t_dongle *dongle, int coder_id)
{
	int	i;

	i = 0;
	while (i < 2)
	{
		if (dongle->queue[i].active && dongle->queue[i].coder_id == coder_id)
		{
			dongle->queue[i].active = false;
			return ;
		}
		i++;
	}
}

int	get_next_coder_id(t_arguments *args, t_dongle *dongle)
{
	int		i;
	int		best_id;
	long	best_key;
	long	current_key;

	i = 0;
	best_id = -1;
	best_key = -1;
	while (i < 2)
	{
		if (dongle->queue[i].active == true)
		{
			if (args->scheduler == FIFO)
				current_key = dongle->queue[i].arrival_time;
			else
				current_key = dongle->queue[i].deadline;
			if (best_key == -1 || current_key < best_key)
			{
				best_key = current_key;
				best_id = dongle->queue[i].coder_id;
			}
		}
		i++;
	}
	return (best_id);
}

void	helper_pick_up_dongle(t_coder *coder, t_dongle *dongle)
{
	t_arguments *args;

	args = coder->arguments;
	pthread_mutex_lock(&dongle->lock);
	register_waiter(coder, dongle);
	while((dongle->in_use
		|| !cooldown_elapsed(coder->arguments, dongle)
		|| get_next_coder_id(args, dongle) != coder->id)
		&& has_to_stop(args) == false)
		pthread_cond_wait(&dongle->cond, &dongle->lock);
	unregister_waiter(dongle, coder->id);
	dongle->in_use = true;
	pthread_mutex_unlock(&dongle->lock);
		if (has_to_stop(args) == true)
			return ;
	log_message(coder->arguments, coder->id, "has taken a dongle");
}

void	pick_up_dongle(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;
	if (coder->id % 2 == 0)
	{
		first = coder->left;
		second = coder->right;
	}
	else
	{
		first = coder->right;
		second = coder->left;
	}
	helper_pick_up_dongle(coder, first);
	helper_pick_up_dongle(coder, second);
}

void	put_dongles_down(t_coder *coder)
{
	pthread_mutex_lock(&coder->left->lock);
	coder->left->in_use = false;
	coder->left->released_at = get_ms_time();
	pthread_cond_broadcast(&coder->left->cond);
	pthread_mutex_unlock(&coder->left->lock);

	pthread_mutex_lock(&coder->right->lock);
	coder->right->in_use = false;
	coder->right->released_at = get_ms_time();
	pthread_cond_broadcast(&coder->right->cond);
	pthread_mutex_unlock(&coder->right->lock);
}

void	coder_compile(t_coder *coder)
{
	pthread_mutex_lock(&coder->lock);
	coder->last_compile_start = get_ms_time();
	pthread_mutex_unlock(&coder->lock);

	log_message(coder->arguments, coder->id, "is compiling");
	usleep(coder->arguments->time_to_compile * 1000);

	pthread_mutex_lock(&coder->lock);
	coder->number_of_finished_compiles++;
	pthread_mutex_unlock(&coder->lock);
}

void	coder_debugg(t_coder *coder)
{
	log_message(coder->arguments, coder->id, "is debugging");
	usleep(coder->arguments->time_to_debug * 1000);
}

void	coder_refactor(t_coder *coder)
{
	log_message(coder->arguments, coder->id, "is refactoring");
	usleep(coder->arguments->time_to_refactor * 1000);
}

void	*coder_routine(void *arg)
{
	t_coder		*coder;
	t_arguments	*arguments;

	coder = (t_coder *) arg;
	arguments = coder->arguments;
	while (has_to_stop(arguments) == false)
	{
		pick_up_dongle(coder);
		if (!has_to_stop(arguments) == true)
			coder_compile(coder);
		if (!has_to_stop(arguments) == true)
			put_dongles_down(coder);
		if (!has_to_stop(arguments) == true)
			coder_debugg(coder);
		if (!has_to_stop(arguments) == true)
			coder_refactor(coder);
	}
	return NULL;
}

void	*wake_up_routine(void *args)
{
	int         i;
	t_arguments	*arguments;

	i = 0;
	arguments = (t_arguments *) args;
	while (has_to_stop(arguments) == false)
	{
		while (i < arguments->number_of_coders)
		{
			pthread_mutex_lock(&arguments->dongles[i].lock);
			if (arguments->dongles[i].in_use == false)
			{
				if (cooldown_elapsed(arguments, &arguments->dongles[i]) == true)
					pthread_cond_broadcast(&arguments->dongles[i].cond);
			}
			pthread_mutex_unlock(&arguments->dongles[i].lock);
			i++;
		}
		usleep(1000);
		i = 0;
	}
	return (NULL);
}

void	wake_up_all(t_arguments *args)
{
	int	i;

	i = 0;
	while (i < args->number_of_coders)
	{
		pthread_mutex_lock(&args->dongles[i].lock);
		pthread_cond_broadcast(&args->dongles[i].cond);
		pthread_mutex_unlock(&args->dongles[i].lock);
		i++;
	}
}

void	*monitoring_routine(void *args)
{
	int			i;
	t_coder		*coders;
	t_arguments	*arguments;
	int			finished_compiling;
	long		time_since_last_compile;

	i = 0;
	finished_compiling = 0;
	arguments = (t_arguments *) args;
	coders = arguments->coders;
	time_since_last_compile = 0;
	while (true)
	{
		while (i < arguments->number_of_coders)
		{
			pthread_mutex_lock(&arguments->coders[i].lock);
			time_since_last_compile = get_ms_time() - coders[i].last_compile_start;
			pthread_mutex_unlock(&arguments->coders[i].lock);
			if (time_since_last_compile > arguments->time_to_burnout)
			{
				log_message(arguments, coders[i].id, "has burned out");
				pthread_mutex_lock(&arguments->stop_lock);
				arguments->stop = true;
				wake_up_all(arguments);
				pthread_mutex_unlock(&arguments->stop_lock);
				return (NULL);
			}
			pthread_mutex_lock(&arguments->coders[i].lock);
			if (coders[i].number_of_finished_compiles >= arguments->number_of_compiles_required)
				finished_compiling++;
			pthread_mutex_unlock(&arguments->coders[i].lock);
			i++;
		}
		if (finished_compiling >= arguments->number_of_coders)
		{
			pthread_mutex_lock(&arguments->stop_lock);
			arguments->stop = true;
			wake_up_all(arguments);
			pthread_mutex_unlock(&arguments->stop_lock);
			return (NULL);
		}
		finished_compiling = 0;
		usleep(1000);
		i = 0;
	}
	return (NULL);
}

int	main(int argc, char **argv)
{
	int	i;
	t_arguments	args;
	pthread_t	wake_up_thread;
	pthread_t   monitoring_thread;

	if (argc != 9)
		return (fprintf(stderr, "Wrong input\n"), 1);
	args = parse_arguments(argv);
	pthread_mutex_init(&args.log_lock, NULL);
	pthread_mutex_init(&args.stop_lock, NULL);
	args.stop = false;
	if (check_arguments(args) == -1)
		return (1);
	if (init_dongles(&args) == -1)
		return (1);
	if (init_coders(&args) == -1)
		return (1);
	distribute_dongles(&args);
	print_args_struct(args);
	i = 0;
	if (pthread_create(&wake_up_thread, NULL, wake_up_routine, &args) != 0)
		return (fprintf(stderr, "Failed creating thread\n"), 1);
	if (pthread_create(&monitoring_thread, NULL, monitoring_routine, &args) != 0)
		return (fprintf(stderr, "Failed creating thread\n"), 1);
	while (i < args.number_of_coders)
	{
	    if (pthread_create(&args.coders[i].thread, NULL, coder_routine, &args.coders[i]) != 0)
	        return (fprintf(stderr, "Failed creating thread\n"), 1);
	    i++;
	}
	i = 0;
	while (i < args.number_of_coders)
	{
	    pthread_join(args.coders[i].thread, NULL);
	    i++;
	}
	pthread_join(wake_up_thread, NULL);
	pthread_join(monitoring_thread, NULL);
	free (args.coders);
	free (args.dongles);
	return (0);
}
