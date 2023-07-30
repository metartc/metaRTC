//
// Copyright (c) 2019-2023 yanggaofeng
// base on Dave Gamble cJSON
//

#ifndef YangJsonImpl__h
#define YangJsonImpl__h

#include <yangjson/YangJson.h>
#if Yang_Enable_Json

#include <stddef.h>


#define Yang_JSON_Invalid (0)
#define Yang_JSON_False  (1 << 0)
#define Yang_JSON_True   (1 << 1)
#define Yang_JSON_NULL   (1 << 2)
#define Yang_JSON_Number (1 << 3)
#define Yang_JSON_String (1 << 4)
#define Yang_JSON_Array  (1 << 5)
#define Yang_JSON_Object (1 << 6)
#define Yang_JSON_Raw    (1 << 7) /* raw json */

#define Yang_JSON_IsReference 256
#define Yang_JSON_StringIsConst 512



typedef struct
{
      void *(*malloc_fn)(size_t sz);
      void (*free_fn)(void *ptr);
} YangJsonHooks;



/* Limits how deeply nested arrays/objects can be before JSON rejects to parse them.
 * This is to prevent stack overflows. */
#ifndef Yang_JSON_NESTING_LIMIT
#define Yang_JSON_NESTING_LIMIT 1000
#endif

YangJson* yang_json_parse(const char *value);
void yang_json_delete(YangJson *item);



int yang_json_getArraySize(const YangJson *array);
YangJson * yang_json_getArrayItem(const YangJson *array, int index);
YangJson * yang_json_getObjectItem(const YangJson * const object, const char * const string);
YangJson * yang_json_getObjectItemCaseSensitive(const YangJson * const object, const char * const string);
yangbool yang_json_hasObjectItem(const YangJson *object, const char *string);
const char * yang_json_getErrorPtr(void);



char * yang_json_getStringValue(const YangJson * const item);
double yang_json_getNumberValue(const YangJson * const item);


yangbool yang_json_isInvalid(const YangJson * const item);
yangbool yang_json_isFalse(const YangJson * const item);
yangbool yang_json_isTrue(const YangJson * const item);
yangbool yang_json_isBool(const YangJson * const item);
yangbool yang_json_isNull(const YangJson * const item);
yangbool yang_json_isNumber(const YangJson * const item);
yangbool yang_json_isString(const YangJson * const item);
yangbool yang_json_isArray(const YangJson * const item);
yangbool yang_json_isObject(const YangJson * const item);
yangbool yang_json_isRaw(const YangJson * const item);


YangJson * yang_json_createNull(void);
YangJson * yang_json_createTrue(void);
YangJson * yang_json_createFalse(void);
YangJson * yang_json_createBool(yangbool boolean);
YangJson * yang_json_createNumber(double num);
YangJson *  yang_json_createString(const char *string);

YangJson* yang_json_createRaw(const char *raw);
YangJson* yang_json_createArray(void);
YangJson* yang_json_createObject(void);

YangJson* yang_json_addNullToObject(YangJson * const object, const char * const name);
YangJson* yang_json_addTrueToObject(YangJson * const object, const char * const name);
YangJson* yang_json_addFalseToObject(YangJson * const object, const char * const name);
YangJson* yang_json_addBoolToObject(YangJson * const object, const char * const name, const yangbool boolean);
YangJson* yang_json_addNumberToObject(YangJson * const object, const char * const name, const double number);
YangJson* yang_json_addStringToObject(YangJson * const object, const char * const name, const char * const string);
YangJson* yang_json_addRawToObject(YangJson * const object, const char * const name, const char * const raw);
YangJson* yang_json_addObjectToObject(YangJson * const object, const char * const name);
YangJson* yang_json_addArrayToObject(YangJson * const object, const char * const name);

char * yang_json_print(YangJson *item);
char * yang_json_printUnformatted(YangJson *item);
char * yang_json_printBuffered(YangJson *item, int prebuffer, yangbool fmt);
/* malloc/free objects using the malloc/free functions that have been set with JSON_InitHooks */
void * yang_json_malloc(size_t size);
void yang_json_free(void *object);

/* Supply malloc, realloc and free functions to JSON */
void yang_json_initHooks(YangJsonHooks* hooks);

/* Memory Management: the caller is always responsible to free the results from all variants of JSON_Parse (with JSON_Delete) and JSON_Print (with stdlib free, JSON_Hooks.free_fn, or JSON_free as appropriate). The exception is JSON_PrintPreallocated, where the caller has full responsibility of the buffer. */
/* Supply a block of JSON, and this returns a JSON object you can interrogate. */

YangJson * yang_json_parseWithLength(const char *value, size_t buffer_length);
/* ParseWithOpts allows you to require (and check) that the JSON is null terminated, and to retrieve the pointer to the final byte parsed. */
/* If you supply a ptr in return_parse_end and parsing fails, then return_parse_end will contain a pointer to the error so will match JSON_GetErrorPtr(). */
YangJson * yang_json_parseWithOpts(const char *value, const char **return_parse_end, yangbool require_null_terminated);
YangJson * yang_json_parseWithLengthOpts(const char *value, size_t buffer_length, const char **return_parse_end, yangbool require_null_terminated);



