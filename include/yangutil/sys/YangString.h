
#ifndef INCLUDE_YANGUTIL_SYS_YANGSTRING_H_
#define INCLUDE_YANGUTIL_SYS_YANGSTRING_H_
#include <vector>
#include <string.h>
#include <string>
using namespace std;
long yang_random();
vector<string> yang_split(string s, char ch);
vector<string> yang_split_first(string s, char ch);
std::vector<std::string> yang_splits(const std::string& str, const std::string& delim);
std::string yang_int2str(int64_t value);
std::string yang_random_str(int32_t len);
void yang_replace(std::string& strBig, const std::string& strsrc, const std::string& strdst);
void skip_first_spaces(std::string& str);

#endif /* INCLUDE_YANGUTIL_SYS_YANGSTRING_H_ */
