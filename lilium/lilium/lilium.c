#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <windows.h>

#define DATA_FILE "f:\\lilium\\mock.data"
#define SOLUTION_FILE "f:\\lilium\\mock.solution"


static HANDLE datafile = NULL;
static HANDLE solutionfile = NULL;
static HANDLE datamap = NULL;
static HANDLE solutionmap = NULL;

static DWORD lasterror = 0;

DWORD check_file_handle(HANDLE file)
{
	LPDWORD filesize = NULL;
	if (file == INVALID_HANDLE_VALUE)
		return lasterror = GetLastError();
	if (!GetFileSize(file, filesize)
		|| *filesize == 0)
		return lasterror = ERROR_HANDLE_EOF;
	return ERROR_SUCCESS;
}

DWORD open_files(void)
{
	datafile = CreateFile(
		DATA_FILE,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
		NULL);

	lasterror = check_file_handle(datafile);
	if (lasterror != ERROR_SUCCESS)
		return (int)lasterror;

	solutionfile = CreateFile(
		SOLUTION_FILE,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
		NULL);

	lasterror = check_file_handle(solutionfile);
	if (lasterror != ERROR_SUCCESS)
		return (int)lasterror;

	datamap =
		CreateFileMapping(datafile, NULL, PAGE_READONLY, 0, 0, NULL);
	lasterror = GetLastError();
	if (datamap == NULL || lasterror != ERROR_SUCCESS)
		return (int)lasterror;

	solutionmap =
		CreateFileMapping(solutionfile, NULL, PAGE_READONLY, 0, 0, NULL);
	lasterror = GetLastError();
	if (solutionmap == NULL || lasterror != ERROR_SUCCESS)
		return (int)lasterror;

	return ERROR_SUCCESS;
}

DWORD close_files(void)
{
	if (CloseHandle(datamap)
		|| CloseHandle(solutionmap)
		|| CloseHandle(datafile)
		|| CloseHandle(solutionfile))
		return lasterror = (int)GetLastError();

	return ERROR_SUCCESS;
}

int main(void)
{
	if (open_files() != ERROR_SUCCESS)
		return lasterror;








	if (close_files() != ERROR_SUCCESS)
		return lasterror;

	return ERROR_SUCCESS;
}
