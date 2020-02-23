/***********************************
Ϊ�˱���ͬһ���ļ���include���
1 #ifndef .... #endif  ��ʽ -- ������֧��������ֲ�Ժ�
2 #pragma once��ʽ           -- ���Ա������ֳ�ͻ
���ܹ�֧�������ַ�ʽ�ı������ϣ����߲�û��̫������𣬵���������Ȼ������һЩϸ΢������
��ʽ1 �����ں����ֲ��ܳ�ͻ���ⲻ����Ա�֤ͬһ���ļ����ᱻ������Σ�Ҳ�ܱ�֤������ȫ��ͬ�������ļ����ᱻ��С��ͬʱ������
	��Ȼ��ȱ����������ͬͷ�ļ��ĺ�����С�ġ�ײ���������ܾͻᵼ��ͷ�ļ��������ڣ�������ȴӲ˵�Ҳ���������״��
��ʽ2 ���ɱ������ṩ��֤��ͬһ���ļ����ᱻ������Ρ�ע��������˵�ġ�ͬһ���ļ�����ָ�����ϵ�һ���ļ���������ָ������ͬ�������ļ���
	�����ĺô��ǣ��㲻���ٷѾ���������ˣ���ȻҲ�Ͳ�����ֺ�����ײ������������⡣��Ӧ��ȱ��������ĳ��ͷ�ļ��ж�ݿ�����
	���������ܱ�֤���ǲ����ظ���������Ȼ����Ⱥ�����ײ�����ġ��Ҳ��������������⣬�ظ����������ױ����ֲ�������
*************************************/

#pragma once

/*******************************
Windows��MFC��include�ļ����ǳ��󣬼�ʹ��һ�����ٵĴ�����򣬱������ҲҪ�����൱����ʱ������ɹ�����
����ÿ��.CPP�ļ���������ͬ��include�ļ���Ϊÿ��.CPP�ļ����ظ�������Щ�ļ����Եú�ɵ�ˡ�
��νͷ�ļ�Ԥ���룬���ǰ�һ������(Project)��ʹ�õ�һЩMFC��׼ͷ�ļ�(��Windows.H��Afxwin.H)Ԥ�ȱ��룬
�Ժ�ù��̱���ʱ�����ٱ����ⲿ��ͷ�ļ�������ʹ��Ԥ����Ľ�����������Լӿ�����ٶȣ���ʡʱ�䡣
��������Ϊ��������ָ��#include "stdafx.h"ǰ�Ĵ��붼��Ԥ����ģ�������#include "stdafx. h"ָ�ʹ��projectname.pch��������ָ��֮������д��롣
��ˣ����е�MFCʵ���ļ���һ����䶼�ǣ�#include "stdafx.h"������ǰ������д��뽫�����ԣ�����������ͷ�ļ�Ӧ������һ�к��汻������
�����㽫��õ���No such file or directory�����������˼�������Ĵ�����ʾ��
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
//#define CDBG(fmt, args...) fprintf (stderr, fmt, args) ��debug�ɱ����Ϊ0��ʱ��CDBG("hello\n")�������������������ķ�ʽ��
#define CDBG(fmt, arg...)  printf("%s,%s(),%d:" fmt "\n", __FILE__,__FUNCTION__,__LINE__, ##arg) //## ��ճ����,linux ��֧������ģʽ 
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
�������꣺ #define myprint printf
�����ڲ�����Ҫ������ˣ�ֻ�ܶ���Ϊ��:  #define myprint 
��������Щ�к���õĴ�����������������䣺("%d",idx); ��Ӧ�ûᱻ��������һ�Σ�
��������������ѹջ����ջһ�Σ��Ӷ������˳�������п���������һ���ϲߡ�
************************************************
*/
#ifdef NOTDEBUG
#define CDBG(fmt, ...) { do {} while (0); }
#else
#define CDBG(fmt, ...)  printf("printf : %s() %d : " fmt "\n",__FUNCTION__,__LINE__,__VA_ARGS__)

//Showing log in consol for windows MFC
//1��cprintf()������\n���ͳ�\r\n�����س����У�����ֻ�ǻ��У�û�лس�
//2��cprintf()ֱ����ǰ�ı��������
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
******************************* inline ���� **************************
�������ж���ĳ�Ա�����Ĺ�ģһ�㶼��С����ϵͳ���ú����Ĺ��������ѵ�ʱ�俪������ǱȽϴ�ġ�
����һ��������ʱ�俪��ԶԶ����С��ģ��������ȫ������ִ��ʱ�䡣Ϊ�˼���ʱ�俪���������
�����ж���ĳ�Ա�����в�����ѭ���ȿ��ƽṹ��C++ϵͳ���Զ���������Ϊ����(inline)����������
C++Ҫ���һ������ú���Ҫ�ùؼ���inline�������������ڶ���ĳ�Ա����������ʡ��inline����Ϊ��Щ��Ա�����ѱ�������ָ��Ϊ���ú���
�ں��������������Ķ�������֮һ��inline�������ɡ�ֵ��ע����ǣ�����������ⶨ��inline������
����뽫�ඨ��ͳ�Ա�����Ķ��嶼����ͬһ��ͷ�ļ���(����д��ͬһ��Դ�ļ���)���������ʱ�޷������û�(����������Ŀ���Ƕ�뵽�������õ�)��
��������������������Ľӿ������ʵ�ַ��룬��������Ϣ���Ρ���Ȼ�����ִ��Ч������ˣ�����������������ĽǶ������������������Ǻõİ취��
ֻ�������ⶨ��ĳ�Ա������ģ��С������Ƶ�ʽϸ�ʱ���Ž��˳�Ա����ָ��Ϊ���ú�����
***********************************************************************
*/

class MessageLogContext
{
public:
	MessageLogContext() : line(0), file(0), function(0) 
	{
		CDBG("construct without param");
	}
	
	//@���ڶ���ĺ����Ѿ�Ĭ���� inline . ��(���� & ��������)ֻ�ܶ���ȫ�ֱ����Ͷ��󣬲���ִ��������ú���
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

