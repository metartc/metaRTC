#ifndef INCLUDE_YANGUTIL_SYS_YANGJSON_H_
#define INCLUDE_YANGUTIL_SYS_YANGJSON_H_
#include <yangutil/yangtype.h>
#include <vector>
#include <string>
using namespace std;
struct YangJsonData{
	string key;
	string value;
};

void yang_gen_jsonstr(vector<YangJsonData> &jsons,string &outstr);


#endif /* INCLUDE_YANGUTIL_SYS_YANGJSON_H_ */
