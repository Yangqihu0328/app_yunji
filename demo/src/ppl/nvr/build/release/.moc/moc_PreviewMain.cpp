/****************************************************************************
** Meta object code from reading C++ file 'PreviewMain.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../gui/ui/preview/PreviewMain.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PreviewMain.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_PreviewMain_t {
    QByteArrayData data[16];
    char stringdata0[231];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PreviewMain_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PreviewMain_t qt_meta_stringdata_PreviewMain = {
    {
QT_MOC_LITERAL(0, 0, 11), // "PreviewMain"
QT_MOC_LITERAL(1, 12, 22), // "signal_result_feedback"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 19), // "AX_NVR_ACTION_RES_T"
QT_MOC_LITERAL(4, 56, 12), // "enActionType"
QT_MOC_LITERAL(5, 69, 18), // "OnChangeSplitVideo"
QT_MOC_LITERAL(6, 88, 10), // "SPLIT_TYPE"
QT_MOC_LITERAL(7, 99, 11), // "enSplitType"
QT_MOC_LITERAL(8, 111, 16), // "OnChangePrevNext"
QT_MOC_LITERAL(9, 128, 14), // "PREV_NEXT_TYPE"
QT_MOC_LITERAL(10, 143, 14), // "enPrevNextType"
QT_MOC_LITERAL(11, 158, 16), // "OnChangeMainSub1"
QT_MOC_LITERAL(12, 175, 11), // "OnEnablePip"
QT_MOC_LITERAL(13, 187, 7), // "bEnable"
QT_MOC_LITERAL(14, 195, 27), // "OnPlaybackStopStatusChanged"
QT_MOC_LITERAL(15, 223, 7) // "nStatus"

    },
    "PreviewMain\0signal_result_feedback\0\0"
    "AX_NVR_ACTION_RES_T\0enActionType\0"
    "OnChangeSplitVideo\0SPLIT_TYPE\0enSplitType\0"
    "OnChangePrevNext\0PREV_NEXT_TYPE\0"
    "enPrevNextType\0OnChangeMainSub1\0"
    "OnEnablePip\0bEnable\0OnPlaybackStopStatusChanged\0"
    "nStatus"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PreviewMain[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,   47,    2, 0x0a /* Public */,
       8,    1,   50,    2, 0x0a /* Public */,
      11,    0,   53,    2, 0x0a /* Public */,
      12,    1,   54,    2, 0x0a /* Public */,
      14,    1,   57,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   13,
    QMetaType::Void, QMetaType::Int,   15,

       0        // eod
};

void PreviewMain::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PreviewMain *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->signal_result_feedback((*reinterpret_cast< const AX_NVR_ACTION_RES_T(*)>(_a[1]))); break;
        case 1: _t->OnChangeSplitVideo((*reinterpret_cast< SPLIT_TYPE(*)>(_a[1]))); break;
        case 2: _t->OnChangePrevNext((*reinterpret_cast< PREV_NEXT_TYPE(*)>(_a[1]))); break;
        case 3: _t->OnChangeMainSub1(); break;
        case 4: _t->OnEnablePip((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->OnPlaybackStopStatusChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (PreviewMain::*)(const AX_NVR_ACTION_RES_T & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PreviewMain::signal_result_feedback)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject PreviewMain::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_PreviewMain.data,
    qt_meta_data_PreviewMain,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *PreviewMain::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PreviewMain::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PreviewMain.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int PreviewMain::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void PreviewMain::signal_result_feedback(const AX_NVR_ACTION_RES_T & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
