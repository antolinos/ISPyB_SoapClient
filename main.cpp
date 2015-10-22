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


        ispyb().setMessageHandler(new MessageHandler());

	/** Local machine JOSS6
	qDebug( "Localhost " );  
	ispyb().setUserName("mx9999");
	ispyb().setPassword("mxtest");
	ispyb().setPort(8080);
	ispyb().setUrl("http://pc593.embl.fr:8080/ispyb-ejb3/ispybWS/GenericSampleChangerBiosaxsWebService?wsdl");
	ispyb().getProposals("mx9999").toString();
	**/

	/** JBOSS6 
	qDebug( "ispyb-test.embl-hamburg.de:8086 " );  
	ispyb().setUserName("mx0000");
	ispyb().setPassword("test");
	ispyb().setPort(8086);
	ispyb().setUrl("http://ispyb-test.embl-hamburg.de:8086/ispyb-ejb3/ispybWS/GenericSampleChangerBiosaxsWebService?wsdl");
	ispyb().getProposals("mx9999").toString();
	qDebug() << (ispyb().getProposals("mx0000").toString());
	qDebug( "End" );
	**/

	
	qDebug( "ispyb-test.embl-hamburg.de:8080 " );  
	ispyb().setUserName("mx0000");
	ispyb().setPassword("test");
	ispyb().setPort(8080);
	ispyb().setUrl("http://ispyb-test.embl-hamburg.de:8086/ispyb-ws/ispybWS/GenericSampleChangerBiosaxsWebService?wsdl");
	ispyb().getProposals("mx9999").toString();
	qDebug() << (ispyb().getProposals("mx0000").toString());
	qDebug( "End" );


	/** Production server @Petra **/
	qDebug( "ispyb.embl-hamburg.de:8080 " );  
	ispyb().setUserName("mx0000");
	ispyb().setPassword("test");
	ispyb().setPort(8080);
	ispyb().setUrl("http://ispyb-test.embl-hamburg.de:8086/ispyb-ejb3/ispybWS/GenericSampleChangerBiosaxsWebService?wsdl");
	ispyb().getProposals("mx9999").toString();
	qDebug() << (ispyb().getProposals("mx0000").toString());
	qDebug( "End" );


	/** wildfly @ESRF
	qDebug( "WILDFLY ispyvalid.esrf.fr" );  
	ispyb().setUserName("mx9999");
	ispyb().setPassword("mxtest");
	ispyb().setPort(8080);
	ispyb().setUrl("http://ispyvalid.esrf.fr:8080/ispyb/ispyb-ws/ispybWS/GenericSampleChangerBiosaxsWebService?wsdl");
	ispyb().getProposals("mx1633").toString();
	qDebug( "End" );
	**/
}
