/****************************************************************************
** Meta object code from reading C++ file 'scopechannel.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/core/scopechannel.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'scopechannel.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN12ScopeChannelE_t {};
} // unnamed namespace

template <> constexpr inline auto ScopeChannel::qt_create_metaobjectdata<qt_meta_tag_ZN12ScopeChannelE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ScopeChannel",
        "enabledChanged",
        "",
        "enabled",
        "scaleChanged",
        "scale",
        "offsetChanged",
        "offset",
        "colorChanged",
        "color",
        "couplingChanged",
        "Coupling",
        "coupling",
        "probeChanged",
        "Probe",
        "probe",
        "dataChanged",
        "name",
        "ChannelType",
        "Analog",
        "Digital",
        "Math",
        "DC",
        "AC",
        "GND",
        "X1",
        "X10",
        "X100",
        "X1000"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'enabledChanged'
        QtMocHelpers::SignalData<void(bool)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 3 },
        }}),
        // Signal 'scaleChanged'
        QtMocHelpers::SignalData<void(double)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 5 },
        }}),
        // Signal 'offsetChanged'
        QtMocHelpers::SignalData<void(double)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 7 },
        }}),
        // Signal 'colorChanged'
        QtMocHelpers::SignalData<void(const QColor &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QColor, 9 },
        }}),
        // Signal 'couplingChanged'
        QtMocHelpers::SignalData<void(enum Coupling)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 11, 12 },
        }}),
        // Signal 'probeChanged'
        QtMocHelpers::SignalData<void(enum Probe)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 14, 15 },
        }}),
        // Signal 'dataChanged'
        QtMocHelpers::SignalData<void()>(16, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'name'
        QtMocHelpers::PropertyData<QString>(17, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Constant),
        // property 'enabled'
        QtMocHelpers::PropertyData<bool>(3, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 0),
        // property 'scale'
        QtMocHelpers::PropertyData<double>(5, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 1),
        // property 'offset'
        QtMocHelpers::PropertyData<double>(7, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 2),
        // property 'color'
        QtMocHelpers::PropertyData<QColor>(9, QMetaType::QColor, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 3),
        // property 'coupling'
        QtMocHelpers::PropertyData<enum Coupling>(12, 0x80000000 | 11, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet, 4),
        // property 'probe'
        QtMocHelpers::PropertyData<enum Probe>(15, 0x80000000 | 14, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet, 5),
    };
    QtMocHelpers::UintData qt_enums {
        // enum 'ChannelType'
        QtMocHelpers::EnumData<enum ChannelType>(18, 18, QMC::EnumIsScoped).add({
            {   19, ChannelType::Analog },
            {   20, ChannelType::Digital },
            {   21, ChannelType::Math },
        }),
        // enum 'Coupling'
        QtMocHelpers::EnumData<enum Coupling>(11, 11, QMC::EnumIsScoped).add({
            {   22, Coupling::DC },
            {   23, Coupling::AC },
            {   24, Coupling::GND },
        }),
        // enum 'Probe'
        QtMocHelpers::EnumData<enum Probe>(14, 14, QMC::EnumIsScoped).add({
            {   25, Probe::X1 },
            {   26, Probe::X10 },
            {   27, Probe::X100 },
            {   28, Probe::X1000 },
        }),
    };
    return QtMocHelpers::metaObjectData<ScopeChannel, qt_meta_tag_ZN12ScopeChannelE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ScopeChannel::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12ScopeChannelE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12ScopeChannelE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN12ScopeChannelE_t>.metaTypes,
    nullptr
} };

void ScopeChannel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ScopeChannel *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->enabledChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 1: _t->scaleChanged((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 2: _t->offsetChanged((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 3: _t->colorChanged((*reinterpret_cast< std::add_pointer_t<QColor>>(_a[1]))); break;
        case 4: _t->couplingChanged((*reinterpret_cast< std::add_pointer_t<enum Coupling>>(_a[1]))); break;
        case 5: _t->probeChanged((*reinterpret_cast< std::add_pointer_t<enum Probe>>(_a[1]))); break;
        case 6: _t->dataChanged(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ScopeChannel::*)(bool )>(_a, &ScopeChannel::enabledChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScopeChannel::*)(double )>(_a, &ScopeChannel::scaleChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScopeChannel::*)(double )>(_a, &ScopeChannel::offsetChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScopeChannel::*)(const QColor & )>(_a, &ScopeChannel::colorChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScopeChannel::*)(Coupling )>(_a, &ScopeChannel::couplingChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScopeChannel::*)(Probe )>(_a, &ScopeChannel::probeChanged, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (ScopeChannel::*)()>(_a, &ScopeChannel::dataChanged, 6))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<QString*>(_v) = _t->name(); break;
        case 1: *reinterpret_cast<bool*>(_v) = _t->isEnabled(); break;
        case 2: *reinterpret_cast<double*>(_v) = _t->scale(); break;
        case 3: *reinterpret_cast<double*>(_v) = _t->offset(); break;
        case 4: *reinterpret_cast<QColor*>(_v) = _t->color(); break;
        case 5: *reinterpret_cast<enum Coupling*>(_v) = _t->coupling(); break;
        case 6: *reinterpret_cast<enum Probe*>(_v) = _t->probe(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 1: _t->setEnabled(*reinterpret_cast<bool*>(_v)); break;
        case 2: _t->setScale(*reinterpret_cast<double*>(_v)); break;
        case 3: _t->setOffset(*reinterpret_cast<double*>(_v)); break;
        case 4: _t->setColor(*reinterpret_cast<QColor*>(_v)); break;
        case 5: _t->setCoupling(*reinterpret_cast<enum Coupling*>(_v)); break;
        case 6: _t->setProbe(*reinterpret_cast<enum Probe*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *ScopeChannel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScopeChannel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12ScopeChannelE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ScopeChannel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void ScopeChannel::enabledChanged(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void ScopeChannel::scaleChanged(double _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void ScopeChannel::offsetChanged(double _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void ScopeChannel::colorChanged(const QColor & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void ScopeChannel::couplingChanged(Coupling _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void ScopeChannel::probeChanged(Probe _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void ScopeChannel::dataChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}
QT_WARNING_POP
