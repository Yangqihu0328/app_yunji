/****************************************************************************
** Meta object code from reading C++ file 'ts_preview.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../gui/ui/test_suite/ts_preview.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ts_preview.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CTestPreview_t {
    QByteArrayData data[18];
    char stringdata0[236];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CTestPreview_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CTestPreview_t qt_meta_stringdata_CTestPreview = {
    {
QT_MOC_LITERAL(0, 0, 12), // "CTestPreview"
QT_MOC_LITERAL(1, 13, 20), // "signal_ts_widget_opr"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 19), // "TS_OPERATION_INFO_T"
QT_MOC_LITERAL(4, 55, 8), // "tOprInfo"
QT_MOC_LITERAL(5, 64, 12), // "OnRecvResult"
QT_MOC_LITERAL(6, 77, 19), // "AX_NVR_ACTION_RES_T"
QT_MOC_LITERAL(7, 97, 13), // "tActionResult"
QT_MOC_LITERAL(8, 111, 12), // "initTestCase"
QT_MOC_LITERAL(9, 124, 7), // "AX_BOOL"
QT_MOC_LITERAL(10, 132, 15), // "cleanupTestCase"
QT_MOC_LITERAL(11, 148, 4), // "init"
QT_MOC_LITERAL(12, 153, 17), // "testChangeMainSub"
QT_MOC_LITERAL(13, 171, 10), // "testMinMax"
QT_MOC_LITERAL(14, 182, 15), // "testChangeSplit"
QT_MOC_LITERAL(15, 198, 18), // "testChangePrevNext"
QT_MOC_LITERAL(16, 217, 7), // "testPIP"
QT_MOC_LITERAL(17, 225, 10) // "testRandom"

    },
    "CTestPreview\0signal_ts_widget_opr\0\0"
    "TS_OPERATION_INFO_T\0tOprInfo\0OnRecvResult\0"
    "AX_NVR_ACTION_RES_T\0tActionResult\0"
    "initTestCase\0AX_BOOL\0cleanupTestCase\0"
    "init\0testChangeMainSub\0testMinMax\0"
    "testChangeSplit\0testChangePrevNext\0"
    "testPIP\0testRandom"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CTestPreview[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   69,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,   72,    2, 0x0a /* Public */,
       8,    0,   75,    2, 0x0a /* Public */,
      10,    0,   76,    2, 0x0a /* Public */,
      11,    0,   77,    2, 0x0a /* Public */,
      12,    0,   78,    2, 0x0a /* Public */,
      13,    0,   79,    2, 0x0a /* Public */,
      14,    0,   80,    2, 0x0a /* Public */,
      15,    0,   81,    2, 0x0a /* Public */,
      16,    0,   82,    2, 0x0a /* Public */,
      17,    0,   83,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 6,    7,
    0x80000000 | 9,
    QMetaType::Void,
    0x80000000 | 9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CTestPreview::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CTestPreview *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->signal_ts_widget_opr((*reinterpret_cast< const TS_OPERATION_INFO_T(*)>(_a[1]))); break;
        case 1: _t->OnRecvResult((*reinterpret_cast< const AX_NVR_ACTION_RES_T(*)>(_a[1]))); break;
        case 2: { AX_BOOL _r = _t->initTestCase();
            if (_a[0]) *reinterpret_cast< AX_BOOL*>(_a[0]) = std::move(_r); }  break;
        case 3: _t->cleanupTestCase(); break;
        case 4: { AX_BOOL _r = _t->init();
            if (_a[0]) *reinterpret_cast< AX_BOOL*>(_a[0]) = std::move(_r); }  break;
        case 5: _t->testChangeMainSub(); break;
        case 6: _t->testMinMax(); break;
        case 7: _t->testChangeSplit(); break;
        case 8: _t->testChangePrevNext(); break;
        case 9: _t->testPIP(); break;
        case 10: _t->testRandom(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CTestPreview::*)(const TS_OPERATION_INFO_T & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CTestPreview::signal_ts_widget_opr)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CTestPreview::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CTestPreview.data,
    qt_meta_data_CTestPreview,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CTestPreview::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CTestPreview::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CTestPreview.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "CTestSuiteBase"))
        return static_cast< CTestSuiteBase*>(this);
    return QObject::qt_metacast(_clname);
}

int CTestPreview::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void CTestPreview::signal_ts_widget_opr(const TS_OPERATION_INFO_T & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
