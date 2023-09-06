//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangutil/sys/YangString.h>
#include <yangutil/sys/YangTime.h>
#include <inttypes.h>

#if Yang_OS_WIN
#include <random>
#define srandom srand
#define random rand
#endif

#ifdef _MSC_VER
#include <windows.h>
#endif



 void yang_write_string(YangBuffer* buf,string value)
 {
     //srs_assert(require((int)value.length()));
	 if(buf==NULL) return;
     yang_memcpy(buf->head, value.data(), value.length());
     buf->head += value.length();
 }

 string yang_read_string(YangBuffer* buf,int32_t len)
 {
     ////srs_assert(require(len));

     std::string value;
     value.append(buf->head, len);

     buf->head+= len;

     return value;
 }

 void skip_first_spaces(std::string& str)
{
    while (! str.empty() && str[0] == ' ') {
        str.erase(0, 1);
    }
}

 vector<string> yang_split_first(string s, char ch) {
 	int32_t len = 0;
 	vector<string> ret;
 	for (size_t i = 0; i < s.length(); i++) {
 		if (s[i] == ch) {
 			ret.push_back(s.substr(0, i));
 			ret.push_back(s.substr(i+1, s.length()-1));
 			return ret;
 		}
 		else {
 			len++;
 		}
 	}

 	return ret;
 }

vector<string> yang_split(string s, char ch) {
	size_t start = 0;
	int32_t len = 0;
	vector<string> ret;
	for (size_t i = 0; i < s.length(); i++) {
		if (s[i] == ch) {
			ret.push_back(s.substr(start, len));
			start = i + 1;
			len = 0;
		}
		else {
			len++;
		}
	}
	if (start < s.length())
		ret.push_back(s.substr(start, len));
	return ret;
}

std::vector<std::string> yang_splits(const std::string& str, const std::string& delim)
{
    std::vector<std::string> ret;
    size_t pre_pos = 0;
    std::string tmp;
    size_t pos = 0;
    do {
        pos = str.find(delim, pre_pos);
        tmp = str.substr(pre_pos, pos - pre_pos);
        ret.push_back(tmp);
        pre_pos = pos + delim.size();
    } while (pos != std::string::npos);

    return ret;
}


std::string yang_int2str(int64_t value) {

	char tmp[22];
    snprintf(tmp, 22, "%" PRId64, value);
	return string(tmp);
}
std::string yang_random_str(int32_t len) {
	static string random_table ="01234567890123456789012345678901234567890123456789abcdefghijklmnopqrstuvwxyz";
	string ret;
	ret.reserve(len);
	for (int32_t i = 0; i < len; ++i) {
		ret.append(1, random_table[yang_random() % random_table.size()]);
	}

	return ret;
}
void yang_replace(std::string& strBig, const std::string& strsrc, const std::string& strdst)
{
	std::string::size_type pos = 0;
	std::string::size_type srclen = strsrc.size();
	std::string::size_type dstlen = strdst.size();

	while ((pos = strBig.find(strsrc, pos)) != std::string::npos)
	{
		strBig.replace(pos, srclen, strdst);
		pos += dstlen;
	}
}


