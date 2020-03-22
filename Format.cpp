#include "stdafx.h"
#include "Format.h"
#include <stdio.h>
#include <iostream>
#include <fstream>

using namespace std;

#include "MessageLogContext.h"

USER_DATA user_data;

char m_userFormatString[64][1024];

string defaultCueForser[5] = {
	{"Please input a txt file with below format:\r\n\r\n"},
	{"0x0d34  0x3b        ------  or \r\n"},
	{"0x0d34, 0x3b        ------  or\r\n"},
	{"0d34	3b            \r\n\r\n"},
	{"Below are default format for addr and data :\r\n\r\n"},
};

string default_eeprom_memory_format_string[21] =
{
	{"  <!--Sequence of register setting to configure the device -->" },
	{"  <memoryMap>" },
	{"	   <!--Contains: register address, register data, register address type, register data type," },
	{"        operation and delay in micro seconds" },
	{"        element for slaveAddr" },
	{"        element for registerAddr" },
	{"        element for registerData" },
	{"        element for regAddrType" },
	{"        element for regDataType" },
	{"        element for operation" },
	{"        element for delayUs -->" },
	{"	   <regSetting>"},
	{"	     <slaveAddr>0x00</slaveAddr>"},
	{"	     <registerAddr>0x00</registerAddr>"},
	{"	     <registerData>0x00</registerData>"},
	{"	     <regAddrType rang=\"[1,4]\">2</regAddrType>"},
	{"	     <regDataType rang=\"[1,4]\">2</regDataType>"},
	{"	     <operation>READ</operation>"},
	{"	     <delayUs>0</delayUs>"},
	{"	     </regSetting>"},
	{"  </memoryMap>" },
};

string default_sensor_init_format_string[15] =
{
	{"  <!--Sequence of register setting to configure the device -->" },
	{"  <initSettings>" },
	{"    <!--Specify which sensor version can support this setting-->" },
	{"      <sensorVersion>0</sensorVersion>" },
	{"      <initSetting>" },
	{"          <regSetting>"},
	{"          <registerAddr>0x0000</registerAddr>"},
	{"          <registerData>0x00</registerData>"},
	{"          <regAddrType rang=\"[1,4]\">2</regAddrType>"},
	{"          <regDataType rang=\"[1,4]\">2</regDataType>"},
	{"          <operation>WRITE</operation>"},
	{"          <delayUs>0</delayUs>"},
	{"        </regSetting>"},
	{"      </initSetting>" },
	{"  </initSettings>" },
};

string default_sensor_res_format_string[15] =
{
	{"  <!--Sequence of register setting to configure the device -->" },
	{"  <resSettings>" },
	{"    <!--Register setting configuration" },
	{"        Contains: register address, register data, register address type, register data type" },
	{"        operation and delay in micro seconds -->" },
	{"    <regSetting>"},
	{"      <registerAddr>0x0000</registerAddr>"},
	{"      <registerData>0x00</registerData>"},
	{"      <regAddrType rang=\"[1,4]\">2</regAddrType>"},
	{"      <regDataType rang=\"[1,4]\">2</regDataType>"},
	{"      <operation>WRITE</operation>"},
	{"      <delayUs>0</delayUs>"},
	{"    </regSetting>"},
	{"  </resSettings>" },
};

Format::Format()
{
}


Format::~Format()
{
}

wchar_t* doMultiByteToWideChar(const char* cStr) {
	int num = MultiByteToWideChar(0, 0, cStr, -1, NULL, 0);
	wchar_t *wChar = new wchar_t[num];
	MultiByteToWideChar(0, 0, cStr, -1, wChar, num);
	return wChar;
}

wchar_t* doMultiByteToWideChar(const string & cStr) {
	int num = MultiByteToWideChar(0, 0, cStr.c_str(), -1, NULL, 0);
	wchar_t *wChar = new wchar_t[num];
	MultiByteToWideChar(0, 0, cStr.c_str(), -1, wChar, num);
	return wChar;
}

int write_string_to_file_append(string &filename, const string str)// write the data to a file in a time
{
	DBG("adding str = %s \n", str.c_str());

	ofstream out;
	out.open(filename, fstream::out | fstream::app);
	out << str;
	out.close();
	return 0;
}

int write_string_to_file_append(ofstream &out, const string str) // if there is no ''&'', build fail , we had't better use this function
{
	DBG("adding str = %s \n", str.c_str());
	out << str;
	return 0;
}

string replace_with_substr(string strline, string substr)
{
	int pos1 = strline.find_first_of(">");
	int pos2 = strline.find("<", pos1);

	string strRt = strline.replace(pos1 + 1, pos2 - pos1 - 1, substr);
	DBG("strRt = %s \n", strRt.c_str());
	return strRt;
}


