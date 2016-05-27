#include "StdAfx.h"
#include "TitleListCtr.h"

#define SY_szTitleListClass "SYTitleListClass"
#define TITLELISTWND_SCROLLSTEP 20

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

LOGFONT GetDefaultLogfont()
{
	LOGFONT lf;
	lf.lfHeight = -13;
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
	lstrcpy(lf.lfFaceName, _T("微软雅黑"));
	return lf;
}

void TitleListItem::Reset()
{
	strDisplay.Empty();
	strGUID = GetNewGUID();
	dwFontColor = RGB(255, 255, 255);
	dwFontColorSel = RGB(255, 255, 255);
	dwBackColor = RGB(51, 51, 51);
	dwBackColorSel = RGB(76, 76, 76);
	pData = NULL;
	lf = GetDefaultLogfont();
	dwHeight = 28;
	bSelected = FALSE;
	rtItem.SetRectEmpty();
}

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CTitleListCtr, CWnd)
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

BOOL CTitleListCtr::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.lpszClass = AfxRegisterWndClass( CS_HREDRAW|CS_VREDRAW|CS_OWNDC );
	return CWnd::PreCreateWindow(cs);
}

BOOL CTitleListCtr::RegisterControlClass(HINSTANCE hInst /*= NULL*/)
{
	if (hInst == NULL)		hInst = AfxGetInstanceHandle();

	WNDCLASS	wcls;
	// check to see if class already registered
	if (::GetClassInfo(hInst, _T(SY_szTitleListClass), &wcls))
	{
		return TRUE;
	}

	memset(&wcls, sizeof(WNDCLASS), 0);

	// Use standard "static" control as a template.
	VERIFY(::GetClassInfo(NULL, _T("Static"), &wcls));

	wcls.style|=CS_DBLCLKS;
	wcls.hInstance = hInst;
	wcls.lpszClassName = _T(SY_szTitleListClass);
	if(AfxRegisterClass(&wcls) == 0)
		return	FALSE;
	return TRUE;
}

BOOL CTitleListCtr::UnregisterControlClass(HINSTANCE hInst /*= NULL*/)
{
	if (hInst == NULL)		 hInst = AfxGetInstanceHandle();
	return UnregisterClass(_T(SY_szTitleListClass), hInst);
}

CTitleListCtr::CTitleListCtr(void)
{
	Reset();
}

CTitleListCtr::~CTitleListCtr(void)
{
}

BOOL CTitleListCtr::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext /* = NULL */)
{
	if (CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext))
	{
		//m_scrollVertical.SetAutoHide(FALSE);
		CRect rtVScrollBar(0, 0, 20, 100);
		m_scrollVertical.Create(WS_CHILD|WS_VISIBLE|SBS_VERT|SBS_RIGHTALIGN, rtVScrollBar, this, 11111); 
		m_scrollVertical.SetWindowPos(&CWnd::wndTop, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
		if (::IsWindow(m_scrollVertical.GetSafeHwnd()))
		{
			CRect rtClient;
			GetClientRect(rtClient);
			m_scrollVertical.MoveWindow(rtClient.Width() - m_nScrollBarWidth, 0, m_nScrollBarWidth, rtClient.Height());
		}
		ResetScrollInfo();
		return TRUE;
	}
	return FALSE;
}

void CTitleListCtr::Reset()
{
	m_dwBackGndColor = RGB(44, 44, 44);
	m_nScrollPos = 0;
	m_nScrollBarWidth = 20;
}

BOOL CTitleListCtr::InsertItem(const TitleListItem & item, int nPos /*= -1*/)
{
	for (UINT i = 0; i < m_vecItems.size(); i++)
	{
		//不能插入重复guid
		if (m_vecItems[i].strGUID == item.strGUID)
		{
			return FALSE;
		}
	}
	if (nPos < 0 || (DWORD)nPos > m_vecItems.size() - 1)
	{
		m_vecItems.push_back(item);
	}
	else
	{
		std::vector<TitleListItem>::iterator it = m_vecItems.begin() + nPos;
		m_vecItems.insert(it, item);
	}
	ResetScrollInfo();
	return TRUE;
}

