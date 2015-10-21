
#include "ispyb.h"
#include "qtsoap.h"
#include "qjson.h"

#include <QtCore>
#include <QtNetwork>
#include <QAbstractMessageHandler>

//
// Helper functions to JSONify the local data structures.
//
static QString json(const QString& key, const QStringList& values, bool *ok = 0L) {
  QVariantList objects;
  foreach (QString value, values) {
    QVariantMap object;
    object.insert(key, value);
    objects.append(object);
  }

  return QJson::Serializer().serialize(objects, ok);
}

static QString json(const QString& key1, const QStringList& values1,
                    const QString& key2, const QStringList& values2,
                    bool *ok = 0L) {

  if (values1.size() != values2.size())
    return QString();

  QVariantList objects;
  int n = values1.size();
  for (int i = 0; i < n; ++i) {
    QVariantMap object;
    object.insert(key1, values1[i]);
    object.insert(key2, values2[i]);
    objects.append(object);
  }

  return QJson::Serializer().serialize(objects, ok);
}




ISPyB& ispyb() {
  static ISPyB db;
  return db;
}


ISPyB::ISPyB()
 : QObject(0L), mMessageHandler(0L) {
  setObjectName("ISpyB Client");
}

ISPyB::~ISPyB() {
}

QString ISPyB::url() const {
  return mUrl.toString();
}

void ISPyB::setUrl(const QString& url) {
  mUrl = url;

  messageHandler()->message(QtDebugMsg,
                            QString("Set URL: '%1'")
                                    .arg(url),
                            objectName());
}

QString ISPyB::userName() const {
  return mUserName;
}

void ISPyB::setUserName(const QString& userName) {
  mUserName = userName;

  messageHandler()->message(QtDebugMsg,
                            QString("Set user: '%1'")
                                    .arg(userName),
                            objectName());
}

QString ISPyB::password() const {
  return mPassword;
}

void ISPyB::setPassword(const QString& password) {
  mPassword = password;

  messageHandler()->message(QtDebugMsg,
                            "Set password: '*****'",
                            objectName());
}

QString ISPyB::proposal() const {
  return mProposalCode + mProposalNumber;
}

QString ISPyB::proposalCode() const {
  return mProposalCode;
}

QString ISPyB::proposalNumber() const {
  return mProposalNumber;
}

void ISPyB::setProposal(const QString& code, const QString& number) {
  mProposalCode = code;
  mProposalNumber = number;

  messageHandler()->message(QtDebugMsg,
                            QString("Set proposal: '%1'")
                                    .arg(proposal()),
                            objectName());
}

QString ISPyB::experimentId() const {
  return mExperimentId;
}

void ISPyB::setExperimentId(const QString& experimentId) {
  mExperimentId = experimentId;

  messageHandler()->message(QtDebugMsg,
                            QString("Set Experiment ID: '%1'")
                                    .arg(experimentId),
                            objectName());
}

QString ISPyB::dataDestinationPath() const {
  return mDataDestinationPath;
}

void ISPyB::setDataDestinationPath(const QString& path) {
  mDataDestinationPath = path;

  messageHandler()->message(QtDebugMsg,
                            QString("Set data destination path: '%1'")
                                    .arg(path),
                            objectName());
}

QAbstractMessageHandler* ISPyB::messageHandler() const {
  return mMessageHandler;
}

void ISPyB::setMessageHandler(QAbstractMessageHandler *handler) {
  mMessageHandler = handler;
}

bool ISPyB::hasUrl() const {
  return !mUrl.isEmpty();
}

bool ISPyB::hasUserName() const {
  return !mUserName.isEmpty();
}

bool ISPyB::hasPassword() const {
  return !mPassword.isEmpty();
}

bool ISPyB::hasProposal() const {
  return !mProposalCode.isEmpty() && !mProposalNumber.isEmpty();
}

bool ISPyB::hasDataDestinationPath() const {
  return !mDataDestinationPath.isEmpty();
}

bool ISPyB::hasExperimentId() const {
  return !mExperimentId.isEmpty();
}

