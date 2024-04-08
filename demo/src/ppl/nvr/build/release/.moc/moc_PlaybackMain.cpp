/****************************************************************************
** Meta object code from reading C++ file 'PlaybackMain.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../gui/ui/playback/PlaybackMain.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PlaybackMain.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_PlaybackMain_t {
    QByteArrayData data[16];
    char stringdata0[248];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PlaybackMain_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PlaybackMain_t qt_meta_stringdata_PlaybackMain = {
    {
QT_MOC_LITERAL(0, 0, 12), // "PlaybackMain"
QT_MOC_LITERAL(1, 13, 22), // "signal_result_feedback"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 19), // "AX_NVR_ACTION_RES_T"
QT_MOC_LITERAL(4, 57, 12), // "enActionType"
QT_MOC_LITERAL(5, 70, 26), // "signal_stop_status_changed"
QT_MOC_LITERAL(6, 97, 7), // "nStatus"
QT_MOC_LITERAL(7, 105, 18), // "OnChangeSplitVideo"
QT_MOC_LITERAL(8, 124, 10), // "SPLIT_TYPE"
QT_MOC_LITERAL(9, 135, 11), // "enSplitType"
QT_MOC_LITERAL(10, 147, 16), // "OnChangeMainSub1"
QT_MOC_LITERAL(11, 164, 18), // "OnDoPlaybackAction"
QT_MOC_LITERAL(12, 183, 20), // "PLAYBACK_ACTION_TYPE"
QT_MOC_LITERAL(13, 204, 24), // "OnDoPlaybackActionUpdate"
QT_MOC_LITERAL(14, 229, 12), // "OnLocateTime"
QT_MOC_LITERAL(15, 242, 5) // "nHHMM"

    },
    "PlaybackMain\0signal_result_feedback\0"
    "\0AX_NVR_ACTION_RES_T\0enActionType\0"
    "signal_stop_status_changed\0nStatus\0"
    "OnChangeSplitVideo\0SPLIT_TYPE\0enSplitType\0"
    "OnChangeMainSub1\0OnDoPlaybackAction\0"
    "PLAYBACK_ACTION_TYPE\0OnDoPlaybackActionUpdate\0"
    "OnLocateTime\0nHHMM"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PlaybackMain[] = {

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
       5,    1,   52,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    1,   55,    2, 0x0a /* Public */,
      10,    0,   58,    2, 0x0a /* Public */,
      11,    1,   59,    2, 0x0a /* Public */,
      13,    1,   62,    2, 0x0a /* Public */,
      14,    1,   65,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::Int,    6,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 12,    4,
    QMetaType::Void, 0x80000000 | 12,    4,
    QMetaType::Void, QMetaType::Int,   15,

       0        // eod
};

void PlaybackMain::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PlaybackMain *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->signal_result_feedback((*reinterpret_cast< const AX_NVR_ACTION_RES_T(*)>(_a[1]))); break;
        case 1: _t->signal_stop_status_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->OnChangeSplitVideo((*reinterpret_cast< SPLIT_TYPE(*)>(_a[1]))); break;
        case 3: _t->OnChangeMainSub1(); break;
        case 4: _t->OnDoPlaybackAction((*reinterpret_cast< PLAYBACK_ACTION_TYPE(*)>(_a[1]))); break;
        case 5: _t->OnDoPlaybackActionUpdate((*reinterpret_cast< PLAYBACK_ACTION_TYPE(*)>(_a[1]))); break;
        case 6: _t->OnLocateTime((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (PlaybackMain::*)(const AX_NVR_ACTION_RES_T & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PlaybackMain::signal_result_feedback)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (PlaybackMain::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PlaybackMain::signal_stop_status_changed)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject PlaybackMain::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_PlaybackMain.data,
    qt_meta_data_PlaybackMain,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *PlaybackMain::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PlaybackMain::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PlaybackMain.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int PlaybackMain::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
void PlaybackMain::signal_result_feedback(const AX_NVR_ACTION_RES_T & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void PlaybackMain::signal_stop_status_changed(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
