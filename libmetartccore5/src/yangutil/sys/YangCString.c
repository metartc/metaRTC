//
// Copyright (c) 2019-2022 yanggaofeng
//
#include <yangutil/sys/YangCString.h>
#include <yangutil/yangtype.h>
#include <yangutil/sys/YangMath.h>
#include <yangutil/sys/YangLog.h>
#include <ctype.h>
void yang_destroy_strings(YangStrings* strs){
	if(strs==NULL||strs->str==NULL) return;
    for (int32_t i=0;i<strs->vsize;i++)           free(strs->str[i]);

       free(strs->str);
}



int32_t yang_cstr_split(char *src, char *delim, YangStrings* istr)
{
	if(src==NULL||delim==NULL||istr==NULL) return 1;
	 memset(istr,0,sizeof(YangStrings));
	istr->capacity=10;
    istr->str=(char**)calloc(istr->capacity*sizeof(char*),1);

    char  *p = NULL;

    istr->vsize = 0;
	p = strtok(src, delim);
	if(p==NULL) return 1;

	while(p){
		istr->str[istr->vsize]=(char*)calloc(strlen(p)+1,1);

		memcpy(istr->str[istr->vsize],p,strlen(p));
		istr->vsize++;
		if(istr->vsize>=istr->capacity){
            char** tmp=(char**)calloc(istr->capacity*sizeof(char*),1);
			memcpy(tmp,istr->str,istr->capacity*sizeof(char*));
			yang_free(istr->str);

            istr->str=(char**)calloc((istr->capacity+10)*sizeof(char*),1);
			memcpy(istr->str,tmp,istr->capacity*sizeof(char*));
			istr->capacity+=10;
			yang_free(tmp);
		}
		p = strtok(NULL, delim);
	}


    return Yang_Ok;
}

void yang_itoa(int32_t num,char* data,int32_t n){
	sprintf(data,"%d",num);
}

int32_t yang_strcmp(char* str1,char* str2){
	int32_t len=strlen(str1);
	for(int32_t i=0;i<len;i++){
		if(tolower(str1[i])!=tolower(str2[i])) return 1;
	}
	return 0;
}

void yang_cstr_random(int32_t len,char* data) {
	if(data==NULL) return;
	static char* random_table ="01234567890123456789012345678901234567890123456789abcdefghijklmnopqrstuvwxyz";

	for (int32_t i = 0; i < len; ++i) {
		data[i]= random_table[yang_random() %strlen(random_table)];
	}
}

void yang_cint32_random(int32_t len,char* data) {
	if(data==NULL) return;
	static char* random_int32_table ="01234567890123456789012345678901234567890123456789";

	for (int32_t i = 0; i < len; ++i) {
		data[i]= random_int32_table[yang_random() %strlen(random_int32_table)];
	}
}

void yang_cstr_replace(char *str,char* dst, char *macth, char *rep)
{
	if(str==NULL||dst==NULL||macth==NULL||rep==NULL) return;
  char *p=NULL,*p1=NULL;

  if(!(p = strstr(str, macth)))  {// Is 'orig' even in 'str'?
	  strcpy(dst,str);
    return;
  }
  int32_t dstlen=p-str;
  int32_t replen=strlen(rep);
  int32_t origlen=strlen(macth);
  memcpy(dst,str,dstlen);
  if(replen>0){
	  memcpy(dst+dstlen,rep,replen);
	  dstlen+=replen;
  }
  while(p){
	  p1=p;
      p=strstr(p1+origlen,macth);
	  if(p){
		  memcpy(dst+dstlen,p1+origlen,p-p1-origlen);
		  dstlen+=p-p1-origlen;
		  if(replen>0){
			  memcpy(dst+dstlen,rep,replen);
			  dstlen+=replen;
		  }
	  }else{
		  if(p1)     memcpy(dst+dstlen,p1+origlen,strlen(str)-(p1-str)-origlen);
	  }
  }
}
int32_t yang_cstr_userfindindex(char* p,char c){
	int slen=strlen(p);
	for(int32_t i=0;i<slen;i++){
		if(p[i]==c) return i+1;//\n
	}
	return 0;
}
int32_t yang_cstr_userfindupindex(char* p,char c,int32_t n){

	for(int32_t i=0;i<n;i++){
		if(*(p-i)==c) return i+1;//\n
	}
	return 0;
}

int32_t yang_cstr_isnumber(char* p,int32_t n){
	if(p==NULL) return -1;
	for(int32_t i=0;i<n;i++){
		if(p[i]>=48&&p[i]<=57) return i;
	}
	return -1;
}
