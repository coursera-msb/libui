// 22 december 2015
#include "test.h"

static void onListFonts(uiButton *b, void *data)
{
	uiDrawFontFamilies *ff;
	char *this;
	uintmax_t i, n;

	uiMultilineEntrySetText(uiMultilineEntry(data), "");
	ff = uiDrawListFontFamilies();
	n = uiDrawFontFamiliesNumFamilies(ff);
	for (i = 0; i < n; i++) {
		this = uiDrawFontFamiliesFamily(ff, i);
		uiMultilineEntryAppend(uiMultilineEntry(data), this);
		uiMultilineEntryAppend(uiMultilineEntry(data), "\n");
		uiFreeText(this);
	}
	uiDrawFreeFontFamilies(ff);
}

static uiEntry *textString;
static uiEntry *textFont;
static uiEntry *textSize;
static uiCombobox *textWeight;
static uiCombobox *textItalic;
static uiCheckbox *textSmallCaps;
static uiCombobox *textStretch;
static uiEntry *textR, *textG, *textB, *textA;
static uiCheckbox *textHasBackground;
static uiEntry *textBR, *textBG, *textBB, *textBA;
static uiCheckbox *textHasStrikethrough;
static uiEntry *textSR, *textSG, *textSB, *textSA;
static uiCheckbox *textHasUnderline;
static uiEntry *textUR, *textUG, *textUB, *textUA;
static uiButton *textApply;
static uiArea *textArea;
static uiAreaHandler textAreaHandler;

static double entryDouble(uiEntry *e)
{
	char *s;
	double d;

	s = uiEntryText(e);
	d = atof(s);
	uiFreeText(s);
	return d;
}

static void handlerDraw(uiAreaHandler *a, uiArea *area, uiAreaDrawParams *dp)
{
	uiDrawTextStyle style;
	char *s;
	char *family;		// make compiler happy

	memset(&style, 0, sizeof (uiDrawTextStyle));
	family = uiEntryText(textFont);
	style.Family = family;
	style.Size = entryDouble(textSize);
	style.Weight = uiComboboxSelected(textWeight);
	style.Italic = uiComboboxSelected(textItalic);
	style.SmallCaps = uiCheckboxChecked(textSmallCaps);
	style.Stretch = uiComboboxSelected(textStretch);
	style.TextR = entryDouble(textR);
	style.TextG = entryDouble(textG);
	style.TextB = entryDouble(textB);
	style.TextA = entryDouble(textA);
	style.HasBackground = uiCheckboxChecked(textHasBackground);
	style.BackgroundR = entryDouble(textBR);
	style.BackgroundG = entryDouble(textBG);
	style.BackgroundB = entryDouble(textBB);
	style.BackgroundA = entryDouble(textBA);
	style.HasStrikethrough = uiCheckboxChecked(textHasStrikethrough);
	style.StrikethroughR = entryDouble(textSR);
	style.StrikethroughG = entryDouble(textSG);
	style.StrikethroughB = entryDouble(textSB);
	style.StrikethroughA = entryDouble(textSA);
	style.HasUnderline = uiCheckboxChecked(textHasUnderline);
	style.UnderlineR = entryDouble(textUR);
	style.UnderlineG = entryDouble(textUG);
	style.UnderlineB = entryDouble(textUB);
	style.UnderlineA = entryDouble(textUA);
	s = uiEntryText(textString);
	uiDrawText(dp->Context, 10, 10, s, &style);
	uiFreeText(s);
	uiFreeText(family);
}

static void handlerMouseEvent(uiAreaHandler *a, uiArea *area, uiAreaMouseEvent *e)
{
	// do nothing
}

static void handlerMouseCrossed(uiAreaHandler *ah, uiArea *a, int left)
{
	// do nothing
}

static void handlerDragBroken(uiAreaHandler *ah, uiArea *a)
{
	// do nothing
}

static int handlerKeyEvent(uiAreaHandler *ah, uiArea *a, uiAreaKeyEvent *e)
{
	// do nothing
	return 0;
}

static void onTextApply(uiButton *b, void *data)
{
	uiAreaQueueRedrawAll(textArea);
}

static void mkRGBA(uiBox *parent, uiCheckbox **has, const char *hasText, uiEntry **r, uiEntry **g, uiEntry **b, uiEntry **a, const char *field)
{
	uiBox *hbox;
	uiBox *vbox;

	hbox = newHorizontalBox();
	uiBoxAppend(parent, uiControl(hbox), 0);

	if (has != NULL) {
		*has = uiNewCheckbox(hasText);
		uiBoxAppend(hbox, uiControl(*has), 0);
	}

	vbox = newVerticalBox();
	uiBoxAppend(hbox, uiControl(vbox), 1);

	hbox = newHorizontalBox();
	uiBoxAppend(vbox, uiControl(hbox), 0);

	*r = uiNewEntry();
	uiEntrySetText(*r, field);
	uiBoxAppend(hbox, uiControl(*r), 1);

	*g = uiNewEntry();
	uiEntrySetText(*g, field);
	uiBoxAppend(hbox, uiControl(*g), 1);

	hbox = newHorizontalBox();
	uiBoxAppend(vbox, uiControl(hbox), 0);

	*b = uiNewEntry();
	uiEntrySetText(*b, field);
	uiBoxAppend(hbox, uiControl(*b), 1);

	*a = uiNewEntry();
	uiEntrySetText(*a, "1.0");
	uiBoxAppend(hbox, uiControl(*a), 1);
}