BOOL CTitleListCtr::SetItemData(int nIndex, void * pData)
{
	if (nIndex > -1 && nIndex < (int)m_vecItems.size())
	{
		m_vecItems[nIndex].pData = pData;
		return TRUE;
	}
	return FALSE;
}

void * CTitleListCtr::GetItemData(int nIndex)
{
	void * pData = NULL;
	if (nIndex > -1 && nIndex < (int)m_vecItems.size())
	{
		pData = m_vecItems[nIndex].pData;
	}
	return pData;
}

BOOL CTitleListCtr::SetItem(int nIndex, const TitleListItem & item)
{
	for (UINT i = 0; i < m_vecItems.size(); i++)
	{
		//不能插入重复guid
		if (m_vecItems[i].strGUID == item.strGUID && nIndex != i)
		{
			return FALSE;
		}
	}
	if (nIndex > -1 && nIndex < (int)m_vecItems.size())
	{
		m_vecItems[nIndex] = item;
		return TRUE;
	}
	return FALSE;
}

BOOL CTitleListCtr::SetItem(const TitleListItem & item)
{
	for (UINT i = 0; i < m_vecItems.size(); i++)
	{
		//不能插入重复guid
		if (m_vecItems[i].strGUID == item.strGUID)
		{
			m_vecItems[i] = item;
			return TRUE;
		}
	}
	return FALSE;
}

int CTitleListCtr::GetItem(void * pData, TitleListItem & item)
{
	for (UINT i = 0; i < m_vecItems.size(); i++)
	{
		if (m_vecItems[i].pData == pData)
		{
			item = m_vecItems[i];
			return i;
		}
	}
	return -1;
}

void CTitleListCtr::SelectAllItem(BOOL bSel /*= TRUE*/)
{
	for (UINT i = 0; i < m_vecItems.size(); i++)
	{
		m_vecItems[i].bSelected = bSel;
	}
}

void CTitleListCtr::OnPaint()
{
	CPaintDC dc(this);
	CRect rtClient;
	GetClientRect(rtClient);
	rtClient.right -= m_nScrollBarWidth;//将滚动条排除在绘制区外

	//双缓冲绘制
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(&dc, rtClient.Width(), rtClient.Height());
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap * pOldBmp = memDC.SelectObject(&bitmap);

	//先画背景色
	memDC.FillSolidRect(rtClient, m_dwBackGndColor);
	//从上到下逐条画条目
	int nScrollTop = m_nScrollPos;
	int nScrollBottom = m_nScrollPos + rtClient.Height();
	int nLastItemBottom = 0;
	for(UINT i = 0; i < m_vecItems.size(); i++)
	{
		//物件的上边界小于可见区的底边界且物件的下边界大于可见区的顶边界，说明物件可见，需要绘制
		if (nLastItemBottom < nScrollBottom && nLastItemBottom + (int)m_vecItems[i].dwHeight > nScrollTop)
		{
			m_vecItems[i].rtItem.SetRect(0, nLastItemBottom - nScrollTop, rtClient.Width(), nLastItemBottom + m_vecItems[i].dwHeight - nScrollTop);
			DrawItem(&memDC, m_vecItems[i]);
		}
		else
		{
			m_vecItems[i].rtItem.SetRectEmpty();
		}
		nLastItemBottom += m_vecItems[i].dwHeight;
	}
	dc.BitBlt(0, 0, rtClient.Width(), rtClient.Height(), &memDC, 0, 0, SRCCOPY);

	memDC.SelectObject(pOldBmp);
	memDC.DeleteDC();
	bitmap.DeleteObject();
}

