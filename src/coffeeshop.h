#ifndef INC_coffeeshop_h
#define INC_coffeeshop_h

#include <ross.h>

#define MEAN_ARRIVAL 3.0
#define MEAN_ORDER 30.0
#define MEAN_COLLECT 50.0

typedef enum coffeeshop_event_t coffeeshop_event_t;
typedef struct coffeeshop_state coffeeshop_state;
typedef struct coffeeshop_message coffeeshop_message;

enum coffeeshop_event_t
{
	ARRIVAL = 1,
	ORDER,
	COLLECT
};

struct coffeeshop_state
{
	int		orders_queued;
	int		orders_fulfilled;

	tw_stime	waiting_time;
	tw_stime	furthest_order_collecting;
};

struct coffeeshop_message
{
	coffeeshop_event_t	 type;

	tw_stime	 waiting_time;
	tw_stime	 saved_furthest_order_collecting;
};

static tw_stime lookahead = 0.00000001;
static tw_lpid	 nlp_per_pe = 1024;
static int	 opt_mem = 1000;
static int	 customers_per_coffeeshop = 1000;

static tw_stime	 wait_time_avg = 0.0;

#endif
