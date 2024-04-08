/****************************************************************************
** Meta object code from reading C++ file 'nvrmainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../gui/ui/nvrmainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'nvrmainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_NVRMainWindow_t {
    QByteArrayData data[16];
    char stringdata0[239];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_NVRMainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_NVRMainWindow_t qt_meta_stringdata_NVRMainWindow = {
    {
QT_MOC_LITERAL(0, 0, 13), // "NVRMainWindow"
QT_MOC_LITERAL(1, 14, 22), // "signal_result_feedback"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 19), // "AX_NVR_ACTION_RES_T"
QT_MOC_LITERAL(4, 58, 12), // "enActionType"
QT_MOC_LITERAL(5, 71, 11), // "signal_exit"
QT_MOC_LITERAL(6, 83, 15), // "OnEnablePolling"
QT_MOC_LITERAL(7, 99, 8), // "bPolling"
QT_MOC_LITERAL(8, 108, 16), // "OnTimeOutPolling"
QT_MOC_LITERAL(9, 125, 20), // "StartTestSuiteThread"
QT_MOC_LITERAL(10, 146, 20), // "AX_NVR_TS_CONFIG_PTR"
QT_MOC_LITERAL(11, 167, 6), // "pTsCfg"
QT_MOC_LITERAL(12, 174, 19), // "StopTestSuiteThread"
QT_MOC_LITERAL(13, 194, 15), // "OnOperateWidget"
QT_MOC_LITERAL(14, 210, 19), // "TS_OPERATION_INFO_T"
QT_MOC_LITERAL(15, 230, 8) // "tOprInfo"

    },
    "NVRMainWindow\0signal_result_feedback\0"
    "\0AX_NVR_ACTION_RES_T\0enActionType\0"
    "signal_exit\0OnEnablePolling\0bPolling\0"
    "OnTimeOutPolling\0StartTestSuiteThread\0"
    "AX_NVR_TS_CONFIG_PTR\0pTsCfg\0"
    "StopTestSuiteThread\0OnOperateWidget\0"
    "TS_OPERATION_INFO_T\0tOprInfo"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_NVRMainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x06 /* Public */,
       5,    0,   52,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    1,   53,    2, 0x0a /* Public */,
       8,    0,   56,    2, 0x08 /* Private */,
       9,    1,   57,    2, 0x08 /* Private */,
      12,    1,   60,    2, 0x08 /* Private */,
      13,    1,   63,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void, 0x80000000 | 14,   15,

       0        // eod
};

void NVRMainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<NVRMainWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->signal_result_feedback((*reinterpret_cast< const AX_NVR_ACTION_RES_T(*)>(_a[1]))); break;
        case 1: _t->signal_exit(); break;
        case 2: _t->OnEnablePolling((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->OnTimeOutPolling(); break;
        case 4: _t->StartTestSuiteThread((*reinterpret_cast< AX_NVR_TS_CONFIG_PTR(*)>(_a[1]))); break;
        case 5: _t->StopTestSuiteThread((*reinterpret_cast< AX_NVR_TS_CONFIG_PTR(*)>(_a[1]))); break;
        case 6: _t->OnOperateWidget((*reinterpret_cast< const TS_OPERATION_INFO_T(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (NVRMainWindow::*)(const AX_NVR_ACTION_RES_T & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NVRMainWindow::signal_result_feedback)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (NVRMainWindow::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&NVRMainWindow::signal_exit)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject NVRMainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_NVRMainWindow.data,
    qt_meta_data_NVRMainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *NVRMainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NVRMainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_NVRMainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int NVRMainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void NVRMainWindow::signal_result_feedback(const AX_NVR_ACTION_RES_T & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void NVRMainWindow::signal_exit()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
