#include <yangutil/yangavtype.h>
#include <yangutil/sys/YangTime.h>
#ifdef _MSC_VER
#define FILETIME_1970 116444736000000000ull /* seconds between 1/1/1601 and 1/1/1970 */
#define HECTONANOSEC_PER_SEC 10000000ull
 union yang_gettimeofday_type {
        unsigned long long ns100; //time since 1 Jan 1601 in 100ns units
        FILETIME ft;
    }  ;
int gettimeofday(struct timeval *tp, void *tzp)
{
	yang_gettimeofday_type t_now;
    GetSystemTimeAsFileTime(&t_now.ft);
    //100 nano-seconds since 1-1-1970
    t_now.ns100 -= FILETIME_1970;
    tp->tv_sec=t_now.ns100 / HECTONANOSEC_PER_SEC;
    tp->tv_usec=(t_now.ns100 % HECTONANOSEC_PER_SEC)/10;

    return (0);
}

#endif
int64_t YangSystime::system_time_us_cache = 0;
int64_t YangSystime::system_time_startup_time = 0;
#ifdef _MSC_VER
int64_t YangSystime::dff = 0;
LARGE_INTEGER  YangSystime::large_interger;
#endif

#define Yang_TIME_RESOLUTION_US 300*1000
void YangSystime::update_system_time() { //milli weimiao
#ifdef _MSC_VER
    if(dff==0){
        QueryPerformanceFrequency(&large_interger);
        dff = large_interger.QuadPart;
    }
    if(system_time_startup_time==0){
        QueryPerformanceCounter(&large_interger);
        system_time_startup_time=large_interger.QuadPart;
    }


#else
    if(system_time_startup_time==0) system_time_startup_time = yang_get_micro_time();
#endif


}

int64_t YangSystime::get_system_micro_time(){
    if(system_time_startup_time==0) update_system_time();
#ifdef _MSC_VER

    QueryPerformanceCounter(&large_interger);
    system_time_us_cache = (large_interger.QuadPart-system_time_startup_time)*1000000/dff;
    return system_time_us_cache;
#else
    return yang_get_micro_time()-system_time_startup_time;


#endif
}

int64_t YangSystime::get_system_milli_time(){
    if(system_time_startup_time==0) update_system_time();
#ifdef _MSC_VER

    QueryPerformanceCounter(&large_interger);
    return (large_interger.QuadPart-system_time_startup_time)*1000/dff;

#else  
    return get_system_micro_time()/1000;
#endif
}

YangSystime::YangSystime(){

}

YangSystime::~YangSystime(){

}

uint64_t YangNtp::kMagicNtpFractionalUnit = 1ULL << 32;

YangNtp::YangNtp()
{
    m_system_ms = 0;
    m_ntp = 0;
    m_ntp_second = 0;
    m_ntp_fractions = 0;
}

YangNtp::~YangNtp()
{

}

YangNtp YangNtp::from_time_ms(uint64_t ms)
{
    YangNtp srs_ntp;
    srs_ntp.m_system_ms = ms;
    srs_ntp.m_ntp_second = ms / 1000;
    srs_ntp.m_ntp_fractions = (static_cast<double>(ms % 1000 / 1000.0)) * kMagicNtpFractionalUnit;
    srs_ntp.m_ntp = (static_cast<uint64_t>(srs_ntp.m_ntp_second) << 32) | srs_ntp.m_ntp_fractions;
    return srs_ntp;
}

YangNtp YangNtp::to_time_ms(uint64_t ntp)
{
    YangNtp srs_ntp;
    srs_ntp.m_ntp = ntp;
    srs_ntp.m_ntp_second = (ntp & 0xFFFFFFFF00000000ULL) >> 32;
    srs_ntp.m_ntp_fractions = (ntp & 0x00000000FFFFFFFFULL);
    srs_ntp.m_system_ms = (static_cast<uint64_t>(srs_ntp.m_ntp_second) * 1000) +
            (static_cast<double>(static_cast<uint64_t>(srs_ntp.m_ntp_fractions) * 1000.0) / kMagicNtpFractionalUnit);
    return srs_ntp;
}




#ifdef _MSC_VER
//haomiao
int64_t yang_get_milli_tick() {
    return YangSystime::get_system_milli_time();

}

#else
//haomiao
/**int64_t yang_get_milli_tick() {
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);


}
//weimiao
int64_t yang_get_micro_tick() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000000 + ts.tv_nsec/1000);
}**/
//haomiao

//namiao
int64_t yang_get_nano_tick(){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000000000 + ts.tv_nsec);
}
#endif

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

