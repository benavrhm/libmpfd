/**
    This file is distributed under GPLv3 licence
    Original author: Gorelov Grigory (gorelov@grigory.info)
    Refactored by Jonathan Ben-Avraham <yba@tkos.co.il>

    Contacts and other info are on the WEB page:  grigory.info/MPFDParser
 */

#include "Field.h"
#include "Parser.h"

/**
 */
MPFD::Field::Field()
{
	type = 0;
	FieldContent = NULL;

	FieldContentLength = 0;
}


/**
 */
MPFD::Field::~Field()
{
	if (FieldContent)
		delete FieldContent;

	if ((type == FileType) && file.is_open()) {
		file.close();
		remove((TempDir + "/" + TempFile).c_str());
	}
}


/**
 */
void MPFD::Field::SetType(int type)
{
	if ((type != TextType) && (type != FileType))
		throw MPFD::Exception("Trying to set type of field, but type "
			"is not Text or File type.");

	this->type = type;
}


/**
 */
int MPFD::Field::GetType()
{
	if (type <= 0)
		throw MPFD::Exception("Trying to get type of field, but no "
			"type was set.");

	return type;
}


/**
 */
void MPFD::Field::AcceptSomeData(char *data, long length)
{
	if (type == TextType) {
		if (FieldContent == NULL) {
			FieldContent = new char[length + 1];
		} else {
			FieldContent = (char*) realloc(FieldContent,
				FieldContentLength + length + 1);
		}

		memcpy(FieldContent + FieldContentLength, data, length);
		FieldContentLength += length;

		FieldContent[FieldContentLength] = 0;
		return;
	}

	if ((type == FileType) && (WhereToStoreUploadedFiles == 
		Parser::StoreUploadedFilesInFilesystem)) {

		if (TempDir.length() <= 0)
			throw MPFD::Exception("Trying to AcceptSomeData for a "
				"file but no TempDir is set.");

		if ( ! file.is_open()) {
			int i = 1;
			std::ifstream testfile;
			std::string tempfile;
			do {
				if (testfile.is_open())
					testfile.close();

				std::stringstream ss;
				ss << "MPFD_Temp_" << i;
				TempFile = ss.str();

				tempfile = TempDir + "/" + TempFile;

				testfile.open(tempfile.c_str(), std::ios::in);
				i++;
			} while (testfile.is_open());

			file.open(tempfile.c_str(), std::ios::out |
				std::ios::binary | std::ios_base::trunc);
		}

		if ( ! file.is_open())
			throw Exception(std::string("Cannot write to file ") +
				TempDir + "/" + TempFile);

		file.write(data, length);
		file.flush();
		return;
	}

	if (type == FileType) { // files are stored in memory
		if (FieldContent == NULL) {
			FieldContent = new char[length];
		} else {
			FieldContent = (char*) realloc(FieldContent,
				FieldContentLength + length);
		}
		memcpy(FieldContent + FieldContentLength, data, length);
		FieldContentLength += length;
		return;
	}

	throw MPFD::Exception("Trying to AcceptSomeData but type was neither "
		"Text nor File.");
}


/**
 */
void MPFD::Field::SetTempDir(std::string dir)
{
	TempDir = dir;
}


/**
 */
unsigned long MPFD::Field::GetFileContentSize()
{
	if (type == 0)
		throw MPFD::Exception("Trying to get file content size, but no "
			"type was set.");

	if (type != FileType)
		throw MPFD::Exception("Trying to get file content "
			"size, but the type is not file.");

	if (WhereToStoreUploadedFiles != Parser::StoreUploadedFilesInMemory)
		throw MPFD::Exception("Trying to get file content size, but "
			"uploaded files are stored in filesystem.");

	return FieldContentLength;
}


/**
 */
char * MPFD::Field::GetFileContent()
{
	if (type == 0)
		throw MPFD::Exception("Trying to get file content, but no type "
			"was set.");

	if (type != FileType)
		throw MPFD::Exception("Trying to get file content, but the "
			"type is not file.");

	if (WhereToStoreUploadedFiles != Parser::StoreUploadedFilesInMemory)
		throw MPFD::Exception("Trying to get file content, but "
			"uploaded files are stored in filesystem.");

	return FieldContent;
}


/**
 */
std::string MPFD::Field::GetTextTypeContent()
{
	if (type == 0)
		throw MPFD::Exception("Trying to get text content of the "
			"field, but no type was set.");

	if (type != TextType)
		throw MPFD::Exception("Trying to get content of the field, but "
			"the type is not text.");

	if (FieldContent == NULL)
		return std::string();

	return std::string(FieldContent);
}


/**
 */
std::string MPFD::Field::GetTempFileName()
{
	if (type == 0)
		throw MPFD::Exception("Trying to get file temp name, but no "
			"type was set.");

	if (type != FileType)
		throw MPFD::Exception("Trying to get file temp name, but the "
			"type is not file.");

	if (WhereToStoreUploadedFiles != Parser::StoreUploadedFilesInFilesystem)
		throw MPFD::Exception("Trying to get file temp name, but "
			"uplaoded files are stored in memory.");

	return std::string(TempDir + "/" + TempFile);
}


/**
 */
std::string MPFD::Field::GetFileName()
{
	if (type == 0)
		throw MPFD::Exception("Trying to get file name, but no type "
			"was set.");

	if (type != FileType)
		throw MPFD::Exception("Trying to get file name, but the type "
			"is not file.");

	return FileName;
}


/**
 */
void MPFD::Field::SetFileName(std::string name)
{
	FileName = name;
}


/**
 */
void MPFD::Field::SetUploadedFilesStorage(int where)
{
	WhereToStoreUploadedFiles = where;
}


/**
 */
void MPFD::Field::SetFileContentType(std::string type)
{
	FileContentType = type;
}


/**
 */
std::string MPFD::Field::GetFileMimeType()
{
	if (type == 0)
		throw MPFD::Exception("Trying to get mime type of file, but no "
			"type was set.");

	if (type != FileType)
		throw MPFD::Exception("Trying to get mime type of the field, "
			"but the type is not File.");

	return std::string(FileContentType);
}
