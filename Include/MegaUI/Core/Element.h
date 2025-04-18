﻿#pragma once
#include <MegaUI/Core/Value.h>
#include <MegaUI/Core/Property.h>
#include <MegaUI/Core/DeferCycle.h>
#include <MegaUI/Core/ControlInfo.h>
#include <Media/Graphics/DrawContext.h>
#include <MegaUI/Core/UIEvent.h>
#include <MegaUI/Render/Render.h>
#include <Base/Containers/Optional.h>

// Global layout positions
#define LP_None         -3
#define LP_Absolute     -2
#define LP_Auto         -1


// Layout cycle queue modes
#define LC_Pass 0
#define LC_Unknow1 1
#define LC_Normal 2
#define LC_Optimize 3

#pragma pack(push, __YY_PACKING)

namespace YY
{
    namespace MegaUI
    {
        // ActiveProp
        enum class ActiveStyle
        {
            None = 0x00000000,
            Mouse = 0x00000001,
            Keyboard = 0x00000002,
            All = Mouse | Keyboard,
        };

        YY_APPLY_ENUM_CALSS_BIT_OPERATOR(ActiveStyle);

        // BorderStyleProp
        enum class BorderStyle
        {
            // 实心边框
            Solid,
            // 凸起样式边框
            Raised,
            // 凹起样式边框
            Sunken,
            // 圆角样式边框
            Rounded,
        };

        // FlowDirectionProp
        enum class FlowDirection
        {
            // 内容从右到左开始流动。
            LeftToRight,
            // 内容从左到右开始流动。
            RightToLeft,
        };

        // W3C Aria Role类别，用于无障碍用途
        // 具体与 MSAA 以及 UI Automation映射关系来自下述链接
        // https://learn.microsoft.com/en-us/windows/win32/winauto/uiauto-ariaspecification
        enum class AccessibleRole
        {
            // clang-format off
            //         Name,             Aria              MSAA                      UI Automation
#define __ACCESSIBLE_ROLE_TABLE(_APPLY) \
    _APPLY(None,             none,             0,                        0) \
    _APPLY(Custom,           custom,           0,                        UIA_CustomControlTypeId) \
    _APPLY(Window,           custom,           ROLE_SYSTEM_WINDOW,       UIA_WindowControlTypeId) \
    _APPLY(Text,             description,      ROLE_SYSTEM_TEXT,         UIA_TextControlTypeId) \
    _APPLY(Button,           button,           ROLE_SYSTEM_PUSHBUTTON,   UIA_ButtonControlTypeId)

#if 0
            _APPLY(None, none, 0, 0) \
            _APPLY(Custom, custom, 0, UIA_CustomControlTypeId) \
            _APPLY(Alert, alert, ROLE_SYSTEM_ALERT, UIA_TextControlTypeId) \
            _APPLY(AlertDialog, alertdialog, ROLE_SYSTEM_DIALOG, UIA_PaneControlTypeId) \
            _APPLY(Application, application, ROLE_SYSTEM_PANE, UIA_PaneControlTypeId) \
            _APPLY(Article, article, ROLE_SYSTEM_DOCUMENT, UIA_DocumentControlTypeId) \
            _APPLY(Banner, banner, ROLE_SYSTEM_GROUPING, UIA_GroupControlTypeId) \
            _APPLY(Button, button, ROLE_SYSTEM_PUSHBUTTON, UIA_ButtonControlTypeId) \
            _APPLY(CheckBox, checkbox, ROLE_SYSTEM_CHECKBUTTON, UIA_CheckBoxControlTypeId) \
            _APPLY(ColumnHeader, columnheader, ROLE_SYSTEM_COLUMNHEADER, UIA_DataItemControlTypeId) \
            _APPLY(ComboBox, combobox, ROLE_SYSTEM_COMBOBOX, UIA_ComboBoxControlTypeId) \
            _APPLY(Complementary, complementary, ROLE_SYSTEM_GROUPING, UIA_GroupControlTypeId) \
            _APPLY(ContentInfo, contentinfo, ROLE_SYSTEM_GROUPING, UIA_GroupControlTypeId) \
            _APPLY(Definition, definition, ROLE_SYSTEM_GROUPING, UIA_GroupControlTypeId) \
            _APPLY(Description, description, ROLE_SYSTEM_TEXT, UIA_TextControlTypeId) \
            _APPLY(Dialog, dialog, ROLE_SYSTEM_DIALOG, UIA_PaneControlTypeId) \
            _APPLY(Directory, directory, ROLE_SYSTEM_LIST, UIA_ListControlTypeId) \
            _APPLY(Document, document, ROLE_SYSTEM_CLIENT, UIA_DocumentControlTypeId) \
            _APPLY(Form, form, ROLE_SYSTEM_GROUPING, UIA_GroupControlTypeId) \
            _APPLY(Grid, grid, ROLE_SYSTEM_TABLE, UIA_DataGridControlTypeId) \
            _APPLY(GridCell, gridcell, ROLE_SYSTEM_CELL, UIA_DataItemControlTypeId) \
            _APPLY(Group, group, ROLE_SYSTEM_GROUPING, UIA_GroupControlTypeId) \
            _APPLY(Heading, heading, ROLE_SYSTEM_TEXT, UIA_TextControlTypeId) \
            _APPLY(Img, img, ROLE_SYSTEM_GRAPHIC, UIA_ImageControlTypeId) \
            _APPLY(Link, link, ROLE_SYSTEM_LINK, UIA_HyperlinkControlTypeId) \
            _APPLY(List, list, ROLE_SYSTEM_LIST, UIA_ListControlTypeId) \
            _APPLY(ListBox, listbox, ROLE_SYSTEM_LIST, UIA_ListControlTypeId) \
            _APPLY(ListItem, listitem, ROLE_SYSTEM_LISTITEM, UIA_ListItemControlTypeId) \
            _APPLY(Log, log, ROLE_SYSTEM_GROUPING, UIA_GroupControlTypeId) \
            _APPLY(Main, main, ROLE_SYSTEM_GROUPING, UIA_GroupControlTypeId) \
            _APPLY(Marquee, marquee, ROLE_SYSTEM_ANIMATION, UIA_TextControlTypeId) \
            _APPLY(Menu, menu, ROLE_SYSTEM_MENUPOPUP, UIA_MenuControlTypeId) \
            _APPLY(MenuBar, menubar, ROLE_SYSTEM_MENUBAR, UIA_MenuBarControlTypeId) \
            _APPLY(MenuItem, menuitem, ROLE_SYSTEM_MENUITEM, UIA_MenuItemControlTypeId) \
            _APPLY(MenuItemCheckbox, menuitemcheckbox, ROLE_SYSTEM_CHECKBUTTON, UIA_CheckBoxControlTypeId) \
            _APPLY(MenuItemRadio, menuitemradio, ROLE_SYSTEM_RADIOBUTTON, UIA_RadioButtonControlTypeId) \
            _APPLY(Navigation, navigation, ROLE_SYSTEM_GROUPING, UIA_GroupControlTypeId) \
            _APPLY(Note, note, ROLE_SYSTEM_GROUPING, UIA_GroupControlTypeId) \
            _APPLY(Option, option, ROLE_SYSTEM_LISTITEM, UIA_ListItemControlTypeId) \
            _APPLY(Presentation, presentation, ROLE_SYSTEM_PANE, UIA_PaneControlTypeId) \
            _APPLY(ProgressBar, progressbar, ROLE_SYSTEM_PROGRESSBAR, UIA_ProgressBarControlTypeId) \
            _APPLY(Radio, radio, ROLE_SYSTEM_RADIOBUTTON, UIA_RadioButtonControlTypeId) \
            _APPLY(RadioGroup, radiogroup, ROLE_SYSTEM_GROUPING, UIA_GroupControlTypeId) \
            _APPLY(Region, region, ROLE_SYSTEM_PANE, UIA_PaneControlTypeId) \
            _APPLY(Row, row, ROLE_SYSTEM_ROW, UIA_DataItemControlTypeId) \
            _APPLY(Rowheader, rowheader, ROLE_SYSTEM_ROWHEADER, UIA_DataItemControlTypeId) \
            _APPLY(Scrollbar, scrollbar, ROLE_SYSTEM_SCROLLBAR, UIA_ScrollBarControlTypeId) \
            _APPLY(Search, search, ROLE_SYSTEM_GROUPING, UIA_GroupControlTypeId) \
            _APPLY(Section, section, ROLE_SYSTEM_GROUPING, UIA_GroupControlTypeId) \
            _APPLY(Separator, separator, ROLE_SYSTEM_SEPARATOR, UIA_SeparatorControlTypeId) \
            _APPLY(Slider, slider, ROLE_SYSTEM_SLIDER, UIA_SliderControlTypeId) \
            _APPLY(SpinButton, spinbutton, ROLE_SYSTEM_SPINBUTTON, UIA_SpinnerControlTypeId) \
            _APPLY(Status, status, ROLE_SYSTEM_STATUSBAR, UIA_StatusBarControlTypeId) \
            _APPLY(Tab, tab, ROLE_SYSTEM_PAGETAB, UIA_TabItemControlTypeId) \
            _APPLY(TabList, tablist, ROLE_SYSTEM_PAGETABLIST, UIA_TabControlTypeId) \
            _APPLY(TabPanel, tabpanel, ROLE_SYSTEM_PANE, UIA_PaneControlTypeId) \
            _APPLY(TextBox, textbox, ROLE_SYSTEM_TEXT, UIA_DocumentControlTypeId) \
            _APPLY(Timer, timer, ROLE_SYSTEM_CLOCK, UIA_PaneControlTypeId) \
            _APPLY(ToolBar, toolbar, ROLE_SYSTEM_TOOLBAR, UIA_ToolBarControlTypeId) \
            _APPLY(Tooltip, tooltip, ROLE_SYSTEM_TOOLTIP, UIA_ToolTipControlTypeId) \
            _APPLY(Tree, tree, ROLE_SYSTEM_OUTLINE, UIA_TreeControlTypeId) \
            _APPLY(TreeGrid, treegrid, ROLE_SYSTEM_TABLE, UIA_DataGridControlTypeId) \
            _APPLY(TreeItem, treeitem, ROLE_SYSTEM_OUTLINEITEM, UIA_TreeItemControlTypeId)
#endif
            // clang-format on

#define _APPLY(_NAME, ...) _NAME,
            __ACCESSIBLE_ROLE_TABLE(_APPLY)
#undef _APPLY

        };

