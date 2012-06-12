#include <stdio.h>

#include "Event.h"
#include "EventHandler.h"
#include "Macros.h"
#include "Trace.h"
#include "Lost_records.h"
#include "Exit_to_guest.h"
#include "Exit_to_xen.h"


int lost_records_init(EventHandler *handler)
{
	return 0;
}

int lost_records_handler(EventHandler *handler, Event *event)
{
	/* Lost records received. Clear all handler data */
	if((event->ns - event->lastNs) > LOST_REC_MAX_TIME)
	{
		exit_to_guest_reset();
		exit_to_xen_reset();
	}
	return 0;
}

int lost_records_finalize(EventHandler *handler)
{
	return 0;
}

struct EventHandler lostRecordsHandler = 
{
	.name = "lost_records",
	.event_id = TRC_LOST_RECORDS,
	.init = lost_records_init,
	.data = (void *)NULL,
	.process_event = lost_records_handler,
	.finalize = lost_records_finalize,
};
