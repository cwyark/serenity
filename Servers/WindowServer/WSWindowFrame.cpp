#include <WindowServer/WSWindowFrame.h>
#include <WindowServer/WSWindowManager.h>
#include <WindowServer/WSWindow.h>
#include <WindowServer/WSMessage.h>
#include <WindowServer/WSButton.h>
#include <SharedGraphics/CharacterBitmap.h>
#include <SharedGraphics/Painter.h>
#include <SharedGraphics/Font.h>
#include <SharedGraphics/StylePainter.h>

static const int window_titlebar_height = 17;

static const char* s_close_button_bitmap_data = {
    "##    ##"
    "###  ###"
    " ###### "
    "  ####  "
    "   ##   "
    "  ####  "
    " ###### "
    "###  ###"
    "##    ##"
};

static CharacterBitmap* s_close_button_bitmap;
static const int s_close_button_bitmap_width = 8;
static const int s_close_button_bitmap_height = 9;

static const char* s_minimize_button_bitmap_data = {
    "        "
    "        "
    "##    ##"
    "###  ###"
    " ###### "
    "  ####  "
    "   ##   "
    "        "
    "        "
};

static CharacterBitmap* s_minimize_button_bitmap;
static const int s_minimize_button_bitmap_width = 8;
static const int s_minimize_button_bitmap_height = 9;


WSWindowFrame::WSWindowFrame(WSWindow& window)
    : m_window(window)
{
    if (!s_close_button_bitmap)
        s_close_button_bitmap = &CharacterBitmap::create_from_ascii(s_close_button_bitmap_data, s_close_button_bitmap_width, s_close_button_bitmap_height).leak_ref();

    if (!s_minimize_button_bitmap)
        s_minimize_button_bitmap = &CharacterBitmap::create_from_ascii(s_minimize_button_bitmap_data, s_minimize_button_bitmap_width, s_minimize_button_bitmap_height).leak_ref();

    m_buttons.append(make<WSButton>(*this, *s_close_button_bitmap, [this] {
        m_window.on_message(WSMessage(WSMessage::WindowCloseRequest));
    }));

    m_buttons.append(make<WSButton>(*this, *s_minimize_button_bitmap, [this] {
        m_window.set_minimized(true);
    }));
}

WSWindowFrame::~WSWindowFrame()
{
}

static inline Rect menu_window_rect(const Rect& rect)
{
    return rect.inflated(2, 2);
}

Rect WSWindowFrame::title_bar_rect() const
{
    return { 2, 2, m_window.width() + 2, window_titlebar_height };
}

Rect WSWindowFrame::title_bar_icon_rect() const
{
    auto titlebar_rect = title_bar_rect();
    return {
        titlebar_rect.x() + 2,
        titlebar_rect.y(),
        16,
        titlebar_rect.height(),
    };
}

Rect WSWindowFrame::title_bar_text_rect() const
{
    auto titlebar_rect = title_bar_rect();
    auto titlebar_icon_rect = title_bar_icon_rect();
    return {
        titlebar_rect.x() + 2 + titlebar_icon_rect.width() + 2,
        titlebar_rect.y(),
        titlebar_rect.width() - 4 - titlebar_icon_rect.width() - 2,
        titlebar_rect.height()
    };
}

Rect WSWindowFrame::middle_border_rect() const
{
    return { 1, 1, m_window.width() + 4, m_window.height() + 4 + window_titlebar_height };
}