        // clang-format off
        //     属性名称             属性Flags                                        属性组FLAGS                       DefaultValue函数                         CustomPropertyHandle                      pEnumMaps              BindCache                                                                    ValidValueType
#define _MEGA_UI_ELEMENT_PROPERTY_TABLE(_APPLY) \
    _APPLY(Parent,         PF_LocalOnly | PF_ReadOnly,            PG_AffectsDesiredSize | PG_AffectsLayout,       &Value::CreateElementNull,            &Element::ParentPropHandle,          nullptr, nullptr, _MEGA_UI_PROP_BIND_ELEMENT(YY_UFIELD_OFFSET(Element, pLocParent), 0, 0),       ValueType::Element) \
    _APPLY(Children,       PF_LocalOnly | PF_ReadOnly,            PG_AffectsDesiredSize | PG_AffectsLayout,       nullptr,                              nullptr,                             nullptr, nullptr, _MEGA_UI_PROP_BIND_ELEMENT(YY_UFIELD_OFFSET(Element, vecLocChildren), 0, 0),   ValueType::ElementList) \
    _APPLY(Visible,        PF_TriLevel | PF_Cascade | PF_Inherit, 0,                                              &Value::CreateBoolFalse,              &Element::VisiblePropHandle,         nullptr, nullptr, _MEGA_UI_PROP_BIND_BOOL(0, UFIELD_BITMAP_OFFSET(Element, ElementBits, bSpecVisible), UFIELD_BITMAP_OFFSET(Element, ElementBits, bCmpVisible)), ValueType::boolean) \
    _APPLY(Enabled,        PF_Normal | PF_Cascade | PF_Inherit,   0,                                              &Value::CreateBoolTrue,               &Element::EnabledPropHandle,         nullptr, nullptr, _MEGA_UI_PROP_BIND_BOOL(0, UFIELD_BITMAP_OFFSET(Element, ElementBits, bSpecEnabled), 0), ValueType::boolean) \
    _APPLY(Active,         PF_Normal,                             0,                                              &Value::CreateInt32Zero,              &Element::ActivePropHandle,          nullptr, ActiveEnumMap, _MEGA_UI_PROP_BIND_INT(0, YY_UFIELD_OFFSET(Element, fSpecActive), 0),    ValueType::int32_t) \
    _APPLY(LayoutPos,      PF_Normal | PF_Cascade,                PG_AffectsDesiredSize | PG_AffectsParentLayout, &Value::CreateInt32<LP_Auto>,         nullptr,                             nullptr, LayoutPosEnumMap, _MEGA_UI_PROP_BIND_INT(0, YY_UFIELD_OFFSET(Element, iSpecLayoutPos), 0), ValueType::int32_t) \
    _APPLY(Width,          PF_TriLevel | PF_Cascade,              PG_AffectsDesiredSize,                          &Value::CreateUnit<-1>,               nullptr,                             nullptr, nullptr, _MEGA_UI_PROP_BIND_UNIT_EX(0, 0, 0, 0, YY_UFIELD_OFFSET(Element, CmpWidth), 1), ValueType::Unit) \
    _APPLY(Height,         PF_TriLevel | PF_Cascade,              PG_AffectsDesiredSize,                          &Value::CreateUnit<-1>,               nullptr,                             nullptr, nullptr, _MEGA_UI_PROP_BIND_UNIT_EX(0, 0, 0, 0, YY_UFIELD_OFFSET(Element, CmpHeight), 1), ValueType::Unit) \
    _APPLY(X,              PF_TriLevel,                           0,                                              &Value::CreateUnit<0>,                nullptr,                             nullptr, nullptr, _MEGA_UI_PROP_BIND_UNIT_EX(0, 0, 0, 0, YY_UFIELD_OFFSET(Element, CmpX), 1),    ValueType::Unit) \
    _APPLY(Y,              PF_TriLevel,                           0,                                              &Value::CreateUnit<0>,                nullptr,                             nullptr, nullptr, _MEGA_UI_PROP_BIND_UNIT_EX(0, 0, 0, 0, YY_UFIELD_OFFSET(Element, CmpY), 1),    ValueType::Unit) \
    _APPLY(Location,       PF_LocalOnly | PF_ReadOnly,            PG_AffectsBounds,                               &Value::CreatePointZero,              &Element::LocationPropHandle,        nullptr, nullptr, _MEGA_UI_PROP_BIND_NONE(),                                                     ValueType::Point) \
    _APPLY(Extent,         PF_LocalOnly | PF_ReadOnly,            PG_AffectsLayout | PG_AffectsBounds,            &Value::CreateSizeZero,               &Element::ExtentPropHandle,          nullptr, nullptr, _MEGA_UI_PROP_BIND_NONE(),                                                     ValueType::UnitSize) \
    _APPLY(PosInLayout,    PF_LocalOnly | PF_ReadOnly,            0,                                              &Value::CreatePointZero,              nullptr,                             nullptr, nullptr, _MEGA_UI_PROP_BIND_POINT(YY_UFIELD_OFFSET(Element, LocPosInLayout), 0, 0),     ValueType::Point) \
    _APPLY(SizeInLayout,   PF_LocalOnly | PF_ReadOnly,            0,                                              &Value::CreateSizeZero,               nullptr,                             nullptr, nullptr, _MEGA_UI_PROP_BIND_SIZE_EX(YY_UFIELD_OFFSET(Element, LocSizeInLayout), 1, 0, 0, 0, 0), ValueType::UnitSize) \
    _APPLY(DesiredSize,    PF_LocalOnly | PF_ReadOnly,            PG_AffectsLayout | PG_AffectsParentLayout,      &Value::CreateSizeZero,               nullptr,                             nullptr, nullptr, _MEGA_UI_PROP_BIND_SIZE_EX(YY_UFIELD_OFFSET(Element, LocDesiredSize), 1, 0, 0, 0, 0), ValueType::UnitSize) \
    _APPLY(LastDesiredSizeConstraint, PF_LocalOnly | PF_ReadOnly, 0,                                              &Value::CreateSizeZero,               nullptr,                             nullptr, nullptr, _MEGA_UI_PROP_BIND_SIZE_EX(YY_UFIELD_OFFSET(Element, LocLastDesiredSizeConstraint), 1, 0, 0, 0, 0), ValueType::UnitSize) \
    _APPLY(Layout,         PF_Normal,                             PG_AffectsDesiredSize | PG_AffectsLayout,       &Value::CreateLayoutNull,             nullptr,                             nullptr, nullptr, _MEGA_UI_PROP_BIND_NONE(),                                                     ValueType::Layout) \
    _APPLY(Background,     PF_Normal | PF_Cascade,                PG_AffectsDisplay,                              &Value::CreateColorTransparant,       nullptr,                             nullptr, nullptr, _MEGA_UI_PROP_BIND_NONE(),                                                     ValueType::Color) \
    _APPLY(Foreground,     PF_Normal | PF_Cascade | PF_Inherit,   PG_AffectsDisplay,                              &Value::CreateColorTransparant,       nullptr,                             nullptr, nullptr, _MEGA_UI_PROP_BIND_NONE(),                                                     ValueType::Color) \
    _APPLY(MinSize,        PF_TriLevel | PF_Cascade,              PG_AffectsLayout | PG_AffectsParentLayout | PG_AffectsBounds | PG_AffectsDisplay, &Value::CreateSizeZero, nullptr,         nullptr, nullptr, _MEGA_UI_PROP_BIND_SIZE_EX(0, 0, YY_UFIELD_OFFSET(Element, SpecMinSize), 0, YY_UFIELD_OFFSET(Element, CmpMinSize), 1), ValueType::UnitSize) \
    _APPLY(BorderThickness, PF_TriLevel | PF_Cascade,             PG_AffectsDesiredSize|PG_AffectsDisplay,        &Value::CreateRectZero,               nullptr,                             nullptr, nullptr, _MEGA_UI_PROP_BIND_RECT_EX(0, 0, YY_UFIELD_OFFSET(Element, SpecBorderThickness), 0, YY_UFIELD_OFFSET(Element, CmpBorderThickness), 1), ValueType::UnitRect) \
    _APPLY(BorderStyle,    PF_Normal | PF_Cascade,                PG_AffectsDisplay,                              &Value::CreateInt32Zero,              nullptr,                             nullptr, BorderStyleEnumMap, _MEGA_UI_PROP_BIND_NONE(),                                          ValueType::int32_t ) \
    _APPLY(BorderColor,    PF_Normal | PF_Cascade,                PG_AffectsDisplay,                              &Value::CreateColorTransparant,       nullptr,                             nullptr, nullptr, _MEGA_UI_PROP_BIND_NONE(),                                                     ValueType::Color  ) \
    _APPLY(Padding,        PF_TriLevel | PF_Cascade,              PG_AffectsDisplay | PG_AffectsDesiredSize,      &Value::CreateRectZero,               nullptr,                             nullptr, nullptr, _MEGA_UI_PROP_BIND_RECT_EX(0, 0, YY_UFIELD_OFFSET(Element, SpecPadding), 0, YY_UFIELD_OFFSET(Element, CmpPadding), 1), ValueType::UnitRect) \
    _APPLY(FocusVisible,   PF_Normal | PF_Cascade,                PG_AffectsDesiredSize|PG_AffectsDisplay,        &Value::CreateBoolFalse,              nullptr,                             nullptr, nullptr, _MEGA_UI_PROP_BIND_BOOL(0, UFIELD_BITMAP_OFFSET(Element, ElementBits, bSpecFocusVisible), 0), ValueType::boolean ) \
    _APPLY(FocusThickness, PF_TriLevel | PF_Cascade,              PG_AffectsDesiredSize|PG_AffectsDisplay,        &Value::CreateRectZero,               nullptr,                             nullptr, nullptr, _MEGA_UI_PROP_BIND_RECT_EX(0, 0, YY_UFIELD_OFFSET(Element, SpecFocusThickness), 0, YY_UFIELD_OFFSET(Element, CmpFocusThickness), 1), ValueType::UnitRect) \
    _APPLY(FocusStyle,     PF_Normal | PF_Cascade,                PG_AffectsDisplay,                              &Value::CreateInt32Zero,              nullptr,                             nullptr, BorderStyleEnumMap, _MEGA_UI_PROP_BIND_NONE(),                                          ValueType::int32_t) \
    _APPLY(FocusColor,     PF_Normal | PF_Cascade,                PG_AffectsDisplay,                              &Value::CreateColorTransparant,       nullptr,                             nullptr, nullptr, _MEGA_UI_PROP_BIND_NONE(),                                                     ValueType::Color) \
    _APPLY(FlowDirection,  PF_Normal | PF_Cascade | PF_Inherit,   PG_AffectsLayout | PG_AffectsDisplay,           nullptr,                              nullptr,                             nullptr, FlowDirectionEnumMap, _MEGA_UI_PROP_BIND_INT(0, YY_UFIELD_OFFSET(Element, iSpecFlowDirection), 0), ValueType::int32_t) \
    _APPLY(MouseFocused,   PF_Normal | PF_ReadOnly | PF_Inherit,  0,                                              &Value::CreateBoolFalse,              &Element::MouseFocusedPropHandle,    nullptr, nullptr, _MEGA_UI_PROP_BIND_BOOL(UFIELD_BITMAP_OFFSET(Element, ElementBits, bLocMouseFocused), UFIELD_BITMAP_OFFSET(Element, ElementBits, bSpecMouseFocused), 0), ValueType::boolean) \
    _APPLY(MouseFocusWithin, PF_LocalOnly | PF_ReadOnly,          0,                                              &Value::CreateBoolFalse,              &Element::MouseFocusWithinPropHandle,nullptr, nullptr, _MEGA_UI_PROP_BIND_NONE(),                                                     ValueType::boolean ) \
    _APPLY(KeyboardFocused,PF_Normal | PF_ReadOnly | PF_Inherit,  0,                                              &Value::CreateBoolFalse,              &Element::KeyboardFocusedPropHandle, nullptr, nullptr, _MEGA_UI_PROP_BIND_BOOL(UFIELD_BITMAP_OFFSET(Element, ElementBits, bLocKeyboardFocused), UFIELD_BITMAP_OFFSET(Element, ElementBits, bSpecKeyboardFocused), 0), ValueType::boolean) \
    _APPLY(KeyboardFocusWithin, PF_LocalOnly | PF_ReadOnly,       0,                                              &Value::CreateBoolFalse,              &Element::KeyboardFocusWithinPropHandle, nullptr, nullptr, _MEGA_UI_PROP_BIND_NONE(),                                                 ValueType::boolean ) \
    _APPLY(Focused,        PF_Normal | PF_ReadOnly | PF_Inherit,  0,                                              &Value::CreateBoolFalse,              &Element::FocusedPropHandle,         nullptr, nullptr, _MEGA_UI_PROP_BIND_BOOL(UFIELD_BITMAP_OFFSET(Element, ElementBits, bLocFocused), UFIELD_BITMAP_OFFSET(Element, ElementBits, bSpecFocused), 0), ValueType::boolean) \
    _APPLY(FocusWithin,    PF_LocalOnly | PF_ReadOnly,            0,                                              &Value::CreateBoolFalse,              &Element::FocusWithinPropHandle,     nullptr, nullptr, _MEGA_UI_PROP_BIND_NONE(),                                                     ValueType::boolean) \
    _APPLY(Id,             PF_Normal,                             0,                                              &Value::CreateAtomZero,               nullptr,                             nullptr, nullptr, _MEGA_UI_PROP_BIND_ATOM(0, YY_UFIELD_OFFSET(Element, SpecID), 0),              ValueType::ATOM) \
    _APPLY(Sheet,          PF_Normal|PF_Inherit,                  0,                                              &Value::CreateSheetNull,              &Element::SheetPropHandle,           nullptr, nullptr, _MEGA_UI_PROP_BIND_SHEET(0, YY_UFIELD_OFFSET(Element, pSheet), 0),             ValueType::StyleSheet) \
    _APPLY(Class,          PF_Normal,                             0,                                              &Value::CreateEmptyString,            nullptr,                             nullptr, nullptr, _MEGA_UI_PROP_BIND_NONE(),                                                     ValueType::String) \
    _APPLY(Content,        PF_Normal | PF_Cascade,                PG_AffectsDesiredSize|PG_AffectsDisplay,        nullptr,                              nullptr,                             nullptr, nullptr, _MEGA_UI_PROP_BIND_NONE(),                                                     ValueType::String) \
    _APPLY(ContentAlign,   PF_Normal | PF_Cascade,                PG_AffectsDesiredSize|PG_AffectsDisplay,        &Value::CreateInt32Zero,              nullptr,                             nullptr, ContentAlignEnumMap, _MEGA_UI_PROP_BIND_INT(0, YY_UFIELD_OFFSET(Element, fSpecContentAlign), 0), ValueType::int32_t ) \
    _APPLY(FontFamily,     PF_Normal | PF_Cascade | PF_Inherit,   PG_AffectsDesiredSize|PG_AffectsDisplay,        &Value::CreateDefaultFontFamily,      nullptr,                             nullptr, nullptr, _MEGA_UI_PROP_BIND_STRING(0, YY_UFIELD_OFFSET(Element, SpecFont.szFace), 0),   ValueType::String) \
    _APPLY(FontSize,       PF_TriLevel | PF_Cascade | PF_Inherit, PG_AffectsDesiredSize|PG_AffectsDisplay,        &Value::CreateUnit<20>,               nullptr,                             nullptr, nullptr, _MEGA_UI_PROP_BIND_UNIT_EX(0, 0, YY_UFIELD_OFFSET(Element, SpecFontSize), 0, YY_UFIELD_OFFSET(Element, SpecFont.iSize), 1), ValueType::Unit) \
    _APPLY(FontWeight,     PF_Normal | PF_Cascade | PF_Inherit,   PG_AffectsDesiredSize|PG_AffectsDisplay,        &Value::CreateInt32<FontWeight::Normal>, nullptr,                          nullptr, FontWeightEnumMaps, _MEGA_UI_PROP_BIND_INT(0, YY_UFIELD_OFFSET(Element, SpecFont.uWeight), 0), ValueType::int32_t) \
    _APPLY(FontStyle,      PF_Normal | PF_Cascade | PF_Inherit,   PG_AffectsDisplay,                              &Value::CreateInt32<(int32_t)FontStyle::None>, nullptr,                    nullptr, FontStyleEnumMap, _MEGA_UI_PROP_BIND_INT(0, YY_UFIELD_OFFSET(Element, SpecFont.fStyle), 0), ValueType::int32_t) \
    _APPLY(Dpi,            PF_LocalOnly | PF_ReadOnly,            0,                                              &Value::CreateDefaultSystemDpi,       &Element::DpiPropHandle,             nullptr, nullptr, _MEGA_UI_PROP_BIND_INT(YY_UFIELD_OFFSET(Element, LocUnitMetrics.uDpi), 0, 0),  ValueType::int32_t) \
    _APPLY(TextScale,      PF_LocalOnly | PF_ReadOnly,            0,                                              &Value::CreateFloat<1>,               &Element::TextScalePropHandle,       nullptr, nullptr, _MEGA_UI_PROP_BIND_FLOAT(YY_UFIELD_OFFSET(Element, LocUnitMetrics.nTextScale), 0, 0), ValueType::float_t) \
    _APPLY(Accessible,     PF_Normal | PF_Cascade,                0,                                              &Value::CreateBoolFalse,              nullptr,                             nullptr, nullptr, _MEGA_UI_PROP_BIND_BOOL(0, UFIELD_BITMAP_OFFSET(Element, ElementBits, bSpecAccessible), 0), ValueType::boolean) \
    _APPLY(AccRole,        PF_Normal | PF_Cascade,                0,                                              &Value::CreateInt32Zero,              nullptr,                             nullptr, AccRoleEnumMap, _MEGA_UI_PROP_BIND_NONE(),                                              ValueType::int32_t) \
    _APPLY(AccName,        PF_Normal | PF_Cascade,                0,                                              &Value::CreateEmptyString,            nullptr,                             nullptr, nullptr, _MEGA_UI_PROP_BIND_NONE(),                                                     ValueType::String) \
    _APPLY(AccHelp,        PF_Normal | PF_Cascade,                0,                                              &Value::CreateEmptyString,            nullptr,                             nullptr, nullptr, _MEGA_UI_PROP_BIND_NONE(),                                                     ValueType::String) \
    _APPLY(AccDescription, PF_Normal | PF_Cascade,                0,                                              &Value::CreateEmptyString,            nullptr,                             nullptr, nullptr, _MEGA_UI_PROP_BIND_NONE(),                                                     ValueType::String) \
    _APPLY(AccItemType,    PF_Normal | PF_Cascade,                0,                                              &Value::CreateEmptyString,            nullptr,                             nullptr, nullptr, _MEGA_UI_PROP_BIND_NONE(),                                                     ValueType::String) \
    _APPLY(AccItemStatus,  PF_Normal | PF_Cascade,                0,                                              &Value::CreateEmptyString,            nullptr,                             nullptr, nullptr, _MEGA_UI_PROP_BIND_NONE(),                                                     ValueType::String) \
    _APPLY(Pressed,        PF_Normal,                             0,                                              &Value::CreateBoolFalse,              nullptr,                             nullptr, nullptr, _MEGA_UI_PROP_BIND_NONE(),                                                     ValueType::boolean) \
    _APPLY(Cursor,         PF_Normal,                             0,                                              &Value::CreateBoolFalse,              nullptr,                             nullptr, nullptr, _MEGA_UI_PROP_BIND_NONE(),                                                     ValueType::boolean) 

// clang-format on

