//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef INCLUDE_YANGTRACE_YANGTRACEINI_H_
#define INCLUDE_YANGTRACE_YANGTRACEINI_H_
#include <string>
struct YangTraceIni{
    std::string traceIp;
    std::string luboIp;
    std::string studentIp;
    std::string teacherIp;
    int32_t studentPort;
    int32_t teacherPort;
    int32_t localPort;
    int32_t luboPort;

};



#endif /* INCLUDE_YANGTRACE_YANGTRACEINI_H_ */