void ISPyB::createExperiment(const QString& name) {
  QtSoapMessage request;
  request.setMethod("createEmptyExperiment", "http://ispyb.ejb3.webservices.biosaxs");
  request.addMethodArgument("code", "", mProposalCode);
  request.addMethodArgument("number", "", mProposalNumber);
  request.addMethodArgument("name", "", name);

  QVariant response = submitAndWaitForResponse(request);
  if (response.isValid()) {
    mExperimentId = response.toMap().value("experimentId").toString();

    messageHandler()->message(QtDebugMsg,
                              QString("Created new experiment '%1' for proposal '%2'.")
                                     .arg(mExperimentId)
                                     .arg(proposal()),
                              objectName());

  } else
    messageHandler()->message(QtCriticalMsg,
                              QString("Failed to creat new experiment '%1'.")
                                      .arg(name),
                              objectName());
}
/*
void ISPyB::addMeasurement(const QString& runNumber, const QString& description, const QString& concentration) {
  if (mExperimentId.isEmpty()) {
    messageHandler()->message(QtCriticalMsg,
                              "internal error: can not add measurement without experiment id",
                              objectName());
    return;
  }

  QtSoapMessage request;
  request.setMethod("appendMeasurementToExperiment", "http://ispyb.ejb3.webservices.biosaxs");
  request.addMethodArgument("experimentId", "", mExperimentId);
  request.addMethodArgument("runNumber", "", runNumber);

  if (concentration.toDouble() > 0.0) {
    request.addMethodArgument("type", "", "SAMPLE");
    request.addMethodArgument("plate", "", "1");
    request.addMethodArgument("row", "", "1");
    request.addMethodArgument("well", "", "1");
    request.addMethodArgument("macromolecule", "", description);
  } else {
    request.addMethodArgument("type", "", "BUFFER");
    request.addMethodArgument("plate", "", "2");
    request.addMethodArgument("row", "", "1");
    request.addMethodArgument("well", "", "1");
    request.addMethodArgument("macromolecule", "", "");      // macromolecule name, must be empty for buffers!
  }

  request.addMethodArgument("bufferName", "", "bufferName");
  request.addMethodArgument("concentration", "", concentration);
  request.addMethodArgument("SEUtemperature", "", "20.0");
  request.addMethodArgument("viscosity", "", "LOW");
  request.addMethodArgument("volume", "", "40");
  request.addMethodArgument("volumeToLoad", "", "40");
  request.addMethodArgument("waitTime", "", "0");
  request.addMethodArgument("transmission", "", "0");
  request.addMethodArgument("comments", "", "");

  if (submit(request))
    messageHandler()->message(QtDebugMsg,
                              QString("added measurement (run: '%1', "
                                      "desc: '%2', conc: '%3')").arg(runNumber)
                                                                .arg(description)
                                                                .arg(concentration),
                              objectName());
}
*/
void ISPyB::addMeasurement(const QString& runNumber,
                           const QString& comment,
                           const QString& macromolecule,
                           const QString& buffer,
                           const QString& concentration,
                           const QString& plateRow,
                           const QString& /* plateColumn */,
                           const QString& wellRow,
                           const QString& wellColumn,
                           const QString& targetCellTemperature,
                           const QString& loadViscosity,
                           const QString& loadVolume,
                           const QString& cellWaitTime) {

  QtSoapMessage request;
  request.setMethod("appendMeasurementToExperiment", "http://ispyb.ejb3.webservices.biosaxs");
  request.addMethodArgument("experimentId", "", mExperimentId);
  request.addMethodArgument("runNumber", "", runNumber);

  request.addMethodArgument("comments", "", comment);
  request.addMethodArgument("type", "", macromolecule.isEmpty() ? "BUFFER" : "SAMPLE");
  request.addMethodArgument("macromolecule", "", macromolecule);
  request.addMethodArgument("bufferName", "", buffer);
  request.addMethodArgument("concentration", "", concentration);
  request.addMethodArgument("plate", "", plateRow);
  request.addMethodArgument("row", "", wellRow);
  request.addMethodArgument("well", "", wellColumn);
  request.addMethodArgument("SEUtemperature", "", targetCellTemperature);
  request.addMethodArgument("viscosity", "", loadViscosity);
  request.addMethodArgument("volume", "", loadVolume);
  request.addMethodArgument("volumeToLoad", "", loadVolume);
  request.addMethodArgument("waitTime", "",cellWaitTime);
  request.addMethodArgument("transmission", "", "100");

  if (submit(request))
    messageHandler()->message(QtDebugMsg,
                              QString("added measurement (run: '%1', "
                                      "sample: '%2', buffer: '%3', "
                                      "conc: '%4')")
                                      .arg(runNumber)
                                      .arg(macromolecule)
                                      .arg(buffer)
                                      .arg(concentration),
                              objectName());

}