        class NativeWindowHost;
        class Window;
        class UIParser;
        class ElementAccessibleProvider;

        struct NavReference
        {
            uint32_t cbSize;
            Element* pElement;
            Rect* pRect;

            NavReference(Element* _pElement, Rect* _pRect = nullptr)
                : cbSize(sizeof(NavReference))
                , pElement(_pElement)
                , pRect(_pRect)
            {
            }
        };

        class Element
        {
            _APPLY_MEGA_UI_STATIC_CONTROL_INFO_EXTERN(Element, void, ControlInfoImp<Element>, 0u, _MEGA_UI_ELEMENT_PROPERTY_TABLE);
            friend NativeWindowHost;
            friend StyleSheet;
            friend Window;

        protected:
            ElementRenderNode RenderNode;
            // _pvmLocal
            // 所有 Local 值的
            Array<Value> LocalPropValue;

            ElementList vecLocChildren;

            // 0x8 Index
            int32_t iIndex = -1;

            // 正常优先级的组 0x10
            int32_t _iGCSlot;
            // 低优先级的组 0x14
            int32_t _iGCLPSlot;
            // v18
            int32_t _iPCTail;

            // StartDefer 时保存的内容
            RefPtr<DeferCycle> pDeferObject;

            // 顶层 Element，所有的Defer将发送到顶层 Element
            Element* pTopLevel;