void CTitleListCtr::DrawItem(CDC * pDC, TitleListItem & item) const
{
	CRect rtItem = item.rtItem;
	int nRestoreDC = pDC->SaveDC();
	//根据选中情况设置属性
	CFont font;
	font.CreateFontIndirect(&item.lf);
	CBrush brush;
	CPen pen;
	pen.CreatePen(PS_NULL, 0, RGB(0, 0, 0));
	DWORD nTxtColor = 0;
	if (item.bSelected)
	{
		brush.CreateSolidBrush(item.dwBackColorSel);
		nTxtColor = item.dwFontColorSel;
	}
	else
	{
		brush.CreateSolidBrush(item.dwBackColor);
		nTxtColor = item.dwFontColor;
	}
	int nOldTxtColor = pDC->SetTextColor(nTxtColor);
	CFont * pOldFont = pDC->SelectObject(&font);
	CBrush * pOldBrush = pDC->SelectObject(&brush);
	int nOldMode = pDC->SetBkMode(TRANSPARENT);
	CPen * pOldPen = pDC->SelectObject(&pen);

	//画背景
	pDC->Rectangle(&rtItem);
	//写文字
	pDC->DrawText(item.strDisplay, rtItem, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

	pDC->SelectObject(pOldPen);
	pDC->SetBkMode(nOldMode);
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldFont);
	pDC->SetTextColor(nOldTxtColor);
	pDC->RestoreDC(nRestoreDC);

	font.DeleteObject();
	brush.DeleteObject();
	pen.DeleteObject();
}

BOOL CTitleListCtr::GetItemRect(int nIndex, CRect &rtItem)
{
	if (nIndex > -1 && nIndex < (int)m_vecItems.size())
	{
		rtItem = m_vecItems[nIndex].rtItem;
		return TRUE;
	}
	return FALSE;
}

