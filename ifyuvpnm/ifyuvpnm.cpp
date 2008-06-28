/**
 * @filename ifyuvpnm.cpp
 * @brief Susie Plugin EntryPoint & Code
 *
 * Original Author: kz_n
 * $Author: najima $
 * $Date: 2004/04/27 07:59:45 $
 * $Log: ifyuvpnm.cpp,v $
 * Revision 1.1.2.1  2004/04/27 07:59:45  najima
 * add: Susie Plugin (Win32 DLL)
 *      This plugin handle ppm images.
 *      If the images inclue comment "#YUV 8:8:8", it converts yuv2rgb and show.
 *      Otherwise, it shows image without conversion.
 *      This tool can't work only it.
 *      It needs Susie Frontend(Susie, Vix, FineView, and so on).
 *
 */

#include "stdafx.h"
BOOL APIENTRY DllMain( HANDLE /*hModule*/, 
                       DWORD  /*ul_reason_for_call*/, 
                       LPVOID /*lpReserved*/
					 )
{
    return TRUE;
}

extern "C" int PASCAL GetPluginInfo(int infono, LPSTR buf, int buflen)
{
	LPCSTR szRets[] = {
		"00IN",
		"YUV PNM Import Filter",
		"*.pbm;*.pgm;*.ppm;*.pnm",
		"PBM/PGM/PPM(ascii/raw)",
	};

	if(0 <= infono && infono <= 3)
	{
		lstrcpyn(buf, szRets[infono], buflen);
		return lstrlen(buf);
	}

	return 0;
}

extern "C" int PASCAL IsSupported(LPSTR /*filename*/, DWORD dw)
{
	CHAR szBuf[3] = {0};

	if(HIWORD(dw))
	{
		CopyMemory(szBuf, reinterpret_cast<LPVOID>(dw), 3);
	}
	else
	{
		HANDLE hFile = reinterpret_cast<HANDLE>(dw);
		SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
		DWORD dwRead;
		ReadFile(hFile, szBuf, 3, &dwRead, NULL);
		if(dwRead != 3)
			return 0;
	}

	if((szBuf[0] != 'P') || (szBuf[1] < '1' || '6' < szBuf[1]) || (szBuf[2] != '\r' && szBuf[2] != '\n'))
		return 0;

	if(szBuf[1] == '4')
		return 0;

	return 1;
}

enum {
	SPI_S_OK			=  0,	// 正常終了
	SPI_E_NOT_IMPLEMENT	= -1,	// その機能はインプリメントされていない
	SPI_E_STOP_EXTRACT	=  1,	// コールバック関数が非0を返したので展開を中止した
	SPI_E_UNKNOWN_FORMAT=  2,	// 未知のフォーマット
	SPI_E_INVALID_DATA	=  3,	// データが壊れている
	SPI_E_OUT_OF_MEMORY	=  4,	// メモリーが確保出来ない
	SPI_E_MEMORY		=  5,	// メモリーエラー（Lock出来ない、等）
	SPI_E_READ_FAILED	=  6,	// ファイルリードエラー
	SPI_E_RESERVED		=  7,	// （予約）
	SPI_E_INTERNAL		=  8,	// 内部エラー
};

extern "C" int PASCAL GetPictureInfo(LPSTR /*buf*/, long /*len*/,
									 unsigned int /*flag*/, struct PictureInfo* /*lpInfo*/)
{
	return SPI_E_NOT_IMPLEMENT;
}

struct PNMInfo
{
	int type;
	bool yuv;
	int width, height;
	int max;
};

bool SkipComment(std::istream& is, PNMInfo& pnmInfo)
{
	while(is.peek() == '#')
	{
		std::string str;
		std::getline(is, str);
		if(!is)
			return true;
		if(str == "#YUV 8:8:8" || str == "#YUV 8:8:8\r")
			pnmInfo.yuv = true;
	}
	return false;
}

bool GetStream(std::istream& is, std::istringstream& iss)
{
	std::string str;
	std::getline(is, str);
	if(!is)
		return true;
	iss.clear();
	iss.str(str);
	return false;
}