            // Local Parent
            Element* pLocParent;

            // 0x2C
            // Position in layout local
            Point LocPosInLayout;
            // 0x34 LayoutSize
            // Size in layout local
            Size LocSizeInLayout;
            // 0x3C _sizeLocLastDSConst
            // Last desired size constraint local
            Size LocLastDesiredSizeConstraint;
            // 0x44 DesiredSize
            // Desired size local
            Size LocDesiredSize;

            // 0x4C LayoutPos
            // Cached Layout Position
            int32_t iSpecLayoutPos;

            // 0x50
            RefPtr<StyleSheet> pSheet;

            // 0x58 ID
            ATOM SpecID;

            ActiveStyle fSpecActive = ActiveStyle::None;

            ElementAccessibleProvider* pAccessibleProvider = nullptr;

            //bits
#define _MEGA_UI_ELEMENT_BITS_TABLE(_APPLY) \
    _APPLY(bSelfLayout, 1)                  \
    _APPLY(bNeedsDSUpdate, 1)               \
    /* UINT8 LayoutType : 2;*/              \
    _APPLY(fNeedsLayout, 2)                 \
    _APPLY(uLocMouseFocusWithin, 2)         \
    _APPLY(bDestroy, 1)                     \
    _APPLY(bSpecVisible, 1)                 \
    _APPLY(bCmpVisible, 1)                  \
    _APPLY(bSpecEnabled, 1)                 \
    _APPLY(bHasLocMouseFocused, 1)          \
    _APPLY(bLocMouseFocused, 1)             \
    _APPLY(bSpecMouseFocused, 1)            \
    _APPLY(bSpecFocusVisible, 1)            \
    _APPLY(uLocKeyboardFocusWithin, 2)      \
    _APPLY(bHasLocKeyboardFocused, 1)       \
    _APPLY(bLocKeyboardFocused, 1)          \
    _APPLY(bSpecKeyboardFocused, 1)         \
    _APPLY(uLocFocusWithin, 2)              \
    _APPLY(bHasLocFocused, 1)               \
    _APPLY(bLocFocused, 1)                  \
    _APPLY(bSpecFocused, 1)                 \
    _APPLY(bSpecAccessible, 1)


