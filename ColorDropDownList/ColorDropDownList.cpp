#include "framework.h"
#include "resource.h"

using namespace std;

#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

// 設定する色
COLORREF rgbColoList[16] =
{
	0xFFFFFF,
	0xC0C0C0,
	0x808080,
	0x000000,
	0x0000FF,
	0x000080,
	0x00FFFF,
	0x008080,
	0x00FF00,
	0x008000,
	0xFFFF00,
	0x808000,
	0xFF0000,
	0x800000,
	0xFF00FF,
	0x800080
};

INT_PTR CALLBACK DialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), nullptr, DialogProc);

	return 0;
}

INT_PTR CALLBACK DialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	static HWND hCombo;

	switch (msg)
	{
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_GETCOLOR:
		{
			// 選択中のインデックス取得
			LRESULT index = SendMessageW(hCombo, CB_GETCURSEL, 0, 0);
			wchar_t szColor[16];
			// 選択中の項目のテキストを取得
			SendMessageW(hCombo, CB_GETLBTEXT, index, (LPARAM)szColor);
			MessageBox(hDlg, szColor, L"Color", MB_OK | MB_ICONINFORMATION);
			return (INT_PTR)TRUE;
		}
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	case WM_INITDIALOG:
	{
		INITCOMMONCONTROLSEX ic;
		ic.dwICC = ICC_COOL_CLASSES;
		ic.dwSize = sizeof(INITCOMMONCONTROLSEX);
		InitCommonControlsEx(&ic);

		hCombo = GetDlgItem(hDlg, IDC_COMBO1);

		// カラーコード追加
		for (const auto& c : rgbColoList)
		{
			wstring wsColor = format(L"{:02X}{:02X}{:02X}", GetRValue(c), GetGValue(c), GetBValue(c));
			SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)wsColor.c_str());
		}

		// 初期選択項目
		// wParam
		//  Specifies the zero-based index of the string to select.
		//  If this parameter is -1, any current selection in the list is removed and the edit control is cleared.
		// lParam
		//  This parameter is not used.
		SendMessageW(hCombo, CB_SETCURSEL, 0, 0);

		return (INT_PTR)TRUE;
	}
	case WM_DRAWITEM:
	{
		LPDRAWITEMSTRUCT lpDis = (LPDRAWITEMSTRUCT)lParam;

		if (lpDis->CtlID != IDC_COMBO1)
			return (INT_PTR)FALSE;

		// 項目が設定されていなければ、処理を抜ける
		if (lpDis->itemID == -1)
			return (INT_PTR)TRUE;

		// カラーコードの表示オフセット
		LONG lOffsetText = (lpDis->rcItem.bottom - lpDis->rcItem.top) * 3;

		// 色塗り潰し領域設定
		// 右側にカラーコードを表示させるための余白を作る
		RECT rcFill{ lpDis->rcItem };
		rcFill.top += 2;
		rcFill.bottom -= 2;
		rcFill.left += 2;
		rcFill.right = lOffsetText - 2;
		HBRUSH hbrColor = CreateSolidBrush(rgbColoList[lpDis->itemID]);
		FillRect(lpDis->hDC, &rcFill, hbrColor);
		DeleteObject(hbrColor);

		// カラーコード取得
		wchar_t wcColorCode[32]{};
		size_t cch = SendMessageW(lpDis->hwndItem, CB_GETLBTEXT, lpDis->itemID, (LPARAM)wcColorCode);
		if (cch == 0 || cch == CB_ERR)
			wcColorCode[0] = L'\0';

		// 選択状態に合わせて表示する色を設定
		COLORREF crForegroundPrev;
		COLORREF crBackgroundPrev;
		if (lpDis->itemState & ODS_SELECTED)
		{
			crForegroundPrev = SetTextColor(lpDis->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
			crBackgroundPrev = SetBkColor(lpDis->hDC, GetSysColor(COLOR_HIGHLIGHT));
		}
		else
		{
			crForegroundPrev = SetTextColor(lpDis->hDC, GetSysColor(COLOR_WINDOWTEXT));
			crBackgroundPrev = SetBkColor(lpDis->hDC, GetSysColor(COLOR_WINDOW));
		}

		// カラーコードを表示する座標を求める
		TEXTMETRIC tm;
		GetTextMetrics(lpDis->hDC, &tm);
		int x = lOffsetText + 5;
		int y = (lpDis->rcItem.bottom + lpDis->rcItem.top - tm.tmHeight) / 2;

		// ETO_CLIPPED、ETO_OPAQUEが適用される領域
		//   選択時の背景色が、表示されている色部分と重ならないように、
		//   left をオフセットさせる
		RECT rcText{ lpDis->rcItem };
		rcText.left = lOffsetText;

		// 現在選択されているフォント、背景色、およびテキストの色を使用してテキストを描画します。
		// ETO_CLIPPED : テキストが四角形にクリップされます。
		// ETO_OPAQUE  : 現在の背景色を使用して四角形を塗りつぶす必要があります。
		ExtTextOutW(lpDis->hDC,
			x, y,
			ETO_CLIPPED | ETO_OPAQUE,
			&rcText,
			wcColorCode, (UINT)cch, nullptr);

		// 設定した色を元に戻す
		SetTextColor(lpDis->hDC, crForegroundPrev);
		SetBkColor(lpDis->hDC, crBackgroundPrev);

		// フォーカスがあれば、FocusRect を表示する
		if (lpDis->itemState & ODS_FOCUS)
			DrawFocusRect(lpDis->hDC, &rcText);

		return (INT_PTR)TRUE;
	}
	default:
		break;
	}

	return (INT_PTR)FALSE;
}
