#include <yangutil/sys/YangCTime.h>
#include <yangutil/yangavtype.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>

int64_t g_system_time_us_cache = 0;
int64_t g_system_time_startup_time = 0;

#define Yang_TIME_RESOLUTION_US 300*1000
void yang_update_system_time() { //milli weimiao

    if(g_system_time_startup_time==0) g_system_time_startup_time = yang_get_micro_time();



}

int64_t yang_get_system_micro_time(){
    if(g_system_time_startup_time==0) yang_update_system_time();

    return yang_get_micro_time()-g_system_time_startup_time;



}

int64_t yang_get_system_milli_time(){
    if(g_system_time_startup_time==0) yang_update_system_time();

    return yang_get_system_micro_time()/1000;

}





//namiao
int64_t yang_get_nano_tick(){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000000000 + ts.tv_nsec);
}


//haomiao
int64_t yang_get_milli_time() {
    struct timeval time;
    gettimeofday(&time, 0);
    // long millis = (time.tv_sec * 1000) + (time.tv_usec / 1000);
    return (time.tv_sec * 1000) + (time.tv_usec / 1000);


}
//weimiao
int64_t yang_get_micro_time() {
    struct timeval time;
    gettimeofday(&time, 0);
    return time.tv_sec * 1000000 + time.tv_usec ;
}




const uint64_t kMagicNtpFractionalUnit = ((uint64_t)1) << 32;



void yang_ntp_from_time_ms(YangNtp* ntp,uint64_t ms)
{

	 ntp->system_ms = ms;
	    ntp->ntp_second = ms / 1000;
	    ntp->ntp_fractions = ((double)(ms % 1000 / 1000.0)) * kMagicNtpFractionalUnit;
	    ntp->ntp = ((uint64_t)(ntp->ntp_second) << 32) | ntp->ntp_fractions;
}

void yang_ntp_to_time_ms(YangNtp* ntp,uint64_t pntp)
{

	ntp->ntp =pntp;
	    ntp->ntp_second = (pntp & 0xFFFFFFFF00000000ULL) >> 32;
	    ntp->ntp_fractions = (pntp & 0x00000000FFFFFFFFULL);
	    ntp->system_ms = ((uint64_t)(ntp->ntp_second) * 1000) +
	            ((double)((uint64_t)(ntp->ntp_fractions) * 1000.0) / kMagicNtpFractionalUnit);

}
