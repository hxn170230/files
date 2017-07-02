#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef struct {
	int source;
	int busy_time;
	char plane_id[2];
} plane_st;

typedef struct {
	int airport;
	int busy_period[2];
	int next_busy_period[100][2];
	int next_busy_period_index;
} slot_st;

int min[3][3] = {{-1, 3, 3}, {3, -1, 3}, {3, 3, -1}};
int start_time = 360;
int end_time = 1320;
char *airports[3] = {"DAL", "AUS", "HOU"};
int ground_time[3] = {30, 25, 35};
int flight_time[3][3] = {{0, 50, 65}, {50, 0, 45}, {65, 45, 0}};

void trace_slot(slot_st *s) {
	int i = 0;

	printf("Slot TRACE: %s\n", airports[s->airport]);
	for (i = 0; i < s->next_busy_period_index; i++) {
		printf("busy_period(%d, %d) ", s->next_busy_period[i][0], s->next_busy_period[i][1]);
	}
	printf("\n");
}

int is_busy(plane_st *pl, int cur_time) {
	printf("plane: %s busy: %d now: %d\n", pl->plane_id, pl->busy_time, cur_time);
	if (pl->busy_time > cur_time) {
		return 1;
	} else {
		return 0;
	}
}

int is_slot_busy(int land_time, slot_st sl) {
	int index = 0;
	printf("Checking slot for land_time: %d in airport: %s\n", land_time, airports[sl.airport]);
	printf("next_busy_period_index: %d\n", sl.next_busy_period_index);
	if (end_time - land_time < 0) {
		printf("Plane cannot fly!\n");
		return 1;
	}
	for (index = 0; index < sl.next_busy_period_index-1; index++) {
		printf("Current slot: %d %d Next slot: %d %d\n", sl.next_busy_period[index][0], sl.next_busy_period[index][1],
				sl.next_busy_period[index+1][0], sl.next_busy_period[index+1][1]);
		if (sl.next_busy_period[index][0] <= land_time &&
			sl.next_busy_period[index][1] >= land_time) {
			return 1;
		} else if (sl.next_busy_period[index][1] <= land_time &&
				sl.next_busy_period[index+1][0] >= land_time) {
			if (sl.next_busy_period[index+1][0] <= land_time + ground_time[sl.airport]) {
				// cannot land here
				return 1;
			} else {
				return 0;
			}
		}
	}
	if (index == sl.next_busy_period_index-1) {
		if (sl.next_busy_period[index][0] <= land_time && sl.next_busy_period[index][1] >= land_time) {
			return 1;
		}
	}
	return 0;
}

void getFreeSlots(plane_st *pl, int cur_time, slot_st *sl, int *free_indexes) {
	// source = pl->source
	// loop sl whose airport is not source
	//	if busy_period[0] <= cur_time + flight_time[source][sl->airport]
	//	 && busy_period[1] >= cur_time + fligjt_time[source][sl->airport]]
	//	 continue;
	//	else
	// 	check if possible ( end_time >= arrival_time)
	//	  add sl to free_slots
	int index = 0;
	int added = 0;
	for (index = 0; index < 6; index++) {
		if (sl[index].airport == pl->source) {
			continue;
		} else {
			int land_time = cur_time + flight_time[pl->source][sl[index].airport];
			// check busy times
			// is slot busy(sl[index], land_time) == 1? continue
			// else {
			//	include index in free_indexes;
			// }
			if (is_slot_busy(land_time, sl[index]) != 1) {
				printf("Slot FREE. Adding index(%d)\n", index);
				free_indexes[added] = index;
				added += 1;
			}
		}
	}
}

void initialize_plane(plane_st *pl, int source, int busy_time, char *id) {
	memset(pl, 0, sizeof(plane_st));
	pl->source = source;
	pl->busy_time = busy_time;
	memcpy(pl->plane_id, id, strlen(id));	
	printf("Plane set to %s using %s\n", pl->plane_id, id);
}

void initialize_slot(slot_st *sl, int airport, int next_index) {
	memset(sl, 0, sizeof(slot_st));
	sl->airport = airport;
	sl->next_busy_period_index = -1;
}