void ISPyB::addRun(const QString& runNumber, const QString& storagetemp,
                   const QString& celltemp, const QString& expperiod,
                   const QString& ringcurrent) {

  QtSoapMessage request;
  request.setMethod("addRun", "http://ispyb.ejb3.webservices.biosaxs");
  request.addMethodArgument("experimentId", "", mExperimentId);
  request.addMethodArgument("runNumber", "", runNumber);
  request.addMethodArgument("exposureTemperature", "", celltemp);
  request.addMethodArgument("storageTemperature", "", storagetemp);
  request.addMethodArgument("timePerFrame", "", expperiod);
  request.addMethodArgument("timeStart", "", "");
  request.addMethodArgument("timeEnd", "", "");
  request.addMethodArgument("energy", "", "0.0");
  request.addMethodArgument("detectorDistance", "", "12.0");
  request.addMethodArgument("snapshotCapillary", "", "");
  request.addMethodArgument("currentMachine", "", ringcurrent);
  request.addMethodArgument("beamCenterX", "", "unknown");
  request.addMethodArgument("beamCenterY", "", "unknown");
  request.addMethodArgument("radiationRelative", "", "ignored");
  request.addMethodArgument("radiationAbsolute", "", "ignored");
  request.addMethodArgument("pixelSizeX", "", "0.000172");
  request.addMethodArgument("pixelSizeY", "", "0.000172");
  request.addMethodArgument("normalization", "", "?");
  request.addMethodArgument("transmission", "", "?");

  if (submit(request))
    messageHandler()->message(QtDebugMsg,
                              QString("added run: '%1'").arg(runNumber),
                              objectName());
}

void ISPyB::addAverage(const QString& runNumber, const QStringList& selected,
                       const QStringList& rejected, const QString& average) {

  QtSoapMessage request;
  request.setMethod("addAveraged", "http://ispyb.ejb3.webservices.biosaxs");
  request.addMethodArgument("experimentId", "", mExperimentId);
  request.addMethodArgument("runNumber", "", runNumber);
  request.addMethodArgument("averaged", "", json("filePath", copy(selected)));
  request.addMethodArgument("discarded", "", json("filePath", copy(rejected)));
  request.addMethodArgument("averageFile", "", copy(average));

  if (submit(request))
    messageHandler()->message(QtDebugMsg,
                              QString("added average '%1' for run: '%2'")
                                     .arg(average).arg(runNumber),
                              objectName());
}

void ISPyB::addSubtraction(const QStringList& runNumberList,
                           const QStringList& bufferSelectedFileNames,
                           const QStringList& /* bufferRejectedFileNames */,
                           const QString&     bufferFileName,
                           const QStringList& sampleSelectedFileNames,
                           const QStringList& /* sampleRejectedFileNames */,
                           const QString&     sampleFileName,
                           const QString&     /* subtractedFileName */,
                           const QString&     normalizedFileName,
                           const QString&     guinier_rg,
                           const QString&     guinier_std_rg,
                           const QString&     guinier_i0,
                           const QString&     guinier_std_i0,
                           const QString&     guinier_first,
                           const QString&     guinier_last,
                           const QString&     guinier_aggr,
                           const QString&     guinier_quality,
                           const QString&     dataThumbnail,
                           const QString&     guinierThumbnail,
                           const QString&     kratkyThumbnail,
                           const QString&     outFileName,
                           const QString&     gnom_rg,
                           const QString&     gnom_dmax,
                           const QString&     gnom_total,
                           const QString&     gnomThumbnail,
                           const QString&     porod_volume) {

  QtSoapMessage request;
  request.setMethod("addSubtraction", "http://ispyb.ejb3.webservices.biosaxs");
  request.addMethodArgument("experimentId", "", mExperimentId);
  request.addMethodArgument("runNumberList", "", "[" + runNumberList.join(",") + "]");

  request.addMethodArgument("sampleOneDimensionalFiles", "", json("filePath", copy(sampleSelectedFileNames)));
  request.addMethodArgument("bufferOneDimensionalFiles", "", json("filePath", copy(bufferSelectedFileNames)));
  request.addMethodArgument("sampleAverageFilePath", "", copy(sampleFileName));
  request.addMethodArgument("bestBufferFilePath", "", copy(bufferFileName));
  request.addMethodArgument("subtractedFilePath", "", copy(normalizedFileName));
  request.addMethodArgument("experimentalDataPlotFilePath", "", copy(dataThumbnail));
  request.addMethodArgument("densityPlotFilePath", "", copy(gnomThumbnail));
  request.addMethodArgument("guinierPlotFilePath", "", copy(guinierThumbnail));
  request.addMethodArgument("kratkyPlotFilePath", "", copy(kratkyThumbnail));
  request.addMethodArgument("gnomOutputFilePath", "", copy(outFileName));

  request.addMethodArgument("rgGuinier", "", guinier_rg);
  request.addMethodArgument("rgStdev", "", guinier_std_rg);
  request.addMethodArgument("i0", "", guinier_i0);
  request.addMethodArgument("i0Stdev", "", guinier_std_i0);
  request.addMethodArgument("firstPointUsed", "", guinier_first);
  request.addMethodArgument("lastPointUsed", "", guinier_last);
  request.addMethodArgument("quality", "", guinier_quality);
  request.addMethodArgument("isagregated", "", guinier_aggr);
  request.addMethodArgument("rgGnom", "", gnom_rg);
  request.addMethodArgument("dmax", "", gnom_dmax);
  request.addMethodArgument("total", "", gnom_total);
  request.addMethodArgument("volume", "", porod_volume);

  if (submit(request))
    messageHandler()->message(QtDebugMsg,
                              QString("added subtraction for runs: '%1'")
                                     .arg(runNumberList.join(", ")),
                              objectName());
}

