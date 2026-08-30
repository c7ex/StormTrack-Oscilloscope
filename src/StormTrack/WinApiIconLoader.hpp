#ifndef WIN_API_ICON_LOADER_HPP
#define WIN_API_ICON_LOADER_HPP	

#include <windows.h>

#include"StormTrackIconData.hpp"

// AI-generated icon data structures for format .ico

class WinApiIconLoader {
#pragma pack(push, 1)
    typedef struct {
        WORD wReserved;
        WORD wType;
        WORD wCount;
    } ICONHEADER;

    typedef struct {
        BYTE bWidth;
        BYTE bHeight;
        BYTE bColorCount;
        BYTE bReserved;
        WORD wPlanes;
        WORD wBitCount;
        DWORD dwBytesInRes;
        DWORD dwImageOffset;
    } ICONDIRENTRY;
#pragma pack(pop)

private:
    static DWORD ReadDWORDLE(const unsigned char* data, size_t offset);
    static WORD ReadWORDLE(const unsigned char* data, size_t offset);

public:
    static HICON CreateIconFromICOMemory(const unsigned char* icoData, size_t icoSize, int width, int height);
};



#endif