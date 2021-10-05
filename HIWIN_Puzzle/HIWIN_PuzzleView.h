
// HIWIN_PuzzleView.h: CHIWINPuzzleView 類別的介面
//

#pragma once


class CHIWINPuzzleView : public CView
{
protected: // 僅從序列化建立
	CHIWINPuzzleView() noexcept;
	DECLARE_DYNCREATE(CHIWINPuzzleView)

// 屬性
public:
	CHIWINPuzzleDoc* GetDocument() const;

// 作業
public:

// 覆寫
public:
	virtual void OnDraw(CDC* pDC);  // 覆寫以描繪此檢視
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 程式碼實作
public:
	virtual ~CHIWINPuzzleView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 產生的訊息對應函式
protected:
	DECLARE_MESSAGE_MAP()

//MainFream 的東西
private:
	CButton BTN_open_cam;

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

#ifndef _DEBUG  // 對 HIWIN_PuzzleView.cpp 中的版本進行偵錯
inline CHIWINPuzzleDoc* CHIWINPuzzleView::GetDocument() const
   { return reinterpret_cast<CHIWINPuzzleDoc*>(m_pDocument); }
#endif