void CTitleListCtr::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar * pScrollBar)
{
	SCROLLINFO ScrollInfo;
	m_scrollVertical.GetScrollInfo(&ScrollInfo);
	int nPosOrg = ScrollInfo.nPos;

	if (nSBCode == SB_LINEUP || nSBCode == SB_LINELEFT)
		ScrollInfo.nPos -= TITLELISTWND_SCROLLSTEP;
	else if (nSBCode == SB_LINEDOWN || nSBCode == SB_LINERIGHT)
		ScrollInfo.nPos += TITLELISTWND_SCROLLSTEP;
	else if (nSBCode == SB_PAGEUP || nSBCode == SB_PAGELEFT)
		ScrollInfo.nPos -= ScrollInfo.nPage;
	else if (nSBCode == SB_PAGEDOWN || nSBCode == SB_PAGERIGHT)
		ScrollInfo.nPos += ScrollInfo.nPage;
	else if (nSBCode == SB_THUMBPOSITION || nSBCode == SB_THUMBTRACK)
		ScrollInfo.nPos = ScrollInfo.nTrackPos;
	else if (nSBCode == SB_TOP || nSBCode == SB_LEFT)
		ScrollInfo.nPos = ScrollInfo.nMin;
	else if (nSBCode == SB_BOTTOM || nSBCode == SB_RIGHT)
		ScrollInfo.nPos = ScrollInfo.nMax;

	if (ScrollInfo.nPos > ScrollInfo.nMax - max((int)(ScrollInfo.nPage) - 1, 0))
		ScrollInfo.nPos = ScrollInfo.nMax - max((int)(ScrollInfo.nPage) - 1, 0);
	if (ScrollInfo.nPos < ScrollInfo.nMin)
		ScrollInfo.nPos = ScrollInfo.nMin;

	if (ScrollInfo.nPos != nPosOrg)
	{
		m_scrollVertical.SetScrollInfo(&ScrollInfo);
		m_nScrollPos = m_scrollVertical.GetScrollPos();

		Invalidate();
	}

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CTitleListCtr::OnSize(UINT nType, int cx, int cy)
{
	ResetScrollInfo();
	if (::IsWindow(m_scrollVertical.GetSafeHwnd()))
	{
		CRect rtClient;
		GetClientRect(rtClient);
		m_scrollVertical.MoveWindow(rtClient.Width() - m_nScrollBarWidth, 0, m_nScrollBarWidth, rtClient.Height());
	}
	CWnd::OnSize(nType, cx, cy);
}

void CTitleListCtr::OnLButtonDown(UINT nFlags, CPoint point)
{
	int nSelIndex = -1;
	for(UINT i = 0; i < m_vecItems.size(); i++)
	{
		if (m_vecItems[i].rtItem.PtInRect(point))
		{
			nSelIndex = i;
			break;
		}
	}
	if (nSelIndex > -1)
	{
		BOOL bCtrl = GetKeyState(VK_CONTROL) < 0;
		BOOL bShift = GetKeyState(VK_SHIFT) < 0;
		if (bCtrl && !bShift)
		{
			//按下ctr为多选
			m_vecItems[nSelIndex].bSelected = !m_vecItems[nSelIndex].bSelected;
		}
		else if (!bCtrl && bShift)
		{
			//按下shift为片选
			int nBeginSel = -1;
			int nEndSel = -1;
			//先找当前项的上一个选中项
			for (int i = nSelIndex; i > nBeginSel; i--)
			{
				if (m_vecItems[i].bSelected)
				{
					nBeginSel = i;
					break;
				}
			}
			//如果没有上一个选中项，则找下一个选中项
			if (nBeginSel < 0)
			{
				for (UINT i = nSelIndex; i < m_vecItems.size(); i++)
				{
					if (m_vecItems[i].bSelected)
					{
						nEndSel = i;
						break;
					}
				}
			}
			SelectAllItem(FALSE);
			if (nBeginSel < 0 && nEndSel < 0)
			{
				//没有找到前一项，则仅选中当前项
				m_vecItems[nSelIndex].bSelected = TRUE;
			}
			else if(nBeginSel > -1)
			{
				for (int i = nBeginSel; i < nSelIndex + 1; i++)
				{
					m_vecItems[i].bSelected = TRUE;
				}
			}
			else if (nEndSel > -1)
			{
				for (int i = nSelIndex; i < nEndSel + 1; i++)
				{
					m_vecItems[i].bSelected = TRUE;
				}
			}
		}
		else if(!bCtrl && !bShift)
		{
			//没有按ctr或shift，则是普通点选动作
			SelectAllItem(FALSE);
			m_vecItems[nSelIndex].bSelected = TRUE;
		}
	}
	Invalidate(FALSE);
}

BOOL CTitleListCtr::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if(nFlags == 0)
	{
		UINT uWheelScrollLines = 2;
		::SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &uWheelScrollLines, 0);
		int nWheelOffset = ::MulDiv(-zDelta, uWheelScrollLines, WHEEL_DELTA);
		nWheelOffset *= TITLELISTWND_SCROLLSTEP;

		CScrollBar * pScrollBar = &m_scrollVertical;
		SCROLLINFO ScrollInfo;
		pScrollBar->GetScrollInfo(&ScrollInfo);
		int nPosOrg = ScrollInfo.nPos;
		ScrollInfo.nPos += nWheelOffset;
		if (ScrollInfo.nPos > ScrollInfo.nMax - max((int)(ScrollInfo.nPage) - 1, 0))
			ScrollInfo.nPos = ScrollInfo.nMax - max((int)(ScrollInfo.nPage) - 1, 0);
		if (ScrollInfo.nPos < ScrollInfo.nMin)
			ScrollInfo.nPos = ScrollInfo.nMin;

		if (ScrollInfo.nPos != nPosOrg)
		{
			pScrollBar->SetScrollInfo(&ScrollInfo);
			m_nScrollPos = pScrollBar->GetScrollPos();
			Invalidate(FALSE);
		}
	}

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CTitleListCtr::ResetScrollInfo()
{
	if (!::IsWindow(m_scrollVertical.GetSafeHwnd()))
	{
		return;
	}
	CRect rtClient;
	GetClientRect(rtClient);
	int nScrollHeight = rtClient.Height();
	int nItemsHeight = 0;
	for (UINT i = 0; i < m_vecItems.size(); i++)
	{
		nItemsHeight += m_vecItems[i].dwHeight;
	}
	if (nItemsHeight > nScrollHeight)
	{
		nScrollHeight = nItemsHeight;
	}
	SCROLLINFO ScrollInfo;
	m_scrollVertical.GetScrollInfo(&ScrollInfo);
	ScrollInfo.nMin = 0;
	ScrollInfo.nMax = nScrollHeight;
	ScrollInfo.nPage = rtClient.Height();
	if (ScrollInfo.nPos > ScrollInfo.nMax)
	{
		ScrollInfo.nPos = ScrollInfo.nMax;
	}
	m_nScrollPos = ScrollInfo.nPos;
	m_scrollVertical.SetScrollInfo(&ScrollInfo);
}