            _APPLY_MEGA_UI_BITMAP_TABLE(ElementBits, _MEGA_UI_ELEMENT_BITS_TABLE);
            //union
            //{
            //    struct
            //    {
            //        // 0
            //        uint32_t bSelfLayout : 1;
            //        // 1
            //        uint32_t bNeedsDSUpdate : 1;

            //        // UINT8 LayoutType : 2;
            //        // 2
            //        uint32_t fNeedsLayout : 2;
            //        // 4
            //        uint32_t bLocMouseWithin : 1;
            //    };

            //    uint8_t BitsBuffer[1];
            //};

            float CmpWidth = -1;
            float CmpHeight = -1;
            float CmpX = 0;
            float CmpY = 0;

            // 边框宽度，四个方向，左上右下
            UnitRect SpecBorderThickness;
            Rect CmpBorderThickness;
            // 内边距
            UnitRect SpecPadding;
            Rect CmpPadding;
            // 边框宽度，四个方向，左上右下
            UnitRect SpecFocusThickness;
            Rect CmpFocusThickness;

            //Layout* pLayout = nullptr;

            // 最小限制
            UnitSize SpecMinSize;
            Size CmpMinSize;
            FlowDirection iSpecFlowDirection = FlowDirection::LeftToRight;
            ContentAlignStyle fSpecContentAlign = ContentAlignStyle::Top | ContentAlignStyle::Left;
            // 当前DPI值
            UnitMetrics LocUnitMetrics;

            // 承载控件的窗口
            Window* pWindow = nullptr;

            // 缓存的字体信息
            Font SpecFont;
            Unit SpecFontSize;

            RefPtr<IDWriteTextLayout> pTextLayout;

        public:
            Element();

            virtual ~Element();

            // 此函数来自 _APPLY_MEGA_UI_STATIC_CALSS_INFO_EXTERN
            // static HRESULT __YYAPI Create(_In_ uint32_t _fCreate, _In_opt_ Element* _pTopLevel, _Out_opt_ intptr_t* _pCooike, _Outptr_ Element** _ppOut);

            HRESULT __YYAPI Initialize(_In_ const UnitMetrics& _oUnitMetrics, _In_ uint32_t _fCreate, _In_opt_ Element* _pTopLevel, _Out_opt_ intptr_t* _pCooike);

            /// <summary>
            /// 根据属性获取Value
            /// </summary>
            /// <param name="_Prop"></param>
            /// <param name="_eIndicies"></param>
            /// <param name="_bUpdateCache">如果为true，那么重新获取值并刷新缓存，如果为 false则直接从缓存返回数据。</param>
            /// <returns>如果返回，则返回 Unavailable。
            /// 如果未设置，则返回 Unset</returns>
            Value __YYAPI GetValue(_In_ const PropertyInfo& _Prop, _In_ PropertyIndicies _eIndicies = PropertyIndicies::PI_Specified, _In_ bool _bUpdateCache = false);

            /// <summary>
            /// 修改 Local Value
            /// </summary>
            /// <param name="_Prop">元数据</param>
            /// <param name="_pValue">需要设置的新值</param>
            /// <returns></returns>
            HRESULT __YYAPI SetValue(_In_ const PropertyInfo& _Prop, _In_ const Value& _Value);

            HRESULT __YYAPI SetValueInternal(_In_ const PropertyInfo& _Prop, _In_ const Value& _Value, _In_ bool _bCanCancel = false);

            _Ret_maybenull_ Element* __YYAPI GetParent();

            int32_t __YYAPI GetLayoutPos();
            HRESULT __YYAPI SetLayoutPos(int32_t _iLayoutPos);

            Unit __YYAPI GetWidth();
            HRESULT __YYAPI SetWidth(const Unit& _iWidth);

            Unit __YYAPI GetHeight();
            HRESULT __YYAPI SetHeight(const Unit& _iHeight);

            Unit __YYAPI GetX();
            HRESULT __YYAPI SetX(const Unit& _iX);

            Unit __YYAPI GetY();
            HRESULT __YYAPI SetY(const Unit& _iY);

            Point __YYAPI GetLocation();

            Size __YYAPI GetExtent();

            ValueIs<ValueType::Layout> __YYAPI GetLayout();

            BorderStyle __YYAPI GetBorderStyle();

            HRESULT __YYAPI SetBorderStyle(BorderStyle _eBorderStyle);

