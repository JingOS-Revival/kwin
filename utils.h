/*
    KWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 1999, 2000 Matthias Ettrich <ettrich@kde.org>
    SPDX-FileCopyrightText: 2003 Lubos Lunak <l.lunak@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KWIN_UTILS_H
#define KWIN_UTILS_H

// cmake stuff
#include <config-kwin.h>
#include <kwinconfig.h>
// kwin
#include <kwinglobals.h>
// Qt
#include <QLoggingCategory>
#include <QList>
#include <QPoint>
#include <QRect>
#include <QScopedPointer>
#include <QProcess>
// system
#include <climits>
Q_DECLARE_LOGGING_CATEGORY(KWIN_CORE)
Q_DECLARE_LOGGING_CATEGORY(KWIN_VIRTUALKEYBOARD)
namespace KWin
{

const QPoint invalidPoint(INT_MIN, INT_MIN);
const qreal APP_DEFAULT_SCALE = 1;

enum StrutArea {
    StrutAreaInvalid = 0, // Null
    StrutAreaTop     = 1 << 0,
    StrutAreaRight   = 1 << 1,
    StrutAreaBottom  = 1 << 2,
    StrutAreaLeft    = 1 << 3,
    StrutAreaAll     = StrutAreaTop | StrutAreaRight | StrutAreaBottom | StrutAreaLeft
};
Q_DECLARE_FLAGS(StrutAreas, StrutArea)

class StrutRect : public QRect
{
public:
    explicit StrutRect(QRect rect = QRect(), StrutArea area = StrutAreaInvalid);
    StrutRect(int x, int y, int width, int height, StrutArea area = StrutAreaInvalid);
    StrutRect(const StrutRect& other);
    StrutRect &operator=(const StrutRect& other);
    inline StrutArea area() const {
        return m_area;
    }
private:
    StrutArea m_area;
};
typedef QVector<StrutRect> StrutRects;


enum ShadeMode {
    ShadeNone, // not shaded
    ShadeNormal, // normally shaded - isShade() is true only here
    ShadeHover, // "shaded", but visible due to hover unshade
    ShadeActivated // "shaded", but visible due to alt+tab to the window
};

/**
 * Maximize mode. These values specify how a window is maximized.
 *
 * @note these values are written to session files, don't change the order
 */
enum MaximizeMode {
    MaximizeRestore    = 0, ///< The window is not maximized in any direction.
    MaximizeVertical   = 1, ///< The window is maximized vertically.
    MaximizeHorizontal = 2, ///< The window is maximized horizontally.
    /// Equal to @p MaximizeVertical | @p MaximizeHorizontal
    MaximizeFull = MaximizeVertical | MaximizeHorizontal
};

inline
MaximizeMode operator^(MaximizeMode m1, MaximizeMode m2)
{
    return MaximizeMode(int(m1) ^ int(m2));
}

enum class QuickTileFlag {
    None        = 0,
    Left        = 1 << 0,
    Right       = 1 << 1,
    Top         = 1 << 2,
    Bottom      = 1 << 3,
    Horizontal  = Left | Right,
    Vertical    = Top | Bottom,
    Maximize    = Left | Right | Top | Bottom,
};
Q_DECLARE_FLAGS(QuickTileMode, QuickTileFlag)

const QList<QString> g_jing_app_list {
"org.kde.jinggallery",
"org.jingos.ub",
"org.kde.media",
"org.kde.krecorder",
"org.kde.index",
"org.kde.kalk",
"org.kde.kclock",
"org.kde.discover",
"org.kde.mobile.plasmasettings",
"org.kde.calindori",
"userguide userguide",
"org.kde.camera",
"userguide",
"org.jingos.qaptupdator.desktop",
"ksmserver-logout-greeter",
"org.kde.plasmashell",
"org.kde.discover.urlhandler",
"org.kde.discover.apt.urlhandler"
};

const QList<QString> g_scale_black_list {
"jinggallery",
"ub",
"krecorder",
"index",
"kalk",
"kclock",
"discover",
"chromium-browser",
"plasmasettings",
"calindori",
"org.kde.plasmashell",
"org.kde.kscreenlocker",
"org.kde.kscreenlocker_greet",
"org.kde.konsole",
"systemsettings",
"org.kde.plasma-camera",
"org.kde.media",
"userguide userguide",
"userguide",
"qaptupdator",
"org.jingos.qaptupdator",
"org.kde.polkit-kde-authentication-agent-1",
"haruna",
"org.kde.haruna",
"io.calamares.calamares",
"ksplashqml"
};


template <typename T> using ScopedCPointer = QScopedPointer<T, QScopedPointerPodDeleter>;

void KWIN_EXPORT updateXTime();
void KWIN_EXPORT grabXServer();
void KWIN_EXPORT ungrabXServer();
bool KWIN_EXPORT grabXKeyboard(xcb_window_t w = XCB_WINDOW_NONE);
void KWIN_EXPORT ungrabXKeyboard();

