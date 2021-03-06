#pragma once

#include <SharedGraphics/Color.h>
#include <SharedGraphics/Rect.h>

typedef unsigned WSAPI_Color;

struct WSAPI_Point {
    int x;
    int y;
};

struct WSAPI_Size {
    int width;
    int height;
};

struct WSAPI_Rect {
    WSAPI_Point location;
    WSAPI_Size size;
};

enum WSAPI_WindowType {
    Invalid = 0,
    Normal,
    Menu,
    WindowSwitcher,
    Taskbar,
};

struct WSAPI_WindowBackingStoreInfo {
    WSAPI_Size size;
    size_t bpp;
    size_t pitch;
    RGBA32* pixels;
};

enum class WSAPI_MouseButton : unsigned char {
    NoButton = 0,
    Left     = 1,
    Right    = 2,
    Middle   = 4,
};

struct WSAPI_KeyModifiers { enum {
    Shift = 1 << 0,
    Alt   = 1 << 1,
    Ctrl  = 1 << 2,
}; };

enum class WSAPI_StandardCursor : unsigned char {
    None = 0,
    Arrow,
    IBeam,
    ResizeHorizontal,
    ResizeVertical,
};

struct WSAPI_ServerMessage {
    enum Type : unsigned {
        Invalid,
        Error,
        Paint,
        MouseMove,
        MouseDown,
        MouseUp,
        WindowEntered,
        WindowLeft,
        KeyDown,
        KeyUp,
        WindowActivated,
        WindowDeactivated,
        WindowResized,
        WindowCloseRequest,
        MenuItemActivated,
        DidCreateMenubar,
        DidDestroyMenubar,
        DidCreateMenu,
        DidDestroyMenu,
        DidAddMenuToMenubar,
        DidSetApplicationMenubar,
        DidAddMenuItem,
        DidAddMenuSeparator,
        DidCreateWindow,
        DidDestroyWindow,
        DidGetWindowTitle,
        DidGetWindowRect,
        DidGetWindowBackingStore,
        Greeting,
        DidGetClipboardContents,
        DidSetClipboardContents,
        DidSetWindowBackingStore,
        DidSetWallpaper,
        DidGetWallpaper,
        ScreenRectChanged,
        WM_WindowRemoved,
        WM_WindowStateChanged,
    };
    Type type { Invalid };
    int window_id { -1 };
    int text_length { 0 };
    char text[256];
    int value { 0 };

    union {
        struct {
            int server_pid;
            WSAPI_Rect screen_rect;
        } greeting;
        struct {
            int client_id;
            int window_id;
            WSAPI_Rect rect;
            bool is_active;
            bool is_minimized;
            WSAPI_WindowType window_type;
        } wm;
        struct {
            WSAPI_Rect rect;
        } screen;
        struct {
            WSAPI_Rect rect;
            WSAPI_Rect old_rect;
        } window;
        struct {
            WSAPI_Rect rect;
            WSAPI_Size window_size;
        } paint;
        struct {
            WSAPI_Point position;
            WSAPI_MouseButton button;
            unsigned buttons;
            byte modifiers;
        } mouse;
        struct {
            char character;
            byte key;
            byte modifiers;
            bool ctrl : 1;
            bool alt : 1;
            bool shift : 1;
        } key;
        struct {
            int menubar_id;
            int menu_id;
            unsigned identifier;
        } menu;
        struct {
            WSAPI_Size size;
            size_t bpp;
            size_t pitch;
            int shared_buffer_id;
            bool has_alpha_channel;
        } backing;
        struct {
            int shared_buffer_id;
            int contents_size;
        } clipboard;
    };
};

struct WSAPI_ClientMessage {
    enum Type : unsigned {
        Invalid,
        CreateMenubar,
        DestroyMenubar,
        CreateMenu,
        DestroyMenu,
        AddMenuToMenubar,
        SetApplicationMenubar,
        AddMenuItem,
        AddMenuSeparator,
        CreateWindow,
        DestroyWindow,
        SetWindowTitle,
        GetWindowTitle,
        SetWindowRect,
        GetWindowRect,
        InvalidateRect,
        DidFinishPainting,
        GetWindowBackingStore,
        SetGlobalCursorTracking,
        SetWindowOpacity,
        SetWindowBackingStore,
        GetClipboardContents,
        SetClipboardContents,
        Greeting,
        SetWallpaper,
        GetWallpaper,
        SetWindowOverrideCursor,
        WM_SetActiveWindow,
    };
    Type type { Invalid };
    int window_id { -1 };
    int text_length { 0 };
    char text[256];
    int value { 0 };

    union {
        struct {
            int client_pid;
        } greeting;
        struct {
            int client_id;
            int window_id;
        } wm;
        struct {
            int menubar_id;
            int menu_id;
            unsigned identifier;
            char shortcut_text[32];
            int shortcut_text_length;
        } menu;
        struct {
            WSAPI_Rect rect;
            bool has_alpha_channel;
            bool modal;
            bool resizable;
            WSAPI_WindowType type;
            float opacity;
            WSAPI_Size base_size;
            WSAPI_Size size_increment;
        } window;
        struct {
            WSAPI_Size size;
            size_t bpp;
            size_t pitch;
            int shared_buffer_id;
            bool has_alpha_channel;
            bool flush_immediately;
        } backing;
        struct {
            int shared_buffer_id;
            int contents_size;
        } clipboard;
        struct {
            WSAPI_StandardCursor cursor;
        } cursor;
    };
};

inline Rect::Rect(const WSAPI_Rect& r) : Rect(r.location, r.size) { }
inline Point::Point(const WSAPI_Point& p) : Point(p.x, p.y) { }
inline Size::Size(const WSAPI_Size& s) : Size(s.width, s.height) { }
inline Rect::operator WSAPI_Rect() const { return { m_location, m_size }; }
inline Point::operator WSAPI_Point() const { return { m_x, m_y }; }
inline Size::operator WSAPI_Size() const { return { m_width, m_height }; }
