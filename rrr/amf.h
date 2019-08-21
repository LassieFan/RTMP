#ifndef __AMF_H_
#define __AMF_H_

#include "Common.h"
#include "BufferReader.h"
#include "BufferWriter.h"

typedef enum
{
	AMF0_NUMBER = 0,			//数字（double）
	AMF0_BOOLEAN,				//布尔
	AMF0_STRING,				//字符串
	AMF0_OBJECT,				//对象
	AMF0_MOVIECLIP,				//保留
	AMF0_NULL,
	AMF0_UNDEFINED,				//未定义的
	AMF0_REFERENCE,				//引用
	AMF0_ECMA_ARRAY,			//数组
	AMF0_OBJECT_END,			//对象结束
	AMF0_STRICT_ARRAY,			//严格的数组
	AMF0_DATE,					//日期
	AMF0_LONG_STRING,			//长字符串
	AMF0_UNSUPPORTED,			//未支持
	AMF0_RECORDSET,				//保留的
	AMF0_XML_DOC,				//xml文档
	AMF0_TYPED_OBJECT,			//有类型的对象
	AMF0_AVMPLUS,				//转换成AMF3 
	AMF0_INVALID = 0xff
} AMF0DataType;

typedef enum
{
	AMF3_UNDEFINED = 0,
	AMF3_NULL,
	AMF3_FALSE,
	AMF3_TRUE,
	AMF3_INTEGER,
	AMF3_DOUBLE,
	AMF3_STRING,
	AMF3_XML_DOC,
	AMF3_DATE,
	AMF3_ARRAY,
	AMF3_OBJECT,
	AMF3_XML,
	AMF3_BYTE_ARRAY
} AMF3DataType;

//将常用的几种类型单独列出来使用
typedef enum
{
	AMF_NUMBER,
	AMF_BOOLEAN,
	AMF_STRING,
} AmfObjectType;

struct AmfObject
{
	AmfObjectType type;

	std::string amf_string;
	double amf_number;
	bool amf_boolean;

	AmfObject()
	{

	}

	AmfObject(std::string str)
	{
		this->type = AMF_STRING;
		this->amf_string = str;
	}

	AmfObject(double number)
	{
		this->type = AMF_NUMBER;
		this->amf_number = number;
	}

	AmfObject(bool boolean)
	{
		this->type = AMF_BOOLEAN;
		this->amf_boolean = boolean;
	}
};

typedef std::unordered_map<std::string, AmfObject> AmfObjects;

class AmfDecoder
{
public:
	int decode(const char *data, int size, int n = -1); //n: 解码次数

	void reset()
	{
		m_obj.amf_string = "";
		m_obj.amf_number = 0;
		m_objs.clear();
	}

	std::string getString() const
	{
		return m_obj.amf_string;
	}

	double getNumber() const
	{
		return m_obj.amf_number;
	}

	bool hasObject(std::string key) const
	{
		return (m_objs.find(key) != m_objs.end());
	}

	AmfObject getObject(std::string key)
	{
		return m_objs[key];
	}

	AmfObject getObject()
	{
		return m_obj;
	}

	AmfObjects getObjects()
	{
		return m_objs;
	}

private:
	static int decodeBoolean(const char *data, int size, bool& amf_boolean);
	static int decodeNumber(const char *data, int size, double& amf_number);
	static int decodeString(const char *data, int size, std::string& amf_string);
	static int decodeObject(const char *data, int size, AmfObjects& amf_objs);
	static uint16_t decodeInt16(const char *data, int size);
	static uint32_t decodeInt24(const char *data, int size);
	static uint32_t decodeInt32(const char *data, int size);

	AmfObject m_obj;
	AmfObjects m_objs;
};

class AmfEncoder
{
public:
	AmfEncoder(uint32_t size = 1024);
	~AmfEncoder();

	void reset()
	{
		m_index = 0;
	}

	std::shared_ptr<char> data()
	{
		return m_data;
	}

	uint32_t size() const
	{
		return m_index;
	}

	void encodeString(const char* str, int len, bool isObject = true);
	void encodeNumber(double value);
	void encodeBoolean(int value);
	void encodeObjects(AmfObjects& objs);
	void encodeECMA(AmfObjects& objs);

private:
	//在编码中调用，用于调用writeUintBE将长度编码存入m_data中
	void encodeInt8(int8_t value);
	void encodeInt16(int16_t value);
	void encodeInt24(int32_t value);
	void encodeInt32(int32_t value);
	void realloc(uint32_t size);

	std::shared_ptr<char> m_data;
	uint32_t m_size = 0;
	uint32_t m_index = 0;
};


#endif
