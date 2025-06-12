#include <stdio.h>
#include <windows.h>

//TODO: file name/path length check
//TODO: include a --help | -h | /?
int main(int argc, char** argv)
{
    if (2 != argc)
    {
        fprintf(stderr, "A filename must be provided as a second argument");
        return 1;
    }
    wchar_t* wideStr = (wchar_t*)malloc((strlen(argv[1]) + 1) * sizeof(wchar_t));
    mbstowcs(wideStr, argv[1], (strlen(argv[1]) + 1));
    HKEY hKey;
    LPCSTR subKey = "SYSTEM\\CurrentControlSet\\Control\\FileSystem";
    DWORD res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, subKey, 0, KEY_QUERY_VALUE, &hKey);
    if (ERROR_SUCCESS != res)
    {
        fprintf(stderr, "Error fetching maximum path value from the registry");
        RegCloseKey(hKey);
        return 1;
    }
    BYTE buff[128];//ideally i should pass smth and then relocate based on the sizeof of returned regtype but meh...
    DWORD buffsz = sizeof(buff) ;
    DWORD qres = RegQueryValueEx(hKey, "LongPathsEnabled", NULL, NULL, buff, &buffsz);
    if (ERROR_SUCCESS != qres)
    {
        fprintf(stderr, "Error fetching maximum path value from the registry");
        RegCloseKey(hKey);
        return 1;
    }
    //DWORD dwordValue = *((DWORD*)buff);
    //if 0 then force 239 length
    RegCloseKey(hKey);
    FILE *fp = NULL;
    //reading a unicode path is fine, creating is not???
    if (NULL != (fp = _wfopen(wideStr, L"r+")))
    {
        goto fup;
    }
    fclose(fp);
    fp = _wfopen(wideStr, L"a");
    if (NULL == fp)
    {
        goto fup;
    }
    else
    {
        goto norm;
    }
fup:
    perror("Error");
    fprintf(stderr,"Failed to create file '%s'\n", argv[1]);
    fclose(fp);
    return 1;
norm:
    fclose(fp);
    return 0;
}
