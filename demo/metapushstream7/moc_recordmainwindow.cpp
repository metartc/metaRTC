/****************************************************************************
** Meta object code from reading C++ file 'recordmainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "recordmainwindow.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'recordmainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.6.2. It"
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

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSRecordMainWindowENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSRecordMainWindowENDCLASS = QtMocHelpers::stringData(
    "RecordMainWindow",
    "on_m_b_rec_clicked",
    "",
    "on_m_c_whip_clicked",
    "on_m_c_janus_clicked"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSRecordMainWindowENDCLASS_t {
    uint offsetsAndSizes[10];
    char stringdata0[17];
    char stringdata1[19];
    char stringdata2[1];
    char stringdata3[20];
    char stringdata4[21];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSRecordMainWindowENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSRecordMainWindowENDCLASS_t qt_meta_stringdata_CLASSRecordMainWindowENDCLASS = {
    {
        QT_MOC_LITERAL(0, 16),  // "RecordMainWindow"
        QT_MOC_LITERAL(17, 18),  // "on_m_b_rec_clicked"
        QT_MOC_LITERAL(36, 0),  // ""
        QT_MOC_LITERAL(37, 19),  // "on_m_c_whip_clicked"
        QT_MOC_LITERAL(57, 20)   // "on_m_c_janus_clicked"
    },
    "RecordMainWindow",
    "on_m_b_rec_clicked",
    "",
    "on_m_c_whip_clicked",
    "on_m_c_janus_clicked"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSRecordMainWindowENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   32,    2, 0x08,    1 /* Private */,
       3,    0,   33,    2, 0x08,    2 /* Private */,
       4,    0,   34,    2, 0x08,    3 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject RecordMainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_CLASSRecordMainWindowENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSRecordMainWindowENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSRecordMainWindowENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<RecordMainWindow, std::true_type>,
        // method 'on_m_b_rec_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_m_c_whip_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_m_c_janus_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void RecordMainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RecordMainWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->on_m_b_rec_clicked(); break;
        case 1: _t->on_m_c_whip_clicked(); break;
        case 2: _t->on_m_c_janus_clicked(); break;
        default: ;
        }
    }
    (void)_a;
}

const QMetaObject *RecordMainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RecordMainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSRecordMainWindowENDCLASS.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "YangSysMessageI"))
        return static_cast< YangSysMessageI*>(this);
    if (!strcmp(_clname, "YangSysMessageHandleI"))
        return static_cast< YangSysMessageHandleI*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int RecordMainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 3;
    }
    return _id;
}
QT_WARNING_POP
