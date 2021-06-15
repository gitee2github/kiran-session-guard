#include <QDebug>
#include <QPainter>
#include <QScreen>
#include <QResizeEvent>
#include <qt5-log-i.h>

#include "greeterloginwindow.h"
#include "greeterbackground.h"
#include "kiran-greeter-prefs.h"
#include "define.h"

QT_BEGIN_NAMESPACE
Q_WIDGETS_EXPORT void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed = 0);
QT_END_NAMESPACE

GreeterBackground::GreeterBackground(QScreen *screen, QWidget *parent)
    : QWidget(parent), m_screen(nullptr)
{
#ifndef TEST
    setWindowFlags(windowFlags() | Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnBottomHint);
#endif

    QStringList backgrounds = {KiranGreeterPrefs::instance()->background(),DEFAULT_BACKGROUND};
    foreach(const QString background,backgrounds)
    {
        if(!m_background.load(background))
        {
            KLOG_WARNING() << "load background <" << background << "> failed!";
            continue;
        }
        else
        {
            KLOG_DEBUG() << "load background <" << background << "> successes!";
            break;
        }
    }

    if(m_background.isNull())
    {
        KLOG_ERROR() << "load greeter background failed!";
    }

    setScreen(screen);
}

GreeterBackground::~GreeterBackground()
{
}

void GreeterBackground::setScreen(QScreen *screen)
{
    if (m_screen != nullptr)
    {
        disconnect(screen, &QScreen::geometryChanged,
                   this, &GreeterBackground::slotScreenGeometryChanged);
    }

    if (screen != nullptr)
    {
        connect(screen, &QScreen::geometryChanged,
                this, &GreeterBackground::slotScreenGeometryChanged);
    }

    m_screen = screen;
    if (m_screen)
    {
        slotScreenGeometryChanged(m_screen->geometry());
    }
}

void GreeterBackground::slotScreenGeometryChanged(const QRect &geometry)
{
    KLOG_DEBUG() << objectName() << "screen geometry changed:" << geometry;
    this->resize(geometry.size());
    this->move(geometry.x(), geometry.y());
}

void GreeterBackground::enterEvent(QEvent *event)
{
    KLOG_DEBUG() << objectName() << "mouse enter";
    emit mouseEnter(this);
    QWidget::enterEvent(event);
}

void GreeterBackground::resizeEvent(QResizeEvent *event)
{
    KLOG_DEBUG() << objectName() << "resize " << event->oldSize() << "->" << event->size();

    if (!m_background.isNull())
    {
        m_scaledBackground = m_background.scaled(this->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        QImage tmp         = m_scaledBackground.toImage();
        qt_blurImage(tmp, 10, true);
        m_scaledBackground = QPixmap::fromImage(tmp);
    }

    //NOTE:子窗体因未加入布局，需要手动Resize
    GreeterLoginWindow *greeterWindow = findChild<GreeterLoginWindow *>("GreeterLoginWindow");
    if (greeterWindow != nullptr)
    {
        KLOG_DEBUG() << "greeter login window resize:" << this->size();
        greeterWindow->resize(this->size());
    }

    QWidget::resizeEvent(event);
}

void GreeterBackground::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if (!m_scaledBackground.isNull())
    {
        painter.drawPixmap(this->rect(), m_scaledBackground, m_scaledBackground.rect());
    }
    else
    {
        painter.fillRect(this->rect(), QColor(0, 0, 0));
    }
    QWidget::paintEvent(event);
}
