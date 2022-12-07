//
// Copyright (c) 2019-2022 yanggaofeng
//
#ifndef __YangAMF_H__
#define __YangAMF_H__
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <ctype.h>
#ifndef TRUE
#define TRUE	1
#define FALSE	0
#endif



  typedef enum AMFDataType
  { AMF_NUMBER = 0, AMF_BOOLEAN, AMF_STRING, AMF_OBJECT,
    AMF_MOVIECLIP,		/* reserved, not used */
    AMF_NULL, AMF_UNDEFINED, AMF_REFERENCE, AMF_ECMA_ARRAY, AMF_OBJECT_END,//9
    AMF_STRICT_ARRAY, AMF_DATE, AMF_LONG_STRING, AMF_UNSUPPORTED,
    AMF_RECORDSET,		/* reserved, not used */
    AMF_XML_DOC, AMF_TYPED_OBJECT,
    AMF_AVMPLUS,		/* switch to AMF3 */
    AMF_INVALID = 0xff
  } AMFDataType;

  typedef enum AMF3DataType
  { AMF3_UNDEFINED = 0, AMF3_NULL, AMF3_FALSE, AMF3_TRUE,
    AMF3_INTEGER, AMF3_DOUBLE, AMF3_STRING, AMF3_XML_DOC, AMF3_DATE,
    AMF3_ARRAY, AMF3_OBJECT, AMF3_XML, AMF3_BYTE_ARRAY
  } AMF3DataType;

   typedef struct AVal
  {
    char *av_val;
    int32_t av_len;
  }AVal;
#define AVC(str)	{(char*)str,sizeof(str)-1}
#define AVMATCH(a1,a2)	((a1)->av_len == (a2)->av_len && !yang_memcmp((a1)->av_val,(a2)->av_val,(a1)->av_len))

   struct AMFObjectProperty;

  typedef struct AMFObject
  {
    int32_t o_num;
     struct AMFObjectProperty *o_props;
  }  AMFObject;

  typedef   struct AMFObjectProperty
  {
	  struct AVal p_name;
    enum AMFDataType p_type;
    union
    {
      double p_number;
      struct  AVal p_aval;
      struct AMFObject p_object;
    } p_vu;
    int16_t p_UTCoffset;
  }   AMFObjectProperty;





  char *AMF_EncodeString(char *output, char *outend, const struct AVal * str);
  char *AMF_EncodeNumber(char *output, char *outend, double dVal);
  char *AMF_EncodeInt16(char *output, char *outend, short nVal);
  char *AMF_EncodeInt24(char *output, char *outend, int32_t nVal);
  char *AMF_EncodeInt32(char *output, char *outend, int32_t nVal);
  char *AMF_EncodeBoolean(char *output, char *outend, int32_t bVal);

  /* Shortcuts for AMFProp_Encode */
  char *AMF_EncodeNamedString(char *output, char *outend, const struct AVal * name, const struct AVal * value);
  char *AMF_EncodeNamedNumber(char *output, char *outend, const struct AVal * name, double dVal);
  char *AMF_EncodeNamedBoolean(char *output, char *outend, const struct AVal * name, int32_t bVal);

  unsigned short AMF_DecodeInt16(const char *data);
  uint32_t  AMF_DecodeInt24(const char *data);
  uint32_t  AMF_DecodeInt32(const char *data);
  void AMF_DecodeString(const char *data,struct AVal * str);
  void AMF_DecodeLongString(const char *data,struct  AVal * str);
  int32_t AMF_DecodeBoolean(const char *data);
  double AMF_DecodeNumber(const char *data);

  char *AMF_Encode(struct AMFObject * obj, char *pBuffer, char *pBufEnd);
  char *AMF_EncodeEcmaArray(struct AMFObject *obj, char *pBuffer, char *pBufEnd);
  char *AMF_EncodeArray(struct AMFObject *obj, char *pBuffer, char *pBufEnd);

  int32_t AMF_Decode(struct AMFObject * obj, const char *pBuffer, int32_t nSize,
		 int32_t bDecodeName);
  int32_t AMF_DecodeArray(struct AMFObject * obj, const char *pBuffer, int32_t nSize,
		      int32_t nArrayLen, int32_t bDecodeName);
  int32_t AMF3_Decode(struct AMFObject * obj, const char *pBuffer, int32_t nSize,
		  int32_t bDecodeName);
  void AMF_Dump(struct AMFObject * obj);
  void AMF_Dump1(struct AMFObject * obj);
  void AMF_Reset(struct AMFObject * obj);

  void AMF_AddProp(struct AMFObject * obj, const struct AMFObjectProperty * prop);
  int32_t AMF_CountProp(struct AMFObject * obj);
  struct AMFObjectProperty *AMF_GetProp(struct AMFObject * obj, const struct AVal * name,
				 int32_t nIndex);

  enum AMFDataType AMFProp_GetType(struct AMFObjectProperty * prop);
  void AMFProp_SetNumber(struct AMFObjectProperty * prop, double dval);
  void AMFProp_SetBoolean(struct AMFObjectProperty * prop, int32_t bflag);
  void AMFProp_SetString(struct AMFObjectProperty * prop, struct AVal * str);
  void AMFProp_SetObject(struct AMFObjectProperty * prop, struct AMFObject * obj);

  void AMFProp_GetName(struct AMFObjectProperty * prop, struct AVal * name);
  void AMFProp_SetName(struct AMFObjectProperty * prop, struct AVal * name);
  double AMFProp_GetNumber(struct AMFObjectProperty * prop);
  int32_t AMFProp_GetBoolean(struct AMFObjectProperty * prop);
  void AMFProp_GetString(struct AMFObjectProperty * prop, struct AVal * str);
  void AMFProp_GetObject(struct AMFObjectProperty * prop, struct AMFObject * obj);

  int32_t AMFProp_IsValid(struct AMFObjectProperty * prop);

  char *AMFProp_Encode(struct AMFObjectProperty * prop, char *pBuffer, char *pBufEnd);
  int32_t AMF3Prop_Decode(struct AMFObjectProperty * prop, const char *pBuffer,
		      int32_t nSize, int32_t bDecodeName);
  int32_t AMFProp_Decode(struct AMFObjectProperty * prop, const char *pBuffer,
		     int32_t nSize, int32_t bDecodeName);

  void AMFProp_Dump(struct AMFObjectProperty * prop);
  void AMFProp_Dump1(struct AMFObjectProperty * prop);
  void AMFProp_Reset(struct AMFObjectProperty * prop);

  typedef struct AMF3ClassDef
  {
	 struct AVal cd_name;
    char cd_externalizable;
    char cd_dynamic;
    int32_t cd_num;
    struct AVal *cd_props;
  } AMF3ClassDef;

  void AMF3CD_AddProp(struct AMF3ClassDef * cd, struct AVal * prop);
  struct AVal *AMF3CD_GetProp(struct AMF3ClassDef * cd, int32_t idx);

#endif				/* __AMF_H__ */
