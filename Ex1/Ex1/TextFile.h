#pragma once
#ifndef TEXTFILE_H
#define TEXTFILE_H
/// <summary>
/// Name: Hải Dương
/// Class TextFile
/// Purpose: Class file
/// </summary>
class TextFile
{
	protected:
		char file_name[100];
		int num_row_of_file;
	public:
		// gán giá trị cho filename
		void setFileName(char* fileName);
		// đọc n dòng từ cuối file
		virtual void tailFile(int numRow) = 0;
		bool checkFile();
};
#endif // DEBUG


