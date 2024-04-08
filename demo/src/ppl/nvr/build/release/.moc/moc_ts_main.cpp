/****************************************************************************
** Meta object code from reading C++ file 'ts_main.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../gui/ui/test_suite/ts_main.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ts_main.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CTestMain_t {
    QByteArrayData data[10];
    char stringdata0[135];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CTestMain_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CTestMain_t qt_meta_stringdata_CTestMain = {
    {
QT_MOC_LITERAL(0, 0, 9), // "CTestMain"
QT_MOC_LITERAL(1, 10, 20), // "signal_ts_widget_opr"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 19), // "TS_OPERATION_INFO_T"
QT_MOC_LITERAL(4, 52, 8), // "tOprInfo"
QT_MOC_LITERAL(5, 61, 12), // "OnRecvResult"
QT_MOC_LITERAL(6, 74, 19), // "AX_NVR_ACTION_RES_T"
QT_MOC_LITERAL(7, 94, 13), // "tActionResult"
QT_MOC_LITERAL(8, 108, 11), // "testVersion"
QT_MOC_LITERAL(9, 120, 14) // "testOpenPatrol"

    },
    "CTestMain\0signal_ts_widget_opr\0\0"
    "TS_OPERATION_INFO_T\0tOprInfo\0OnRecvResult\0"
    "AX_NVR_ACTION_RES_T\0tActionResult\0"
    "testVersion\0testOpenPatrol"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CTestMain[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,   37,    2, 0x0a /* Public */,
       8,    0,   40,    2, 0x0a /* Public */,
       9,    0,   41,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CTestMain::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CTestMain *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->signal_ts_widget_opr((*reinterpret_cast< const TS_OPERATION_INFO_T(*)>(_a[1]))); break;
        case 1: _t->OnRecvResult((*reinterpret_cast< const AX_NVR_ACTION_RES_T(*)>(_a[1]))); break;
        case 2: _t->testVersion(); break;
        case 3: _t->testOpenPatrol(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CTestMain::*)(const TS_OPERATION_INFO_T & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CTestMain::signal_ts_widget_opr)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CTestMain::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CTestMain.data,
    qt_meta_data_CTestMain,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CTestMain::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CTestMain::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CTestMain.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "CTestSuiteBase"))
        return static_cast< CTestSuiteBase*>(this);
    return QObject::qt_metacast(_clname);
}

int CTestMain::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void CTestMain::signal_ts_widget_opr(const TS_OPERATION_INFO_T & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