//how to skip string except addr like 0xAAAA ?
void Format::txt2xml_for_modules(string filename, USER_DATA user_data) {
	printf_err(filename.c_str());

	//do a test for file operation
	test_file_read_write();

	int pos_of_dog = find_pos_substr(filename, ".");
	string str_sub1 = filename.substr(0, pos_of_dog);
	string str_sub2 = filename.substr(pos_of_dog, strlen(filename.c_str()));
	DBG("str_sub1 = %s str_sub2 = %s \n", str_sub1.c_str(), str_sub2.c_str());
	string writefilename = str_sub1 + "_output" + ".xml";
	DBG("writefilename = %s \n", writefilename.c_str());

	//process excel file
	if (filename.find(".csv") != string::npos
		|| filename.find(".xls") != string::npos
		|| filename.find(".xlsx") != string::npos)
	{
		DBG("process csv file");
		read_from_excel_file(filename.c_str());
	}

	//clear file
	ofstream out;
	out.open(writefilename.c_str());
	out.close();

	//open file and update
	out.open(writefilename, fstream::out | fstream::app);


	string formatStr = "";
	int regSetting_pos_start = 0;
	int regSetting_pos_end = 0;
	int regSetting_pos_end_use = 0;
	int registerAddr_pos = 0;
	int registerData_pos = 0;
	int regAddrType_pos = 0;
	int regDataType_pos = 0;

	int nFomatStringLineCnt = 0;

	if (user_data.nRadio_select_mode == SENSOR_INIT || user_data.nRadio_select_mode == SENSOR_RES)
	{
		nFomatStringLineCnt = 15;
	}
	else if (user_data.nRadio_select_mode == EEPROM) {
		nFomatStringLineCnt = 21;
	}

	if (user_data.nUserFormatStringLine > 0)
	{
		nFomatStringLineCnt = user_data.nUserFormatStringLine;
	}

	for (int i = regSetting_pos_start; i < nFomatStringLineCnt; i++)
	{
		if (user_data.nUserFormatStringLine > 0)
		{
			formatStr = m_userFormatString[i];

		}
		else
		{
			if (user_data.nRadio_select_mode == SENSOR_INIT)
			{
				formatStr = default_sensor_init_format_string[i];
			}
			else if (user_data.nRadio_select_mode == SENSOR_RES) {
				formatStr = default_sensor_res_format_string[i];
			}
			else if (user_data.nRadio_select_mode == EEPROM) {
				formatStr = default_eeprom_memory_format_string[i];
			}
			else {
				DBG("There is nothing I can do, go back");
				return;
			}
		}

		DBG("user setting str = %s", formatStr.c_str());
		if (string::npos != formatStr.find("<regSetting>"))
		{
			regSetting_pos_start = i;

		}
		else if (string::npos != formatStr.find("<registerAddr>"))
		{
			registerAddr_pos = i;
		}
		else if (string::npos != formatStr.find("<registerData>"))
		{
			registerData_pos = i;
		}
		else if (string::npos != formatStr.find("<regAddrType"))
		{
			regAddrType_pos = i;
		}
		else if (string::npos != formatStr.find("<regDataType"))
		{
			regDataType_pos = i;
		}
		else if (string::npos != formatStr.find("</regSetting>"))
		{
			regSetting_pos_end = i;
			break;
		}
		if (regSetting_pos_start == 0)
		{
			write_string_to_file_append(out, formatStr);
			write_string_to_file_append(out, "\n");
		}
	}
	DBG("regSetting_pos_start=%d regSetting_pos_end=%d", regSetting_pos_start, regSetting_pos_end);

	string readfilename = filename;
	ifstream in;
	in.open(readfilename.c_str(), std::ios::in);
	if (!in.is_open()) {
		cout << "open readFile Failded." << endl;
		return;
	}

	string strline;
	int line_cnt = 0;
	while (getline(in, strline))
	{
		if (user_data.nRadio_select_mode == EEPROM) {
			static int num_of_memory_block = 0;
			if (strncmp("block", strline.c_str(), 5) == 0)
			{
				num_of_memory_block++;
				DBG("strline = %s num_of_memory_block=%d\n", strline.c_str(), num_of_memory_block);
				continue;
			}
		}
		int pos1 = 0;
		int pos2 = 0;
		int pos3 = 0;
		string str_sub1;
		string str_sub2;
		string str_sub = strline.substr(0, 2);
		line_cnt++;
		int flag = 1;
		DBG("strline = %s str_sub = %s\n", strline.c_str(), str_sub.c_str());
		int len = strline.length(); // or strline.size()

		if ((strcmp(str_sub.c_str(), "0x") == 0) && (strline.find(",", 0)) != string::npos) // for 0x1234, 1234 or 0x1234 1234 case
		{
			pos1 = strline.find_first_of("x");
			pos2 = strline.find_first_of(",");
			if (pos2 == string::npos) //if there is no ',', check if there is empty space
			{
				if ((pos2 = strline.find_first_of(" ")) == string::npos)
					pos2 = strline.find_first_of("	");
				if (pos2 == string::npos)
				{
					DBG("This line has some problem, goto skip pos1=%d pos2=%d", pos1, pos2);
					goto SKIP;
				}
			}
			pos3 = strline.find_last_of("x");
			DBG("pos1=%d pos2=%d pos3=%d \n", pos1, pos2, pos3);
			str_sub1 = strline.substr(pos1 - 1, pos2);
			str_sub2 = strline.substr(pos3 - 1, len);
			flag = 5;
			DBG("str_sub1=%s str_sub2=%s str_sub=%s\n", str_sub1.c_str(), str_sub2.c_str(), str_sub.c_str());
		}
		else if ((strcmp(str_sub.c_str(), "0x") == 0) && (strline.find("0x", 2)) != string::npos)// for 0x1234 0x1234 or 0x1234, 0x1234 case
		{
			pos1 = strline.find_first_of("x");
			pos2 = strline.find_first_of(",");//if there is no empty space, check if there is ','
			if (pos2 == string::npos)
			{
				if ((pos2 = strline.find_first_of(" ")) == string::npos)
					pos2 = strline.find_first_of("	");
				if (pos2 == string::npos)

				{
					DBG("This line has some problem, goto skip pos1=%d pos2=%d", pos1, pos2);
					goto SKIP;
				}
			}
			pos3 = strline.find_last_of("x");
			DBG("pos1=%d pos2=%d pos3=%d \n", pos1, pos2, pos3);
			str_sub1 = strline.substr(pos1 - 1, pos2);
			str_sub2 = strline.substr(pos3 - 1, len);
			flag = 5;
			DBG("str_sub1=%s str_sub2=%s str_sub=%s\n", str_sub1.c_str(), str_sub2.c_str(), str_sub.c_str());
		}
		else //For there is no 0x -- orignal
		{
			str_sub1 += "0x";
			str_sub2 += "0x";
			//int len = strline.length(); // or strline.size()
			for (int i = 0; i < len; i++)
			{
				if (strline[i] < 'Z' && strline[i] > 'F' || strline[i] < 'z' && strline[i] > 'f')
				{
					DBG("This line has some problem with elem %d", i);
					goto SKIP;
				}
				if (strline[i] >= 'A' && strline[i] <= 'F' || strline[i] >= 'a' && strline[i] <= 'f' || strline[i] >= '0' && strline[i] <= '9')
				{
					if (flag == 1 || flag == 3) //flag == 3 means str1 filled
					{
						str_sub1 += strline[i];
						flag = 3;
					}
					else if (flag == 2)
						str_sub2 += strline[i];
				}
				else if ((strline[i] < 48 || (strline[i] > 57 && strline[i] < 65) || (strline[i] > 90 && strline[i] < 97) || strline[i] > 122) && flag == 3)
				{
					flag = 2;
				}
			}
			DBG("str_sub1=%s str_sub2=%s str_sub=%s \n", str_sub1.c_str(), str_sub2.c_str(), str_sub.c_str());
		}

		if (flag == 1)
		{
			cout << "There is no data in this line, check next line" << endl;
			continue;
		}


		for (int i = regSetting_pos_start; i < regSetting_pos_end + 1; i++)
		{
			if (user_data.nUserFormatStringLine > 0)
			{

				formatStr = m_userFormatString[i];
			}
			else if (user_data.nRadio_select_mode == SENSOR_INIT) {
				formatStr = default_sensor_init_format_string[i];
			}
			else if (user_data.nRadio_select_mode == SENSOR_RES)
			{
				formatStr = default_sensor_res_format_string[i];
			}
			else if (user_data.nRadio_select_mode == EEPROM) {
				formatStr = default_eeprom_memory_format_string[i];
			}
			else {
				DBG("There is nothing I can do, go back");
				return;
			}

			string newStr = formatStr;
			DBG("format str = %s", formatStr.c_str());

			if (registerAddr_pos == i)
			{
				newStr = replace_with_substr(formatStr, str_sub1);
			}
			else if (registerData_pos == i)
			{
				newStr = replace_with_substr(formatStr, str_sub2);
			}
			else if (regAddrType_pos == i)
			{
				newStr = replace_with_substr(formatStr, to_string(user_data.nRadio_addr_type));
			}
			else if (regDataType_pos == i)
			{
				newStr = replace_with_substr(formatStr, to_string(user_data.nRadio_data_type));
			}
			write_string_to_file_append(out, newStr);
			write_string_to_file_append(out, "\n");
		}
		goto ADD;
	SKIP:
		DBG("skip strline = %s \n", strline.c_str());
		continue;

	ADD:
		DBG("add strline = %s \n", strline.c_str());
		continue;
	}

	for (int i = regSetting_pos_end + 1; i < nFomatStringLineCnt; i++)
	{

		if (user_data.nUserFormatStringLine > 0)
		{
			formatStr = m_userFormatString[i];
		}
		else if (user_data.nRadio_select_mode == SENSOR_INIT)
		{
			formatStr = default_sensor_init_format_string[i];
		}
		else if (user_data.nRadio_select_mode == SENSOR_RES)
		{
			formatStr = default_sensor_res_format_string[i];
		}
		else if (user_data.nRadio_select_mode == EEPROM)
		{
			formatStr = default_eeprom_memory_format_string[i];
		}
		if (formatStr.size() > 0) {
			write_string_to_file_append(out, formatStr);
			write_string_to_file_append(out, "\n");
		}
	}
	in.close();
	out.flush();
	out.close();
	cout << "cout:txt2xml process " << line_cnt << " line for sensor done " << filename.c_str() << endl;
	fprintf(stderr, "fprintf: txt2xml process %d line for sensor done %s\n", line_cnt, filename.c_str());
}

