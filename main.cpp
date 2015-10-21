#include "ispyb.h"
#include "qtsoap.h"
#include "qjson.h"

#include <QtCore>
#include <QtNetwork>
#include <QAbstractMessageHandler>


main() {
	qDebug( "Start" );


	ispyb().setUrl("test");

	qDebug( "End" );
}
