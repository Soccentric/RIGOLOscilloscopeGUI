#include "cursormanager.h"

CursorManager::CursorManager(QObject *parent)
    : QObject(parent)
{
}

void CursorManager::setMode(CursorMode mode)
{
    if (m_mode != mode) {
        m_mode = mode;
        emit modeChanged(mode);
        emit cursorsChanged();
    }
}

void CursorManager::setType(CursorType type)
{
    if (m_type != type) {
        m_type = type;
        emit typeChanged(type);
        emit cursorsChanged();
    }
}

void CursorManager::setSourceChannel(const QString &channel)
{
    if (m_sourceChannel != channel) {
        m_sourceChannel = channel;
        emit sourceChannelChanged(channel);
        emit cursorsChanged();
    }
}

void CursorManager::setCursorX1(double x)
{
    if (!qFuzzyCompare(m_x1, x)) {
        m_x1 = x;
        emit cursorX1Changed(x);
        emit cursorsChanged();
    }
}

void CursorManager::setCursorX2(double x)
{
    if (!qFuzzyCompare(m_x2, x)) {
        m_x2 = x;
        emit cursorX2Changed(x);
        emit cursorsChanged();
    }
}

void CursorManager::setCursorY1(double y)
{
    if (!qFuzzyCompare(m_y1, y)) {
        m_y1 = y;
        emit cursorY1Changed(y);
        emit cursorsChanged();
    }
}

void CursorManager::setCursorY2(double y)
{
    if (!qFuzzyCompare(m_y2, y)) {
        m_y2 = y;
        emit cursorY2Changed(y);
        emit cursorsChanged();
    }
}
