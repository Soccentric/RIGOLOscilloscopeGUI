/****************************************************************************
** Meta object code from reading C++ file 'scopesettings.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/core/scopesettings.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'scopesettings.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN13ScopeSettingsE_t {};
} // unnamed namespace

template <> constexpr inline auto ScopeSettings::qt_create_metaobjectdata<qt_meta_tag_ZN13ScopeSettingsE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ScopeSettings",
        "timebaseChanged",
        "",
        "timebase",
        "horizontalOffsetChanged",
        "offset",
        "acquisitionModeChanged",
        "AcquisitionMode",
        "mode",
        "averageCountChanged",
        "count",
        "memoryDepthChanged",
        "depth",
        "triggerModeChanged",
        "TriggerMode",
        "triggerSlopeChanged",
        "TriggerSlope",
        "slope",
        "triggerSourceChanged",
        "source",
        "triggerLevelChanged",
        "level",
        "runStateChanged",
        "RunState",
        "state",
        "horizontalOffset",
        "acquisitionMode",
        "triggerMode",
        "triggerSlope",
        "triggerSource",
        "triggerLevel",
        "memoryDepth",
        "Normal",
        "Average",
        "HighResolution",
        "PeakDetect",
        "Auto",
        "Single",
        "Rising",
        "Falling",
        "Either",
        "Stopped",
        "Running",
        "Waiting"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'timebaseChanged'
        QtMocHelpers::SignalData<void(double)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 3 },
        }}),
        // Signal 'horizontalOffsetChanged'
        QtMocHelpers::SignalData<void(double)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 5 },
        }}),
        // Signal 'acquisitionModeChanged'
        QtMocHelpers::SignalData<void(enum AcquisitionMode)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 7, 8 },
        }}),
        // Signal 'averageCountChanged'
        QtMocHelpers::SignalData<void(int)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 10 },
        }}),
        // Signal 'memoryDepthChanged'
        QtMocHelpers::SignalData<void(int)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 12 },
        }}),
        // Signal 'triggerModeChanged'
        QtMocHelpers::SignalData<void(enum TriggerMode)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 14, 8 },
        }}),
        // Signal 'triggerSlopeChanged'
        QtMocHelpers::SignalData<void(enum TriggerSlope)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 16, 17 },
        }}),
        // Signal 'triggerSourceChanged'
        QtMocHelpers::SignalData<void(const QString &)>(18, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 19 },
        }}),
        // Signal 'triggerLevelChanged'
        QtMocHelpers::SignalData<void(double)>(20, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 21 },
        }}),
        // Signal 'runStateChanged'
        QtMocHelpers::SignalData<void(enum RunState)>(22, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 23, 24 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'timebase'
        QtMocHelpers::PropertyData<double>(3, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 0),
        // property 'horizontalOffset'
        QtMocHelpers::PropertyData<double>(25, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 1),
        // property 'acquisitionMode'
        QtMocHelpers::PropertyData<enum AcquisitionMode>(26, 0x80000000 | 7, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet, 2),
        // property 'triggerMode'
        QtMocHelpers::PropertyData<enum TriggerMode>(27, 0x80000000 | 14, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet, 5),
        // property 'triggerSlope'
        QtMocHelpers::PropertyData<enum TriggerSlope>(28, 0x80000000 | 16, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet, 6),
        // property 'triggerSource'
        QtMocHelpers::PropertyData<QString>(29, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 7),
        // property 'triggerLevel'
        QtMocHelpers::PropertyData<double>(30, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 8),
        // property 'memoryDepth'
        QtMocHelpers::PropertyData<int>(31, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 4),
    };
    QtMocHelpers::UintData qt_enums {
        // enum 'AcquisitionMode'
        QtMocHelpers::EnumData<enum AcquisitionMode>(7, 7, QMC::EnumIsScoped).add({
            {   32, AcquisitionMode::Normal },
            {   33, AcquisitionMode::Average },
            {   34, AcquisitionMode::HighResolution },
            {   35, AcquisitionMode::PeakDetect },
        }),
        // enum 'TriggerMode'
        QtMocHelpers::EnumData<enum TriggerMode>(14, 14, QMC::EnumIsScoped).add({
            {   36, TriggerMode::Auto },
            {   32, TriggerMode::Normal },
            {   37, TriggerMode::Single },
        }),
        // enum 'TriggerSlope'
        QtMocHelpers::EnumData<enum TriggerSlope>(16, 16, QMC::EnumIsScoped).add({
            {   38, TriggerSlope::Rising },
            {   39, TriggerSlope::Falling },
            {   40, TriggerSlope::Either },
        }),
        // enum 'RunState'
        QtMocHelpers::EnumData<enum RunState>(23, 23, QMC::EnumIsScoped).add({
            {   41, RunState::Stopped },
            {   42, RunState::Running },
            {   37, RunState::Single },
            {   43, RunState::Waiting },
        }),
    };
    return QtMocHelpers::metaObjectData<ScopeSettings, qt_meta_tag_ZN13ScopeSettingsE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ScopeSettings::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13ScopeSettingsE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13ScopeSettingsE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN13ScopeSettingsE_t>.metaTypes,
    nullptr
} };

void ScopeSettings::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ScopeSettings *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->timebaseChanged((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 1: _t->horizontalOffsetChanged((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 2: _t->acquisitionModeChanged((*reinterpret_cast< std::add_pointer_t<enum AcquisitionMode>>(_a[1]))); break;
        case 3: _t->averageCountChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 4: _t->memoryDepthChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->triggerModeChanged((*reinterpret_cast< std::add_pointer_t<enum TriggerMode>>(_a[1]))); break;
        case 6: _t->triggerSlopeChanged((*reinterpret_cast< std::add_pointer_t<enum TriggerSlope>>(_a[1]))); break;
        case 7: _t->triggerSourceChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 8: _t->triggerLevelChanged((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 9: _t->runStateChanged((*reinterpret_cast< std::add_pointer_t<enum RunState>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ScopeSettings::*)(double )>(_a, &ScopeSettings::timebaseChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScopeSettings::*)(double )>(_a, &ScopeSettings::horizontalOffsetChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScopeSettings::*)(AcquisitionMode )>(_a, &ScopeSettings::acquisitionModeChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScopeSettings::*)(int )>(_a, &ScopeSettings::averageCountChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScopeSettings::*)(int )>(_a, &ScopeSettings::memoryDepthChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScopeSettings::*)(TriggerMode )>(_a, &ScopeSettings::triggerModeChanged, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScopeSettings::*)(TriggerSlope )>(_a, &ScopeSettings::triggerSlopeChanged, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScopeSettings::*)(const QString & )>(_a, &ScopeSettings::triggerSourceChanged, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScopeSettings::*)(double )>(_a, &ScopeSettings::triggerLevelChanged, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScopeSettings::*)(RunState )>(_a, &ScopeSettings::runStateChanged, 9))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<double*>(_v) = _t->timebase(); break;
        case 1: *reinterpret_cast<double*>(_v) = _t->horizontalOffset(); break;
        case 2: *reinterpret_cast<enum AcquisitionMode*>(_v) = _t->acquisitionMode(); break;
        case 3: *reinterpret_cast<enum TriggerMode*>(_v) = _t->triggerMode(); break;
        case 4: *reinterpret_cast<enum TriggerSlope*>(_v) = _t->triggerSlope(); break;
        case 5: *reinterpret_cast<QString*>(_v) = _t->triggerSource(); break;
        case 6: *reinterpret_cast<double*>(_v) = _t->triggerLevel(); break;
        case 7: *reinterpret_cast<int*>(_v) = _t->memoryDepth(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setTimebase(*reinterpret_cast<double*>(_v)); break;
        case 1: _t->setHorizontalOffset(*reinterpret_cast<double*>(_v)); break;
        case 2: _t->setAcquisitionMode(*reinterpret_cast<enum AcquisitionMode*>(_v)); break;
        case 3: _t->setTriggerMode(*reinterpret_cast<enum TriggerMode*>(_v)); break;
        case 4: _t->setTriggerSlope(*reinterpret_cast<enum TriggerSlope*>(_v)); break;
        case 5: _t->setTriggerSource(*reinterpret_cast<QString*>(_v)); break;
        case 6: _t->setTriggerLevel(*reinterpret_cast<double*>(_v)); break;
        case 7: _t->setMemoryDepth(*reinterpret_cast<int*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *ScopeSettings::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScopeSettings::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13ScopeSettingsE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ScopeSettings::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 10;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void ScopeSettings::timebaseChanged(double _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void ScopeSettings::horizontalOffsetChanged(double _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void ScopeSettings::acquisitionModeChanged(AcquisitionMode _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void ScopeSettings::averageCountChanged(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void ScopeSettings::memoryDepthChanged(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void ScopeSettings::triggerModeChanged(TriggerMode _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void ScopeSettings::triggerSlopeChanged(TriggerSlope _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}

// SIGNAL 7
void ScopeSettings::triggerSourceChanged(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1);
}

// SIGNAL 8
void ScopeSettings::triggerLevelChanged(double _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1);
}

// SIGNAL 9
void ScopeSettings::runStateChanged(RunState _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 9, nullptr, _t1);
}
QT_WARNING_POP
