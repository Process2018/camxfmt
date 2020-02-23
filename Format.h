#pragma once
#ifndef __FORMATE_H__
#define __FORMATE_H__
#include <string> //注意这里不是string.h string.h是C字符串头文件
#include <vector>
using namespace std;

enum FORMAT_TYPE {
	SENSOR_INIT,
	SENSOR_RES,
	EEPROM,
	ACTUATOR,
};

typedef struct USER_DATA_T{
	int nRadio_select_mode;
	int nRadio_addr_type;
	int nRadio_data_type;
	int nUserFormatStringLine;	
}USER_DATA;

class Format
{
public:
	Format();
	~Format();

public:
	static void txt2xml_sensor(string filename, USER_DATA user_data);
	static void xml2txt_sensor(string filename, USER_DATA user_data);
	static void txt2xml_eeprom(string filename, USER_DATA user_data);
	static void xml2txt_eeprom(string filename, USER_DATA user_data);
};

unsigned int find_pos_substr(string src_str, string sub_str);
void format_function(string filePath, int cmd);
void test_main();
int write_string_to_file_append(string &filename, const string str);
void check_bit(int data, int data_mask);
unsigned int find_pos_substr(string src_str, string sub_str);
bool is_file(string fileName);
void getFiles(string path, string path2, vector<string> & files, bool add_folder);
bool isNum(string str);
bool is_a_command(string strline);

#endif