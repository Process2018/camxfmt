#include "stdafx.h"
#include "Format.h"
#include <stdio.h>
#include <iostream>
#include <fstream>

using namespace std;

#include "MessageLogContext.h"

USER_DATA user_data;

char m_userFormatString[64][256];

string default_eeprom_format_string[9] =
{
	{"	  <regSetting>"},
	{"		<slaveAddr>0xA2</slaveAddr>"},
	{"		<registerAddr>0x00</registerAddr>"},
	{"		<registerData>0x00</registerData>"},
	{"		<regAddrType rang=\"[1,4]\">2</regAddrType>"},
	{"		<regDataType rang=\"[1,4]\">2</regDataType>"},
	{"		<operation>READ</operation>"},
	{"		<delayUs>0</delayUs>"},
	{"	  </regSetting>"},
};

string default_eeprom_memoryMap_string[12] =
{
	{ "  <!--Sequence of register setting to configure the device -->" },
	{ "  <memoryMap>" },
	{ "  Contains: register address, register data, register address type, register data type, " },
	{ "  operation and delay in micro seconds" },
	{ "  element for slaveAddr" },
	{ "  element for registerAddr" },
	{ "  element for registerData" },
	{ "  element for regAddrType" },
	{ "  element for regDataType" },
	{ "  element for operation" },
	{ "  element for delayUs" },
	{ "  </memoryMap>" },
};

string default_sensor_regSetting_format[20] =
{
	{"	  <regSetting>"},
	{"		<registerAddr>0x0000</registerAddr>"},
	{"		<registerData>0x00</registerData>"},
	{"		<regAddrType rang=\"[1,4]\">2</regAddrType>"},
	{"		<regDataType rang=\"[1,4]\">2</regDataType>"},
	{"		<operation>WRITE</operation>"},
	{"		<delayUs>0</delayUs>"},
	{"	  </regSetting>"},
};

string default_sensor_init_string[7] =
{
	{ "  <!--Sequence of register setting to configure the device -->" },
	{ "  <initSettings>" },
	{ "  	<!--Specify which sensor version can support this setting-->" },
	{ "  	<sensorVersion>0</sensorVersion>" },
	{ "  	<initSetting>" },
	{ "  	</initSetting>" },
	{ "  </initSettings>" },
};

string default_sensor_res_string[7] =
{
	{ "  <!--Sequence of register setting to configure the device -->" },
	{ "  <resSettings>" },
	{ "  	<!--Register setting configuration" },
	{ "  		Contains: register address, register data, register address type, register data type" },
	{ "  		operation and delay in micro seconds -->" },
	{ "  </resSettings>" },
};

Format::Format()
{
}


Format::~Format()
{
}

int write_string_to_file_append(string &filename, const string str)
{
	CDBG("adding str = %s \n", str.c_str());

	ofstream out;
	out.open(filename, fstream::out | fstream::app);
	out << str;
	out.close();
	return 0;
}

//how to skip string except addr like 0xAAAA ?

