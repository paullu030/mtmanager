#pragma once

#include <time.h>
#include <cassert>
#include <codecvt>


using namespace std;


LPCWSTR stringToLPCWSTR(const string& orig)
{
    wstring tempWstr;
    wstring_convert<codecvt_utf8<wchar_t>> myconv;
    tempWstr = myconv.from_bytes(orig);
    wchar_t* wcstring = _wcsdup(tempWstr.c_str());
    return wcstring;
}

string LPCWSTRTostring(const wstring& s)
{
    wstring_convert<codecvt_utf8<wchar_t>> myconv;
    return myconv.to_bytes(s);
}