            Color __YYAPI GetBorderColor();

            HRESULT __YYAPI SetBorderColor(Color _BorderColor);

            BorderStyle __YYAPI GetFocusBorderStyle();

            HRESULT __YYAPI SetFocusBorderStyle(BorderStyle _eBorderStyle);

            Color __YYAPI GetFocusBorderColor();

            HRESULT __YYAPI SetFocusBorderColor(Color _BorderColor);

            /// <summary>
            /// 获取内容流动方式，从左到右还是从右到左。
            /// </summary>
            /// <returns></returns>
            FlowDirection __YYAPI GetFlowDirection();

            HRESULT __YYAPI SetFlowDirection(FlowDirection _eFlowDirection);

            Rect __YYAPI ApplyFlowDirection(const Rect& _Src);

            bool __YYAPI IsMouseFocusWithin();

            /// <summary>
            /// 返回控件绑定的 Class，它用于 Sheet 表达式匹配。
            /// </summary>
            /// <returns>Class</returns>
            uString __YYAPI GetClass();

            HRESULT __YYAPI SetClass(uString _szClass);

            /// <summary>
            /// 控件是否被禁用。
            /// </summary>
            /// <returns></returns>
            bool __YYAPI IsEnabled();

            HRESULT __YYAPI SetEnabled(bool _bEnabled);

            /// <summary>
            /// 返回控件是否需要主动处理鼠标或者键盘的焦点状态。
            /// </summary>
            /// <returns>Active的位组合</returns>
            ActiveStyle __YYAPI GetActive();

            /// <summary>
            /// 设置控件需要主动处理是焦点状态。比如设置鼠标后可以主动处理鼠标焦点。
            /// </summary>
            /// <param name="_fActive">Active的位组合</param>
            /// <returns>HRESULT</returns>
            HRESULT __YYAPI SetActive(ActiveStyle _fActive);

            bool __YYAPI IsMouseFocused();

            int32_t __YYAPI GetDpi();

            float __YYAPI GetTextScale() const noexcept;

            uString __YYAPI GetFontFamily();

            HRESULT __YYAPI SetFontFamily(uString _szFontFamily);

            Unit __YYAPI GetFontSize();

            HRESULT __YYAPI SetFontSize(const Unit& _iFontSize);

            uint32_t __YYAPI GetFontWeight();

            HRESULT __YYAPI SetFontWeight(uint32_t _iFontWeight);

            FontStyle __YYAPI GetFontStyle();

            HRESULT __YYAPI SetFontStyle(FontStyle _fFontStyle);

            ContentAlignStyle __YYAPI GetContentAlign();

            /// <summary>
            /// 当属性正在更改时调用，可以终止属性更改。
            /// </summary>
            /// <returns>如果返回 true，那么允许更改。如果返回false，更改将被撤销。</returns>
            virtual bool __YYAPI OnPropertyChanging(_In_ const PropertyInfo& _Prop, _In_ PropertyIndicies _eIndicies, _In_ const Value& _OldValue, _In_ const Value& _NewValue);

            virtual void __YYAPI OnPropertyChanged(_In_ const PropertyInfo& _Prop, _In_ PropertyIndicies _eIndicies, _In_ const Value& _OldValue, _In_ const Value& _NewValue);

            /// <summary>
            /// 用于通知 PropertyGroup 的状态
            /// </summary>
            /// <param name="_fGroups">PropertyGroup的组合</param>
            void __YYAPI OnGroupChanged(uint32_t _fGroups);

            /// <summary>
            /// 获取顶层 Element，便于 StartDefer，如果未设置 pTopLevel，则自身为顶层 Element
            /// </summary>
            /// <returns></returns>
            _Ret_notnull_ Element* __YYAPI GetTopLevel();
            /*_Ret_maybenull_*/ RefPtr<DeferCycle> __YYAPI GetDeferObject(_In_ bool _bAllowCreate = true);
            void __YYAPI StartDefer(_Out_ intptr_t* _pCooike);
            void __YYAPI EndDefer(_In_ intptr_t _Cookie);

            HRESULT __YYAPI SetVisible(bool bVisible);

            bool __YYAPI IsVisible();

            ElementList __YYAPI GetChildren();

            virtual HRESULT __YYAPI Insert(_In_reads_(_cChildren) Element* const* _ppChildren, _In_ uint_t _cChildren, _In_ uint_t _uInsert);

            __inline HRESULT __YYAPI Add(_In_reads_(_cChildren) Element* const* _ppChildren, _In_ uint_t _cChildren)
            {
                return Insert(_ppChildren, _cChildren, vecLocChildren.GetSize());
            }

            __inline HRESULT __YYAPI Add(_In_ Element* _ppChildren)
            {
                return Insert(&_ppChildren, 1, vecLocChildren.GetSize());
            }

            virtual HRESULT __YYAPI Remove(_In_reads_(_cChildren) Element* const* _ppChildren, _In_ uint_t _cChildren);

            __inline HRESULT __YYAPI Remove(_In_ Element* _pChild)
            {
                return Remove(&_pChild, 1);
            }

            __inline HRESULT __YYAPI RemoveAll()
            {
                return Remove(vecLocChildren.GetData(), vecLocChildren.GetSize());
            }

            /// <summary>
            /// 当销毁控制时被调用。
            /// </summary>
            /// <returns></returns>
            virtual void __YYAPI OnDestroy();

            /// <summary>
            /// 释放控件相关资源，包含子控件。
            /// </summary>
            /// <param name="_fDelayed">是否延迟删除。</param>
            /// <returns></returns>
            HRESULT __YYAPI Destroy(_In_ bool _fDelayed = true);

            /// <summary>
            /// 释放子控件的相关资源（不会释放控件自己）。
            /// </summary>
            /// <param name="_fDelayed">是否延迟删除。</param>
            /// <returns></returns>
            HRESULT __YYAPI DestroyAllChildren(_In_ bool _fDelayed = true);

            virtual void __YYAPI Paint(_In_ DrawContext* _pDrawContext, _In_ const Rect& _Bounds);

            void __YYAPI PaintBorder(_In_ DrawContext* _pDrawContext, _In_ BorderStyle _eBorderStyle, _In_ const Rect& _BorderThickness, Color _BorderColor, _Inout_ Rect& _Bounds);

            void __YYAPI PaintBackground(_In_ DrawContext* _pDrawContext, const Value& _Background, _In_ const Rect& _Bounds);

            virtual void __YYAPI PaintContent(
                _In_ DrawContext* _pDrawContext,
                _In_ const Value& _Content,
                _In_ const Font& _FontInfo,
                _In_ Color _ForegroundColor,
                _In_ const Rect& _Bounds,
                _In_ ContentAlignStyle _fContentAlign
                );

            virtual Size __YYAPI GetContentSize(Size _ConstraintSize);
            virtual Size __YYAPI SelfLayoutUpdateDesiredSize(Size _ConstraintSize);
            virtual void __YYAPI SelfLayoutDoLayout(Size _ConstraintSize);

            void __YYAPI Detach(DeferCycle* _pDeferCycle);

            void __YYAPI Invalidate();


            template<typename _Type>
            _Type* __YYAPI TryCast()
            {
                auto _pControlInfo = GetControlInfo();
                if (!_pControlInfo)
                    return nullptr;

                if (!_pControlInfo->IsSubclassOf(_Type::GetStaticControlInfo()))
                    return nullptr;

                return (_Type*)this;
            }