int GetHeader(std::istream& is, PNMInfo& pnmInfo)
{
	// ヘッダチェック
	std::string str;
	std::getline(is, str);
	if((str[0] != 'P') || (str[1] < '1' || '6' < str[1]))
		return SPI_E_UNKNOWN_FORMAT;
	if(str[1] == '4')
		return SPI_E_UNKNOWN_FORMAT;
	pnmInfo.type = str[1] - '0';

	pnmInfo.yuv = false;

	std::istringstream iss;

	// 幅と高さ
	if(SkipComment(is, pnmInfo) || GetStream(is, iss))
		return SPI_E_UNKNOWN_FORMAT;
	iss >> pnmInfo.width >> pnmInfo.height;
	if(!iss)
		return SPI_E_UNKNOWN_FORMAT;

	// 最大輝度
	if(pnmInfo.type == 1 || pnmInfo.type == 4)
	{
		pnmInfo.max = 1;
	}
	else
	{
		if(SkipComment(is, pnmInfo) || GetStream(is, iss))
			return SPI_E_UNKNOWN_FORMAT;
		iss >> pnmInfo.max;
		if(!iss)
			return SPI_E_UNKNOWN_FORMAT;
	}

	return SPI_S_OK;
}

void yuv2rgb(int& c1, int& c2, int& c3)
{
#if 1
	int y = c1;
	int cr = c2 - 128;
	int cb = c3 - 128;

	c1 = static_cast<int>(y+1.402*cr);
	c2 = static_cast<int>(y-0.714*cr-0.344*cb);
	c3 = static_cast<int>(y+1.772*cb);
#else
	int y = c1;
	int u = c2 * 2 - 255;
	int v = c3 * 2 - 255;

	c1 = y + u;
	c2 = static_cast<int>(y - 0.51 * u - 0.19 * v);
	c3 = y + v;
#endif
}

bool GetColor(std::istream& is, int max, int& color)
{
	is >> color;
	if(!is)
		return true;
	color = color * 255 / max;
	return false;
}

inline BYTE int2byte(int x)
{
	if(x < 0)
		x = 0;
	else if(x > 255)
		x = 255;
	return static_cast<BYTE>(x);
}

int GetAsciiData(std::istream& is, const PNMInfo& pnmInfo, int linesize, LPBYTE pBuf,
				 int (PASCAL *lpProgressCallback)(int, int, long), long lData)
{
	for(int i = 0; i < pnmInfo.height; ++i)
	{
		if(lpProgressCallback && lpProgressCallback(i, pnmInfo.height, lData))
			return SPI_E_STOP_EXTRACT;
		for(int j = 0; j < pnmInfo.width; ++j)
		{
			int c1, c2, c3;
			if(GetColor(is, pnmInfo.max, c1))
				return SPI_E_INVALID_DATA;
			if(pnmInfo.type == 3)
			{
				if(GetColor(is, pnmInfo.max, c2) || GetColor(is, pnmInfo.max, c3))
					return SPI_E_INVALID_DATA;
			}
			else
			{
				c3 = c2 = c1;
			}
			if(pnmInfo.yuv)
				yuv2rgb(c1, c2, c3);
			pBuf[j * 3 + 0] = int2byte(c3);
			pBuf[j * 3 + 1] = int2byte(c2);
			pBuf[j * 3 + 2] = int2byte(c1);
		}
		pBuf -= linesize;
	}

	return SPI_S_OK;
}

int GetRawData(std::istream& is, const PNMInfo& pnmInfo, int linesize, LPBYTE pBuf,
			   int (PASCAL *lpProgressCallback)(int, int, long), long lData)
{
	int size = 0;
	switch(pnmInfo.type)
	{
	case 4:
		return SPI_E_INTERNAL;
	case 5:
		size = pnmInfo.width * pnmInfo.height;
		break;
	case 6:
		size = pnmInfo.width * pnmInfo.height * 3;
		break;
	}

	std::vector<BYTE> vecSource(size);
	is.read(reinterpret_cast<char*>(&vecSource[0]), size);
	if(!is)
		return SPI_E_INVALID_DATA;
	LPBYTE pSource = &vecSource[0];

	for(int i = 0; i < pnmInfo.height; ++i)
	{
		if(lpProgressCallback && lpProgressCallback(i, pnmInfo.height, lData))
			return SPI_E_STOP_EXTRACT;
		for(int j = 0; j < pnmInfo.width; ++j)
		{
			int c1, c2, c3;
			c1 = *(pSource++) * 255 / pnmInfo.max;
			if(pnmInfo.type == 6)
			{
				c2 = *(pSource++) * 255 / pnmInfo.max;
				c3 = *(pSource++) * 255 / pnmInfo.max;
			}
			else
			{
				c3 = c2 = c1;
			}
			if(pnmInfo.yuv)
				yuv2rgb(c1, c2, c3);
			pBuf[j * 3 + 0] = int2byte(c3);
			pBuf[j * 3 + 1] = int2byte(c2);
			pBuf[j * 3 + 2] = int2byte(c1);
		}
		pBuf -= linesize;
	}

	return SPI_S_OK;
}

