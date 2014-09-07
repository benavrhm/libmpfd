/**
    This file is distributed under GPLv3 licence
    Original Author: Gorelov Grigory (gorelov@grigory.info)
    Current maintainer: Jonathan Ben-Avraham <yba@tkos.co.il>

    Contacts and other info are on the WEB page:  grigory.info/MPFDParser
 */

#ifndef _PARSER_H
#define	_PARSER_H

#include <iostream>
#include <string>
#include <map>
#include <libmpfd/Exception.h>
#include <libmpfd/Field.h>
#include <string.h>
#include <stdlib.h>

namespace MPFD
{

/**
*/
class Parser
{
public:
	static const int StoreUploadedFilesInFilesystem = 1,
		StoreUploadedFilesInMemory = 2;

	Parser(
		const char *content_type,
		const char *file_dir="/tmp",
		int max_collector_length=16*1024*1024
	);

	~Parser();

	void AcceptSomeData(const char *data, const long length);
	void SetMaxCollectedDataLength(long max);
	void SetTempDirForFileUpload(std::string dir);
	std::string GetTempDirForFileUpload();
	void SetUploadedFilesStorage(int where);

	std::map<std::string, Field *> GetFieldsMap();
	Field * GetField(std::string Name);

private:
	int WhereToStoreUploadedFiles;

	std::map<std::string, Field *> Fields;

	std::string TempDirForFileUpload;
	int CurrentStatus;

	// Work statuses
	static int const Status_LookingForStartingBoundary = 1;
	static int const Status_ProcessingHeaders = 2;
	static int const Status_ProcessingContentOfTheField = 3;

	std::string Boundary;
	std::string ProcessingFieldName;
	bool _HeadersOfTheFieldAreProcessed;
	long ContentLength;
	char *DataCollector;
	long DataCollectorLength, MaxDataCollectorLength;

	void SetContentType(const std::string type);
	bool FindStartingBoundaryAndTruncData();
	void _ProcessData();
	void _ParseHeaders(std::string headers);
	bool FindContentType();
	bool WaitForHeadersEndAndParseThem();
	void TruncateDataCollectorFromTheBeginning(long n);
	long BoundaryPositionInDataCollector();
	bool ProcessContentOfTheField();
};
}

#endif	/* _PARSER_H */

