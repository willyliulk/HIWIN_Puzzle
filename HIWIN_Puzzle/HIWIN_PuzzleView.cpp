
// HIWIN_PuzzleView.cpp: CHIWINPuzzleView 類別的實作
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以定義在實作預覽、縮圖和搜尋篩選條件處理常式的
// ATL 專案中，並允許與該專案共用文件程式碼。
#ifndef SHARED_HANDLERS
#include "HIWIN_Puzzle.h"
#endif

#include "HIWIN_PuzzleDoc.h"
#include "HIWIN_PuzzleView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CHIWINPuzzleView

IMPLEMENT_DYNCREATE(CHIWINPuzzleView, CView)

BEGIN_MESSAGE_MAP(CHIWINPuzzleView, CView)
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CHIWINPuzzleView 建構/解構

CHIWINPuzzleView::CHIWINPuzzleView() noexcept
{
	// TODO: 在此加入建構程式碼

}

CHIWINPuzzleView::~CHIWINPuzzleView()
{
}

BOOL CHIWINPuzzleView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此經由修改 CREATESTRUCT cs 
	// 達到修改視窗類別或樣式的目的

	return CView::PreCreateWindow(cs);
}

// CHIWINPuzzleView 繪圖

void CHIWINPuzzleView::OnDraw(CDC* /*pDC*/)
{
	CHIWINPuzzleDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此加入原生資料的描繪程式碼
}


// CHIWINPuzzleView 診斷

#ifdef _DEBUG
void CHIWINPuzzleView::AssertValid() const
{
	CView::AssertValid();
}

void CHIWINPuzzleView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CHIWINPuzzleDoc* CHIWINPuzzleView::GetDocument() const // 內嵌非偵錯版本
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CHIWINPuzzleDoc)));
	return (CHIWINPuzzleDoc*)m_pDocument;
}
#endif //_DEBUG


// CHIWINPuzzleView 訊息處理常式


int CHIWINPuzzleView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此加入特別建立的程式碼
	BTN_open_cam.Create("open a camera windows", BS_PUSHBUTTON, CRect(300, 100, 600, 150), this, IDC_BUTTON_OPENCAM);
	BTN_open_cam.ShowWindow(SW_SHOW);


	return 0;
}
