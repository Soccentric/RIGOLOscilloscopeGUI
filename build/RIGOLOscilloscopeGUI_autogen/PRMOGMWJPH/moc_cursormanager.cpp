/****************************************************************************
** Meta object code from reading C++ file 'cursormanager.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/core/cursormanager.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cursormanager.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN13CursorManagerE_t {};
} // unnamed namespace

template <> constexpr inline auto CursorManager::qt_create_metaobjectdata<qt_meta_tag_ZN13CursorManagerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "CursorManager",
        "modeChanged",
        "",
        "CursorMode",
        "mode",
        "typeChanged",
        "CursorType",
        "type",
        "sourceChannelChanged",
        "channel",
        "cursorX1Changed",
        "x",
        "cursorX2Changed",
        "cursorY1Changed",
        "y",
        "cursorY2Changed",
        "cursorsChanged",
        "Off",
        "Manual",
        "Track",
        "Auto",
        "XY",
        "Horizontal",
        "Vertical",
        "Both"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'modeChanged'
        QtMocHelpers::SignalData<void(enum CursorMode)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'typeChanged'
        QtMocHelpers::SignalData<void(enum CursorType)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 6, 7 },
        }}),
        // Signal 'sourceChannelChanged'
        QtMocHelpers::SignalData<void(const QString &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 9 },
        }}),
        // Signal 'cursorX1Changed'
        QtMocHelpers::SignalData<void(double)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 11 },
        }}),
        // Signal 'cursorX2Changed'
        QtMocHelpers::SignalData<void(double)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 11 },
        }}),
        // Signal 'cursorY1Changed'
        QtMocHelpers::SignalData<void(double)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 14 },
        }}),
        // Signal 'cursorY2Changed'
        QtMocHelpers::SignalData<void(double)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 14 },
        }}),
        // Signal 'cursorsChanged'
        QtMocHelpers::SignalData<void()>(16, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
        // enum 'CursorMode'
        QtMocHelpers::EnumData<enum CursorMode>(3, 3, QMC::EnumIsScoped).add({
            {   17, CursorMode::Off },
            {   18, CursorMode::Manual },
            {   19, CursorMode::Track },
            {   20, CursorMode::Auto },
            {   21, CursorMode::XY },
        }),
        // enum 'CursorType'
        QtMocHelpers::EnumData<enum CursorType>(6, 6, QMC::EnumIsScoped).add({
            {   22, CursorType::Horizontal },
            {   23, CursorType::Vertical },
            {   24, CursorType::Both },
        }),
    };
    return QtMocHelpers::metaObjectData<CursorManager, qt_meta_tag_ZN13CursorManagerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject CursorManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13CursorManagerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13CursorManagerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN13CursorManagerE_t>.metaTypes,
    nullptr
} };

void CursorManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<CursorManager *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->modeChanged((*reinterpret_cast< std::add_pointer_t<enum CursorMode>>(_a[1]))); break;
        case 1: _t->typeChanged((*reinterpret_cast< std::add_pointer_t<enum CursorType>>(_a[1]))); break;
        case 2: _t->sourceChannelChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->cursorX1Changed((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 4: _t->cursorX2Changed((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 5: _t->cursorY1Changed((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 6: _t->cursorY2Changed((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 7: _t->cursorsChanged(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (CursorManager::*)(CursorMode )>(_a, &CursorManager::modeChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (CursorManager::*)(CursorType )>(_a, &CursorManager::typeChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (CursorManager::*)(const QString & )>(_a, &CursorManager::sourceChannelChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (CursorManager::*)(double )>(_a, &CursorManager::cursorX1Changed, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (CursorManager::*)(double )>(_a, &CursorManager::cursorX2Changed, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (CursorManager::*)(double )>(_a, &CursorManager::cursorY1Changed, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (CursorManager::*)(double )>(_a, &CursorManager::cursorY2Changed, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (CursorManager::*)()>(_a, &CursorManager::cursorsChanged, 7))
            return;
    }
}

const QMetaObject *CursorManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CursorManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13CursorManagerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int CursorManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void CursorManager::modeChanged(CursorMode _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void CursorManager::typeChanged(CursorType _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void CursorManager::sourceChannelChanged(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void CursorManager::cursorX1Changed(double _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void CursorManager::cursorX2Changed(double _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void CursorManager::cursorY1Changed(double _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void CursorManager::cursorY2Changed(double _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}

// SIGNAL 7
void CursorManager::cursorsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}
QT_WARNING_POP