void Format::xml2txt_for_modules(string filename, USER_DATA user_data) {
	printf_err(filename.c_str());

	int pos_of_dog = find_pos_substr(filename, ".");
	string str_sub1 = filename.substr(0, pos_of_dog);
	string str_sub2 = filename.substr(pos_of_dog, strlen(filename.c_str()));
	cout << "str_sub = " << str_sub1 << " str_sub2 = " << str_sub2 << endl;
	string writefilename = str_sub1 + "_output" + ".txt";
	cout << "writefilename = " << writefilename << endl;

	//clear file
	ofstream out;
	out.open(writefilename.c_str());
	out.close();

	//open file and update
	out.open(writefilename, fstream::out | fstream::app);

	string readfilename = filename;
	ifstream in;
	in.open(readfilename.c_str(), std::ios::in);
	if (!in.is_open()) {
		cout << "open readFile Failded." << endl;
		return;
	}
	cout << "open read xml File success." << endl;

	string strline;
	while (getline(in, strline))
	{
		DBG("strline = %s", strline.c_str());
		if (string::npos != strline.find("<initSettings>") || string::npos != strline.find("<resSettings>") || string::npos != strline.find("<memoryMap>"))
		{
			DBG("strline = %s", strline.c_str());
			while (getline(in, strline))
			{
				if (string::npos != strline.find("<regSetting>"))
				{
					DBG("strline = %s", strline.c_str());
					while (getline(in, strline))
					{
						int startpos = 0;
						int endpos = 0;
						int len = strlen("<registerAddr>");
						if ((startpos = strline.find("<registerAddr>")) != string::npos)
						{
							cout << "strline = " << strline << endl;
							endpos = strline.find("</registerAddr>");
							string substring = strline.substr(startpos + len, endpos - startpos - len);
							//cout << "addr = " << substring << endl;
							write_string_to_file_append(out, substring);
							write_string_to_file_append(out, ",");
						}
						else if ((startpos = strline.find("<registerData>")) != string::npos)
						{
							cout << "strline = " << strline << endl;
							endpos = strline.find("</registerData>");
							string substring = strline.substr(startpos + len, endpos - startpos - len);
							//cout << "data = " << substring.c_str() << endl;
							write_string_to_file_append(out, substring);
							write_string_to_file_append(out, "\n");
						}
					}
				}
				else if (string::npos != strline.find("</regSetting>"))
				{
					cout << "got to </regSetting>" << endl;
					break;
				}
			}
		}
		else if (string::npos != strline.find("</initSettings>") || string::npos != strline.find("</resSettings>") || string::npos != strline.find("</memoryMap>"))
		{
			cout << "got to </memoryMap> or </initSettings> or </resSettings>" << endl;
			break;
		}
	}
	in.close();
	out.flush();
	out.close();
}