uiBox *makePage8(void)
{
	uiBox *page8;
	uiGroup *group;
	uiBox *vbox;
	uiMultilineEntry *me;
	uiButton *button;

	page8 = newHorizontalBox();

	group = newGroup("Font Families");
	uiBoxAppend(page8, uiControl(group), 1);

	vbox = newVerticalBox();
	uiGroupSetChild(group, uiControl(vbox));

	me = uiNewMultilineEntry();
	uiBoxAppend(vbox, uiControl(me), 1);

	button = uiNewButton("List Font Families");
	uiButtonOnClicked(button, onListFonts, me);
	uiBoxAppend(vbox, uiControl(button), 0);

	group = newGroup("Text Rendering");
	uiBoxAppend(page8, uiControl(group), 1);

	vbox = newVerticalBox();
	uiGroupSetChild(group, uiControl(vbox));

	textString = uiNewEntry();
	// TODO make it placeholder
	uiEntrySetText(textString, "Enter text here");
	uiBoxAppend(vbox, uiControl(textString), 0);

	textFont = uiNewEntry();
	uiEntrySetText(textFont, "Arial");
	uiBoxAppend(vbox, uiControl(textFont), 0);

	textSize = uiNewEntry();
	uiEntrySetText(textSize, "10");
	uiBoxAppend(vbox, uiControl(textSize), 0);

	textWeight = uiNewCombobox();
	uiComboboxAppend(textWeight, "Thin");
	uiComboboxAppend(textWeight, "Ultra Light");
	uiComboboxAppend(textWeight, "Light");
	uiComboboxAppend(textWeight, "Book");
	uiComboboxAppend(textWeight, "Normal");
	uiComboboxAppend(textWeight, "Medium");
	uiComboboxAppend(textWeight, "Semi Bold");
	uiComboboxAppend(textWeight, "Bold");
	uiComboboxAppend(textWeight, "Utra Bold");
	uiComboboxAppend(textWeight, "Heavy");
	uiComboboxAppend(textWeight, "Ultra Heavy");
	uiComboboxSetSelected(textWeight, uiDrawTextWeightNormal);
	uiBoxAppend(vbox, uiControl(textWeight), 0);

	textItalic = uiNewCombobox();
	uiComboboxAppend(textItalic, "Normal");
	uiComboboxAppend(textItalic, "Oblique");
	uiComboboxAppend(textItalic, "Italic");
	uiComboboxSetSelected(textItalic, uiDrawTextItalicNormal);
	uiBoxAppend(vbox, uiControl(textItalic), 0);

	textSmallCaps = uiNewCheckbox("Small Caps");
	uiBoxAppend(vbox, uiControl(textSmallCaps), 0);

	textStretch = uiNewCombobox();
	uiComboboxAppend(textStretch, "Ultra Condensed");
	uiComboboxAppend(textStretch, "Extra Condensed");
	uiComboboxAppend(textStretch, "Condensed");
	uiComboboxAppend(textStretch, "Semi Condensed");
	uiComboboxAppend(textStretch, "Normal");
	uiComboboxAppend(textStretch, "Semi Expanded");
	uiComboboxAppend(textStretch, "Expanded");
	uiComboboxAppend(textStretch, "Extra Expanded");
	uiComboboxAppend(textStretch, "Ultra Expanded");
	uiComboboxSetSelected(textStretch, uiDrawTextStretchNormal);
	uiBoxAppend(vbox, uiControl(textStretch), 0);

	mkRGBA(vbox, NULL, NULL, &textR, &textG, &textB, &textA, "0.0");
	mkRGBA(vbox, &textHasBackground, "Background", &textBR, &textBG, &textBB, &textBA, "1.0");
	mkRGBA(vbox, &textHasStrikethrough, "Strikethrough", &textSR, &textSG, &textSB, &textSA, "0.0");
	mkRGBA(vbox, &textHasUnderline, "Underline", &textUR, &textUG, &textUB, &textUA, "0.0");

	textApply = uiNewButton("Apply");
	uiButtonOnClicked(textApply, onTextApply, NULL);
	uiBoxAppend(vbox, uiControl(textApply), 0);

	textAreaHandler.Draw = handlerDraw;
	textAreaHandler.MouseEvent = handlerMouseEvent;
	textAreaHandler.MouseCrossed = handlerMouseCrossed;
	textAreaHandler.DragBroken = handlerDragBroken;
	textAreaHandler.KeyEvent = handlerKeyEvent;
	textArea = uiNewArea(&textAreaHandler);
	uiBoxAppend(vbox, uiControl(textArea), 1);

	return page8;
}