#pragma once
#include <iostream>  
#include <iomanip>  
#include <fstream>  
#include <string>  
#include <cstdlib>  
#include <stdint.h>  
#include <sstream> 
#ifdef QT
#include "Math/Vector2.h"  
#include "Math/Vector3.h" 
#endif
#include <vector>

#include "MessageLogContext.h"

#include <stdlib.h>
#include <stdio.h>

//#include <io.h>    
//#include <fcntl.h> 
//#include <time.h>
//#include <windows.h>


using namespace std;

class Debug
{
public:
	struct Stream {
		std::ostringstream ss;
		bool space;
		MessageLogContext context;
		LogType logType;

		Stream() : ss(), space(true), context() {}
		Stream(std::string *s) : ss(*s), space(true), context() {
			CDBG("construct by new Stream and *s = %s",s->c_str());
		}
	} *stream;

	inline Debug() : stream(new Stream()) { count++;}
	Debug(std::string *s);
	Debug::Debug(const Debug &object);
	~Debug();

	inline Debug &operator<<(bool t) { stream->ss << (t ? "true" : "false"); return maybeSpace(); }
	inline Debug &operator<<(char t) { stream->ss << t; return maybeSpace(); }
	inline Debug &operator<<(signed short t) { stream->ss << t; return maybeSpace(); }
	inline Debug &operator<<(unsigned short t) { stream->ss << t; return maybeSpace(); }
	inline Debug &operator<<(std::string s) { stream->ss << s; return maybeSpace(); }
	inline Debug &operator<<(const char* c) { stream->ss << c; return maybeSpace(); }
#ifdef QT
	inline Debug &operator<<(Vector2 vec) { stream->ss << "(" << vec.x << "," << vec.y << ")"; return maybeSpace(); }
	inline Debug &operator<<(Vector3 vec) { stream->ss << "(" << vec.x << "," << vec.y << "," << vec.z << ")"; return maybeSpace(); }
#endif
	inline Debug &space() { stream->space = true; stream->ss << ' '; return *this; }
	inline Debug &nospace() { stream->space = false; return *this; }
	inline Debug &maybeSpace() { if (stream->space) stream->ss << ' '; return *this; }

	template <typename T>
	inline Debug &operator<<(const std::vector<T> &vec)
	{
		stream->ss << '(';
		for (int i = 0; i < vec.size(); ++i) {
			stream->ss << vec.at(i);
			stream->ss << ", ";
		}
		stream->ss << ')';
		return maybeSpace();
	}

	void LogToConsole(LogType type, const MessageLogContext &context, std::string logBuffer);
private:
	//static Debug* _instance;  //没看到有什么用
	static int count;           // 一般用静态成员做为计数器
	friend class Log; //常写为私有(把类看成一个变量),当希望一个类可以存取另一个类的私有成员时，可以将该类声明为另一类的友元类,
	                  //这样在 MessageLogger 类中可以访问 Debug 的私有成员count (友元类可以访问与之为友元关系的类的所有成员，包括私有)
	//friend Debug MessageLogger::debug()const; //友元函数，在 debug 函数中不能通过（类或者类的对象）访问类的静态成员变量 count , 为啥
    friend void test_friend_method(Debug *dbg);

};

/**
“<<”操作符应定义为非成员函数，并将其作为所操作类的友元。
C:如何决定把一个操作符重载为类成员函数还是全局名字空间的成员呢？
①如果一个重载操作符是类成员，那么只有当与他一起使用的左操作数是该类的对象时，该操作符才会被调用。如果该操作符的左操作数必须是其他的类型，则操作符必须被重载为全局名字空间的成员。
②C++要求赋值=，下标[]，调用()， 和成员指向-> 操作符必须被定义为类成员操作符。任何把这些操作符定义为名字空间成员的定义都会被标记为编译时刻错误。
③如果有一个操作数是类类型如string类的情形那么对于对称操作符比如等于操作符最好定义为全局名字空间成员。
**/
#pragma once
