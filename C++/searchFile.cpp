#include <io.h>

//string path = "D:\\testFindFileFunc\\";
void getFiles(string path, vector<string>& files)
{
	long hFile = 0;
	struct _finddata_t fileinfo;
	string p;

	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					getFiles(p.assign(path).append("\\").append(fileinfo.name), files);
				}
			}
			else
			{
      //如果有过滤条件，就在这里添加
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
				//files.push_back(fileinfo.name);
			}
		} while (_findnext(hFile, &fileinfo) == 0);

		_findclose(hFile);
	}
}
