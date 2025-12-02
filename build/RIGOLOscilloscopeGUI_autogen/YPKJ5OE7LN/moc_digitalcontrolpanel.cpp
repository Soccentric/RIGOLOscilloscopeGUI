/****************************************************************************
** Meta object code from reading C++ file 'digitalcontrolpanel.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/ui/digitalcontrolpanel.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'digitalcontrolpanel.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN19DigitalControlPanelE_t {};
} // unnamed namespace

template <> constexpr inline auto DigitalControlPanel::qt_create_metaobjectdata<qt_meta_tag_ZN19DigitalControlPanelE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "DigitalControlPanel",
        "runStopClicked",
        "",
        "singleClicked",
        "autoScaleClicked",
        "forceTriggerClicked",
        "clearClicked",
        "fftClicked",
        "protocolDecoderClicked",
        "measurementAdded",
        "type",
        "channel",
        "triggerSettingsChanged",
        "onRunStopClicked",
        "onTriggerModeChanged",
        "index",
        "onTriggerTypeChanged",
        "onTriggerSourceChanged",
        "onTriggerSlopeChanged",
        "onAddMeasurement",
        "updateMeasurementTable"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'runStopClicked'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'singleClicked'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'autoScaleClicked'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'forceTriggerClicked'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'clearClicked'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'fftClicked'
        QtMocHelpers::SignalData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'protocolDecoderClicked'
        QtMocHelpers::SignalData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'measurementAdded'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 10 }, { QMetaType::QString, 11 },
        }}),
        // Signal 'triggerSettingsChanged'
        QtMocHelpers::SignalData<void()>(12, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onRunStopClicked'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onTriggerModeChanged'
        QtMocHelpers::SlotData<void(int)>(14, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 15 },
        }}),
        // Slot 'onTriggerTypeChanged'
        QtMocHelpers::SlotData<void(int)>(16, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 15 },
        }}),
        // Slot 'onTriggerSourceChanged'
        QtMocHelpers::SlotData<void(int)>(17, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 15 },
        }}),
        // Slot 'onTriggerSlopeChanged'
        QtMocHelpers::SlotData<void(int)>(18, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 15 },
        }}),
        // Slot 'onAddMeasurement'
        QtMocHelpers::SlotData<void()>(19, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updateMeasurementTable'
        QtMocHelpers::SlotData<void()>(20, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<DigitalControlPanel, qt_meta_tag_ZN19DigitalControlPanelE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject DigitalControlPanel::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN19DigitalControlPanelE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN19DigitalControlPanelE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN19DigitalControlPanelE_t>.metaTypes,
    nullptr
} };

void DigitalControlPanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<DigitalControlPanel *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->runStopClicked(); break;
        case 1: _t->singleClicked(); break;
        case 2: _t->autoScaleClicked(); break;
        case 3: _t->forceTriggerClicked(); break;
        case 4: _t->clearClicked(); break;
        case 5: _t->fftClicked(); break;
        case 6: _t->protocolDecoderClicked(); break;
        case 7: _t->measurementAdded((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 8: _t->triggerSettingsChanged(); break;
        case 9: _t->onRunStopClicked(); break;
        case 10: _t->onTriggerModeChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 11: _t->onTriggerTypeChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 12: _t->onTriggerSourceChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 13: _t->onTriggerSlopeChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 14: _t->onAddMeasurement(); break;
        case 15: _t->updateMeasurementTable(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (DigitalControlPanel::*)()>(_a, &DigitalControlPanel::runStopClicked, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (DigitalControlPanel::*)()>(_a, &DigitalControlPanel::singleClicked, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (DigitalControlPanel::*)()>(_a, &DigitalControlPanel::autoScaleClicked, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (DigitalControlPanel::*)()>(_a, &DigitalControlPanel::forceTriggerClicked, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (DigitalControlPanel::*)()>(_a, &DigitalControlPanel::clearClicked, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (DigitalControlPanel::*)()>(_a, &DigitalControlPanel::fftClicked, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (DigitalControlPanel::*)()>(_a, &DigitalControlPanel::protocolDecoderClicked, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (DigitalControlPanel::*)(const QString & , const QString & )>(_a, &DigitalControlPanel::measurementAdded, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (DigitalControlPanel::*)()>(_a, &DigitalControlPanel::triggerSettingsChanged, 8))
            return;
    }
}

const QMetaObject *DigitalControlPanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DigitalControlPanel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN19DigitalControlPanelE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int DigitalControlPanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 16;
    }
    return _id;
}

// SIGNAL 0
void DigitalControlPanel::runStopClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void DigitalControlPanel::singleClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void DigitalControlPanel::autoScaleClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void DigitalControlPanel::forceTriggerClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void DigitalControlPanel::clearClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void DigitalControlPanel::fftClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void DigitalControlPanel::protocolDecoderClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void DigitalControlPanel::measurementAdded(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1, _t2);
}

// SIGNAL 8
void DigitalControlPanel::triggerSettingsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}
QT_WARNING_POP
