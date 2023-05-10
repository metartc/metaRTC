//
// Copyright (c) 2019-2023 yanggaofeng
//

#ifndef INCLUDE_YANGJSON_YANGJSON_H_
#define INCLUDE_YANGJSON_YANGJSON_H_
#include <yangutil/yangtype.h>

typedef struct YangJson
{
    struct YangJson *next;
    struct YangJson *prev;
    struct YangJson *child;

    int type;
    char *valuestring;
    int valueint;
    double valuedouble;
    char *string;
    uint8_t* outString;
} YangJson;


typedef struct{
	YangJson* session;
	int (*getArraySize)(const YangJson *array);
	YangJson * (*getArrayItem)(const YangJson *array, int index);
	YangJson * (*getObjectItem)(const YangJson * const object, const char * const string);
	YangJson * (*getObjectItemCaseSensitive)(const YangJson * const object, const char * const string);
	yangbool (*hasObjectItem)(const YangJson *object, const char *string);
	const char * (*getErrorPtr)(void);



	char * (*getStringValue)(const YangJson * const item);
	double (*getNumberValue)(const YangJson * const item);


	yangbool (*isInvalid)(const YangJson * const item);
	yangbool (*isFalse)(const YangJson * const item);
	yangbool (*isTrue)(const YangJson * const item);
	yangbool (*isBool)(const YangJson * const item);
	yangbool (*isNull)(const YangJson * const item);
	yangbool (*isNumber)(const YangJson * const item);
	yangbool (*isString)(const YangJson * const item);
	yangbool (*isArray)(const YangJson * const item);
	yangbool (*isObject)(const YangJson * const item);
	yangbool (*isRaw)(const YangJson * const item);
}YangJsonReader;

typedef struct{
	YangJson* session;
	YangJson * (*createNull)(void);
	YangJson * (*createTrue)(void);
	YangJson * (*createFalse)(void);
	YangJson * (*createBool)(yangbool boolean);
	YangJson * (*createNumber)(double num);
	YangJson *  (*createString)(const char *string);

	YangJson* (*createRaw)(const char *raw);
	YangJson* (*createArray)(void);
	YangJson* (*createObject)(void);

	YangJson* (*addNullToObject)(YangJson * const object, const char * const name);
	YangJson* (*addTrueToObject)(YangJson * const object, const char * const name);
	YangJson* (*addFalseToObject)(YangJson * const object, const char * const name);
	YangJson* (*addBoolToObject)(YangJson * const object, const char * const name, const yangbool boolean);
	YangJson* (*addNumberToObject)(YangJson * const object, const char * const name, const double number);
	YangJson* (*addStringToObject)(YangJson * const object, const char * const name, const char * const string);
	YangJson* (*addRawToObject)(YangJson * const object, const char * const name, const char * const raw);
	YangJson* (*addObjectToObject)(YangJson * const object, const char * const name);
	YangJson* (*addArrayToObject)(YangJson * const object, const char * const name);

	char * (*print)(YangJson *item);
	char * (*printUnformatted)(YangJson *item);
	char * (*printBuffered)(YangJson *item, int prebuffer, yangbool fmt);

	//void (*free)(void *object);
}YangJsonWriter;



#ifdef __cplusplus
extern "C"{
#endif

int32_t yang_create_jsonReader(YangJsonReader* reader,char* jsonStr);
void yang_destroy_jsonReader(YangJsonReader* reader);

int32_t yang_create_jsonWriter(YangJsonWriter* writer);
void yang_destroy_jsonWriter(YangJsonWriter* writer);



#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_YANGJSON_YANGJSON_H_ */