void read_from_excel_file(const char *readFileName)
{

	FILE *fp = NULL;
	int i, j;
	float da[6][5] = { 0 };
	int ret = fopen_s(&fp, readFileName, "rb");
	if (ret != 0)
	{
		DBG("open file %s fail %d\n", readFileName, ret);
		return;
	}
	else
		DBG("open file %s success %d\n", readFileName, ret);

	fseek(fp, 0, SEEK_SET);  // fp指针指向文件头部

	for (i = 0; i < 6; i++)
		for (j = 0; j < 5; j++)
		{
			fscanf_s(fp, "%f", &da[i][j]);
			fseek(fp, 5L, SEEK_CUR);   /*fp指针从当前位置向后移动*/
		}

	for (i = 0; i < 6; i++)
		printf("%f\t%f\t%f\t%f\t%f\t\n", da[i][0],
			da[i][1], da[i][2], da[i][3], da[i][4]);
	fclose(fp);

	char *file_content = NULL;
	int fileSize_r = read_from_file(readFileName, "rb", &file_content);

	char writeFileName[256] = "E:\\C++\\camera\\Debug\\test.csv";
	write_data_to_file(writeFileName, "wb", &file_content, fileSize_r);
}

void write_to_excel_file(const char *fileName)
{
	char chy[4] = { 'x' ,'a' ,'h','w' };
	int data[4] = { 1 , 3 , 6 ,9 };
	int i;
	FILE *fp = NULL;
	int ret = fopen_s(&fp, fileName, "w");
	if (ret != 0)
	{
		DBG("open file %s fail %d\n", fileName, ret);
		return;
	}
	else
		DBG("open file %s success %d\n", fileName, ret);

	for (i = 0; i < 4; i++)
	{
		fprintf(fp, "%c\t%d\n", chy[i], data[i]);
	}

	fclose(fp);
}

