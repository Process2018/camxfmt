#include "Log.h"

Debug Log::error() const
{
	Debug dbg = Debug();
	MessageLogContext &ctxt = dbg.stream->context;
	ctxt.copy(context);
	dbg.stream->logType = LOG_ERROR;
	return dbg;
}


Debug Log::info() const
{
	Debug dbg = Debug();
	MessageLogContext &ctxt = dbg.stream->context;
	ctxt.copy(context);
	dbg.stream->logType = LOG_INFO;
	return dbg;
}

Debug Log::warning() const
{
	Debug dbg = Debug();
	MessageLogContext &ctxt = dbg.stream->context;
	ctxt.copy(context);
	dbg.stream->logType = LOG_WARN;
	return dbg;
}

Debug Log::debug() const
{
	CDBG("E");
	std::string debug = "debug";
	char c[20];
	strcpy(c, debug.c_str()); // just for testing

	Debug dbg = Debug(&debug); //包括: 创建 Debug对象 -> 创建 Stream对象 -> 创建 context 对象
	MessageLogContext &ctxt = dbg.stream->context; // 获取一份引用
	ctxt.copy(context); // 用 context 的 stream->context 的 (file,line,function) 去初始化局部变量 ctxt 
	dbg.stream->logType = LOG_DEBUG;
	printf("%s %d X dbg.count = %d\n", __FUNCTION__, __LINE__, Debug::count); // 因为是类Debug的友元类，所以这里可以访问Debug 的私有成员变量
	if (dbg.count == 1) //如果不设置友元类(可以是私有友元)，则通过对象也不能访问（因为是私有变量，否则要通过间接调用Debug的公有接口访问）,友元类使用较少
		dbg.LogToConsole(dbg.stream->logType, dbg.stream->context, dbg.stream->ss.str());
	return dbg;       // 返回类的对象会调用拷贝构造函数，所以需要自己定义一个拷贝构造函数
}

Debug Log::verbose() const
{
	Debug dbg = Debug();
	MessageLogContext &ctxt = dbg.stream->context;
	ctxt.copy(context);
	dbg.stream->logType = LOG_VERBOSE;
	return dbg;
}