            /// <summary>
            /// 只比较二个控件的SpecCache值是否相等。注意这个函数只是为了性能需要，并且经常失败……
            /// </summary>
            /// <param name="_pElement1"></param>
            /// <param name="_pElement2"></param>
            /// <param name="_Prop"></param>
            /// <returns>返回1表示相等，返回0表示不相等，返回 -1 表示比较失败。</returns>
            static int32_t __YYAPI SpecCacheIsEqual(
                _In_ Element* _pElement1,
                _In_ Element* _pElement2,
                _In_ const PropertyInfo& _Prop
            );

            /// <summary>
            /// 从 _pFrom，获取父节点 = this 的子节点
            /// </summary>
            /// <param name="_pFrom"></param>
            /// <returns>如果没有这样的节点则返回 nullptr。</returns>
            _Ret_maybenull_ Element* __YYAPI GetImmediateChild(_In_opt_ Element* _pFrom);

            bool __YYAPI IsKeyboardFocus();

            /// <summary>
            /// 设置键盘焦点。
            /// 注意：控件需要Visible、未禁用，且拥有 ActiveStyle::Keyboard，否则将失败。
            /// 注意：因为键盘焦点属于物理焦点，设置键盘焦点时同时也将改变逻辑焦点。
            /// </summary>
            /// <returns>如果设置成功，则返回 true。</returns>
            bool __YYAPI SetKeyboardFocus();

            /// <summary>
            /// 设置逻辑焦点。
            /// 注意：控件需要Visible、未禁用，且拥有 ActiveStyle::Keyboard 或者 ActiveStyle::Mouse，否则将失败。
            /// 注意：如果逻辑焦点与键盘焦点不同，那么将清除逻辑焦点。
            /// </summary>
            /// <returns>如果设置成功，则返回 true。</returns>
            bool __YYAPI SetFocus();

            /// <summary>
            /// 内容是否被保护。如果被保护那么不应该直接暴露内容。
            /// </summary>
            bool __YYAPI IsContentProtected();

            /// <summary>
            /// 获取键盘快键（只可能是ASCII字符）。
            /// </summary>
            /// <returns></returns>
            achar_t __YYAPI GetShortcutChar();

            ATOM __YYAPI GetId();

            Window* __YYAPI GetWindow();

            /// <summary>
            /// 是否允许无障碍接口将无法感知到其存在。
            /// </summary>
            /// <returns>如果为 true，那么允许无障碍设施访问。反之不允许。</returns>
            bool __YYAPI IsAccessible();

            AccessibleRole __YYAPI GetAccRole();

            uString __YYAPI GetAccName();

            uString __YYAPI GetAccNameAsDisplayed();

            uString __YYAPI GetContentStringAsDisplayed();

            uString __YYAPI GetAccHelp();

            uString __YYAPI GetAccDescription();

            uString __YYAPI GetAccItemType();

            uString __YYAPI GetAccItemStatus();

            virtual HRESULT __YYAPI GetAccessibleProvider(_Outptr_ ElementAccessibleProvider** _ppAccessibleProvider);

            virtual HRESULT __YYAPI DefaultAction();

            void __YYAPI ElementToWindow(_Inout_ Point* _poPoint);

            void __YYAPI WindowToElement(_Inout_ Point* _poPoint);

        protected:
            RefPtr<IDWriteTextLayout> __YYAPI GetTextLayout(Size _ConstraintSize);

            // Value Update
            HRESULT __YYAPI PreSourceChange(_In_ const PropertyInfo& _Prop, _In_ PropertyIndicies _eIndicies, _In_ const Value& _OldValue, _In_ const Value& _NewValue);
            HRESULT __YYAPI PostSourceChange();
            HRESULT __YYAPI GetDependencies(_In_ const PropertyInfo& _Prop, _In_ PropertyIndicies _eIndicies, DepRecs* pdr, int iPCSrcRoot, const Value& _NewValue, DeferCycle* _pDeferCycle);

            static HRESULT __YYAPI AddDependency(Element* _pElement, const PropertyInfo& _Prop, PropertyIndicies _eIndicies, DepRecs* pdr, DeferCycle* _pDeferCycle);

            HRESULT __YYAPI GetBuriedSheetDependencies(const PropertyInfo* _pProp, Element* _pElement, DepRecs* _pDR, DeferCycle* _pDeferCycle);

            static void __YYAPI VoidPCNotifyTree(int, DeferCycle*);

            /// <summary>
            /// 通用处理程序，可以处理属性的默认缓存行为。
            /// </summary>
            /// <param name="_eType"></param>
            /// <param name="_pHandleData"></param>
            /// <returns>如果被处理，则返回 true。</returns>
            bool __YYAPI GeneralHandle(_In_ CustomPropertyHandleType _eType, _Inout_ CustomPropertyBaseHandleData* _pHandleData);

            template<typename _HandleType>
            bool __YYAPI GeneralHandle(_Inout_ _HandleType* _pHandleData)
            {
                return GeneralHandle(_HandleType::HandleType, _pHandleData);
            }

            Value __YYAPI GetGeneralCacheValue(ValueType _eType, uint16_t _uOffsetToCache, uint8_t _uCacheBit);

            bool __YYAPI GetGeneralLocalValue(_Inout_ GetValueHandleData* _pHandleData);

            bool __YYAPI GetGeneralSpecifiedValue(_Inout_ GetValueHandleData* _pHandleData);

            bool __YYAPI GetGeneralComputedValue(_Inout_ GetValueHandleData* _pHandleData);

            bool __YYAPI SetGeneralCacheValue(
                ValueType _eType,
                uint16_t _uOffsetToCache,
                uint8_t _uCacheBit,
                Value _NewValue);

            bool __YYAPI SetGeneralLocalValue(_Inout_ SetValueHandleData* _pHandleData);

            bool __YYAPI SetGeneralSpecifiedValue(_Inout_ SetValueHandleData* _pHandleData);

            bool __YYAPI SetGeneralComputedValue(_Inout_ SetValueHandleData* _pHandleData);

            bool __YYAPI GetGeneralFastSpecValueCompare(_Inout_ FastSpecValueCompareHandleData* _pHandleData);

            bool __YYAPI ParentPropHandle(_In_ CustomPropertyHandleType _eType, _Inout_ CustomPropertyBaseHandleData* _pHandleData);

            virtual bool __YYAPI OnParentPropChanged(_In_ OnPropertyChangedHandleData* _pHandleData);

            virtual bool __YYAPI GetParentPropDependencies(GetDependenciesHandleData* _pHandleData);

            bool __YYAPI VisiblePropHandle(_In_ CustomPropertyHandleType _eType, _Inout_ CustomPropertyBaseHandleData* _pHandleData);

            virtual bool __YYAPI OnVisiblePropChanged(_In_ OnPropertyChangedHandleData* _pHandle);

            virtual bool __YYAPI GetVisiblePropDependencies(_In_ GetDependenciesHandleData* _pHandleData);

            bool __YYAPI GetVisiblePropValue(_In_ GetValueHandleData* _pHandleData);

            bool __YYAPI SetVisiblePropValue(_In_ SetValueHandleData* _pHandleData);

            bool __YYAPI EnabledPropHandle(_In_ CustomPropertyHandleType _eType, _Inout_ CustomPropertyBaseHandleData* _pHandleData);

            virtual bool __YYAPI OnEnabledPropChanged(_In_ OnPropertyChangedHandleData* _pHandle);

            bool __YYAPI ActivePropHandle(_In_ CustomPropertyHandleType _eType, _Inout_ CustomPropertyBaseHandleData* _pHandleData);

            virtual bool __YYAPI OnActivePropChanged(_In_ OnPropertyChangedHandleData* _pHandle);

            bool __YYAPI DpiPropHandle(_In_ CustomPropertyHandleType _eType, _Inout_ CustomPropertyBaseHandleData* _pHandleData);

