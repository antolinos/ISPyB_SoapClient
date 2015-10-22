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


	qDebug( "Start 1 2 3" );  
        ispyb().setMessageHandler(new MessageHandler());

	/** Test server JBOSS6: there is a permission denied problem because of certificate but there is communication then it works **/
//	ispyb().setUrl("http://ispyb-test.embl-hamburg.de:8086/ispyb-ejb3/ispybWS/GenericSampleChangerBiosaxsWebService?wsdl");

	ispyb().setUserName("mx415");
	ispyb().setPassword("pimx415");
	//ispyb().setUrl("http://localhost:8080/ispyb-ejb3/ispybWS/GenericSampleChangerBiosaxsWebService?wsdl");
	ispyb().setUrl("http://ispyb-test.embl-hamburg.de:8086/ispyb-ejb3/ispybWS/GenericSampleChangerBiosaxsWebService?wsdl");

//	ispyb().setUrl("http://ispyb-test:8080/whatever?wsdl");
//	ispyb().setUrl("http://ispyvalid.esrf.fr:8080/ispyb/ispyb-ws/ispybWS/GenericSampleChangerBiosaxsWebService?wsdl");
//	ispyb().setUrl("http://ispyb-test.embl-hamburg.de:8080/whatever?wsdl");
//	ispyb().setUrl("http://ispyvalid.esrf.fr:8080/ispyb/ispyb-ws/ispybWS/GenericSampleChangerBiosaxsWebService?wsdl");
	ispyb().getProposals("mx415");
	qDebug( "End" );
}