#include <direct.h>
void getTimeAndFileName(const char * nodeName, char *sufix, char **fileName)
{
	time_t timep;
	time(&timep);
	char curTime[256];
	strftime(curTime, sizeof(curTime), "%Y%m%d_%H%M%S", localtime(&timep));

	char cur_dir[64];
	_getcwd(cur_dir, sizeof(cur_dir));
	//snprintf(cur_dir, strlen("E:\\C++\\YUV\\"), "E:\\C++\\YUV\\");

	*fileName = (char *)malloc(256 * sizeof(char));
	if (*fileName == NULL)
	{
		DBG("malloc fail for fileName, return");
		return;
	}
	sprintf(*fileName, "%s\\%s_%s.%s", cur_dir, curTime, nodeName, sufix);
	DBG("fileName = %s cur_dir = %s", *fileName, cur_dir);

	time_t times = time(NULL);
	struct tm* utcTime = gmtime(&times);
	char timeStr[64];
	int timeStrLen = sprintf(timeStr, "%04d%02d%02dT%02d%02d%02dZ",
		utcTime->tm_year + 1900, utcTime->tm_mon + 1, utcTime->tm_mday, utcTime->tm_hour, utcTime->tm_min, utcTime->tm_sec);
	time_t tt = time(NULL);
	tm* t = localtime(&tt);
	DBG("Current Time : %d-%02d-%02d %02d:%02d:%02d\n",
		t->tm_year + 1900,
		t->tm_mon + 1,
		t->tm_mday,
		t->tm_hour,
		t->tm_min,
		t->tm_sec);

	return;
}

int read_from_file(const char *cFileName, const char * mode, char** Content)
{
	DBG("reading file from %s with mode %s ..", cFileName, mode);
	FILE* pFile = fopen(cFileName, mode);
	int fileSize = 0;
	if (pFile == NULL)
	{
		fprintf(stderr, "error: not found: %s\n", cFileName);
		return 0;
	}
	else if (pFile != NULL)
	{
		fseek(pFile, 0L, SEEK_END);
		fileSize = ftell(pFile);
		DBG("fileSize = %d", fileSize);

		fseek(pFile, 0L, SEEK_SET);
		char* pData = (char*)malloc(fileSize);
		memset(pData, 0, fileSize);
		int real_read_bytes = 0;
		int block_size = 1024;
		if (fileSize > block_size)
		{
			int read_byte = 0;
			while (!feof(pFile) || real_read_bytes < fileSize)
			{
				read_byte = fread(pData + real_read_bytes, 1, block_size, pFile);
				real_read_bytes += read_byte;
				//DBG("pData=%p real_read_bytes=%d read_byte=%d", pData, real_read_bytes, read_byte);
				if (read_byte < block_size) // 每次读 block_size 字节，而且返回的是字节数，同时也是块数，当返回的块数小于最大块数时，表明读到最后一块
				{
					int err = ferror(pFile);
					int is_feof = feof(pFile);
					DBG("err = %d is_feof = %d", err, is_feof);
					break; //用文本文件方式读取二进制文件会有 bug，没有读到文件末尾但读出来的 size < block_size , err = 0 , feof = 16
				}
			}
		}
		else
		{
			real_read_bytes = fread(pData, 1, fileSize, pFile);
		}
		*Content = (char*)malloc(fileSize);
		memcpy(*Content, pData, fileSize);
		DBG("real_read_bytes=%d fileSize=%d", real_read_bytes, fileSize);

		fclose(pFile);
		free(pData);
	}
	DBG("reading file from %s .. done", cFileName);
	return fileSize;
}

void write_data_to_file(const char *cFileName, const char * mode, char** Content, int size)
{
	DBG("saving file to %s with mode %s size=%d ..", cFileName, mode, size);
	if (size == 0)
		return;
	FILE* file = fopen(cFileName, mode);
	int real_write_bytes = 0;
	if (strncmp("wb", mode, 2) == 0)
	{
		int block_size = 1024;
		if (size > block_size)
		{
			int write_byte = 0;
			int left_bytes = size;
			while (left_bytes > 0)
			{
				if (left_bytes > block_size)
				{
					write_byte = fwrite(*Content + real_write_bytes, 1, block_size, file);
				}
				else
				{
					write_byte = fwrite(*Content + real_write_bytes, 1, left_bytes, file);
				}
				left_bytes -= write_byte;
				real_write_bytes += write_byte;
				//DBG("left_bytes=%d real_write_bytes=%d write_byte=%d size=%d", left_bytes, real_write_bytes, write_byte, size);
			}
		}
		else
		{
			real_write_bytes = fwrite(*Content, 1, size, file);
		}
	}
	else
	{
		for (int i = 0; i < 64 * (size % 64); i += 64)
		{
			char *p = *Content;
			int k = 0;
			fprintf(file, "line %04d ", i);
			for (k = 0; k < 4; k++)
			{
				fprintf(file, "[0x%02x] [0x%02x] [0x%02x] [0x%02x] [0x%02x] [0x%02x] [0x%02x] [0x%02x] [0x%02x] [0x%02x] [0x%02x] [0x%02x] [0x%02x] [0x%02x] [0x%02x] [0x%02x]",
					*(p + 16 * k), *(p + 16 * k + 1), *(p + 16 * k + 2), *(p + 16 * k + 3), *(p + 16 * k + 4), *(p + 16 * k + 5), *(p + 16 * k + 6), *(p + 16 * k + 7), *(p + 16 * k + 8),
					*(p + 16 * k + 9), *(p + 16 * k + 10), *(p + 16 * k + 11), *(p + 16 * k + 12), *(p + 16 * k + 13), *(p + 16 * k + 14), *(p + 16 * k + 15));

				DBG("[0x%02x] [0x%02x] [0x%02x] [0x%02x] [0x%02x] [0x%02x] [0x%02x] [0x%02x] [0x%02x] [0x%02x] [0x%02x] [0x%02x] [0x%02x] [0x%02x] [0x%02x] [0x%02x]",
					*(p + 16 * k), *(p + 16 * k + 1), *(p + 16 * k + 2), *(p + 16 * k + 3), *(p + 16 * k + 4), *(p + 16 * k + 5), *(p + 16 * k + 6), *(p + 16 * k + 7), *(p + 16 * k + 8),
					*(p + 16 * k + 9), *(p + 16 * k + 10), *(p + 16 * k + 11), *(p + 16 * k + 12), *(p + 16 * k + 13), *(p + 16 * k + 14), *(p + 16 * k + 15));
			}
			fprintf(file, "%c", '\n');
			p += 64;
		}
	}
	fflush(file);
	fclose(file);
	DBG("saving file to %s .. done", cFileName);
}

