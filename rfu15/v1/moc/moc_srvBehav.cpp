/****************************************************************************
** Meta object code from reading C++ file 'srvBehav.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../srvBehav.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'srvBehav.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TSrvBehav[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x05,
      33,   10,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
      58,   10,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_TSrvBehav[] = {
    "TSrvBehav\0\0signalErrMsg(QString)\0"
    "signalStatusMsg(QString)\0timeAlarm()\0"
};

void TSrvBehav::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        TSrvBehav *_t = static_cast<TSrvBehav *>(_o);
        switch (_id) {
        case 0: _t->signalErrMsg((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->signalStatusMsg((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->timeAlarm(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData TSrvBehav::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject TSrvBehav::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_TSrvBehav,
      qt_meta_data_TSrvBehav, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &TSrvBehav::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *TSrvBehav::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *TSrvBehav::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TSrvBehav))
        return static_cast<void*>(const_cast< TSrvBehav*>(this));
    return QThread::qt_metacast(_clname);
}

int TSrvBehav::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void TSrvBehav::signalErrMsg(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void TSrvBehav::signalStatusMsg(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