void ISPyB::addAbinitioModel(const QString& runNumber,
                             const QString& type,
                             const QString& pdbFileName,
                             const QString& firFileName,
                             const QString& logFileName,
                             const QString& rg,
                             const QString& dmax,
                             const QString& volume,
                             const QString& weight) {

  QtSoapMessage request;
  request.setMethod("addModel", "http://ispyb.ejb3.webservices.biosaxs");
  request.addMethodArgument("experimentId", "", mExperimentId);
  request.addMethodArgument("runNumber", "", runNumber);

  request.addMethodArgument("type", "", type);
  request.addMethodArgument("pdbFilePath", "", copy(pdbFileName));
  request.addMethodArgument("firFilePath", "", copy(firFileName));
  request.addMethodArgument("logFilePath", "", copy(logFileName));
  request.addMethodArgument("rg", "", rg);
  request.addMethodArgument("dmax", "", dmax);
  request.addMethodArgument("volume", "", volume);
  request.addMethodArgument("weight", "", weight);

  if (submit(request))
    messageHandler()->message(QtDebugMsg,
                              QString("added '%1' ab initio model '%2'"
                                      " for run '%3'").arg(type)
                                                      .arg(pdbFileName)
                                                      .arg(runNumber),
                              objectName());
}

void ISPyB::addMixtureAnalysis(const QString& runNumber,
                               const QString& fitFileName,
                               const QString& aprioriPdbFileName) {
  addMixtureAnalysis(runNumber,
                     fitFileName,
                     QStringList() << aprioriPdbFileName,
                     QStringList() << "1.0");
}

void ISPyB::addMixtureAnalysis(const QString& runNumber,
                               const QString& fitFileName,
                               const QStringList& aprioriPdbFileName,
                               const QStringList& volume) {

  QtSoapMessage request;
  request.setMethod("addMixtureAnalysis", "http://ispyb.ejb3.webservices.biosaxs");
  request.addMethodArgument("experimentId", "", mExperimentId);
  request.addMethodArgument("runNumber", "", runNumber);

  request.addMethodArgument("fitFilePath", "", copy(fitFileName));
  request.addMethodArgument("aprioriPdbFilePath", "", json("filePath", copy(aprioriPdbFileName),
                                                           "volumeFraction", volume));

  if (submit(request))
    messageHandler()->message(QtDebugMsg,
                              QString("added mixture fit '%1' for run '%2'")
                                      .arg(fitFileName)
                                      .arg(runNumber),
                              objectName());
}

