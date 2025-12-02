/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/ui/mainwindow.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN10MainWindowE_t {};
} // unnamed namespace

template <> constexpr inline auto MainWindow::qt_create_metaobjectdata<qt_meta_tag_ZN10MainWindowE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "MainWindow",
        "onConnectAction",
        "",
        "onDisconnectAction",
        "onConnectionStateChanged",
        "state",
        "onRunStop",
        "onSingle",
        "onAutoScale",
        "onForceTrigger",
        "onClear",
        "onToggleFullScreen",
        "onToggleDarkMode",
        "onResetLayout",
        "onToggleLeftPanel",
        "onToggleRightPanel",
        "onToggleFFTDisplay",
        "onExportData",
        "onTakeScreenshot",
        "onOpenFFT",
        "onOpenProtocolDecoder",
        "onOpenMathChannel",
        "onAbout",
        "onCheckForUpdates",
        "onDeviceIdReceived",
        "id",
        "updateStatusBar",
        "onWaveformUpdated",
        "channel",
        "onMeasurementsUpdated",
        "toggleChannel",
        "channelName",
        "onRunModeChanged",
        "mode",
        "onTimebaseChanged",
        "timePerDiv",
        "onChannelSettingsChanged",
        "onTriggerLevelChanged",
        "level",
        "onMeasurementAdded",
        "type"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'onConnectAction'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDisconnectAction'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onConnectionStateChanged'
        QtMocHelpers::SlotData<void(int)>(4, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 5 },
        }}),
        // Slot 'onRunStop'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSingle'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAutoScale'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onForceTrigger'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onClear'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onToggleFullScreen'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onToggleDarkMode'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onResetLayout'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onToggleLeftPanel'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onToggleRightPanel'
        QtMocHelpers::SlotData<void()>(15, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onToggleFFTDisplay'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onExportData'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onTakeScreenshot'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onOpenFFT'
        QtMocHelpers::SlotData<void()>(19, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onOpenProtocolDecoder'
        QtMocHelpers::SlotData<void()>(20, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onOpenMathChannel'
        QtMocHelpers::SlotData<void()>(21, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAbout'
        QtMocHelpers::SlotData<void()>(22, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onCheckForUpdates'
        QtMocHelpers::SlotData<void()>(23, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDeviceIdReceived'
        QtMocHelpers::SlotData<void(const QString &)>(24, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 25 },
        }}),
        // Slot 'updateStatusBar'
        QtMocHelpers::SlotData<void()>(26, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onWaveformUpdated'
        QtMocHelpers::SlotData<void(const QString &)>(27, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 28 },
        }}),
        // Slot 'onMeasurementsUpdated'
        QtMocHelpers::SlotData<void()>(29, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'toggleChannel'
        QtMocHelpers::SlotData<void(const QString &)>(30, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 31 },
        }}),
        // Slot 'onRunModeChanged'
        QtMocHelpers::SlotData<void(const QString &)>(32, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 33 },
        }}),
        // Slot 'onTimebaseChanged'
        QtMocHelpers::SlotData<void(double)>(34, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Double, 35 },
        }}),
        // Slot 'onChannelSettingsChanged'
        QtMocHelpers::SlotData<void(const QString &)>(36, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 28 },
        }}),
        // Slot 'onTriggerLevelChanged'
        QtMocHelpers::SlotData<void(double)>(37, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Double, 38 },
        }}),
        // Slot 'onMeasurementAdded'
        QtMocHelpers::SlotData<void(const QString &, const QString &)>(39, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 40 }, { QMetaType::QString, 28 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<MainWindow, qt_meta_tag_ZN10MainWindowE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10MainWindowE_t>.metaTypes,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MainWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->onConnectAction(); break;
        case 1: _t->onDisconnectAction(); break;
        case 2: _t->onConnectionStateChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->onRunStop(); break;
        case 4: _t->onSingle(); break;
        case 5: _t->onAutoScale(); break;
        case 6: _t->onForceTrigger(); break;
        case 7: _t->onClear(); break;
        case 8: _t->onToggleFullScreen(); break;
        case 9: _t->onToggleDarkMode(); break;
        case 10: _t->onResetLayout(); break;
        case 11: _t->onToggleLeftPanel(); break;
        case 12: _t->onToggleRightPanel(); break;
        case 13: _t->onToggleFFTDisplay(); break;
        case 14: _t->onExportData(); break;
        case 15: _t->onTakeScreenshot(); break;
        case 16: _t->onOpenFFT(); break;
        case 17: _t->onOpenProtocolDecoder(); break;
        case 18: _t->onOpenMathChannel(); break;
        case 19: _t->onAbout(); break;
        case 20: _t->onCheckForUpdates(); break;
        case 21: _t->onDeviceIdReceived((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 22: _t->updateStatusBar(); break;
        case 23: _t->onWaveformUpdated((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 24: _t->onMeasurementsUpdated(); break;
        case 25: _t->toggleChannel((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 26: _t->onRunModeChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 27: _t->onTimebaseChanged((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 28: _t->onChannelSettingsChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 29: _t->onTriggerLevelChanged((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 30: _t->onMeasurementAdded((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.strings))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 31)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 31;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 31)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 31;
    }
    return _id;
}
QT_WARNING_POP