/* Render a JSON entity to text using a buffer already allocated in memory with given length. Returns 1 on success and 0 on failure. */
/* NOTE: JSON is not always 100% accurate in estimating how much memory it will use, so to be safe allocate 5 bytes more than you actually need */
yangbool yang_json_printPreallocated(YangJson *item, char *buffer, const int length, const yangbool format);

/* Create a string where valuestring references a string so
 * it will not be freed by JSON_Delete */
YangJson* yang_json_createStringReference(const char *string);
/* Create an object/array that only references it's elements so
 * they will not be freed by JSON_Delete */
YangJson* yang_json_createObjectReference(const YangJson *child);
YangJson* yang_json_CreateArrayReference(const YangJson *child);

/* These utilities create an Array of count items.
 * The parameter count cannot be greater than the number of elements in the number array, otherwise array access will be out of bounds.*/
YangJson* yang_json_createIntArray(const int *numbers, int count);
YangJson* yang_json_createFloatArray(const float *numbers, int count);
YangJson* yang_json_createDoubleArray(const double *numbers, int count);
YangJson* yang_json_createStringArray(const char *const *strings, int count);

/* Append item to the specified array/object. */
yangbool yang_json_addItemToArray(YangJson *array, YangJson *item);
yangbool yang_json_addItemToObject(YangJson *object, const char *string, YangJson *item);
/* Use this when string is definitely const (i.e. a literal, or as good as), and will definitely survive the JSON object.
 * WARNING: When this function was used, make sure to always check that (item->type & JSON_StringIsConst) is zero before
 * writing to `item->string` */
yangbool yang_json_addItemToObjectCS(YangJson *object, const char *string, YangJson *item);
/* Append reference to item to the specified array/object. Use this when you want to add an existing JSON to a new JSON, but don't want to corrupt your existing JSON. */
yangbool yang_json_addItemReferenceToArray(YangJson *array, YangJson *item);
yangbool yang_json_addItemReferenceToObject(YangJson *object, const char *string, YangJson *item);

/* Remove/Detach items from Arrays/Objects. */
YangJson* yang_json_detachItemViaPointer(YangJson *parent, YangJson * const item);
YangJson* yang_json_detachItemFromArray(YangJson *array, int which);
void yang_json_deleteItemFromArray(YangJson *array, int which);
YangJson* yang_json_detachItemFromObject(YangJson *object, const char *string);
YangJson* yang_json_detachItemFromObjectCaseSensitive(YangJson *object, const char *string);
void yang_json_deleteItemFromObject(YangJson *object, const char *string);
void yang_json_deleteItemFromObjectCaseSensitive(YangJson *object, const char *string);

/* Update array items. */
yangbool yang_json_insertItemInArray(YangJson *array, int which, YangJson *newitem); /* Shifts pre-existing items to the right. */
yangbool yang_json_replaceItemViaPointer(YangJson * const parent, YangJson * const item, YangJson * replacement);
yangbool yang_json_replaceItemInArray(YangJson *array, int which, YangJson *newitem);
yangbool yang_json_replaceItemInObject(YangJson *object,const char *string,YangJson *newitem);
yangbool yang_json_replaceItemInObjectCaseSensitive(YangJson *object,const char *string,YangJson *newitem);

/* Duplicate a JSON item */
YangJson * yang_json_duplicate(const YangJson *item, yangbool recurse);
/* Duplicate will create a new, identical JSON item to the one you pass, in new memory that will
 * need to be released. With recurse!=0, it will duplicate any children connected to the item.
 * The item->next and ->prev pointers are always zero on return from Duplicate. */
/* Recursively compare two JSON items for equality. If either a or b is NULL or invalid, they will be considered unequal.
 * case_sensitive determines if object keys are treated case sensitive (1) or case insensitive (0) */
yangbool yang_json_compare(const YangJson * const a, const YangJson * const b, const yangbool case_sensitive);

/* Minify a strings, remove blank characters(such as ' ', '\t', '\r', '\n') from strings.
 * The input pointer json cannot point to a read-only address area, such as a string constant, 
 * but should point to a readable and writable address area. */
void yang_json_minify(char *json);

/* Helper functions for creating and adding items to an object at the same time.
 * They return the added item or NULL on failure. */


/* When assigning an integer value, it needs to be propagated to valuedouble too. */
#define yang_json_setIntValue(object, number) ((object) ? (object)->valueint = (object)->valuedouble = (number) : (number))
/* helper for the JSON_SetNumberValue macro */
double yang_json_setNumberHelper(YangJson *object, double number);
#define yang_json_setNumberValue(object, number) ((object != NULL) ? JSON_SetNumberHelper(object, (double)number) : (number))
/* Change the valuestring of a JSON_String object, only takes effect when type of object is JSON_String */
char* yang_json_setValuestring(YangJson *object, const char *valuestring);

/* Macro for iterating over an array or object */
#define yang_json_arrayForEach(element, array) for(element = (array != NULL) ? (array)->child : NULL; element != NULL; element = element->next)

#endif
#endif