void ISPyB::addSuperposition(const QString& runNumber,
                             const QString& abinitioPdbFilePath,
                             const QString& aprioriPdbFilePath,
                             const QString& alignedPdbFilePath) {

  QtSoapMessage request;
  request.setMethod("addSuperposition", "http://ispyb.ejb3.webservices.biosaxs");
  request.addMethodArgument("experimentId", "", mExperimentId);
  request.addMethodArgument("runNumber", "", runNumber);

  request.addMethodArgument("abinitioModelPdbFilePath", "", copy(abinitioPdbFilePath));
  request.addMethodArgument("aprioriPdbFilePath", "", copy(aprioriPdbFilePath));
  request.addMethodArgument("alignedPdbFilePath", "", copy(alignedPdbFilePath));

  if (submit(request))
    messageHandler()->message(QtDebugMsg,
                              QString("added model superposition '%1' for run '%2'")
                                      .arg(alignedPdbFilePath)
                                      .arg(runNumber),
                              objectName());
}

void ISPyB::addRigidBodyModel(const QString& runNumber,
                              const QString& fitFileName,
                              const QString& rigidBodyPdbFileName,
                              const QString& logFileName,
                              const QString& curveConfigFileName,
                              const QString& subunitConfigFileName,
                              const QString& crosscorrConfigFileName,
                              const QString& contactConditionsFileName,
                              const QString& masterSymmetry) {

  QtSoapMessage request;
  request.setMethod("addRigidBodyModel", "http://ispyb.ejb3.webservices.biosaxs");
  request.addMethodArgument("experimentId", "", mExperimentId);
  request.addMethodArgument("runNumber", "", runNumber);

  request.addMethodArgument("fitFilePath", "", copy(fitFileName));
  request.addMethodArgument("rigidBodyModelFilePath", "", copy(rigidBodyPdbFileName));
  request.addMethodArgument("logFilePath", "", copy(logFileName));
  request.addMethodArgument("curveConfigFilePath", "", copy(curveConfigFileName));
  request.addMethodArgument("subunitConfigFilePath", "", copy(subunitConfigFileName));
  request.addMethodArgument("crosscorrConfigFilePath", "", copy(crosscorrConfigFileName));
  request.addMethodArgument("contactConditionsFilePath", "", copy(contactConditionsFileName));
  request.addMethodArgument("masterSymmetry", "", masterSymmetry);

  if (submit(request))
    messageHandler()->message(QtDebugMsg,
                              QString("added rigid body model '%1' for run '%2'")
                                      .arg(rigidBodyPdbFileName)
                                      .arg(runNumber),
                              objectName());
}

QVariant ISPyB::getAprioriInformation(const QString& description) {
  if (!hasUrl())
    return QVariant();

  QtSoapMessage request;
  request.setMethod("getAprioriInformationByAcronym", "http://ispyb.ejb3.webservices.biosaxs");
  request.addMethodArgument("code", "", mProposalCode);
  request.addMethodArgument("number", "", mProposalNumber);
  request.addMethodArgument("macromolecule", "", description);

  return submitAndWaitForResponse(request);
}

QVariant ISPyB::getProposals(const QString& userName) {
  if (!hasUrl())
    return QVariant();

  QtSoapMessage request;
  request.setMethod("getProposalsByLoginName", "http://ispyb.ejb3.webservices.biosaxs");
  request.addMethodArgument("userName", "", userName);

  return submitAndWaitForResponse(request);
}

QVariant ISPyB::getExperimentList() {
  if (!hasUrl())
    return QVariant();

  QtSoapMessage request;
  request.setMethod("getExperimentByProposalCode", "http://ispyb.ejb3.webservices.biosaxs");
  request.addMethodArgument("code", "", mProposalCode);
  request.addMethodArgument("number", "", mProposalNumber);

  return submitAndWaitForResponse(request);
}

QVariant ISPyB::getExperimentDefinition(const QString& id) {
  if (!hasUrl())
    return QVariant();

  QtSoapMessage request;
  request.setMethod("getDescriptionByExperimentId", "http://ispyb.ejb3.webservices.biosaxs");
  request.addMethodArgument("experimentId", "", id);

  return submitAndWaitForResponse(request);
}

QVariant ISPyB::getBuffers() {
  if (!hasUrl() || !hasProposal())
    return QVariant();

  QtSoapMessage request;
  request.setMethod("getBuffersByProposal", "http://ispyb.ejb3.webservices.biosaxs");
  request.addMethodArgument("code", "", mProposalCode);
  request.addMethodArgument("number", "", mProposalNumber);

  return submitAndWaitForResponse(request);
}

