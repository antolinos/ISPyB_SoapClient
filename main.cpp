#include "ispyb.h"
#include "qtsoap.h"
#include "qjson.h"

#include <QtCore>
#include <QtNetwork>
#include <QAbstractMessageHandler>
#include <QCoreApplication>


class MessageHandler : public QAbstractMessageHandler {
public:
protected:
  void handleMessage(QtMsgType type,
                     const QString& description,
                     const QUrl& identifier,
                     const QSourceLocation&);
};

void MessageHandler::handleMessage(QtMsgType type,
                                   const QString& description,
                                   const QUrl& identifier,
                                   const QSourceLocation&) {
  qDebug() << description;
}



int main(int argc, char *argv[])
{
	/** QEventLoop: Cannot be used without QApplication **/
	QCoreApplication a(argc, argv);


	qDebug( "Start" );
        ispyb().setMessageHandler(new MessageHandler());
	ispyb().setUrl("http://ispyvalid.esrf.fr:8080/ispyb/ispyb-ws/ispybWS/GenericSampleChangerBiosaxsWebService?wsdl");
	ispyb().getProposals("test");
	qDebug( "End" );
}
