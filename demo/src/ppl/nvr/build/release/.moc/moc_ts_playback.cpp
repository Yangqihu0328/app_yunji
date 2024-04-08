/****************************************************************************
** Meta object code from reading C++ file 'ts_playback.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../gui/ui/test_suite/ts_playback.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ts_playback.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CTestPlayback_t {
    QByteArrayData data[20];
    char stringdata0[264];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CTestPlayback_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CTestPlayback_t qt_meta_stringdata_CTestPlayback = {
    {
QT_MOC_LITERAL(0, 0, 13), // "CTestPlayback"
QT_MOC_LITERAL(1, 14, 16), // "tableCellClicked"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 4), // "nRow"
QT_MOC_LITERAL(4, 37, 4), // "nCol"
QT_MOC_LITERAL(5, 42, 10), // "timeLocate"
QT_MOC_LITERAL(6, 53, 5), // "nHHMM"
QT_MOC_LITERAL(7, 59, 20), // "signal_ts_widget_opr"
QT_MOC_LITERAL(8, 80, 19), // "TS_OPERATION_INFO_T"
QT_MOC_LITERAL(9, 100, 8), // "tOprInfo"
QT_MOC_LITERAL(10, 109, 12), // "OnRecvResult"
QT_MOC_LITERAL(11, 122, 19), // "AX_NVR_ACTION_RES_T"
QT_MOC_LITERAL(12, 142, 13), // "tActionResult"
QT_MOC_LITERAL(13, 156, 12), // "initTestCase"
QT_MOC_LITERAL(14, 169, 7), // "AX_BOOL"
QT_MOC_LITERAL(15, 177, 15), // "cleanupTestCase"
QT_MOC_LITERAL(16, 193, 4), // "init"
QT_MOC_LITERAL(17, 198, 27), // "testSingleStreamPlayProcess"
QT_MOC_LITERAL(18, 226, 26), // "testMultiStreamPlayProcess"
QT_MOC_LITERAL(19, 253, 10) // "testRandom"

    },
    "CTestPlayback\0tableCellClicked\0\0nRow\0"
    "nCol\0timeLocate\0nHHMM\0signal_ts_widget_opr\0"
    "TS_OPERATION_INFO_T\0tOprInfo\0OnRecvResult\0"
    "AX_NVR_ACTION_RES_T\0tActionResult\0"
    "initTestCase\0AX_BOOL\0cleanupTestCase\0"
    "init\0testSingleStreamPlayProcess\0"
    "testMultiStreamPlayProcess\0testRandom"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CTestPlayback[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   64,    2, 0x06 /* Public */,
       5,    1,   69,    2, 0x06 /* Public */,
       7,    1,   72,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    1,   75,    2, 0x0a /* Public */,
      13,    0,   78,    2, 0x0a /* Public */,
      15,    0,   79,    2, 0x0a /* Public */,
      16,    0,   80,    2, 0x0a /* Public */,
      17,    0,   81,    2, 0x0a /* Public */,
      18,    0,   82,    2, 0x0a /* Public */,
      19,    0,   83,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    4,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, 0x80000000 | 8,    9,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 11,   12,
    0x80000000 | 14,
    QMetaType::Void,
    0x80000000 | 14,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CTestPlayback::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CTestPlayback *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->tableCellClicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->timeLocate((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->signal_ts_widget_opr((*reinterpret_cast< const TS_OPERATION_INFO_T(*)>(_a[1]))); break;
        case 3: _t->OnRecvResult((*reinterpret_cast< const AX_NVR_ACTION_RES_T(*)>(_a[1]))); break;
        case 4: { AX_BOOL _r = _t->initTestCase();
            if (_a[0]) *reinterpret_cast< AX_BOOL*>(_a[0]) = std::move(_r); }  break;
        case 5: _t->cleanupTestCase(); break;
        case 6: { AX_BOOL _r = _t->init();
            if (_a[0]) *reinterpret_cast< AX_BOOL*>(_a[0]) = std::move(_r); }  break;
        case 7: _t->testSingleStreamPlayProcess(); break;
        case 8: _t->testMultiStreamPlayProcess(); break;
        case 9: _t->testRandom(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CTestPlayback::*)(int , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CTestPlayback::tableCellClicked)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CTestPlayback::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CTestPlayback::timeLocate)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (CTestPlayback::*)(const TS_OPERATION_INFO_T & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CTestPlayback::signal_ts_widget_opr)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CTestPlayback::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CTestPlayback.data,
    qt_meta_data_CTestPlayback,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CTestPlayback::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CTestPlayback::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CTestPlayback.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "CTestSuiteBase"))
        return static_cast< CTestSuiteBase*>(this);
    return QObject::qt_metacast(_clname);
}

int CTestPlayback::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void CTestPlayback::tableCellClicked(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CTestPlayback::timeLocate(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CTestPlayback::signal_ts_widget_opr(const TS_OPERATION_INFO_T & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
