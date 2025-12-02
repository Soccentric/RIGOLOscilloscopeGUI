/**
 * @file footerwidget.h
 * @brief Modern footer widget with real-time readings and status
 */

#ifndef FOOTERWIDGET_H
#define FOOTERWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QHBoxLayout>
#include <QTimer>
#include <QMap>

class ScopeChannel;
class MeasurementEngine;

/**
 * @brief Professional footer widget displaying real-time measurements and system status
 * 
 * Layout: [Channel Readings] [Trigger Status] [Acquisition Progress] [Performance]
 */
class FooterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FooterWidget(const QMap<QString, ScopeChannel*> &channels,
                         MeasurementEngine *engine,
                         QWidget *parent = nullptr);
    ~FooterWidget() override = default;

    void setChannels(const QMap<QString, ScopeChannel*> &channels);
    void setTriggerStatus(const QString &status);
    void setAcquisitionProgress(int percent);
    void setAcquisitionCount(int count);
    void setFPS(int fps);
    void setRunning(bool running);

signals:
    void channelClicked(const QString &channel);
    void measurementClicked();

private slots:
    void updateReadings();
    void updatePerformance();

private:
    void setupUI();
    void applyStyles();
    QString formatValue(double value, const QString &unit, int precision = 3);

    QMap<QString, ScopeChannel*> m_channels;
    MeasurementEngine *m_engine;

    // Channel readings section
    struct ChannelReadingWidget {
        QWidget *container;
        QLabel *nameLabel;
        QLabel *vppLabel;
        QLabel *freqLabel;
    };
    QMap<QString, ChannelReadingWidget> m_channelReadings;

    // Status section
    QLabel *m_triggerStatusLabel;
    QLabel *m_acqCountLabel;
    QProgressBar *m_acqProgress;

    // Performance section
    QLabel *m_fpsLabel;
    QLabel *m_cpuLabel;
    QLabel *m_memLabel;

    // Running indicator
    QLabel *m_runningIndicator;
    
    // Update timers
    QTimer *m_readingsTimer;
    QTimer *m_perfTimer;
    
    bool m_isRunning = false;
};

#endif // FOOTERWIDGET_H
