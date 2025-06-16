//
// Copyright (c) 2019-2025 yanggaofeng
//
#include <yangutil/sys/YangCString.h>
#include <yangutil/sys/YangMath.h>
#include <yangutil/sys/YangLog.h>
#include <ctype.h>

void yang_destroy_strings(YangStrings* strs){
	int32_t i;
	if(strs==NULL||strs->str==NULL)
		return;

    for (i=0;i<strs->vsize;i++)
    	yang_free(strs->str[i]);

     yang_free(strs->str);
}

int32_t yang_cstr_split(char *src, char *delim, YangStrings* istr)
{
	char  *p = NULL;

	if(src==NULL||delim==NULL||istr==NULL)
		return 1;

	yang_memset(istr,0,sizeof(YangStrings));
	istr->capacity=10;
    istr->str=(char**)yang_calloc(istr->capacity*sizeof(char*),1);

    istr->vsize = 0;
	p = strtok(src, delim);

	if(p==NULL)
		return 1;

	while(p){
		istr->str[istr->vsize]=(char*)yang_calloc(yang_strlen(p)+1,1);

		yang_memcpy(istr->str[istr->vsize],p,yang_strlen(p));
		istr->vsize++;
		if(istr->vsize>=istr->capacity){
            char** tmp=(char**)yang_calloc(istr->capacity*sizeof(char*),1);
			yang_memcpy(tmp,istr->str,istr->capacity*sizeof(char*));
			yang_free(istr->str);

            istr->str=(char**)yang_calloc((istr->capacity+10)*sizeof(char*),1);
			yang_memcpy(istr->str,tmp,istr->capacity*sizeof(char*));
			istr->capacity+=10;
			yang_free(tmp);
		}
		p = strtok(NULL, delim);
	}


    return Yang_Ok;
}

void yang_itoa(int32_t num,char* data,int32_t n){
	yang_sprintf(data,"%d",num);
}

void yang_itoa2(uint32_t num,char* data,int32_t n){
	yang_sprintf(data,"%u",num);
}

int32_t yang_get_line(char* buf,char *line, int32_t line_size)
{
	char ch;
	int32_t i;
	char* q=line;

    for (i=0;i<line_size;i++) {
    	ch=buf[i];
        if (ch == '\n') {
             if (q > line && q[-1] == '\r')
                 q--;
             *q = '\0';

             return 0;
         } else {
             if ((q - line) < line_size - 1)
                 *q++ = ch;
         }
    }
    return 1;
}


int32_t yang_yang_strcmp(char* str1,char* str2){
	int32_t i;
	int32_t len=yang_strlen(str1);
	for(i=0;i<len;i++){
		if(yang_tolower(str1[i])!=yang_tolower(str2[i]))
			return 1;
	}
	return 0;
}

void yang_cstr_random(int32_t len,char* data) {
	int32_t i;
	static char* random_table ="01234567890123456789012345678901234567890123456789abcdefghijklmnopqrstuvwxyz";

	if(data==NULL)
		return;

	for (i = 0; i < len; ++i) {
		data[i]= random_table[yang_random() %yang_strlen(random_table)];
	}
}

void yang_cint32_random(int32_t len,char* data) {
	int32_t i;
	static char* random_int32_table ="01234567890123456789012345678901234567890123456789";

	if(data==NULL)
		return;

	for (i = 0; i < len; ++i) {
		data[i]= random_int32_table[yang_random() %yang_strlen(random_int32_table)];
	}
}

void yang_cstr_replace(char *str,char* dst, char *macth, char *rep)
{
	int32_t dstlen;
	int32_t replen;
	int32_t origlen;
	char *p=NULL,*p1=NULL;

	if(str==NULL||dst==NULL||macth==NULL||rep==NULL)
		return;


	if(!(p = yang_strstr(str, macth)))  {// Is 'orig' even in 'str'?
		yang_strcpy(dst,str);
		return;
	}

	dstlen=p-str;
	replen=yang_strlen(rep);
	origlen=yang_strlen(macth);

	yang_memcpy(dst,str,dstlen);

	if(replen>0){
		yang_memcpy(dst+dstlen,rep,replen);
		dstlen+=replen;
	}

	while(p){
		p1=p;
		p=yang_strstr(p1+origlen,macth);
		if(p){
			yang_memcpy(dst+dstlen,p1+origlen,p-p1-origlen);
			dstlen+=p-p1-origlen;
			if(replen>0){
				yang_memcpy(dst+dstlen,rep,replen);
				dstlen+=replen;
			}
		}else{
			if(p1)     yang_memcpy(dst+dstlen,p1+origlen,yang_strlen(str)-(p1-str)-origlen);
		}
	}
}

int32_t yang_cstr_userfindindex(char* p,char c){
	int32_t i;
	int32_t slen=yang_strlen(p);

	for(i=0;i<slen;i++){
		if(p[i]==c) return i+1;//\n
	}

	return 0;
}

int32_t yang_cstr_userfindupindex(char* p,char c,int32_t n){
	int32_t i;

	for(i=0;i<n;i++){
		if(*(p-i)==c) return i+1;//\n
	}

	return 0;
}

int32_t yang_cstr_isnumber(char* p,int32_t n){
	int32_t i;

	if(p==NULL)
		return -1;

	for(i=0;i<n;i++){
		if(p[i]>=48&&p[i]<=57) return i;
	}

	return -1;
}