void find_and_place(slot_st *s, int cur_time, plane_st *p) {
	int busy_period[2];
	busy_period[0] = cur_time + flight_time[p->source][s->airport] - ground_time[s->airport];	
	busy_period[1] = cur_time + flight_time[p->source][s->airport] + ground_time[s->airport];
	int i = 0;
	int index = s->next_busy_period_index;

	if (index < 0) {
		index = 0;
		s->next_busy_period[index][0] = busy_period[0];
		s->next_busy_period[index][1] = busy_period[1];
		s->next_busy_period_index = 1;
		return;
	}

	for (i = 0; i < s->next_busy_period_index-1; i++) {
		if (cur_time + flight_time[p->source][s->airport] > s->next_busy_period[i][1] &&
			busy_period[1] < s->next_busy_period[i+1][0]) {
			index = i;
		}
	}

	if (index <= s->next_busy_period_index-1) {
		int i = index+1;
		int temp1 = s->next_busy_period[i+1][0];
		int temp2 = s->next_busy_period[i+1][1];
		s->next_busy_period[i+1][0] = s->next_busy_period[i][0];
		s->next_busy_period[i+1][1] = s->next_busy_period[i][1];
		i++;
		while (i <= s->next_busy_period_index) {
			int temp3 = s->next_busy_period[i+1][0];	
			int temp4 = s->next_busy_period[i+1][1];	
			s->next_busy_period[i+1][0] = temp1;
			s->next_busy_period[i+1][0] = temp2;
			temp1 = temp3;
			temp2 = temp4;
			i+=1;
		}
		s->next_busy_period[index+1][0] = busy_period[0];
		s->next_busy_period[index+1][1] = busy_period[1];
		s->next_busy_period_index+=1;

	} else {
		s->next_busy_period[index][0] = busy_period[0];
		s->next_busy_period[index][1] = busy_period[1];
		s->next_busy_period_index++;
	}
}

void set_next_busy_period(slot_st *s, int cur_time, plane_st *p) {
	// loop to find slot and add in slot_st
	find_and_place(s, cur_time, p);
	trace_slot(s);

	// finally set p->busy_time = cur_time + flight_time[p->source][s->airport] + ground_time[s->airport];
	p->busy_time = cur_time + flight_time[p->source][s->airport] + ground_time[s->airport];
	printf("Plane: %s busy set to %d\n", p->plane_id, p->busy_time);
}

int getMinIndex(int *free_I, slot_st *sl) {
	int i = 0;
	int index = 0;
	int j = 0;

	for (i = 0; i < 6; i++) {
		for (j = 0; j < 3; j++) {
			if (min[sl[free_I[i]].airport][j] > 0) {
				min[sl[free_I[i]].airport][j] -= 1;
				return i;
			}
		}
	}
	return -1;
}

int main() {
	int index = 0;

	plane_st pls[6];	
	initialize_plane(&pls[0], 2, 0, "T1");
	initialize_plane(&pls[1], 2, 0, "T2");
	initialize_plane(&pls[2], 2, 0, "T3");
	initialize_plane(&pls[3], 0, 0, "T4");
	initialize_plane(&pls[4], 0, 0, "T5");
	initialize_plane(&pls[5], 1, 0, "T6");

	slot_st sl[6];
	initialize_slot(&sl[0], 0,  -1);
	initialize_slot(&sl[1], 0,  -1);
	initialize_slot(&sl[2], 1,  -1);
	initialize_slot(&sl[3], 2,  -1);
	initialize_slot(&sl[4], 2,  -1);
	initialize_slot(&sl[5], 2,  -1);

	for (index = start_time; index <= end_time; index++) {
		// for pl in plane_st
		// 	if (is_busy != 1) {
		//		slot_st *free = getFreeSlots
		//		free_slot.next_busy_period[next_busy_index] = [min, max];
		//		pl.busy_time = cur_time + flight_time[pl->source][sl->airport] + ground_time[sl->airport];
		//		printf("%s %s %s %d %d\n", pl->plane_id, airports[pl->source], airport[sl->airport],
		//			cur_time, cur_time + flight_time[pl->source][sl->airport]);
		// 	}
		int pl_index = 0;
		int cur_time = index;
		for (pl_index = 0; pl_index < 6; pl_index++) {
			if (is_busy(&pls[pl_index], cur_time ) == 0) {
				int free_indexes[6] = {-1, };
				getFreeSlots(&pls[pl_index], cur_time, sl, free_indexes);
				int x = getMinIndex(free_indexes, sl);
				if (x == -1) {
					x = 0;
				}
				if (free_indexes[x] != -1) {
					// TODO sort the free_indexes
					slot_st *s = &sl[free_indexes[x]];
					// plane flew
					set_next_busy_period(s, cur_time, &pls[pl_index]);
					trace_slot(s);
					// print plane
					printf("OUTPUT: %s %s %s %d %d\n", pls[pl_index].plane_id, airports[pls[pl_index].source], airports[s->airport],
						cur_time, cur_time + flight_time[pls[pl_index].source][s->airport]);
					pls[pl_index].source = s->airport;
				}
			}
		}
	}
	return 0;
}
