#include "StdAfx.h"
#include "TitleListCtr.h"

#pragma comment(lib, "Rpcrt4.lib")
HRESULT GuidToString(const GUID& guid, CString& str)//转换GUID到字符串
{
	GUID guidNo = guid;
#ifdef UNICODE
	RPC_WSTR pUuidString;
#else
	BYTE* pUuidString;
#endif
	UuidToString(&guidNo, &pUuidString);
	str =(LPCTSTR)pUuidString;
	str.MakeUpper();

	if (RPC_S_OK == RpcStringFree(&pUuidString))
		return S_OK;

	return S_FALSE;
}

CString GetNewGUID()//生成新的GUID
{
	GUID guid = GUID_NULL;
	::CoCreateGuid(&guid);
	CString strGUID;
	GuidToString( guid, strGUID );
	strGUID.MakeUpper();
	return strGUID;
}

void TitleListItem::Reset()
{
	strDisplay.Empty();
	strGUID = GetNewGUID();
	dwFontColor = RGB(255, 255, 255);
	dwFontColorSel = RGB(0, 0, 0);
	dwBackColor = RGB(0, 0, 0);
	dwBackColorSel = RGB(255, 255, 255);
	pData = NULL;
	lf.lfHeight = -30;
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = 5;
	lf.lfItalic = 0;
	lf.lfUnderline = 0;
	lf.lfStrikeOut = 0;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = 0;
	lf.lfClipPrecision = 0;
	lf.lfQuality = 0;
	lf.lfPitchAndFamily = 0;
	lstrcpy(lf.lfFaceName, _T("Arial"));
}

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CTitleListCtr, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()

CTitleListCtr::CTitleListCtr(void)
{
	m_lf.lfHeight = -30;
	m_lf.lfWidth = 0;
	m_lf.lfEscapement = 0;
	m_lf.lfOrientation = 0;
	m_lf.lfWeight = 5;
	m_lf.lfItalic = 0;
	m_lf.lfUnderline = 0;
	m_lf.lfStrikeOut = 0;
	m_lf.lfCharSet = DEFAULT_CHARSET;
	m_lf.lfOutPrecision = 0;
	m_lf.lfClipPrecision = 0;
	m_lf.lfQuality = 0;
	m_lf.lfPitchAndFamily = 0;
	lstrcpy(m_lf.lfFaceName, _T("Arial"));
}


CTitleListCtr::~CTitleListCtr(void)
{
}

void CTitleListCtr::Reset()
{

}

void CTitleListCtr::OnPaint()
{
}