void WSWindowFrame::paint(Painter& painter)
{
    PainterStateSaver saver(painter);
    painter.translate(rect().location());

    if (m_window.type() == WSWindowType::Menu) {
        painter.draw_rect(menu_window_rect(m_window.rect()), Color::LightGray);
        return;
    }

    if (m_window.type() == WSWindowType::WindowSwitcher)
        return;

    if (m_window.type() == WSWindowType::Taskbar)
        return;

    auto& window = m_window;

    auto titlebar_rect = title_bar_rect();
    auto titlebar_icon_rect = title_bar_icon_rect();
    auto titlebar_inner_rect = title_bar_text_rect();
    Rect outer_rect = { { }, rect().size() };

    auto titlebar_title_rect = titlebar_inner_rect;
    titlebar_title_rect.set_width(Font::default_bold_font().width(window.title()));

    Rect inner_border_rect {
        2,
        2 + window_titlebar_height,
        window.width() + 2,
        window.height() + 2
    };

    Color title_color;
    Color border_color;
    Color border_color2;
    Color middle_border_color;

    auto& wm = WSWindowManager::the();

    if (&window == wm.m_highlight_window.ptr()) {
        border_color = wm.m_highlight_window_border_color;
        border_color2 = wm.m_highlight_window_border_color2;
        title_color = wm.m_highlight_window_title_color;
        middle_border_color = Color::White;
    } else if (&window == wm.m_drag_window.ptr()) {
        border_color = wm.m_dragging_window_border_color;
        border_color2 = wm.m_dragging_window_border_color2;
        title_color = wm.m_dragging_window_title_color;
        middle_border_color = Color::from_rgb(0xf9b36a);
    } else if (&window == wm.m_active_window.ptr()) {
        border_color = wm.m_active_window_border_color;
        border_color2 = wm.m_active_window_border_color2;
        title_color = wm.m_active_window_title_color;
        middle_border_color = Color::from_rgb(0x8f673d);
    } else {
        border_color = wm.m_inactive_window_border_color;
        border_color2 = wm.m_inactive_window_border_color2;
        title_color = wm.m_inactive_window_title_color;
        middle_border_color = Color::MidGray;
    }

    auto leftmost_button_rect = m_buttons.is_empty() ? Rect() : m_buttons.last()->relative_rect();

    painter.fill_rect_with_gradient(titlebar_rect, border_color, border_color2);
    for (int i = 2; i <= titlebar_inner_rect.height() - 2; i += 2) {
        painter.draw_line({ titlebar_title_rect.right() + 4, titlebar_inner_rect.y() + i }, { leftmost_button_rect.left() - 3, titlebar_inner_rect.y() + i }, border_color);
    }
    painter.draw_rect(middle_border_rect(), middle_border_color);
    painter.draw_rect(outer_rect, border_color);
    painter.draw_rect(inner_border_rect, border_color);

    painter.draw_text(titlebar_title_rect, window.title(), wm.window_title_font(), TextAlignment::CenterLeft, title_color);

    painter.blit(titlebar_icon_rect.location(), window.icon(), window.icon().rect());

    for (auto& button : m_buttons) {
        button->paint(painter);
    }
}

static Rect frame_rect_for_window_type(WSWindowType type, const Rect& rect)
{
    switch (type) {
    case WSWindowType::Menu:
        return menu_window_rect(rect);
    case WSWindowType::Normal:
        return { rect.x() - 3, rect.y() - window_titlebar_height - 3, rect.width() + 6, rect.height() + 6 + window_titlebar_height };
    case WSWindowType::WindowSwitcher:
        return rect;
    case WSWindowType::Taskbar:
        return rect;
    default:
        ASSERT_NOT_REACHED();
    }
}

Rect WSWindowFrame::rect() const
{
    return frame_rect_for_window_type(m_window.type(), m_window.rect());
}

void WSWindowFrame::invalidate_title_bar()
{
    WSWindowManager::the().invalidate(title_bar_rect().translated(rect().location()));
}

void WSWindowFrame::notify_window_rect_changed(const Rect& old_rect, const Rect& new_rect)
{
    int window_button_width = 15;
    int window_button_height = 15;
    int x = title_bar_text_rect().right() + 1;;
    for (auto& button : m_buttons) {
        x -= window_button_width;
        Rect rect { x, 0, window_button_width, window_button_height };
        rect.center_vertically_within(title_bar_text_rect());
        button->set_relative_rect(rect);
    }

    auto& wm = WSWindowManager::the();
    wm.invalidate(frame_rect_for_window_type(m_window.type(), old_rect));
    wm.invalidate(frame_rect_for_window_type(m_window.type(), new_rect));
    wm.notify_rect_changed(m_window, old_rect, new_rect);
}

void WSWindowFrame::on_mouse_event(const WSMouseEvent& event)
{
    auto& wm = WSWindowManager::the();
    if (m_window.type() != WSWindowType::Normal)
        return;
    if (title_bar_rect().contains(event.position())) {
        if (event.type() == WSMessage::MouseDown)
            wm.move_to_front_and_make_active(m_window);

        for (auto& button : m_buttons) {
            if (button->relative_rect().contains(event.position()))
                return button->on_mouse_event(event.translated(-button->relative_rect().location()));
        }
        if (event.type() == WSMessage::MouseDown && event.button() == MouseButton::Left)
            wm.start_window_drag(m_window, event.translated(rect().location()));
    }
}