            bool __YYAPI TextScalePropHandle(_In_ CustomPropertyHandleType _eType, _Inout_ CustomPropertyBaseHandleData* _pHandleData);

            virtual bool __YYAPI OnDpiPropChanged(_In_ OnPropertyChangedHandleData* _pHandleData);

            bool __YYAPI GetAnyScalePropDependencies(_In_ GetDependenciesHandleData* _pHandleData);

            void __YYAPI FlushDesiredSize(DeferCycle* _pDeferCycle);

            void __YYAPI FlushLayout(DeferCycle* _pDeferCycle);

            static bool __YYAPI SetGroupChanges(Element* pElement, uint32_t _fGroups, DeferCycle* pDeferCycle);

            static void __YYAPI TransferGroupFlags(Element* pElement, uint32_t _fGroups);

            static bool __YYAPI MarkElementForDesiredSize(Element* _pElement);

            static bool __YYAPI MarkElementForLayout(Element* _pElement, uint32_t _fNeedsLayoutNew);

            bool __YYAPI SetNeedsLayout(uint32_t _fNeedsLayoutNew);

            Size __YYAPI UpdateDesiredSize(Size _ConstraintSize);

            void __YYAPI UpdateLayoutPosition(Point _LayoutPosition);

            void __YYAPI UpdateLayoutSize(Size _LayoutSize);

            bool __YYAPI ExtentPropHandle(_In_ CustomPropertyHandleType _eType, _Inout_ CustomPropertyBaseHandleData* _pHandleData);

            bool __YYAPI GetExtentPropValue(_Inout_ GetValueHandleData* _pHandleData);

            bool __YYAPI LocationPropHandle(_In_ CustomPropertyHandleType _eType, _Inout_ CustomPropertyBaseHandleData* _pHandleData);

            bool __YYAPI GetLocationPropValue(_Inout_ GetValueHandleData* _pHandleData);

            bool __YYAPI MouseFocusedPropHandle(_In_ CustomPropertyHandleType _eType, _Inout_ CustomPropertyBaseHandleData* _pHandleData);

            virtual bool __YYAPI OnMouseFocusedPropChanged(_In_ OnPropertyChangedHandleData* _pHandleData);

            bool __YYAPI GetMouseFocusedPropValue(_Inout_ GetValueHandleData* _pHandleData);

            bool __YYAPI SetMouseFocusedPropValue(_Inout_ SetValueHandleData* _pHandleData);

            bool __YYAPI MouseFocusWithinPropHandle(_In_ CustomPropertyHandleType _eType, _Inout_ CustomPropertyBaseHandleData* _pHandleData);

            bool __YYAPI GetMouseFocusWithinPropValue(_Inout_ GetValueHandleData* _pHandleData);

            bool __YYAPI KeyboardFocusedPropHandle(_In_ CustomPropertyHandleType _eType, _Inout_ CustomPropertyBaseHandleData* _pHandleData);

            virtual bool __YYAPI OnKeyboardFocusedPropChanged(_In_ OnPropertyChangedHandleData* _pHandleData);

            bool __YYAPI GetKeyboardFocusedPropValue(_Inout_ GetValueHandleData* _pHandleData);

            bool __YYAPI SetKeyboardFocusedPropValue(_Inout_ SetValueHandleData* _pHandleData);

            bool __YYAPI KeyboardFocusWithinPropHandle(_In_ CustomPropertyHandleType _eType, _Inout_ CustomPropertyBaseHandleData* _pHandleData);

            bool __YYAPI GetKeyboardFocusWithinPropValue(_Inout_ GetValueHandleData* _pHandleData);

            bool __YYAPI FocusedPropHandle(_In_ CustomPropertyHandleType _eType, _Inout_ CustomPropertyBaseHandleData* _pHandleData);

            virtual bool __YYAPI OnFocusedPropChanged(_In_ OnPropertyChangedHandleData* _pHandleData);

            bool __YYAPI GetFocusedPropValue(_Inout_ GetValueHandleData* _pHandleData);
            
            bool __YYAPI SetFocusedPropValue(_Inout_ SetValueHandleData* _pHandleData);
            
            bool __YYAPI FocusWithinPropHandle(_In_ CustomPropertyHandleType _eType, _Inout_ CustomPropertyBaseHandleData* _pHandleData);

            bool __YYAPI GetFocusWithinPropValue(_Inout_ GetValueHandleData* _pHandleData);

            bool __YYAPI SheetPropHandle(_In_ CustomPropertyHandleType _eType, _Inout_ CustomPropertyBaseHandleData* _pHandleData);

            virtual bool __YYAPI GetSheetPropDependencies(_Inout_ GetDependenciesHandleData* _pHandleData);

            virtual HRESULT __YYAPI OnHosted(Window* _pNewWindow);

            virtual HRESULT __YYAPI OnUnHosted(Window* _pOldWindow);

            /// <summary>
            /// GetAdjacent 过程中，当前元素自身是否可以被选择。
            /// </summary>
            /// <param name="_eNavigate"></param>
            /// <param name="_pnr"></param>
            /// <param name="_bKeyableOnly"></param>
            /// <returns>如果返回 S_OK，那么当前元素可以被选择。
            /// 如果返回 大于 0，那么当前元素不能被选择，但是其子元素可能可以被选择。
            /// 如果返回 小于 0，当前元素及其子元素均不可能被选择。</returns>
            virtual HRESULT __YYAPI CanChooseSelf(_In_ NavigatingType _eNavigate, _In_opt_ NavReference const* _pnr, _In_ bool _bKeyableOnly);

            /// <summary>
            /// GetAdjacent 过程中，尝试从孩子中选取附近的元素。
            /// </summary>
            /// <param name="_pFrom"></param>
            /// <param name="_eNavigate"></param>
            /// <param name="_pnr"></param>
            /// <param name="_bKeyableOnly"></param>
            /// <returns>需要移动的新元素。</returns>
            virtual Element* __YYAPI GetAdjacentChild(_In_opt_ Element* _pFrom, _In_ NavigatingType _eNavigate, _In_opt_ NavReference const* _pnr, _In_ bool _bKeyableOnly);

            /// <summary>
            /// 选取靠近_pFrom 附近的元素。此函数可能向上搜索。
            /// </summary>
            /// <param name="_pFrom"></param>
            /// <param name="_eNavigate"></param>
            /// <param name="_pnr"></param>
            /// <param name="_bKeyableOnly"></param>
            /// <returns></returns>
            virtual Element* __YYAPI GetAdjacent(_In_opt_ Element* _pFrom, _In_ NavigatingType _eNavigate, _In_opt_ NavReference const* _pnr, _In_ bool _bKeyableOnly);

            virtual bool __YYAPI OnKeyDown(const KeyboardEvent& _KeyEvent);

            virtual bool __YYAPI OnKeyUp(const KeyboardEvent& _KeyEvent);

            virtual bool __YYAPI OnChar(const KeyboardEvent& _KeyEvent);

            /// <summary>
            /// 处理键盘导航事件。
            /// </summary>
            /// <param name="_Event">键盘导航事件。</param>
            /// <returns>如果已经处理，则返回 true</returns>
            virtual bool __YYAPI OnKeyboardNavigate(const KeyboardNavigateEvent& _Event);

            virtual bool __YYAPI OnLeftButtonDown(const MouseEvent& _Event);

            virtual bool __YYAPI OnLeftButtonUp(const MouseEvent& _Event);

            virtual bool __YYAPI OnClick(const ClickEvent& _Event);

            virtual bool __YYAPI OnMouseMove(const MouseEvent& _Event);

        };
    }
}

#pragma pack(pop)