void test_file_read_write()
{
	//read from txt file and save to a txt file
	const char src_filename[256] = "E:\\C++\\in.txt";
	const char dstFile_txt[256] = "E:\\C++\\out.txt";
	char *dataBuffer = NULL;

	int fileSize = read_from_file(src_filename, "r", &dataBuffer);
	write_data_to_file(dstFile_txt, "w", &dataBuffer, fileSize);

	//read data from yuv file and save to txt file, YUV 是二进制文件，使用文本文件方式读取再写入txt 文件会出现问题,所以下面要用 rb 和 wb
	const char src_filename_yuv[256] = "E:\\C++\\20200306_173656_node[IN]_w[4032]_h[3024].yuv";
	//char dstFile_yuv_in[256] = "E:\\C++\\CPP\\yuvin.txt";
	const char *nodeName = "IN";
	char *dstFile_yuv_in = NULL;
	getTimeAndFileName(nodeName, "txt", &dstFile_yuv_in);
	char *yuv_addr_r = NULL;
	int fileSize_r = read_from_file(src_filename_yuv, "rb", &yuv_addr_r);
	write_data_to_file(dstFile_yuv_in, "wb", &yuv_addr_r, fileSize_r);

	//read data from yuv file and save to txt file
	const char dst_Filename_yuv[256] = "E:\\C++\\20200306_173656_node[OUT]_w[4032]_h[3024].yuv";
	const char dstFile_yuv_out[256] = "E:\\C++\\yuvout.txt";
	char *yuv_addr_w = NULL;
	int fileSize_w = read_from_file(dst_Filename_yuv, "rb", &yuv_addr_w);
	write_data_to_file(dstFile_yuv_out, "wb", &yuv_addr_w, fileSize_w);
}

void check_bit(int data, int data_mask)
{
	int reg_data = 15;
	if (data == (reg_data & ~data_mask))
	{
		DBG("it is match ~data_mask = 0x%x\n", ~data_mask);
	}
	else
	{
		DBG("it does not match ~data_mask=0x%x\n", ~data_mask);
	}
}

unsigned int find_pos_substr(string src_str, string sub_str)
{
	unsigned int pos = -1;
	if ((pos = src_str.find(sub_str, 0)) != string::npos) //从字符串src_str 下标0开始，查找字符串sub_str ,返回sub_strs 在src_str 中的下标, 注意放括号
	{
		DBG("pos = %d src_str=%s sub_str=%s\n", pos, src_str.c_str(), sub_str.c_str());
	}
	return pos;
}

#include <sys/stat.h>
#include "io.h" //_finddata_t
bool is_file(string fileName)
{
	struct _stat buf;
	int result;
	result = _stat(fileName.c_str(), &buf);
	if (_S_IFDIR & buf.st_mode)
	{
		DBG("This is a folder name = %s \n", fileName.c_str());
		return false;
	}
	else if (_S_IFREG & buf.st_mode)
	{
		DBG("This is a file name = %s \n", fileName.c_str());
	}
	return true;
}