void Format::txt2xml_sensor(string filename, USER_DATA user_data) {
	printf_err(filename);
	//DEBG("start processing for %s",filename.c_str());//good

	int pos_of_dog = find_pos_substr(filename, ".");
	string str_sub1 = filename.substr(0, pos_of_dog);
	string str_sub2 = filename.substr(pos_of_dog, strlen(filename.c_str()));
	CDBG("str_sub1 = %s str_sub2 = %s \n", str_sub1.c_str(), str_sub2.c_str());
	string writefilename = str_sub1 + "_ouput" + ".xml";
	CDBG("writefilename = %s \n", writefilename.c_str());

	//clear file
	ofstream out;
	out.open(writefilename.c_str());
	out.close();

	for (int i = 0; i < 5; i++)
	{
		if (user_data.nRadio_select_mode == SENSOR_INIT)
		{
			write_string_to_file_append(writefilename, default_sensor_init_string[i]);
			write_string_to_file_append(writefilename, "\n");
		}
		else if (user_data.nRadio_select_mode == SENSOR_RES) {
			write_string_to_file_append(writefilename, default_sensor_res_string[i]);
			write_string_to_file_append(writefilename, "\n");
		}
		else {
			CDBG("There is nothing I can do, go back");
			return;
		}
	}

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
		int pos1 = 0;
		int pos2 = 0;
		int pos3 = 0;
		string str_sub1;
		string str_sub2;
		string str_sub = strline.substr(0, 2);
		line_cnt++;
		int flag = 1;
		CDBG("strline = %s str_sub = %s\n", strline.c_str(), str_sub.c_str());
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
					CDBG("This line has some problem, goto skip pos1=%d pos2=%d", pos1, pos2);
					goto SKIP;
				}
			}
			pos3 = strline.find_last_of("x");
			CDBG("pos1=%d pos2=%d pos3=%d \n", pos1, pos2, pos3);
			str_sub1 = strline.substr(pos1 - 1, pos2);
			str_sub2 = strline.substr(pos3 - 1, len);
			flag = 5;
			CDBG("str_sub1=%s str_sub2=%s str_sub=%s\n", str_sub1.c_str(), str_sub2.c_str(), str_sub.c_str());
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
					CDBG("This line has some problem, goto skip pos1=%d pos2=%d", pos1, pos2);
					goto SKIP;
				}
			}
			pos3 = strline.find_last_of("x");
			CDBG("pos1=%d pos2=%d pos3=%d \n", pos1, pos2, pos3);
			str_sub1 = strline.substr(pos1 - 1, pos2);
			str_sub2 = strline.substr(pos3 - 1, len);
			flag = 5;
			CDBG("str_sub1=%s str_sub2=%s str_sub=%s\n", str_sub1.c_str(), str_sub2.c_str(), str_sub.c_str());
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
					CDBG("This line has some problem with elem %d", i);
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
			CDBG("str_sub1=%s str_sub2=%s str_sub=%s \n", str_sub1.c_str(), str_sub2.c_str(), str_sub.c_str());
		}

		if (flag == 1)
		{
			cout << "There is no data in this line, check next line" << endl;
			continue;
		}
		pos1 = (default_sensor_regSetting_format[1]).find_first_of(">");
		pos2 = (default_sensor_regSetting_format[1]).find("<", pos1);
		default_sensor_regSetting_format[1] = (default_sensor_regSetting_format[1]).replace(pos1 + 1, pos2 - pos1 - 1, str_sub1);

		pos1 = (default_sensor_regSetting_format[2]).find_first_of(">");
		pos2 = (default_sensor_regSetting_format[2]).find("<", pos1);
		default_sensor_regSetting_format[2] = (default_sensor_regSetting_format[2]).replace(pos1 + 1, pos2 - pos1 - 1, str_sub2);

		for (int i = 0; i < 8; i++)
		{
			write_string_to_file_append(writefilename, default_sensor_regSetting_format[i]);
			write_string_to_file_append(writefilename, "\n");
		}
		goto ADD;
	SKIP:
		CDBG("skip strline = %s \n", strline.c_str());
		continue;

	ADD:
		CDBG("add strline = %s \n", strline.c_str());
		continue;
	}
	if (user_data.nRadio_select_mode == SENSOR_INIT) {
		write_string_to_file_append(writefilename, default_sensor_init_string[5]);
		write_string_to_file_append(writefilename, string("\n"));
		write_string_to_file_append(writefilename, default_sensor_init_string[6]);
	}
	else
	{
		write_string_to_file_append(writefilename, default_sensor_res_string[5]);
	}
	cout << "cout:txt2xml process " << line_cnt << " line for sensor done " << filename.c_str() << endl;
	fprintf(stderr, "fprintf: txt2xml process %d line for sensor done %s\n", line_cnt, filename.c_str());
	//DEBG("process for %s done",filename.c_str());//good	
}

