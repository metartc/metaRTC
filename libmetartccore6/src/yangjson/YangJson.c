//
// Copyright (c) 2019-2023 yanggaofeng
//

#include "YangJsonImpl.h"
#include <yangutil/sys/YangLog.h>
#if Yang_Enable_Json

int32_t yang_create_jsonReader(YangJsonReader* reader,char* jsonStr){
	if(reader==NULL || jsonStr==NULL) return ERROR_JSON;

	reader->session=yang_json_parse(jsonStr);
	if(reader->session==NULL)
		return yang_error_wrap(ERROR_JSON,"json reader parse fail!");

	reader->session->outString=NULL;

	reader->getArraySize=yang_json_getArraySize;
	reader->getArrayItem=yang_json_getArrayItem;
	reader->getObjectItem=yang_json_getObjectItem;
	reader->getObjectItemCaseSensitive=yang_json_getObjectItemCaseSensitive;
	reader->hasObjectItem=yang_json_hasObjectItem;
	reader->getErrorPtr=yang_json_getErrorPtr;



	reader->getStringValue=yang_json_getStringValue;
	reader->getNumberValue=yang_json_getNumberValue;


	reader->isInvalid=yang_json_isInvalid;
	reader->isFalse=yang_json_isFalse;
	reader->isTrue=yang_json_isTrue;
	reader->isBool=yang_json_isBool;
	reader->isNull=yang_json_isNull;
	reader->isNumber=yang_json_isNumber;
	reader->isString=yang_json_isString;
	reader->isArray=yang_json_isArray;
	reader->isObject=yang_json_isObject;
	reader->isRaw=yang_json_isRaw;
	return Yang_Ok;
}
void yang_destroy_jsonReader(YangJsonReader* reader){
	if(reader==NULL) return;
	if(reader->session) yang_json_delete(reader->session);
}

int32_t yang_create_jsonWriter(YangJsonWriter* writer){
	if(writer==NULL) return ERROR_JSON;
	writer->session=yang_json_createObject();
	writer->createNull=yang_json_createNull;
	writer->createTrue=yang_json_createTrue;
	writer->createFalse=yang_json_createFalse;
	writer->createBool=yang_json_createBool;
	writer->createNumber=yang_json_createNumber;
	writer->createString=yang_json_createString;

	writer->createRaw=yang_json_createRaw;
	writer->createArray=yang_json_createArray;
	writer->createObject=yang_json_createObject;

	writer->addNullToObject=yang_json_addNullToObject;
	writer->addTrueToObject=yang_json_addTrueToObject;
	writer->addFalseToObject=yang_json_addFalseToObject;
	writer->addBoolToObject=yang_json_addBoolToObject;
	writer->addNumberToObject=yang_json_addNumberToObject;
	writer->addStringToObject=yang_json_addStringToObject;
	writer->addRawToObject=yang_json_addRawToObject;
	writer->addObjectToObject=yang_json_addObjectToObject;
	writer->addArrayToObject=yang_json_addArrayToObject;

	writer->print=yang_json_print;
	writer->printUnformatted=yang_json_printUnformatted;
	writer->printBuffered=yang_json_printBuffered;

	//writer->free=yang_json_free;
	return Yang_Ok;
}
void yang_destroy_jsonWriter(YangJsonWriter* writer){
	if(writer==NULL) return;
	if(writer->session) {

		if(writer->session->outString)
			yang_json_free(writer->session->outString);

		yang_json_delete(writer->session);
	}
}

#endif
