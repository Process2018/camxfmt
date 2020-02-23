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
	//static Debug* _instance;  //û������ʲô��
	static int count;           // һ���þ�̬��Ա��Ϊ������
	friend class Log; //��дΪ˽��(���࿴��һ������),��ϣ��һ������Դ�ȡ��һ�����˽�г�Աʱ�����Խ���������Ϊ��һ�����Ԫ��,
	                  //������ MessageLogger ���п��Է��� Debug ��˽�г�Աcount (��Ԫ����Է�����֮Ϊ��Ԫ��ϵ��������г�Ա������˽��)
	//friend Debug MessageLogger::debug()const; //��Ԫ�������� debug �����в���ͨ�����������Ķ��󣩷�����ľ�̬��Ա���� count , Ϊɶ
    friend void test_friend_method(Debug *dbg);

};

/**
��<<��������Ӧ����Ϊ�ǳ�Ա��������������Ϊ�����������Ԫ��
C:��ξ�����һ������������Ϊ���Ա��������ȫ�����ֿռ�ĳ�Ա�أ�
�����һ�����ز����������Ա����ôֻ�е�����һ��ʹ�õ���������Ǹ���Ķ���ʱ���ò������Żᱻ���á�����ò�����������������������������ͣ�����������뱻����Ϊȫ�����ֿռ�ĳ�Ա��
��C++Ҫ��ֵ=���±�[]������()�� �ͳ�Աָ��-> ���������뱻����Ϊ���Ա���������κΰ���Щ����������Ϊ���ֿռ��Ա�Ķ��嶼�ᱻ���Ϊ����ʱ�̴���
�������һ������������������string���������ô���ڶԳƲ�����������ڲ�������ö���Ϊȫ�����ֿռ��Ա��
**/
#pragma once
