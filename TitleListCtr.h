#pragma once
#include "afxwin.h"
#include <vector>

struct TitleListItem
{
	CString strDisplay;
	CString strGUID;
	LOGFONT lf;
	DWORD dwFontColor;
	DWORD dwFontColorSel;
	DWORD dwBackColor;
	DWORD dwBackColorSel;
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
	LOGFONT m_lf;
	DWORD m_dwFontCorlor;
	DWORD m_dwFontCorlorSel;
	DWORD m_dwBackCorlor;
	DWORD m_dwBackCorlorSel;
public:
	CTitleListCtr(void);
	virtual ~CTitleListCtr(void);
	void Reset();
	//设定控件属性会覆盖掉各行的个性化属性
	void SetCtrFont(LOGFONT lf);
	LOGFONT GetCtrFont();
	void SetCtrFontColor(DWORD dwColor, DWORD dwColorSel);
	DWORD GetCtrFontColor();
	void SetCtrBackColor(DWORD dwColor, DWORD dwColorSel);
	DWORD GetCtrBackColor();
	//设定各行的个性化属性
	void SetItemFont(int nIndex, LOGFONT lf);
	LOGFONT GetItemFont(int nIndex);
	void SetItemFontColor(int nIndex, DWORD dwColor, DWORD dwColorSel);
	DWORD GetvFontColor(int nIndex);
	void SetItemBackColor(int nIndex, DWORD dwColor, DWORD dwColorSel);
	DWORD GetItemBackColor(int nIndex);
	//数据接口
	void SetItemData(int nIndex, void * pData);
	void * GetItemData(int nIndex);
	void SetItem(int nIndex, const TitleListItem & item);
	TitleListItem GetItem(int nIndex);
	TitleListItem GetItem(const CString & strGUID);
	TitleListItem GetItem(void * pData);//当有重复值时仅返回第一个
	UINT GetItemCount();
	CString GetItemGUID();
	//刷新指定行，小于0表示刷新所有行
	void Refresh(int nIndex = -1);
protected:
	afx_msg void OnPaint();
};

