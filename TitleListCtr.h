#pragma once
#include "afxwin.h"
#include <vector>

#define WM_TITLELIST_SELCHANGE WM_USER + 0x0010

struct TitleListItem
{
	CString strGUID;//自动生成，无需设置
	CString strDisplay;
	LOGFONT lf;
	DWORD dwFontColor;
	DWORD dwFontColorSel;
	DWORD dwBackColor;
	DWORD dwBackColorSel;
	DWORD dwHeight;
	BOOL bSelected;
	CRect rtItem;//上一次绘制的rect
	void * pData;
	TitleListItem(){Reset();}
	void Reset();
};

//////////////////////////////////////////////////////////////////////////
class CTitleListCtr :
	public CWnd
{
	DECLARE_MESSAGE_MAP()
private:
	std::vector<TitleListItem> m_vecItems;//一个显示文本一个data指针
	DWORD m_dwBackGndColor;
	DWORD m_dwHeight;

	CScrollBar m_scrollVertical;
	int m_nScrollPos;
	int m_nScrollBarWidth;
protected:
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar * pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	void ResetScrollInfo();
	void DrawItem(CDC * pDC, TitleListItem & item)const;
	BOOL GetItemRect(int nIndex, CRect &rtItem);//考虑了滚动条的rect
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	static BOOL RegisterControlClass(HINSTANCE hInst = NULL);
	static BOOL UnregisterControlClass(HINSTANCE hInst = NULL);
	static LRESULT CALLBACK EXPORT WndProcHook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	CTitleListCtr(void);
	virtual ~CTitleListCtr(void);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext /* = NULL */);
	void Reset();
	//设定控件属性会覆盖掉各行的个性化属性
	void SetCtrFont(LOGFONT lf);
	LOGFONT GetCtrFont();
	void SetCtrFontColor(DWORD dwColor, DWORD dwColorSel);
	DWORD GetCtrFontColor();
	void SetCtrBackColor(DWORD dwColor, DWORD dwColorSel);
	DWORD GetCtrBackColor();
	void SetBackGndColor(DWORD dwColor);
	void SetCtrItemHeight(DWORD dwHeight);
	//设定各行的个性化属性
	BOOL SetItemFont(int nIndex, LOGFONT lf);
	BOOL GetItemFont(int nIndex, LOGFONT & lf);
	BOOL SetItemFontColor(int nIndex, DWORD dwColor, DWORD dwColorSel);
	BOOL GetItemFontColor(int nIndex, DWORD & dwColor);
	BOOL SetItemBackColor(int nIndex, DWORD dwColor, DWORD dwColorSel);
	BOOL GetItemBackColor(int nIndex, DWORD & dwColor);
	BOOL SetItemHeight(int nIndex, DWORD dwHeight);
	//数据接口
	BOOL InsertItem(const TitleListItem & item, int nPos = -1);//在指定位置插入一条数据，pos<0或pos>size则插入到末尾，guid重复插入失败
	BOOL SetItemData(int nIndex, void * pData);
	void * GetItemData(int nIndex);
	BOOL SetItem(int nIndex, const TitleListItem & item);//仅有修改功能，不具备添加功能
	BOOL SetItem(const TitleListItem & item);//按照guid相等为准，修改指定项
	BOOL GetItem(int nIndex, TitleListItem & item);
	int GetItem(const CString & strGUID, TitleListItem & item);//未找到返回-1，否则返回序号
	int GetItem(void * pData, TitleListItem & item);//当有重复值时仅返回第一个，未找到返回-1，否则返回序号
	UINT GetItemCount();
	CString GetItemGUID();
	//选中
	void SelectAllItem(BOOL bSel = TRUE);
};