void getFiles(string path, string path2, vector<string> & files, bool add_folder)
{
	long hFile = 0;
	struct _finddata_t fileinfo;
	string p;

	if ((hFile = _findfirst(p.assign(path).append(path2).append("*").c_str(), &fileinfo)) != -1)
	{
		do {
			if ((fileinfo.attrib & _A_SUBDIR) && add_folder)
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					//getFiles(p.assign(path).append("\\"), p2.assign(fileinfo.name).append("\\"), file, true);
					//files.push_back(p2.assign(path2).append(fileinfo.name).append("\\"));
					files.push_back(p.assign(fileinfo.name).append("\\"));
				}
				else {
					files.push_back(p.assign(fileinfo.name));
					DBG("fileinfo.name = %s \n", fileinfo.name);
				}
			}
			else
			{
				files.push_back(p.assign(fileinfo.name));
				DBG("fileinfo.name = %s \n", fileinfo.name);
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

bool isNum(string str)
{
	for (unsigned int i = 0; i < str.size(); i++)
	{
		int tmp = (int)str[i];
		if (tmp >= 48 && tmp <= 57)
		{
			continue;
		}
		else
		{
			return false;
		}
	}
	return true;
}

bool is_a_command(string strline)
{
	int len = strline.length();
	if (len == 0)
		return false;
	for (int i = 0; i < len; i++)
	{
		//NULL, tab , 换行 , 回车, 空格 , 逗号
		//if(strline[i] == 0 || strline[i] == 9 || strline[i] == 10 || strline[i] == 13|| strline[i] == 32 || strline[i] == 44)
		if (strline[i] >= 'a' && strline[i] <= 'z' || strline[i] >= 'A' && strline[i] <= 'Z')
		{
			return false;
		}
	}
	return true;
}

#define PATH_SIZE 256
string rootPath;

string select_filepath(string srcPath, string file_string, char filename[256])
{
	string dstPath;
#if defined(HARDCODE)
	dstPath.assign(srcPath).append("\\data\\s5k3l6");
#else
	DBG("srcPath = %s file_string=%s length = %d \n", srcPath.c_str(), file_string.c_str(), file_string.length());
	int cmd = -1;
	int retry = 0;
	int isGoback = 0;
	string subFolder = "\\data\\";

	if (file_string.length() == 0)
	{
		dstPath.assign(srcPath).append(subFolder);
		DBG("subFolder %s : \n", subFolder.c_str());
	}
	else if (!strcmp("..", file_string.c_str()))
	{
		isGoback += 1;
		DBG("srcPath=%s file_string=%s isGoback = %d \n", srcPath.c_str(), file_string.c_str(), isGoback);
	}
	else if (file_string.length() == srcPath.length() && string::npos != file_string.find(srcPath)) // file_string include srcPath
	{
		DBG("Root path, can not go back srcPath = %s file_string = %s \n", srcPath.c_str(), file_string.c_str());
	}
	else
	{
		DBG("Input wrong srcPath=%s file_string=%s \n", srcPath.c_str(), file_string.c_str());
	}

	vector <string> file_list;
	getFiles(dstPath, "", file_list, true);
	printf("dstPath = %s has : \n", dstPath.c_str());
	int size = file_list.size();
	for (int i = 0; i < size; i++)
	{
		if (strcmp(".", file_list[i].c_str()) == 0)
		{
			printf("[%d]   %s  --- showing current folder\n", i, file_list[i].c_str());
		}
		else if (strcmp("..", file_list[i].c_str()) == 0)
		{
			if (srcPath.length() < file_string.length())
			{
				printf("[%d]   %s  --- going to parent path\n", i, file_list[i].c_str());
			}
			else if (srcPath.length() == file_string.length() || file_string.length() == 0)
			{
				printf("[%d]   %s  --- root dir, can not go back\n", i, file_list[i].c_str());
			}
		}
		else
		{
			printf("[%d]    %s\n", i, file_list[i].c_str());
		}
	}
	printf("\n");

	while ((cmd >= size || cmd < 0) && retry < 3)
	{
		DBG("Pick one file or folder : \n");
		cin >> cmd;
		retry++;
	}
	if (cmd < 0 || cmd >= size)
	{
		cout << "exit !" << endl;
		fprintf(stderr, "exit !\n");
		dstPath = ""; //NULL
	}
	DBG("isGoback = %d cmd = %d length_of_file = %d\n", isGoback, cmd, strlen(file_list[cmd].c_str()));

	if (isGoback == 0) // not go back
	{
		filename[0] = '\0';
	}

	if (strcmp(".", file_list[cmd].c_str()) != 0 && strcmp("..", file_list[cmd].c_str()))
	{
		strncpy_s(filename, sizeof(filename), file_list[cmd].c_str(), strlen(file_list[cmd].c_str()));
	}
	else if (strcmp("..", file_list[cmd].c_str()) == 0)
	{
		isGoback += 2;
		printf("srcPath = %s dstPath = %s file_string = %s isGoback = %d \n", srcPath.c_str(), dstPath.c_str(), file_string.c_str(), isGoback);
	}

	DBG("selected [%s] \n", file_list[cmd].c_str());

	if (isGoback == 3)
	{
		int pos = dstPath.find_last_of("\\", dstPath.length());
		int len = dstPath.length();
		DBG("pos = %d dstPath=%s dstPath.length = %d\n", pos, dstPath.c_str(), len);
		string subString = dstPath;
		for (int i = len - 1; i >= 0; i--)
		{
			DBG("subString[%d] = %c \n", i, subString[i]);
			if (subString[i] == '\\')
			{
				dstPath.erase(i, dstPath.length());
				DBG("dstPath = %s \n", dstPath.c_str());
				break;
			}
		}
	}
	else if (isGoback == 1)
	{
		DBG("This is root path, can not go back \n");
	}

#endif
	return dstPath;
}


/*
**********************
Format the camer sub module
**********************
*/
void format_function(string filePath, int cmd)
{
	int size = 0;
	vector<string> files;
	if (is_file(filePath))
	{
		getFiles(filePath, "", files, false);
		printf("Opening File Path : %s\n", filePath.c_str());
		bool isfile = is_file(filePath);
		DBG("let's start .... is file=%d cmd= %d \n", isfile, cmd);
		size = files.size();
		cout << "There are " << size << " files under \"" << filePath << "\":" << endl << endl;
	}
	else
	{
		files.push_back(filePath);
	}

#ifdef debug
	FILE * fd = NULL;
	fileout.assign(filePath).append("\out.txt");
	int stdout_fd = dup(fileno(stdout));
	if ((fd = freopen(fileout.c_str(), "w", stdout)) == NULL)
	{
		printf("IO err \n");
		return;
	}
	fprintf(stderr, "Let's start and save log to : %s\n", fileout.c_str());
#endif

	for (int i = 0; i < size; i++)
	{
		string filename = filePath + files[i].c_str();
		DBG("The %d filename = %s \n", i, filename.c_str());

		//change from txt to xml for sensor
		if (cmd == 1)
			if ((files[i].find("sensor", 0) != string::npos || files[i].find("global", 0) != string::npos)
				&& (files[i].find("txt", 0)) != string::npos && files[i].find("output", 0) == string::npos)
			{
				user_data.nRadio_select_mode = SENSOR_INIT;
				Format::txt2xml_for_modules(filename, user_data);
			}
		//change from xml to txt for sensor
		if (cmd == 2)
			if ((files[i].find("sensor", 0) != string::npos || files[i].find("global", 0) != string::npos)
				&& (files[i].find("xml", 0)) != string::npos && files[i].find("output", 0) != string::npos)
			{
				user_data.nRadio_select_mode = SENSOR_INIT;
				Format::xml2txt_for_modules(filename, user_data);
			}
		//change from txt to xml for sensor
		if (cmd == 3)
			if (files[i].find("eeprom", 0) != string::npos && (files[i].find("txt", 0)) != string::npos
				&& files[i].find("output", 0) == string::npos)
			{
				user_data.nRadio_select_mode = EEPROM;
				Format::txt2xml_for_modules(filename, user_data);
			}
		//change from xml to txt for sensor
		if (cmd == 4)
			if (files[i].find("eeprom", 0) != string::npos && (files[i].find("xml", 0)) != string::npos
				&& files[i].find("output", 0) != string::npos)
			{
				user_data.nRadio_select_mode = EEPROM;
				Format::xml2txt_for_modules(filename, user_data);
			}
	}
	DBG("done \n");
#ifdef debug
	fclose(stdout);
	if (fd)
	{
		fclose(fd);
	}
	if ((fd = fdopen(stdout_fd, "w")) == NULL)
	{
		printf("fdopen open\n");
		return;
	}
	DBG("done\n");
#endif
}

#include<direct.h>
//#include<unistd.h> /// This is for unix
void test_main()
{
	int retry = 0;
	fprintf(stderr, "Welcome!\n");
	string filePath_out;
	string path;
	char fileName[256] = { '\0' };
	char buffer[PATH_SIZE];
	_getcwd(buffer, PATH_SIZE);
	rootPath.assign(buffer); // save root path
	path.assign("");

#define TEST 1
#if TEST
	FILE *fd = NULL;
	errno_t fd_rt = 0;
	string fileout;
	string filePath = rootPath;
	fileout.assign(filePath).append("\\out.txt"); /// save log to out.txt file
	int stdout_fd = _dup(_fileno(stdout)); // If you use codeblock and compile fail, choice "Have gcc follow the 1999 ISO C language standard [-std = C99]"
	if ((fd_rt = freopen_s(&fd, fileout.c_str(), "w", stdout)) != 0)
	{
		printf("IO err \n");
	}
	fprintf(stderr, "Let's start and save log to : %s \n", fileout.c_str());

	///below just for other simple test
	//check_bit(15, 1);
	//find_added_partial_cl("old_ls.txt", "new_cls.txt");
	//test format();
#else
RETRY:
	int cmd = -1;
	filePath_out = select_filepath(rootPath, fileName);
	DBG("rootPath = %s fileName=%s filePath_out=%s \n", rootPath.c_str(), fileName.c_str(), filePath_out.c_str());
	if (0 == strncmp("NULL", filePath_out.c_str(), 4))
	{
		goto RETRY;
	}
	else {
		path.assign(filePath_out);
#if !defined(HARDCODE)		
		if (strcmp(".", fileName) != 0 && strcmp("..", fileName) != 0)
		{
			filePath_out.append(fileName);
			if (false == is_file(filePath_out))
			{
				path.append(fileName);
			}
		}
#endif 
		DBG("You will process %s : %s \n", is_file(filePath_out) == true ? "file" : "folder", filePath_out.c_str());
	}
	while ((cmd >= 7 || cmd < 0) && retry < 3)
	{
		cout << "Please select eeprom or sensor driver : " << endl;
		cout << "[1] Format sensor" << endl;
		cout << "[2] revert sensor to txt" << endl;
		cout << "[3] Format eeprom" << endl;
		cout << "[4] revert eeprom to txt" << endl;
		cout << "[5] Format gc5035 OTP setting file" << endl;
		cout << "[6] ... for go back" << endl;
		cout << "[0] exit !" << endl;
		cin >> cmd;
		retry++;
	}
	if (cmd < 0 || cmd >= 7)
	{
		cout << "exit !" << endl;
		fprintf(stderr, "exit !\n");
		goto EXIT;
	}
	else
	{
		if (0 != filePath_out.compare(""))
			format_function(filePath_out, cmd);
		goto RETRY;
	}
#endif 
}

