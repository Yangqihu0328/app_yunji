/****************************************************************************
** Meta object code from reading C++ file 'PlaybackLeftToolbar.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../gui/ui/playback/PlaybackLeftToolbar.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PlaybackLeftToolbar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_PlaybackLeftToolbar_t {
    QByteArrayData data[15];
    char stringdata0[210];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PlaybackLeftToolbar_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PlaybackLeftToolbar_t qt_meta_stringdata_PlaybackLeftToolbar = {
    {
QT_MOC_LITERAL(0, 0, 19), // "PlaybackLeftToolbar"
QT_MOC_LITERAL(1, 20, 19), // "signal_change_split"
QT_MOC_LITERAL(2, 40, 0), // ""
QT_MOC_LITERAL(3, 41, 10), // "SPLIT_TYPE"
QT_MOC_LITERAL(4, 52, 12), // "enSpliteType"
QT_MOC_LITERAL(5, 65, 22), // "signal_playback_action"
QT_MOC_LITERAL(6, 88, 20), // "PLAYBACK_ACTION_TYPE"
QT_MOC_LITERAL(7, 109, 12), // "enActionType"
QT_MOC_LITERAL(8, 122, 18), // "onTableCellClicked"
QT_MOC_LITERAL(9, 141, 3), // "row"
QT_MOC_LITERAL(10, 145, 3), // "col"
QT_MOC_LITERAL(11, 149, 19), // "onTabCurrentChanged"
QT_MOC_LITERAL(12, 169, 5), // "index"
QT_MOC_LITERAL(13, 175, 17), // "onCalendarChanged"
QT_MOC_LITERAL(14, 193, 16) // "onClickedRefresh"

    },
    "PlaybackLeftToolbar\0signal_change_split\0"
    "\0SPLIT_TYPE\0enSpliteType\0"
    "signal_playback_action\0PLAYBACK_ACTION_TYPE\0"
    "enActionType\0onTableCellClicked\0row\0"
    "col\0onTabCurrentChanged\0index\0"
    "onCalendarChanged\0onClickedRefresh"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PlaybackLeftToolbar[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06 /* Public */,
       5,    1,   47,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    2,   50,    2, 0x0a /* Public */,
      11,    1,   55,    2, 0x09 /* Protected */,
      13,    0,   58,    2, 0x09 /* Protected */,
      14,    0,   59,    2, 0x09 /* Protected */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6,    7,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    9,   10,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void PlaybackLeftToolbar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PlaybackLeftToolbar *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->signal_change_split((*reinterpret_cast< SPLIT_TYPE(*)>(_a[1]))); break;
        case 1: _t->signal_playback_action((*reinterpret_cast< PLAYBACK_ACTION_TYPE(*)>(_a[1]))); break;
        case 2: _t->onTableCellClicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: _t->onTabCurrentChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->onCalendarChanged(); break;
        case 5: _t->onClickedRefresh(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (PlaybackLeftToolbar::*)(SPLIT_TYPE );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PlaybackLeftToolbar::signal_change_split)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (PlaybackLeftToolbar::*)(PLAYBACK_ACTION_TYPE );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PlaybackLeftToolbar::signal_playback_action)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject PlaybackLeftToolbar::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_PlaybackLeftToolbar.data,
    qt_meta_data_PlaybackLeftToolbar,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *PlaybackLeftToolbar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PlaybackLeftToolbar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PlaybackLeftToolbar.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int PlaybackLeftToolbar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void PlaybackLeftToolbar::signal_change_split(SPLIT_TYPE _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void PlaybackLeftToolbar::signal_playback_action(PLAYBACK_ACTION_TYPE _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
