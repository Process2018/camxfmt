/***********************************
为了避免同一个文件被include多次
1 #ifndef .... #endif  方式 -- 由语言支持所以移植性好
2 #pragma once方式           -- 可以避免名字冲突
在能够支持这两种方式的编译器上，二者并没有太大的区别，但是两者仍然还是有一些细微的区别。
方式1 依赖于宏名字不能冲突，这不光可以保证同一个文件不会被包含多次，也能保证内容完全相同的两个文件不会被不小心同时包含。
	当然，缺点就是如果不同头文件的宏名不小心“撞车”，可能就会导致头文件明明存在，编译器却硬说找不到声明的状况
方式2 则由编译器提供保证：同一个文件不会被包含多次。注意这里所说的“同一个文件”是指物理上的一个文件，而不是指内容相同的两个文件。
	带来的好处是，你不必再费劲想个宏名了，当然也就不会出现宏名碰撞引发的奇怪问题。对应的缺点就是如果某个头文件有多份拷贝，
	本方法不能保证他们不被重复包含。当然，相比宏名碰撞引发的“找不到声明”的问题，重复包含更容易被发现并修正。
*************************************/

#pragma once

/*******************************
Windows和MFC的include文件都非常大，即使有一个快速的处理程序，编译程序也要花费相当长的时间来完成工作。
由于每个.CPP文件都包含相同的include文件，为每个.CPP文件都重复处理这些文件就显得很傻了。
所谓头文件预编译，就是把一个工程(Project)中使用的一些MFC标准头文件(如Windows.H、Afxwin.H)预先编译，
以后该工程编译时，不再编译这部分头文件，仅仅使用预编译的结果。这样可以加快编译速度，节省时间。
编译器认为，所有在指令#include "stdafx.h"前的代码都是预编译的，它跳过#include "stdafx. h"指令，使用projectname.pch编译这条指令之后的所有代码。
因此，所有的MFC实现文件第一条语句都是：#include "stdafx.h"。在它前面的所有代码将被忽略，所以其他的头文件应该在这一行后面被包含。
否则，你将会得到“No such file or directory”这样让你百思不得其解的错误提示。
***********************************/
#include "stdafx.h" 
#include <string>
#include <stdio.h>
//#define NOTDEBUG
#define USE_VS2017

enum LogType
{
	LOG_VERBOSE,
	LOG_DEBUG,
	LOG_INFO,
	LOG_WARN,
	LOG_ERROR,
};

#ifdef GCC
//#define CDBG(fmt, args...) fprintf (stderr, fmt, args) 在debug可变参数为0的时候，CDBG("hello\n")，编译会出错，采用这样的方式：
#define CDBG(fmt, arg...)  printf("%s,%s(),%d:" fmt "\n", __FILE__,__FUNCTION__,__LINE__, ##arg) //## 是粘连符,linux 才支持这种模式 
#define CDBG(fmt, args...) fprintf (stderr, fmt, args)
#define CDBG(arg...) {\  
	printf("[debug]:%s:%s:%d ---->", __FILE__, __FUNCTION__, __LINE__); \
	printf(arg); \
	fflush(stdout); \
}
#elif defined (USE_VS2017)
#define printf_err(s)   printf("\[%s:%d    func:%s]%s\n", __FILE__, __LINE__, __FUNCTION__, s);
#define printf_info(s)  printf("\[%s:%d    func:%s]%s\n", __FILE__, __LINE__, __FUNCTION__, s);
#define printf_warn(s)  printf("\[%s:%d    func:%s]%s\n", __FILE__, __LINE__, __FUNCTION__, s);

#define printf_log(t,s) \
        switch (t) \
        { \
            case 1: \
            case 2: \
                printf_info(s); \
                break; \
            case 3: \
                printf_warn(s); \
				break; \
            case 4: \
            default: \
                printf_err(s); \
                break; \
        }
/*
************************************************
如果定义宏： #define myprint printf
当后期不想再要宏输出了，只能定义为空:  #define myprint 
但是在那些有宏调用的代码区会留下类似语句：("%d",idx); 它应该会被程序运算一次，
像函数参数那样被压栈，出栈一次，从而增加了程序的运行开销，不是一个上策。
************************************************
*/
#ifdef NOTDEBUG
#define CDBG(fmt, ...) { do {} while (0); }
#else
#define CDBG(fmt, ...)  printf("printf : %s() %d : " fmt "\n",__FUNCTION__,__LINE__,__VA_ARGS__)

//Showing log in consol for windows MFC
//1：cprintf()并不将\n解释成\r\n（即回车换行），它只是换行，没有回车
//2：cprintf()直接向当前文本窗口输出
#define DBG(fmt, ...)  _cprintf("_cprintf : %s() %d : " fmt "\n",__FUNCTION__,__LINE__,__VA_ARGS__)
#endif

//// Below somtime make process crash , careful (cameraDlg.cpp / format.cpp)
#define MYDEBUG
#ifdef MYDEBUG
#define DEBG(arg) {\
		printf("[debug]:%s:%s:%d ---->",__FILE__,__FUNCTION__,__LINE__);\
		printf(arg);\
		fflush(stdout);\
		printf("\n");\
	}
#else
#define DEBG(arg...) {}
#endif
#endif

/*
******************************* inline 函数 **************************
在类体中定义的成员函数的规模一般都很小，而系统调用函数的过程所花费的时间开销相对是比较大的。
调用一个函数的时间开销远远大于小规模函数体中全部语句的执行时间。为了减少时间开销，如果在
类体中定义的成员函数中不包括循环等控制结构，C++系统会自动将它们作为内置(inline)函数来处理。
C++要求对一般的内置函数要用关键字inline声明，但对类内定义的成员函数，可以省略inline，因为这些成员函数已被隐含地指定为内置函数
在函数的声明或函数的定义两者之一作inline声明即可。值得注意的是，如果在类体外定义inline函数，
则必须将类定义和成员函数的定义都放在同一个头文件中(或者写在同一个源文件中)，否则编译时无法进行置换(将函数代码的拷贝嵌入到函数调用点)。
但是这样做，不利于类的接口与类的实现分离，不利于信息隐蔽。虽然程序的执行效率提高了，但从软件工程质量的角度来看，这样做并不是好的办法。
只有在类外定义的成员函数规模很小而调用频率较高时，才将此成员函数指定为内置函数。
***********************************************************************
*/

class MessageLogContext
{
public:
	MessageLogContext() : line(0), file(0), function(0) 
	{
		CDBG("construct without param");
	}
	
	//@类内定义的函数已经默认是 inline . 在(类内 & 函数体外)只能定义全局变量和对象，不能执行语句或调用函数
	MessageLogContext::MessageLogContext(const char *fileName, const char *functionName, int lineNumber)
		: file(fileName), function(functionName), line(lineNumber) {
		CDBG("construct with param");
	}
	int line;
	const char *file;
	const char *function;
	void copy(const MessageLogContext &logContext)
	{
		CDBG("function=%s line=%d",logContext.function,logContext.line);
		this->file = logContext.file;
		this->line = logContext.line;
		this->function = logContext.function;
	}

// To be a public class for Log.h & Debug.h access
private:
	friend class MessageLogger;
	friend class Debug;
};

#pragma once

