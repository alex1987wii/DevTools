#ifndef _EVENT_H_
#define _EVENT_H_

enum{
	EV_HEADER,
	EV_START = 0x3001,
	EV_UPGRADE,
	EV_NETWORK_ERROR,
	EV_VERIFY_MSN,
	EV_MSN_DB_LOST,
	EV_MSN_NOT_MATCH,
	EV_MSN_MATCH,
	EV_INIT_SUCCESS,
	EV_INIT_ERROR,
	EV_BURNPARTITION,
	EV_BURNIMAGE,
	EV_UPGRADE_FAILED,
	EV_ERROR,
	EV_COMPLETE,
};

#define EVENT_FILE	"event.txt"

int event_recorder_init();
void event_recorder_exit();
int event_record(int ev_type, int ev_code, void *ev_value);

#endif