extern "C" int PASCAL GetPicture(LPSTR buf, long len, unsigned int flag, HANDLE* pHBInfo, HANDLE* pHBm,
								 int (PASCAL *lpProgressCallback)(int, int, long), long lData)
{
	try
	{
		std::ifstream ifs;
		std::istringstream iss(std::ios_base::in | std::ios_base::binary);
		std::istream* pIS = NULL;

		switch(flag & 7)
		{
		case 0: // ファイル
			ifs.open(buf, std::ios_base::in | std::ios_base::binary);
			if(!ifs.is_open())
				return SPI_E_READ_FAILED;
			ifs.seekg(len);
			pIS = &ifs;
			break;
		case 1: // メモリ
			iss.str(std::string(buf, len));
			pIS = &iss;
			break;
		default:
			return SPI_E_INTERNAL;
		}

		int ret = 0;

		PNMInfo pnmInfo;
		ret = GetHeader(*pIS, pnmInfo);
		if(ret)
			return ret;

		*pHBInfo = LocalAlloc(LMEM_FIXED, sizeof(BITMAPINFOHEADER));
		if(!*pHBInfo)
			return SPI_E_OUT_OF_MEMORY;
		LPBITMAPINFOHEADER pBMIH = static_cast<LPBITMAPINFOHEADER>(*pHBInfo);
		pBMIH->biSize = sizeof(BITMAPINFOHEADER);
		pBMIH->biWidth = pnmInfo.width;
		pBMIH->biHeight = pnmInfo.height;
		pBMIH->biPlanes = 1;
		pBMIH->biBitCount = 24;
		pBMIH->biCompression = BI_RGB;
		pBMIH->biSizeImage = 0;
		pBMIH->biXPelsPerMeter = 0;
		pBMIH->biYPelsPerMeter = 0;
		pBMIH->biClrUsed = 0;
		pBMIH->biClrImportant = 0;

		int linesize = (pnmInfo.width * 3 + 3) & ~3;
		*pHBm = LocalAlloc(LMEM_FIXED, linesize * pnmInfo.height);
		if(!*pHBm)
			return SPI_E_OUT_OF_MEMORY;
		LPBYTE pBuf = static_cast<LPBYTE>(*pHBm);
		pBuf += linesize * (pnmInfo.height - 1);

		if(pnmInfo.type <= 3) // ascii
		{
			ret = GetAsciiData(*pIS, pnmInfo, linesize, pBuf, lpProgressCallback, lData);
			if(ret)
				return ret;
		}
		else // raw
		{
			ret = GetRawData(*pIS, pnmInfo, linesize, pBuf, lpProgressCallback, lData);
			if(ret)
				return ret;
		}
		if(lpProgressCallback)
			lpProgressCallback(pnmInfo.height, pnmInfo.height, lData);

		return SPI_S_OK;
	}
	catch(std::bad_alloc&)
	{
		return SPI_E_OUT_OF_MEMORY;
	}
	catch(...)
	{
		return SPI_E_INTERNAL;
	}
}

extern "C" int PASCAL GetPreview(LPSTR /*buf*/, long /*len*/, unsigned int /*flag*/,
								 HANDLE* /*pHBInfo*/, HANDLE* /*pHBm*/,
								 FARPROC /*lpPrgressCallback*/, long /*lData*/)
{
	return SPI_E_NOT_IMPLEMENT;
}
