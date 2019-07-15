#include "Directory.h"

#if defined(_WIN32)  || defined(WIN32)
#define getcwd(ptr,n) _getcwd(ptr,n)
#define chdir(str) _chdir(str)
#if defined(_MSC_VER)
#define DIRECTORY_SEPARATOR '\\'
#else
#define DIRECTORY_SEPARATOR '/'
#endif
#include <Windows.h>
#include <direct.h>
#else
#define DIRECTORY_SEPARATOR '/'
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#endif

#include <stdlib.h>
#include <memory.h>

void dircurrentdirectory(std::string& str)
{
	char filepathbuffer[1024];
	memset((char*)& filepathbuffer[0], 0, 1024);
	getcwd(&filepathbuffer[0], 1024);

	for (int ctr = 0; filepathbuffer[ctr] != '\0'; ctr++)
	{
		if (filepathbuffer[ctr] == '\\')
		{
			filepathbuffer[ctr] = '/';
		}
	}

	str = filepathbuffer;
}

void dirgetparentdirectory(const std::string& str, std::string& pstr)
{
	size_t origlen = str.length();

	char* ptr = new char[origlen + 1];
	memset(ptr, 0, origlen + 1);
	memcpy(ptr, str.c_str(), origlen);

	int len = strlen(ptr);

	if (len < 2)
		return;

	int ctr = len - 1;

	while (true)
	{
		ptr[ctr] = 0;
		ctr--;
		if (ptr[ctr] == '/' || ptr[ctr] == '\\')
		{
			break;
		}
	}

	pstr = ptr;

	delete[] ptr;
}

void dirgetextension(const std::string& str, std::string& ext)
{
	int i = 0;
	ext = "";
	int len = str.length();

	if (len < 1)
		return;

	for (i = len - 1; i > 2; i--)
	{
		if (str[i] == '.')
		{
			ext = &str.c_str()[i];
			break;
		}
	}

	return;
}

bool dirfileexists(const std::string& str)
{
	FILE* fp = fopen(str.c_str(), "r");

	if (fp)
	{
		fclose(fp);
		return true;
	}

	return false;
}


void dirgetname(const std::string& fullpath, std::string& str)
{
	int i = 0;
	str = "";
	int len = fullpath.length();

	if (len < 1)
		return;

	for (i = len - 1; ; i--)
	{
		if (fullpath[i] == '\\' || fullpath[i] == '/')
		{
			str = &fullpath.c_str()[i + 1];
			break;
		}
	}

	return;
}

#if defined(_WIN32) || defined(WIN32)

void dircreatedirectory(const std::string& str)
{
	_mkdir(str.c_str());
}

bool dirisdirectory(const std::string& str)
{
	DWORD attr = GetFileAttributesA(str.c_str());

	if (attr == INVALID_FILE_ATTRIBUTES)
	{
		return false;
	}
	return true;
}

void dirgetdirectorylist(const std::string& dirname, std::vector<DirEntry>& dlist)
{
	HANDLE hfind = 0;
	WIN32_FIND_DATAA dstruct;
	DirEntry finfo;
	int err;

	std::string fullpath;

	std::string str = dirname;

	if (str[str.length() - 1] != '/')
	{
		str += "/";
	}


	for (size_t cx = 0; cx < str.length(); cx++)
	{
		if (str[cx] == '\\')
		{
			str[cx] = '/';
		}
	}

	fullpath = str;

	str += "*.*";

	dlist.clear();

	memset((void*)& dstruct, 0, sizeof(dstruct));

	hfind = FindFirstFileA(str.c_str(), &dstruct);

	if (hfind == INVALID_HANDLE_VALUE)
	{
		err = GetLastError();
	}

	while (hfind != INVALID_HANDLE_VALUE)
	{
		if (dstruct.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY || dstruct.dwFileAttributes == 0x30)
		{
			if ((strcmp(dstruct.cFileName, ".") != 0) && (strcmp(dstruct.cFileName, "..") != 0))
			{
				ULONGLONG ull = 0;
				finfo.Name = dstruct.cFileName;
				finfo.FullPath = fullpath + finfo.Name;

				time_t createtm;
				ull = 0;
				ull = reinterpret_cast<const ULONGLONG&>(dstruct.ftCreationTime);
				ull -= 116444736000000000;
				ull /= 10000000;
				if ((ull > ULONG_MAX))
				{
					ull = ULONG_MAX;
				}
				finfo.CreationTime = static_cast<time_t>(ull);

				time_t modifytm;
				ull = 0;
				ull = reinterpret_cast<const ULONGLONG&>(dstruct.ftLastWriteTime);
				ull -= 116444736000000000;
				ull /= 10000000;
				if ((ull > ULONG_MAX))
				{
					ull = ULONG_MAX;
				}
				finfo.LastModifiedTime = static_cast<time_t>(ull);

				dlist.push_back(finfo);
			}
		}

		memset((void*)& dstruct, 0, sizeof(dstruct));

		if (!FindNextFileA(hfind, &dstruct))
		{
			break;
		}
	}
	FindClose(hfind);
}

