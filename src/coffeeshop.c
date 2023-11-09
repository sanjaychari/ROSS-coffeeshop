#include "coffeeshop.h"

tw_peid
mapping(tw_lpid gid)
{
	return (tw_peid) gid / g_tw_nlp;
}

void
init(coffeeshop_state * s, tw_lp * lp)
{
  int i;
  tw_event *e;
  coffeeshop_message *m;

  s->orders_queued = 0;
  s->orders_fulfilled = 0;
  s->waiting_time = 0.0;
  s->furthest_order_collecting = 0.0;

  for(i = 0; i < customers_per_coffeeshop; i++)
    {
      e = tw_event_new(lp->gid, tw_rand_exponential(lp->rng, MEAN_ARRIVAL), lp);
      m = tw_event_data(e);
      m->type = ARRIVAL;
      tw_event_send(e);
    }
}

void
event_handler(coffeeshop_state * s, tw_bf * bf, coffeeshop_message * msg, tw_lp * lp)
{
  int rand_result;
  tw_lpid dst_lp;
  tw_stime ts;
  tw_event *e;
  coffeeshop_message *m;

  switch(msg->type)
    {

    case ARRIVAL:
      {
	e = tw_event_new(lp->gid, tw_rand_exponential(lp->rng, MEAN_ORDER), lp);
        m = tw_event_data(e);
        m->type = ORDER;
        tw_event_send(e);
	break;
      }

    case ORDER:
      {
	s->orders_queued++;
	msg->saved_furthest_order_collecting = s->furthest_order_collecting;
        s->furthest_order_collecting = ROSS_MAX(s->furthest_order_collecting, tw_now(lp));
        ts = tw_rand_exponential(lp->rng, MEAN_COLLECT);
        e = tw_event_new(lp->gid, ts + s->furthest_order_collecting - tw_now(lp), lp);
        m = tw_event_data(e);
        m->type = COLLECT;
        m->waiting_time = s->furthest_order_collecting - tw_now(lp);
        s->furthest_order_collecting += ts;
        tw_event_send(e);
      }

    case COLLECT:
      {
	s->orders_queued--;
	s->orders_fulfilled++;
	s->waiting_time += msg->waiting_time;
	break;
      }

    }
}

void
rc_event_handler(coffeeshop_state * s, tw_bf * bf, coffeeshop_message * msg, tw_lp * lp)
{
  switch(msg->type)
  {
    case ARRIVAL:
     {
	tw_rand_reverse_unif(lp->rng);
	break;
      }
    case ORDER:
      {
	s->orders_queued--;
	s->furthest_order_collecting = msg->saved_furthest_order_collecting;
	tw_rand_reverse_unif(lp->rng);
	break;
      }
    case COLLECT:
      {
	s->orders_queued++;
	s->orders_fulfilled--;
	s->waiting_time -= msg->waiting_time;
	break;
      }
  }
}

void
final(coffeeshop_state * s, tw_lp * lp)
{
	wait_time_avg += ((s->waiting_time / (double) s->orders_fulfilled) / nlp_per_pe);
}

tw_lptype coffeeshop_lps[] =
{
	{
		(init_f) init,
        (pre_run_f) NULL,
		(event_f) event_handler,
		(revent_f) rc_event_handler,
		(commit_f) NULL,
		(final_f) final,
		(map_f) mapping,
		sizeof(coffeeshop_state),
	},
	{0},
};

const tw_optdef app_opt [] =
{
	TWOPT_GROUP("coffeeshop Model"),
    TWOPT_STIME("lookahead", lookahead, "lookahead for events"),
	TWOPT_UINT("ncustomers", customers_per_coffeeshop, "initial # of customers per coffeeshop(events)"),
	TWOPT_UINT("memory", opt_mem, "optimistic memory"),
	TWOPT_END()
};

int
main(int argc, char **argv, char **env)
{
	int i;

	tw_opt_add(app_opt);
	tw_init(&argc, &argv);

	nlp_per_pe /= (tw_nnodes());
	g_tw_events_per_pe =(customers_per_coffeeshop * nlp_per_pe) + opt_mem;

    g_tw_lookahead = lookahead;

	tw_define_lps(nlp_per_pe, sizeof(coffeeshop_message));

	for(i = 0; i < g_tw_nlp; i++)
		tw_lp_settype(i, &coffeeshop_lps[0]);

	tw_run();

	if(tw_ismaster())
	{
		printf("\ncoffeeshop Model Statistics:\n");
		printf("\t%-50s %11.4lf\n", "Average Waiting Time", wait_time_avg);
		printf("\t%-50s %11lld\n", "Number of coffeeshops",
			nlp_per_pe * tw_nnodes());
		printf("\t%-50s %11lld\n", "Number of customers",
			customers_per_coffeeshop * nlp_per_pe * tw_nnodes());
	}

	tw_end();

	return 0;
}
