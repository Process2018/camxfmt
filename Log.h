#pragma once

#include "Debug.h"
#include "MessageLogContext.h"

class Log
{
public:
	Log::Log():context() 
	{ 
		CDBG("construct with none param"); 
	}

	inline Log::Log(const char *fileName, const char *functionName, int lineNumber)
		:context(fileName, functionName, lineNumber) 
	{
		CDBG("construct with param");
	}
	
	//这里必须返回 Debug 类型对象，这样重载操作符才能被调用: object->member
	Debug error() const;
	Debug info() const;
	Debug warning() const;
	Debug debug() const;
	Debug verbose() const;

protected:
private:
	MessageLogContext context;
};

#define LOGE Log(__FILE__, __FUNCTION__, __LINE__).error
#define LOGI Log(__FILE__, __FUNCTION__, __LINE__).info
#define LOGW Log(__FILE__, __FUNCTION__, __LINE__).warning
#define LOGD Log(__FILE__, __FUNCTION__, __LINE__).debug
#define LOGV Log(__FILE__, __FUNCTION__, __LINE__).verbose

#pragma once



