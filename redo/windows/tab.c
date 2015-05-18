// 16 may 2015
#include "uipriv_windows.h"

struct tab {
	uiTab t;
	HWND hwnd;
	struct ptrArray *pages;
	void (*baseResize)(uiControl *, intmax_t, intmax_t, intmax_t, intmax_t, uiSizing *);
};

struct tabPage {
	uiControl *control;
	int margined;
};

// utility functions

static LRESULT curpage(struct tab *t)
{
	return SendMessageW(t->hwnd, TCM_GETCURSEL, 0, 0);
}

static void showHidePage(struct tab *t, LRESULT which, int hide)
{
	struct tabPage *page;

	if (which == (LRESULT) (-1))
		return;
	page = ptrArrayIndex(t->pages, struct tabPage *, which);
	if (hide)
		uiControlContainerHide(page->control);
	else {
		uiControlContainerShow(page->control);
		// we only resize the current page, so we have to do this here
		uiControlQueueResize(page->control);
	}
}

// control implementation

static BOOL onWM_COMMAND(uiControl *c, WORD code, LRESULT *lResult)
{
	return FALSE;
}

static BOOL onWM_NOTIFY(uiControl *c, NMHDR *nm, LRESULT *lResult)
{
	return FALSE;
}

static void onDestroy(void *data)
{
	// TODO
}

static void tabPreferredSize(uiControl *c, uiSizing *d, intmax_t *width, intmax_t *height)
{
	// TODO
}

static void tabResize(uiControl *c, intmax_t x, intmax_t y, intmax_t width, intmax_t height, uiSizing *d)
{
	struct tab *t = (struct tab *) c;
	LRESULT n;
	struct tabPage *page;
	RECT r;

	(*(t->baseResize))(uiControl(t), x, y, width, height, d);
	n = curpage(t);
	if (n == (LRESULT) (-1))
		return;
	page = ptrArrayIndex(t->pages, struct tabPage *, n);
	r.left = x;
	r.top = y;
	r.right = x + width;
	r.bottom = y + height;
	// TODO convert from parent window coordinates to screen coordinates
	SendMessageW(t->hwnd, TCM_ADJUSTRECT, (WPARAM) FALSE, (LPARAM) (&r));
	// TODO convert back to parent window coordinates
	if (page->margined) {
		// TODO
	}
	uiControlResize(page->control, x, y, width, height, d);
}

static void tabAppend(uiTab *tt, const char *name, uiControl *child)
{
	struct tab *t = (struct tab *) tt;

	uiTabInsertAt(tt, name, t->pages->len, child);
}

static void tabInsertAt(uiTab *tt, const char *name, uintmax_t n, uiControl *child)
{
	struct tab *t = (struct tab *) tt;
	struct tabPage *page;
	LRESULT hide, show;
	TCITEMW item;
	WCHAR *wname;

	// see below
	hide = curpage(t);

	page = uiNew(struct tabPage);
	page->control = child;
	uiControlSetParent(page->control, uiControl(t));
	ptrArrayInsertAt(t->pages, n, page);

	ZeroMemory(&item, sizeof (TCITEMW));
	item.mask = TCIF_TEXT;
	wname = toUTF16(name);
	item.pszText = wname;
	if (SendMessageW(t->hwnd, TCM_INSERTITEM, (WPARAM) n, (LPARAM) (&item)) == (LRESULT) -1)
		logLastError("error adding tab to uiTab in uiTabInsertAt()");
	uiFree(wname);

	// we need to do this because adding the first tab doesn't send a TCN_SELCHANGE; it just shows the page
	show = curpage(t);
	if (show != hide) {
		showHidePage(t, hide, 1);
		showHidePage(t, show, 0);
	}
}

static void tabDelete(uiTab *tt, uintmax_t n)
{
	// TODO
}

static uintmax_t tabNumPages(uiTab *tt)
{
	// TODO
	return 0;
}

static int tabMargined(uiTab *tt, uintmax_t n)
{
	// TODO
	return 0;
}

static void tabSetMargined(uiTab *tt, uintmax_t n, int margined)
{
	// TODO
}

uiTab *uiNewTab(void)
{
	struct tab *t;
	uiWindowsMakeControlParams p;

	t = uiNew(struct tab);
	uiTyped(t)->Type = uiTypeTab();

	p.dwExStyle = 0;		// don't set WS_EX_CONTROLPARENT yet; we do that dynamically in the message loop (see main_windows.c)
	p.lpClassName = WC_TABCONTROLW;
	p.lpWindowName = L"";
	p.dwStyle = TCS_TOOLTIPS | WS_TABSTOP;		// start with this; we will alternate between this and WS_EX_CONTROLPARENT as needed (see main.c and msgHasTabStops above and the toggling functions below)
	p.hInstance = hInstance;
	p.lpParam = NULL;
	p.useStandardControlFont = TRUE;
	p.onWM_COMMAND = onWM_COMMAND;
	p.onWM_NOTIFY = onWM_NOTIFY;
	p.onDestroy = onDestroy;
	p.onDestroyData = t;
	uiWindowsMakeControl(uiControl(t), &p);

	t->hwnd = (HWND) uiControlHandle(uiControl(t));

	t->pages = newPtrArray();

	uiControl(t)->PreferredSize = tabPreferredSize;
	t->baseResize = uiControl(t)->Resize;
	uiControl(t)->Resize = tabResize;

	uiTab(t)->Append = tabAppend;
	uiTab(t)->InsertAt = tabInsertAt;
	uiTab(t)->Delete = tabDelete;
	uiTab(t)->NumPages = tabNumPages;
	uiTab(t)->Margined = tabMargined;
	uiTab(t)->SetMargined = tabSetMargined;

	return uiTab(t);
}

// unfortunately WS_TABSTOP and WS_EX_CONTROLPARENT are mutually exclusive, so we have to toggle between them
// see main.c for more details

void tabEnterTabNavigation(HWND hwnd)
{
	setStyle(hwnd, getStyle(hwnd) & ~WS_TABSTOP);
	setExStyle(hwnd, getExStyle(hwnd) | WS_EX_CONTROLPARENT);
}

void tabLeaveTabNavigation(HWND hwnd)
{
	setExStyle(hwnd, getExStyle(hwnd) & ~WS_EX_CONTROLPARENT);
	setStyle(hwnd, getStyle(hwnd) | WS_TABSTOP);
}