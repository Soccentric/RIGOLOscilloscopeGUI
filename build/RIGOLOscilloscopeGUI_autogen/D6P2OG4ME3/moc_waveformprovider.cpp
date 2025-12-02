/****************************************************************************
** Meta object code from reading C++ file 'waveformprovider.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/communication/waveformprovider.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'waveformprovider.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN16WaveformProviderE_t {};
} // unnamed namespace

template <> constexpr inline auto WaveformProvider::qt_create_metaobjectdata<qt_meta_tag_ZN16WaveformProviderE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "WaveformProvider",
        "waveformDataReady",
        "",
        "channel",
        "QList<QPointF>",
        "data",
        "preambleUpdated",
        "WaveformPreamble",
        "preamble",
        "acquisitionError",
        "errorMessage",
        "acquisitionStarted",
        "acquisitionStopped",
        "onAcquisitionTimer",
        "WaveformMode",
        "Normal",
        "Maximum",
        "Raw"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'waveformDataReady'
        QtMocHelpers::SignalData<void(const QString &, const QVector<QPointF> &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { 0x80000000 | 4, 5 },
        }}),
        // Signal 'preambleUpdated'
        QtMocHelpers::SignalData<void(const QString &, const WaveformPreamble &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { 0x80000000 | 7, 8 },
        }}),
        // Signal 'acquisitionError'
        QtMocHelpers::SignalData<void(const QString &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 10 },
        }}),
        // Signal 'acquisitionStarted'
        QtMocHelpers::SignalData<void()>(11, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'acquisitionStopped'
        QtMocHelpers::SignalData<void()>(12, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onAcquisitionTimer'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
        // enum 'WaveformMode'
        QtMocHelpers::EnumData<enum WaveformMode>(14, 14, QMC::EnumIsScoped).add({
            {   15, WaveformMode::Normal },
            {   16, WaveformMode::Maximum },
            {   17, WaveformMode::Raw },
        }),
    };
    return QtMocHelpers::metaObjectData<WaveformProvider, qt_meta_tag_ZN16WaveformProviderE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject WaveformProvider::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16WaveformProviderE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16WaveformProviderE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN16WaveformProviderE_t>.metaTypes,
    nullptr
} };

void WaveformProvider::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<WaveformProvider *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->waveformDataReady((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QList<QPointF>>>(_a[2]))); break;
        case 1: _t->preambleUpdated((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<WaveformPreamble>>(_a[2]))); break;
        case 2: _t->acquisitionError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->acquisitionStarted(); break;
        case 4: _t->acquisitionStopped(); break;
        case 5: _t->onAcquisitionTimer(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<QPointF> >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (WaveformProvider::*)(const QString & , const QVector<QPointF> & )>(_a, &WaveformProvider::waveformDataReady, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (WaveformProvider::*)(const QString & , const WaveformPreamble & )>(_a, &WaveformProvider::preambleUpdated, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (WaveformProvider::*)(const QString & )>(_a, &WaveformProvider::acquisitionError, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (WaveformProvider::*)()>(_a, &WaveformProvider::acquisitionStarted, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (WaveformProvider::*)()>(_a, &WaveformProvider::acquisitionStopped, 4))
            return;
    }
}

const QMetaObject *WaveformProvider::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WaveformProvider::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16WaveformProviderE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int WaveformProvider::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void WaveformProvider::waveformDataReady(const QString & _t1, const QVector<QPointF> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void WaveformProvider::preambleUpdated(const QString & _t1, const WaveformPreamble & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}

// SIGNAL 2
void WaveformProvider::acquisitionError(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void WaveformProvider::acquisitionStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void WaveformProvider::acquisitionStopped()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}
QT_WARNING_POP