void dirgetfilelist(const std::string& dirname, std::vector<DirEntry>& dlist, const std::string& extension)
{
	HANDLE hfind = 0;
	WIN32_FIND_DATAA dstruct;
	DirEntry finfo;
	int err;

	std::string fullpath;

	std::string str = dirname;

	if (str[str.length() - 1] != '/')
	{
		str += "/";
	}


	for (size_t cx = 0; cx < str.length(); cx++)
	{
		if (str[cx] == '\\')
		{
			str[cx] = '/';
		}
	}

	fullpath = str;

	if (extension[0] == '.')
	{
		str += "*" + extension;
	}
	else
	{
		str += "*." + extension;
	}

	dlist.clear();

	hfind = FindFirstFileA(str.c_str(), &dstruct);

	if (hfind == INVALID_HANDLE_VALUE)
	{
		err = GetLastError();
	}

	while (hfind != INVALID_HANDLE_VALUE)
	{
		if (dstruct.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY && dstruct.dwFileAttributes != FILE_ATTRIBUTE_NORMAL)
		{
			ULONGLONG ull = 0;
			finfo.Name = dstruct.cFileName;
			finfo.FullPath = fullpath + finfo.Name;

			time_t createtm;
			ull = 0;
			ull = reinterpret_cast<const ULONGLONG&>(dstruct.ftCreationTime);
			ull -= 116444736000000000;
			ull /= 10000000;
			if ((ull > ULONG_MAX))
			{
				ull = ULONG_MAX;
			}
			finfo.CreationTime = static_cast<time_t>(ull);

			time_t modifytm;
			ull = 0;
			ull = reinterpret_cast<const ULONGLONG&>(dstruct.ftLastWriteTime);
			ull -= 116444736000000000;
			ull /= 10000000;
			if ((ull > ULONG_MAX))
			{
				ull = ULONG_MAX;
			}
			finfo.LastModifiedTime = static_cast<time_t>(ull);

			if ((strcmp(dstruct.cFileName, ".") == 0) || (strcmp(dstruct.cFileName, "..") == 0))
			{
			}
			else
			{
				dlist.push_back(finfo);
			}
		}

		if (!FindNextFileA(hfind, &dstruct))
		{
			break;
		}
	}
	FindClose(hfind);
}

#else

void dircreatedirectory(const std::string& str)
{
	mkdir(str.c_str(), S_IRWXU);
}

bool dirisdirectory(const std::string& str)
{
	DIR* dirp;

	dirp = opendir(str.c_str());
	if (dirp == NULL)
	{
		closedir(dirp);
		return false;
	}
	closedir(dirp);
	return true;
}

void dirgetdirectorylist(const std::string& dirname, std::vector<DirEntry>& dlist)
{
	DIR* dir;
	struct dirent* dent;

	std::string fullpath;
	std::string str = dirname;

	if (str[str.length() - 1] != '/')
	{
		str += "/";
	}

	fullpath = str;

	dlist.clear();

	dir = opendir(dirname.c_str());

	if (dir != NULL)
	{
		while (true)
		{
			dent = readdir(dir);
			if (dent == NULL)
			{
				break;
			}

			if (dent->d_type == DT_DIR && (strcmp(dent->d_name, ".") != 0 && strcmp(dent->d_name, "..") != 0))
			{
				DirEntry  finfo;
				finfo.Name = dent->d_name;
				finfo.FullPath = fullpath + finfo.Name;

				struct stat filestat;
				stat(finfo.FullPath.c_str(), &filestat);
				time_t createtm = filestat.st_mtime;
				time_t modifytm;

				if (filestat.st_ctime > filestat.st_mtime)
				{
					modifytm = filestat.st_mtime;
				}
				else
				{
					modifytm = filestat.st_ctime;
				}

				finfo.CreationTime = createtm;
				finfo.LastModifiedTime = modifytm;

				if ((strcmp(dent->d_name, ".") == 0) || (strcmp(dent->d_name, "..") == 0))
				{
					continue;
				}

				dlist.push_back(finfo);
			}
		}
	}

	closedir(dir);
}

void dirgetfilelist(const std::string& dirname, std::vector<DirEntry>& dlist, const std::string& extension)
{
	DIR* dir = NULL;
	struct dirent* dent = NULL;

	std::string fullpath;
	std::string str = dirname;

	if (str[str.length() - 1] != '/')
	{
		str += "/";
	}

	fullpath = str;

	dlist.clear();

	dir = opendir(dirname.c_str());

	if (dir != NULL)
	{
		while (true)
		{
			dent = readdir(dir);
			if (dent == NULL)
			{
				break;
			}

			if ((strcmp(dent->d_name, ".") == 0) || (strcmp(dent->d_name, "..") == 0))
			{
				continue;
			}
			else
			{
				DirEntry  finfo;
				finfo.Name = dent->d_name;
				finfo.FullPath = fullpath + finfo.Name;

				std::string ext;
				dirgetextension(dent->d_name, ext);

				struct stat filestat;
				stat(finfo.FullPath.c_str(), &filestat);
				time_t createtm = filestat.st_mtime;
				time_t modifytm;

				if (filestat.st_ctime > filestat.st_mtime)
				{
					modifytm = filestat.st_mtime;
				}
				else
				{
					modifytm = filestat.st_ctime;
				}

				finfo.CreationTime = createtm;
				finfo.LastModifiedTime = modifytm;

				if (extension.empty() || extension == ".*")
				{
					dlist.push_back(finfo);
				}
				else
				{
					if (extension == ext)
					{
						dlist.push_back(finfo);
					}
				}
			}
		}

		closedir(dir);
	}
}

#endif