/**
 * Small helper class which performs grabXServer in the ctor and
 * ungrabXServer in the dtor. Use this class to ensure that grab and
 * ungrab are matched.
 */
class XServerGrabber
{
public:
    XServerGrabber() {
        grabXServer();
    }
    ~XServerGrabber() {
        ungrabXServer();
    }
};

// the docs say it's UrgencyHint, but it's often #defined as XUrgencyHint
#ifndef UrgencyHint
#define UrgencyHint XUrgencyHint
#endif

// converting between X11 mouse/keyboard state mask and Qt button/keyboard states
Qt::MouseButton x11ToQtMouseButton(int button);
Qt::MouseButton KWIN_EXPORT x11ToQtMouseButton(int button);
Qt::MouseButtons KWIN_EXPORT x11ToQtMouseButtons(int state);
Qt::KeyboardModifiers KWIN_EXPORT x11ToQtKeyboardModifiers(int state);

/**
 * Separate the concept of an unet QPoint and 0,0
 */
class ClearablePoint
{
public:
    inline bool isValid() const {
        return m_valid;
    }

    inline void clear(){
        m_valid = false;
    }

    inline void setPoint(const QPoint &point) {
        m_point = point; m_valid = true;
    }

    inline QPoint point() const {
        return m_point;
    }

private:
    QPoint m_point;
    bool m_valid = false;
};

/**
 * QProcess subclass which unblocks SIGUSR in the child process.
 */
class KWIN_EXPORT Process : public QProcess
{
    Q_OBJECT
public:
    explicit Process(QObject *parent = nullptr);
    ~Process() override;

#ifndef KCMRULES
protected:
    void setupChildProcess() override;
#endif
};

} // namespace

enum  JingWindowType {
    TYPE_WALLPAPER = 2000,
    TYPE_DESKTOP = 2001,
    TYPE_DIALOG = 2002,
    TYPE_SYS_SPLASH = 2003,
    TYPE_SEARCH_BAR = 2004,
    TYPE_NOTIFICATION = 2005,
    TYPE_CRITICAL_NOTIFICATION = 2006,
    TYPE_INPUT_METHOD = 2007,
    TYPE_INPUT_METHOD_DIALOG = 2008,
    TYPE_DND = 2009,
    TYPE_DOCK = 2010,
    TYPE_STATUS_BAR = 2011,
    TYPE_STATUS_BAR_PANEL = 2012,
    TYPE_TOAST = 2013,
    TYPE_KEYGUARD = 2014,
    TYPE_PHONE = 2015,
    TYPE_SYSTEM_DIALOG = 2016,
    TYPE_SYSTEM_ERROR = 2017,
    TYPE_VOICE_INTERACTION = 2018,
    TYPE_SYSTEM_OVERLAY = 2019,
    TYPE_SCREENSHOT = 2020,
    TYPE_BOOT_PROGRESS = 2021,
    TYPE_POINTER = 2022,
    TYPE_LAST_SYS_LAYER = 2099,
    TYPE_BASE_APPLICATION = 1,
    TYPE_APPLICATION = 2,
    TYPE_APPLICATION_STARTING = 3,
    TYPE_APPLICATION_OVERLAY = 4,
    TYPE_LAST_APPLICATION_WINDOW = 99,
    TYPE_UNKNOW = -1
};

enum JingLayer {
    LAYER_FIRST_LAYER = -1,
    LAYER_WALLPAPER = 0,
    LAYER_DESKTOP = 1,
    LAYER_APPLICATION = 2,
    LAYER_DIALOG = 3,
    LAYER_SEARCH_BAR = 4,
    LAYER_INPUT_METHOD = 5,
    LAYER_INPUT_METHOD_DIALOG = 6,
    LAYER_DND = 7,
    LAYER_DOCK = 8,
    LAYER_APPLICATION_OVERLAY = 9,
    LAYER_STATUS_BAR = 10,
    LAYER_STATUS_BAR_PANEL = 11,
    LAYER_FULL_SCREEN = 12,
    LAYER_SYS_SPLASH = 13,
    LAYER_UNMANAGER = 14,
    LAYER_SYSTEM_DIALOG = 15,
    LAYER_TOAST = 17,
    LAYER_KEYGUARD = 18,
    LAYER_SYSTEM_OVERLAY = 19,
    LAYER_NOTIFICATION = 20,
    LAYER_CRITICAL_NOTIFICATION = 21,
    LAYER_PHONE = 22,
    LAYER_SYSTEM_ERROR = 23,
    LAYER_SYSTEM_INTERACTION = 24,
    LAYER_SCREENSHOT = 25,
    LAYER_BOOT_PROGRESS = 26,
    LAYER_POINTER = 27,
    LAYER_LAST_LAYER = 28
};

// Must be outside namespace
Q_DECLARE_OPERATORS_FOR_FLAGS(KWin::StrutAreas)
Q_DECLARE_OPERATORS_FOR_FLAGS(KWin::QuickTileMode)

#endif
