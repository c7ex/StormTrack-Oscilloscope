#include "WinApiIconLoader.hpp"

DWORD WinApiIconLoader::ReadDWORDLE(const unsigned char* data, size_t offset) {
    return data[offset] | (data[offset + 1] << 8) |
        (data[offset + 2] << 16) | (data[offset + 3] << 24);
}

WORD WinApiIconLoader::ReadWORDLE(const unsigned char* data, size_t offset) {
    return data[offset] | (data[offset + 1] << 8);
}

HICON WinApiIconLoader::CreateIconFromICOMemory(const unsigned char* icoData, size_t icoSize, int width, int height) {
    if (!icoData || icoSize < sizeof(ICONHEADER)) {
        return NULL;
    }

    const ICONHEADER* pHeader = reinterpret_cast<const ICONHEADER*>(icoData);

    if (pHeader->wReserved != 0 || pHeader->wType != 1) {
        return NULL;
    }

    int imageCount = pHeader->wCount;
    if (imageCount <= 0 || imageCount > 10) {
        imageCount = 10;
    }

    if (icoSize < sizeof(ICONHEADER) + imageCount * sizeof(ICONDIRENTRY)) {
        return NULL;
    }

    const ICONDIRENTRY* pEntries = reinterpret_cast<const ICONDIRENTRY*>(
        icoData + sizeof(ICONHEADER)
        );

    auto CreateIconFromEntry = [&](const ICONDIRENTRY* pEntry, int reqWidth, int reqHeight) -> HICON {
        if (pEntry->dwImageOffset + pEntry->dwBytesInRes > icoSize) {
            return NULL;
        }

        return CreateIconFromResourceEx(
            const_cast<PBYTE>(icoData + pEntry->dwImageOffset),
            pEntry->dwBytesInRes,
            TRUE,
            0x00030000,
            reqWidth, reqHeight,
            LR_DEFAULTCOLOR
        );
        };

    for (int i = 0; i < imageCount; ++i) {
        const ICONDIRENTRY* pEntry = &pEntries[i];

        int entryWidth = (pEntry->bWidth == 0) ? 256 : pEntry->bWidth;
        int entryHeight = (pEntry->bHeight == 0) ? 256 : pEntry->bHeight;

        if (entryWidth == width && entryHeight == height) {
            HICON hIcon = CreateIconFromEntry(pEntry, width, height);
            if (hIcon) return hIcon;
        }
    }

    ICONDIRENTRY bestEntry = { 0 };
    bool foundBest = false;
    int bestScore = -1;

    for (int i = 0; i < imageCount; ++i) {
        const ICONDIRENTRY* pEntry = &pEntries[i];

        int entryWidth = (pEntry->bWidth == 0) ? 256 : pEntry->bWidth;
        int entryHeight = (pEntry->bHeight == 0) ? 256 : pEntry->bHeight;

        if (entryWidth >= width && entryHeight >= height) {
            int score = entryWidth + entryHeight;
            if (score > bestScore) {
                bestScore = score;
                bestEntry = *pEntry;
                foundBest = true;
            }
        }
    }

    if (!foundBest) {
        for (int i = 0; i < imageCount; ++i) {
            const ICONDIRENTRY* pEntry = &pEntries[i];
            int entryWidth = (pEntry->bWidth == 0) ? 256 : pEntry->bWidth;
            int entryHeight = (pEntry->bHeight == 0) ? 256 : pEntry->bHeight;

            int score = entryWidth + entryHeight;
            if (score > bestScore) {
                bestScore = score;
                bestEntry = *pEntry;
                foundBest = true;
            }
        }
    }

    if (foundBest) {
        HICON hIcon = CreateIconFromEntry(&bestEntry, 0, 0);
        if (hIcon) return hIcon;
    }

    HICON hIcon = CreateIconFromEntry(&pEntries[0], 0, 0);
    if (hIcon) return hIcon;

    return NULL;
}