QVariant ISPyB::getMacromolecules() {
  if (!hasUrl() || !hasProposal())
    return QVariant();

  QtSoapMessage request;
  request.setMethod("getMacromoleculesByProposal", "http://ispyb.ejb3.webservices.biosaxs");
  request.addMethodArgument("code", "", mProposalCode);
  request.addMethodArgument("number", "", mProposalNumber);

  return submitAndWaitForResponse(request);
}


bool ISPyB::submit(QtSoapMessage& request) {
  if (!hasUrl())
    return false;

  //
  // To avoid duplicated submission of the same entries over and over again,
  // and thus delaying the overall processing and progress, filter out any
  // message that has been sent before exactly like the current message.
  //
  // This requires a bit of storage, but is very lightweight in terms of
  // coding overhead.
  //
  QString message = request.toXmlString();
  if (!messageList.contains(message)) {
    QtSoapHttpTransport soap(this);
    soap.setHost(mUrl.host());
    soap.setUser(mUserName);
    soap.setPassword(mPassword);

    QEventLoop synchronize;
    connect(&soap, SIGNAL(responseReady()),
            &synchronize, SLOT(quit()));

//     QTimer timer;  
//     connect(&timer, SIGNAL(timeout()),
//             &synchronize, SLOT(quit()));
// 
//     timer.start(10000);
    soap.submitRequest(request, mUrl.path());

    synchronize.exec();

    messageList.append(message);
    return true;

  } else
    return false;
}

QVariant ISPyB::submitAndWaitForResponse(QtSoapMessage& request) {
  QtSoapHttpTransport soap(this);
  soap.setHost(mUrl.host());
  soap.setUser(mUserName);
  soap.setPassword(mPassword);
  soap.submitRequest(request, mUrl.path());

  QEventLoop synchronize;
  connect(&soap, SIGNAL(responseReady()),
          &synchronize, SLOT(quit()));
  synchronize.exec();

  QtSoapMessage response = soap.getResponse();
  if (response.isFault()) {
    switch (soap.networkReply()->error()) {
      case QNetworkReply::AuthenticationRequiredError:
        messageHandler()->message(QtCriticalMsg,
                                  "Permission denied. Please check user name and password.",
                                  objectName());
        break;

      default:
qDebug() << response.toXmlString();
        messageHandler()->message(QtCriticalMsg,
                                  QString("Network error querying information: '%1'")
                                         .arg(response.faultString().toString()),
                                  objectName());
    }

    return QVariant();
  }

  const QtSoapType &result = response.returnValue();
  if (!result.isValid()) {
    messageHandler()->message(QtCriticalMsg,
                              QString("SOAP error querying information: '%1'")
                                      .arg(result.errorString()),
                              objectName());
    return QVariant();
  }

  bool ok = false;
  return QJson::Parser().parse(result.toString().toAscii(), &ok);
}


QStringList ISPyB::copy(const QStringList& srcFilePaths) {
  QStringList copied;
  foreach (QString filePath, srcFilePaths)
    copied.append(copy(filePath));
  return copied;
}

QString ISPyB::copy(const QString& filePath) {
  //
  // In case something is not available, don't bother.
  //
  if (filePath.isEmpty()) {
    messageHandler()->message(QtCriticalMsg,
                              "internal error: ignored attempt to copy empty file",
                              objectName());
    return filePath;
  }

  //
  // Final destination directory: destinationprefix/proposalId/experimentId/
  //
  QDir destinationDir(mDataDestinationPath);

  if (!destinationDir.exists(proposal()))
    destinationDir.mkdir(proposal());
  destinationDir.cd(proposal());

  if (!destinationDir.exists(mExperimentId))
    destinationDir.mkdir(mExperimentId);
  destinationDir.cd(mExperimentId);

  QFileInfo sourceInfo(filePath);
  QString path = sourceInfo.path();

  //
  // NOTE: Absolute paths are (probably) user provided information.
  //       Leave them alone.
  //
  if (sourceInfo.isAbsolute())
    return filePath;

  if (!destinationDir.exists(path))
    destinationDir.mkpath(path);
  destinationDir.cd(path);

  QString destinationFilePath = destinationDir.filePath(sourceInfo.fileName());
  if (!QFile::exists(destinationFilePath)) {
    messageHandler()->message(QtDebugMsg,
                              QString("copying file from '%1' to '%2'")
                                      .arg(filePath)
                                      .arg(destinationFilePath),
                              objectName());

    QFile::copy(filePath, destinationFilePath);
  }

  return destinationFilePath;
}
