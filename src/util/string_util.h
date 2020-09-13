#pragma once

#include <string>
#include <codecvt>
#include <locale>

namespace cks {

    using convert_t = std::codecvt_utf8<wchar_t>;
    static std::wstring_convert<convert_t, wchar_t> converter;

    class StringUtil {
    public:
        static std::wstring toWString(std::string str) {
            return converter.from_bytes(str);
        };
        static std::string toString(std::wstring wstr) {
            return converter.to_bytes(wstr);
        };
        static char* wctoc(const wchar_t* pWCStr) {
            //반환할 char* 변수 선언
            char* pCStr;
            //입력받은 wchar_t 변수의 길이를 구함
            int size = WideCharToMultiByte(CP_ACP, 0, pWCStr, -1, NULL, 0, NULL, NULL);
            //char* 메모리 할당
            pCStr = new char[size];
            //형 변환
            WideCharToMultiByte(CP_ACP, 0, pWCStr, -1, pCStr, size, 0, 0);
            return pCStr;
        };
    };
}