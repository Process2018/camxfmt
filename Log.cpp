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

	Debug dbg = Debug(&debug); //����: ���� Debug���� -> ���� Stream���� -> ���� context ����
	MessageLogContext &ctxt = dbg.stream->context; // ��ȡһ������
	ctxt.copy(context); // �� context �� stream->context �� (file,line,function) ȥ��ʼ���ֲ����� ctxt 
	dbg.stream->logType = LOG_DEBUG;
	printf("%s %d X dbg.count = %d\n", __FUNCTION__, __LINE__, Debug::count); // ��Ϊ����Debug����Ԫ�࣬����������Է���Debug ��˽�г�Ա����
	if (dbg.count == 1) //�����������Ԫ��(������˽����Ԫ)����ͨ������Ҳ���ܷ��ʣ���Ϊ��˽�б���������Ҫͨ����ӵ���Debug�Ĺ��нӿڷ��ʣ�,��Ԫ��ʹ�ý���
		dbg.LogToConsole(dbg.stream->logType, dbg.stream->context, dbg.stream->ss.str());
	return dbg;       // ������Ķ������ÿ������캯����������Ҫ�Լ�����һ���������캯��
}

Debug Log::verbose() const
{
	Debug dbg = Debug();
	MessageLogContext &ctxt = dbg.stream->context;
	ctxt.copy(context);
	dbg.stream->logType = LOG_VERBOSE;
	return dbg;
}
