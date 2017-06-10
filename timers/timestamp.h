#ifndef _TIMESTAMP_H_
#define _TIMESTAMP_H_

#ifdef __linux__
#include <stdlib.h>
#include <sys/time.h>
typedef timeval timestamp;
inline timestamp getTimestamp(void){
	timeval t;
	gettimeofday(&t, NULL);
	return t;
}
inline float getElapsedtime(timestamp t){
	timeval tn;
	gettimeofday(&tn, NULL);
	return (tn.tv_sec - t.tv_sec) * 1000.0f + (tn.tv_usec - t.tv_usec) / 1000.0f;
}
#else
#include <time.h>
typedef clock_t timestamp;
inline timestamp getTimestamp(void){
	return clock();
}
inline float getElapsedtime(timestamp t){
	return ((float)clock()-t) / CLOCKS_PER_SEC * 1000.0f;
}
#endif

#endif
