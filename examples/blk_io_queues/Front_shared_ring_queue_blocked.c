#include <stdio.h>
#include <string.h>

#include "Event.h"
#include "EventHandler.h"
#include "Macros.h"
#include "Trace.h"
#include "Lost_records.h"
#include "Front_shared_ring_queue_blocked.h"
#include "Front_shared_ring_queue_unblocked.h"

SRBlockData srblockData;

int front_shared_ring_queue_blocked_init(EventHandler *handler)
{
	memset(&srblockData, 0, sizeof(SRBlockData));
	return 0;
}

int front_shared_ring_queue_blocked_handler(EventHandler *handler, Event *event)
{
	unsigned long long lastSRUnblockNs = get_last_srunblock_ns();
	unsigned long long lastLostRecordNs = get_last_lost_records_ns(event->cpu);

	/* If lost_rec seen before the last block msg
	 * no need to reset last_block_ns
	 *
	 * last_unblock
	 *	.
	 * lost_rec
	 *	.
	 *	.
	 * last_block
	 *	.
	 *	.
	 * block
	 */

	if(lastLostRecordNs < srblockData.lastSRBlockNs)
	{
		if(lastSRUnblockNs < srblockData.lastSRBlockNs)
		{
			/* Successive Block msgs, w/o an unblock msg.
			 * Ignore the latest blocked queue event.
			 * */
			return 0;
		}
	}

	/* Seeing a SR block msg 1st time, after an unblock msg,
	 * or after a lost_rec event. */
	srblockData.lastSRBlockNs = event->ns;

	return 0;
}

int front_shared_ring_queue_blocked_finalize(EventHandler *handler)
{
	return 0;
}

void front_shared_ring_queue_blocked_reset(void)
{

}

unsigned long long get_last_srblock_ns()
{
	return srblockData.lastSRBlockNs;
}

struct EventHandler frontSharedRingQueueBlockedHandler = 
{
	.name = "front_shared_ring_queue_blocked",
	.event_id = TRC_BLKFRONT_SHARED_RING_QUEUE_BLOCKED,
	.init = front_shared_ring_queue_blocked_init,
	.data = (void *)NULL,
	.process_event = front_shared_ring_queue_blocked_handler,
	.finalize = front_shared_ring_queue_blocked_finalize,
};