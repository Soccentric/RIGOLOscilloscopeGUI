/****************************************************************************
** Meta object code from reading C++ file 'dataexporter.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/utils/dataexporter.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dataexporter.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN12DataExporterE_t {};
} // unnamed namespace

template <> constexpr inline auto DataExporter::qt_create_metaobjectdata<qt_meta_tag_ZN12DataExporterE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "DataExporter",
        "exportProgress",
        "",
        "percent",
        "exportComplete",
        "filename",
        "exportError",
        "error",
        "Format",
        "CSV",
        "TSV",
        "Binary",
        "MATLAB",
        "VCD",
        "WAV",
        "PNG",
        "JSON"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'exportProgress'
        QtMocHelpers::SignalData<void(int)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Signal 'exportComplete'
        QtMocHelpers::SignalData<void(const QString &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 5 },
        }}),
        // Signal 'exportError'
        QtMocHelpers::SignalData<void(const QString &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
        // enum 'Format'
        QtMocHelpers::EnumData<enum Format>(8, 8, QMC::EnumIsScoped).add({
            {    9, Format::CSV },
            {   10, Format::TSV },
            {   11, Format::Binary },
            {   12, Format::MATLAB },
            {   13, Format::VCD },
            {   14, Format::WAV },
            {   15, Format::PNG },
            {   16, Format::JSON },
        }),
    };
    return QtMocHelpers::metaObjectData<DataExporter, qt_meta_tag_ZN12DataExporterE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject DataExporter::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12DataExporterE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12DataExporterE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN12DataExporterE_t>.metaTypes,
    nullptr
} };

void DataExporter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<DataExporter *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->exportProgress((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->exportComplete((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->exportError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (DataExporter::*)(int )>(_a, &DataExporter::exportProgress, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (DataExporter::*)(const QString & )>(_a, &DataExporter::exportComplete, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (DataExporter::*)(const QString & )>(_a, &DataExporter::exportError, 2))
            return;
    }
}

const QMetaObject *DataExporter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DataExporter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12DataExporterE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int DataExporter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void DataExporter::exportProgress(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void DataExporter::exportComplete(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void DataExporter::exportError(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}
QT_WARNING_POP