void Format::xml2txt_sensor(string filename, USER_DATA user_data) {
	printf_err(filename);
	//DEBG(filename.c_str());//good
	//DEBG("start processing for %s",filename.c_str());//good

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
		if (string::npos != strline.find("<initSetting>") || string::npos != strline.find("<resSetting>"))
		{
			cout << strline << endl;
			while (getline(in, strline))
			{
				if (string::npos != strline.find("<regSetting>"))
				{
					//cout << strline << endl;
					while (getline(in, strline))
					{
						int startpos = 0;
						int endpos = 0;
						int len = strlen("<registerAddr>");
						if ((startpos = strline.find("<registerAddr>")) != string::npos)
						{
							//cout << "strline = " << strline <<endl;
							endpos = strline.find("</registerAddr>");
							string substring = strline.substr(startpos + len, endpos - startpos - len);
							//cout << "addr = " << substring << endl;
							write_string_to_file_append(writefilename, substring);
							write_string_to_file_append(writefilename, string(","));
						}
						else if ((startpos = strline.find("<registerData>")) != string::npos)
						{
							//cout << "strline = " << strline <, endl;
							endpos = strline.find("</registerData>");
							string substring = strline.substr(startpos + len, endpos - startpos - len);
							//cout << "data = " << substring.c_str() << endl;
							write_string_to_file_append(writefilename, substring);
							write_string_to_file_append(writefilename, string("\n"));
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
	}
	//DEBG("process for %s done",filename.c_str());//good	
}

void Format::txt2xml_eeprom(string filename, USER_DATA user_data)
{
	printf_err(filename.c_str());
	//DEBG("user_data.nRadio_select_mode = %d %s\n", user_data.nRadio_select_mode, filename.c_str());//some problem
	//DEBG("start processing for %s",filename.c_str());//good	

	int pos_of_dog = find_pos_substr(filename, ".");
	string str_sub1 = filename.substr(0, pos_of_dog);
	string str_sub2 = filename.substr(pos_of_dog, strlen(filename.c_str()));
	cout << "str_sub = " << str_sub1 << " str_sub2 = " << str_sub2 << endl;
	string writefilename = str_sub1 + "_output" + ".xml";
	cout << "writefilename = " << writefilename << endl;

	//clear file
	ofstream out;
	out.open(writefilename.c_str());
	out.close();

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
		cout << strline.c_str() << endl;
		int num_block = 0;
		if (strncmp("block", strline.c_str(), 5) == 0)
		{
			num_block++;
		}
		else {
			cout << strline << endl;
			int pos1 = 0;
			int pos2 = 0;
			int pos3 = 0;
			string str_sub1;
			string str_sub2;
			string str_sub = strline.substr(0, 2);
			line_cnt++;
			int flag = 1;
			int len = strline.length(); // or strline.size()

			if (strcmp(str_sub.c_str(), "0x") == 0)// 0x1234, 0x1234 or 0x1234 0x1234 case
			{
				pos1 = strline.find_first_of("x");
				pos2 = strline.find_first_of(",");//if there is no ',', check if there is empty space
				if (pos2 == string::npos)
				{
					if ((pos2 = strline.find_first_of(" ")) == string::npos)
						pos2 = strline.find_first_of("	");
					if (pos2 == string::npos)

					{
						CDBG("This line has some problem, goto skip pos1=%d pos2=%d", pos1, pos2);
						goto SKIP;
					}
				}
				pos3 = strline.find_last_of("x");
				CDBG("pos1=%d pos2=%d pos3=%d \n", pos1, pos2, pos3);
				str_sub1 = strline.substr(pos1 - 1, pos2);
				str_sub2 = strline.substr(pos3 - 1, len);
				flag = 5;
				CDBG("str_sub1=%s str_sub2=%s str_sub=%s\n", str_sub1.c_str(), str_sub2.c_str(), str_sub.c_str());
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
						CDBG("This line has some problem with elem %d", i);
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
				CDBG("str_sub1=%s str_sub2=%s str_sub=%s \n", str_sub1.c_str(), str_sub2.c_str(), str_sub.c_str());
			}

			if (flag == 1)
			{
				cout << "There is no data in this line, check next line" << endl;
				continue;
			}
			pos1 = (default_eeprom_format_string[2]).find_first_of(">");
			pos2 = (default_eeprom_format_string[2]).find("<", pos1);
			default_eeprom_format_string[2] = (default_eeprom_format_string[2]).replace(pos1 + 1, pos2 - pos1 - 1, str_sub1);

			pos1 = (default_eeprom_format_string[3]).find_first_of(">");
			pos2 = (default_eeprom_format_string[3]).find("<", pos1);
			default_eeprom_format_string[3] = (default_eeprom_format_string[3]).replace(pos1 + 1, pos2 - pos1 - 1, str_sub2);

			for (int i = 0; i < 9; i++)
			{
				write_string_to_file_append(writefilename, default_eeprom_format_string[i]);
				write_string_to_file_append(writefilename, default_eeprom_format_string[i]);
			}

			goto ADD;
		}
	SKIP:
		CDBG("skip strline = %s \n", strline.c_str());
		continue;

	ADD:
		CDBG("add strline = %s \n", strline.c_str());
		continue;
	}

	write_string_to_file_append(writefilename, default_eeprom_memoryMap_string[12]);
	cout << "txt2xml process " << line_cnt << " line for eeprom done " << filename.c_str() << endl;
	fprintf(stderr, "txt2xml process %d line for eeprom done %s\n", line_cnt, filename.c_str());
	//DEBG("process for %s done",filename.c_str());//good	
}
void Format::xml2txt_eeprom(string filename, USER_DATA user_data) {
	printf_err(filename.c_str());
	//DEBG("user_data.nRadio_select_mode = %d %s\n", user_data.nRadio_select_mode , filename.c_str());//some problem
	//DEBG("start processing for %s ",filename.c_str());//good

	int pos_of_dog = find_pos_substr(filename, ".");
	string str_sub1 = filename.substr(0, pos_of_dog);
	string str_sub2 = filename.substr(pos_of_dog, strlen(filename.c_str()));
	cout << "str_sub1 = " << str_sub1 << " str_sub2 = " << str_sub2 << endl;

	string writefilename = str_sub1 + "_output" + ".txt";
	cout << "writefilename = " << writefilename << endl;

	//clear file
	ofstream out;
	out.open(writefilename.c_str());
	out.close();

	string readfilename = filename;
	ifstream in;
	in.open(readfilename.c_str(), std::ios::in);
	if (!in.is_open())
	{
		cout << "open readFile Failed." << endl;
		return;
	}

	cout << "open read xml File success." << endl;

	string strline;
	while (getline(in, strline))
	{
		if (string::npos != strline.find("<memoryMap>"))
		{
			while (getline(in, strline))
			{
				if (string::npos != strline.find("<regSetting>"))
				{
					while (getline(in, strline))
					{
						int startpos = 0;
						int endpos = 0;
						int len = strlen("<registerAddr>");
						if ((startpos = strline.find("<registerAddr>")) != string::npos)
						{
							//cout << "strline = " << strline <<endl;
							endpos = strline.find("</registerAddr>");
							string substring = strline.substr(startpos + len, endpos - startpos - len);
							//cout << "addr = " << substring << endl;
							write_string_to_file_append(writefilename, substring);
							write_string_to_file_append(writefilename, string(","));
						}
						else if ((startpos = strline.find("<registerData>")) != string::npos)
						{
							//cout << "strline = " << strline <, endl;
							endpos = strline.find("</registerData>");
							string substring = strline.substr(startpos + len, endpos - startpos - len);
							//cout << "data = " << substring.c_str() << endl;
							write_string_to_file_append(writefilename, substring);
							write_string_to_file_append(writefilename, string("\n"));
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
		else if (string::npos != strline.find("</memoryMap>"))
		{
			cout << "got to </memoryMap>" << endl;
			break;
		}
	}
	//DEBG("process for %s done",filename.c_str());//good	
}

void check_bit(int data, int data_mask)
{
	int reg_data = 15;
	if (data == (reg_data & ~data_mask))
	{
		CDBG("it is match ~data_mask = 0x%x\n", ~data_mask);
	}
	else
	{
		CDBG("it does not match ~data_mask=0x%x\n", ~data_mask);
	}
}

unsigned int find_pos_substr(string src_str, string sub_str)
{
	unsigned int pos = -1;
	if ((pos = src_str.find(sub_str, 0)) != string::npos) //从字符串src_str 下标0开始，查找字符串sub_str ,返回sub_strs 在src_str 中的下标, 注意放括号
	{
		CDBG("pos = %d src_str=%s sub_str=%s\n", pos, src_str.c_str(), sub_str.c_str());
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
		CDBG("This is a folder name = %s \n", fileName.c_str());
		return false;
	}
	else if (_S_IFREG & buf.st_mode)
	{
		CDBG("This is a file name = %s \n", fileName.c_str());
		return true;
	}
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
					CDBG("fileinfo.name = %s \n", fileinfo.name);
				}
			}
			else
			{
				files.push_back(p.assign(fileinfo.name));
				CDBG("fileinfo.name = %s \n", fileinfo.name);
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

bool isNum(string str)
{
	for (int i = 0; i < str.size(); i++)
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
		//NULL tab 鎹㈣ 鍥炶溅 绌烘牸 閫楀彿
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
	CDBG("srcPath = %s file_string=%s length = %d \n", srcPath.c_str(), file_string.c_str(), file_string.length());
	int cmd = -1;
	int retry = 0;
	int isGoback = 0;
	string subFolder = "\\data\\";

	if (file_string.length() == 0)
	{
		dstPath.assign(srcPath).append(subFolder);
		CDBG("subFolder %s : \n", subFolder.c_str());
	}
	else if (!strcmp("..", file_string.c_str()))
	{
		isGoback += 1;
		CDBG("srcPath=%s file_string=%s isGoback = %d \n", srcPath.c_str(), file_string.c_str(), isGoback);
	}
	else if (file_string.length() == srcPath.length() && string::npos != file_string.find(srcPath)) // file_string include srcPath
	{
		CDBG("Root path, can not go back srcPath = %s file_string = %s \n", srcPath.c_str(), file_string.c_str());
	}
	else
	{
		CDBG("Input wrong srcPath=%s file_string=%s \n", srcPath.c_str(), file_string.c_str());
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
		CDBG("Pick one file or folder : \n");
		cin >> cmd;
		retry++;
	}
	if (cmd < 0 || cmd >= size)
	{
		cout << "exit !" << endl;
		fprintf(stderr, "exit !\n");
		dstPath = ""; //NULL
	}
	CDBG("isGoback = %d cmd = %d length_of_file = %d\n", isGoback, cmd, strlen(file_list[cmd].c_str()));

	if (isGoback == 0) // not go back
	{
		filename[0] = '\0';
	}

	if (strcmp(".", file_list[cmd].c_str()) != 0 && strcmp("..", file_list[cmd].c_str()))
	{
		strncpy(filename, file_list[cmd].c_str(), strlen(file_list[cmd].c_str()));
	}
	else if (strcmp("..", file_list[cmd].c_str()) == 0)
	{
		isGoback += 2;
		printf("srcPath = %s dstPath = %s file_string = %s isGoback = %d \n", srcPath.c_str(), dstPath.c_str(), file_string.c_str(), isGoback);
	}

	CDBG("selected [%s] \n", file_list[cmd].c_str());

	if (isGoback == 3)
	{
		int pos = dstPath.find_last_of("\\", dstPath.length());
		int len = dstPath.length();
		CDBG("pos = %d dstPath=%s dstPath.length = %d\n", pos, dstPath.c_str(), len);
		string subString;
		for (int i = len - 1; i >= 0; i--)
		{
			CDBG("subString[%d] = %c \n", i, subString);
			if (subString[i] == '\\')
			{
				dstPath.erase(i, dstPath.length());
				CDBG("dstPath = %s \n", dstPath.c_str());
				break;
			}
		}
	}
	else if (isGoback == 1)
	{
		CDBG("This is root path, can not go back \n");
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
		CDBG("let's start .... is file=%d cmd= %d \n", isfile, cmd);
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
		CDBG("The %d filename = %s \n", i, filename.c_str());

		//change from txt to xml for sensor
		if (cmd == 1)
			if ((files[i].find("sensor", 0) != string::npos || files[i].find("global", 0) != string::npos)
				&& (files[i].find("txt", 0)) != string::npos && files[i].find("output", 0) == string::npos)
			{
				user_data.nRadio_select_mode = SENSOR_INIT;
				Format::txt2xml_sensor(filename, user_data);
			}
		//change from xml to txt for sensor
		if (cmd == 2)
			if ((files[i].find("sensor", 0) != string::npos || files[i].find("global", 0) != string::npos)
				&& (files[i].find("xml", 0)) != string::npos && files[i].find("output", 0) != string::npos)
			{
				user_data.nRadio_select_mode = SENSOR_INIT;
				Format::xml2txt_sensor(filename, user_data);
			}
		//change from txt to xml for sensor
		if (cmd == 3)
			if (files[i].find("eeprom", 0) != string::npos && (files[i].find("txt", 0)) != string::npos
				&& files[i].find("output", 0) == string::npos)
			{
				user_data.nRadio_select_mode = EEPROM;
				Format::txt2xml_eeprom(filename, user_data);
			}
		//change from xml to txt for sensor
		if (cmd == 4)
			if (files[i].find("eeprom", 0) != string::npos && (files[i].find("xml", 0)) != string::npos
				&& files[i].find("output", 0) != string::npos)
			{
				user_data.nRadio_select_mode = EEPROM;
				Format::xml2txt_eeprom(filename, user_data);
			}
	}
	CDBG("done \n");
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
	CDBG("done\n");
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
	getcwd(buffer, PATH_SIZE);
	rootPath.assign(buffer); // save root path
	path.assign("");

#define TEST 1
#if TEST
	FILE *fd = NULL;
	string fileout;
	string filePath = rootPath;
	fileout.assign(filePath).append("\\out.txt"); /// save log to out.txt file
	int stdout_fd = dup(fileno(stdout)); // If you use codeblock and compile fail, choice "Have gcc follow the 1999 ISO C language standard [-std = C99]"
	if ((fd = freopen(fileout.c_str(), "w", stdout)) == NULL)
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
	CDBG("rootPath = %s fileName=%s filePath_out=%s \n", rootPath.c_str(), fileName.c_str(), filePath_out.c_str());
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
		CDBG("You will process %s : %s \n", is_file(filePath_out) == true ? "file" : "folder", filePath_out.c_str());
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









