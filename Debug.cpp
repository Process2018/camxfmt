#include "Debug.h"
#include <iostream>
#include <stdio.h>
using namespace std;

int Debug::count = 0;  // ������(���ʵ���ļ�)��ʼ����ľ�̬��Ա���� - ������ 

Debug::Debug(std::string *s) : stream(new Stream(s)) {
	count++;
	CDBG(" construct count=%d", count);
	cout << "s = " << *s << endl;
}

Debug::Debug(const Debug &object) {
	count++;
	stream = new Stream();
	stream->context = object.stream->context;
	stream->logType = object.stream->logType;
	stream->space = object.stream->space;
	//stream->ss = object.stream->ss;
	CDBG("This is copy construct count=%d", count);
}

Debug::~Debug()
{
	count--;
	CDBG(" distruct and call LogToConsole count=%d", count);
	/*
	************************************************
	//���ڽ� LogToConsole���� �ŵ� Debug ��ĳ�Ա���� MessageLogger::debug()��
	if(count == 0) 
		LogToConsole(stream->logType, stream->context, stream->ss.str());
	************************************************
	*/
	delete stream; //��ӿ������캯�����������ڴ漰������ز����Ժ󣬵��� delete �ͷ��ڴ棬�����ٱ���

/*******
���ﵼ�³������,������������������Σ����������캯���йأ��ڷ��ض���Ͱ�ֵ���ݲ���ʱ��Ҫ������ʱ����
������ʱ����Ҫ����Ĭ�Ͽ������캯��,ֻҪ������ָ������͵��Լ�д�������캯���͸�ֵ������������ȷ���ò�����Щ����ʱ��
���԰���Щ������private��������ȥʵ��������ͷ�ֹ�˻�����ȥ�������ǣ�Ҳ��ֹ�˱�����ȥ�������ǡ�

�������������������ٶ���ģ�ֻ���ڶ��������ڽ�����ǰ����һ�������ͷ���ռ�õ���Դ����������������Ҳ��һ��������
ֻҪ�������Ķ�����ڣ�����������ζ�û���⡣��Ȼ��������������������ͷ���ĳЩ��Դ���ٴε��ûᵼ���ظ��ͷű���
�ɱ�������ɵ����ٶ������ڽ������ն���ֱ��ռ�õ��ڴ棬������������ڻ�ͨ��ָ����ռ���������ڴ棬
����ռ����������ϵͳ��Դ�����ļ������������Ҫ�ڶ�������ǰͨ�������������ͷš������ֻ�Ǹ��ǳ��򵥵��࣬��ôʵ���ϸ�������Ҫ��ʽ������������
��������������ͬ��һ�㺯���ĵط����ڣ��ڶ�������֮ǰ�����ᱻ�Զ����ã���һ�㺯�����ᱻ�Զ����á�

*********/
}

void Debug::LogToConsole(LogType type, const MessageLogContext &context, std::string logBuffer)
{
	std::string logString;
	CDBG(" E ");
	switch (type)
	{
	case LOG_DEBUG:
		logString.append("LOG_DEBUG: ");
		break;
	case LOG_ERROR:
		logString.append("LOG_ERROR: ");
		break;
	case LOG_INFO:
		logString.append("LOG_INFO: ");
		break;
	case LOG_WARN:
		logString.append("LOG_WARN: ");
		break;
	default:
		break;
	}
	logString.append(logBuffer);
	logString.append("......");
	logString.append(context.file);
	logString.append(" ");
	logString.append(context.function);
	logString.append("()");
	logString.append(to_string(context.line));
	std::cout << logString << std::endl;
	CDBG(" X ");
	test_friend_method(this);
}

void test_friend_method(Debug *dbg) { //Debug const dbg
	CDBG("qfh Debug::count=%d", Debug::count);// private ���� public �޶��� friend ���������Է���
	CDBG("qfh Debug::count=%d", dbg->count);//private friend ���Է���
	//CDBG("qfh Debug::count=%d", count); //�޷�ȷ���ĸ�ʵ��
}