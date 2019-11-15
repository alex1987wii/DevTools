#include "state_machine.h"

void state_machine_process(struct state_machine_t *p_mach)
{
	static int errcnt = 0;
	do{
		switch(p_mach->state){
			case S_IDLE:
				switch

		}
	}while(p_mach->state != S_FINISHED);
}
