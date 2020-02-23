#include "Debug.h"
#include <iostream>
#include <stdio.h>
using namespace std;

int Debug::count = 0;  // 在类外(类的实现文件)初始化类的静态成员变量 - 计数器 

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
	//现在将 LogToConsole调用 放到 Debug 类的成员函数 MessageLogger::debug()中
	if(count == 0) 
		LogToConsole(stream->logType, stream->context, stream->ss.str());
	************************************************
	*/
	delete stream; //添加拷贝构造函数并且申请内存及拷贝相关参数以后，调用 delete 释放内存，程序不再崩溃

/*******
这里导致程序崩溃,这个函数被调用了两次，跟拷贝构造函数有关，在返回对象和按值传递参数时，要生成临时对象，
生成临时对象要调用默认拷贝构造函数,只要类里有指针变量就得自己写拷贝构造函数和赋值函数，但是你确定用不着这些函数时，
可以把这些函数做private声明而不去实现它，这就防止了会有人去调用它们，也防止了编译器去生成它们。

析构函数不是用来销毁对象的，只是在对象生存期结束以前给你一个机会释放所占用的资源。本质上析构函数也是一个函数，
只要它所属的对象存在，你调用它几次都没问题。当然，如果你在析构函数里释放了某些资源，再次调用会导致重复释放崩溃
由编译器完成的销毁动作由于仅仅回收对象直接占用的内存，所以如果对象内还通过指针间接占用了其他内存，
或者占用了其他的系统资源（如文件句柄），就需要在对象销毁前通过析构函数来释放。如果类只是个非常简单的类，那么实际上根本不需要显式的析构函数。
析构函数真正不同于一般函数的地方在于：在对象销毁之前，它会被自动调用，而一般函数不会被自动调用。

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
	CDBG("qfh Debug::count=%d", Debug::count);// private 或者 public 限定的 friend 函数都可以访问
	CDBG("qfh Debug::count=%d", dbg->count);//private friend 可以访问
	//CDBG("qfh Debug::count=%d", count); //无法确定哪个实例
}