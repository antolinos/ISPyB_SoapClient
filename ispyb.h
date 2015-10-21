#ifndef ISPYB_H
#define ISPYB_H

#include <QtCore>
class QAbstractMessageHandler;
class QNetworkReply;
class QAuthenticator;
class QtSoapMessage;

/**
 * ISPyB communication interface, uses QtSoap.
 */
class ISPyB : public QObject {
  Q_OBJECT

public:
  QString url() const;
  void setUrl(const QString& url);

  QString userName() const;
  void setUserName(const QString& user);

  QString password() const;
  void setPassword(const QString& password);

  QString proposal() const;
  QString proposalCode() const;
  QString proposalNumber() const;
  void setProposal(const QString& code, const QString& number);

  QString experimentId() const;
  void setExperimentId(const QString& experimentId);

  QString dataDestinationPath() const;
  void setDataDestinationPath(const QString& path);

  QAbstractMessageHandler* messageHandler() const;
  void setMessageHandler(QAbstractMessageHandler *handler);

  bool hasUrl() const;
  bool hasUserName() const;
  bool hasPassword() const;
  bool hasProposal() const;
  bool hasDataDestinationPath() const;
  bool hasExperimentId() const;

  void createExperiment(const QString& name);

  void addMeasurement(const QString& runNumber,
                      const QString& comment,
                      const QString& macromolecule,
                      const QString& buffer,
                      const QString& concentration,
                      const QString& plateRow,
                      const QString& plateColumn,
                      const QString& wellRow,
                      const QString& wellColumn,
                      const QString& targetCellTemperature,
                      const QString& loadViscosity,
                      const QString& loadVolume,
                      const QString& cellWaitTime);

  void addRun(const QString& runNumber, const QString& storagetemp,
              const QString& celltemp, const QString& expperiod,
              const QString& ringcurrent);

  void addAverage(const QString& runNumber, const QStringList& selected,
                  const QStringList& rejected, const QString& average);

  void addSubtraction(const QStringList& runNumberList,
                      const QStringList& bufferSelectedFileNames,
                      const QStringList& bufferRejectedFileNames,
                      const QString&     bufferFileName,
                      const QStringList& sampleSelectedFileNames,
                      const QStringList& sampleRejectedFileNames,
                      const QString&     sampleFileName,
                      const QString&     subtractedFileName,
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
                      const QString&     porod_volume);

  void addAbinitioModel(const QString& runNumber,
                        const QString& type,
                        const QString& pdbFileName,
                        const QString& firFileName,
                        const QString& logFileName,
                        const QString& rg,
                        const QString& dmax,
                        const QString& volume,
                        const QString& weight);

  // crysol fit
  void addMixtureAnalysis(const QString& runNumber,
                          const QString& fitFileName,
                          const QString& aprioriPdbFileName);

  // oligomer fit
  void addMixtureAnalysis(const QString& runNumber,
                          const QString& fitFileName,
                          const QStringList& aprioriPdbFileName,
                          const QStringList& volume);

  // supcomb overlay
  void addSuperposition(const QString& runNumber,
                        const QString& abitioPdbFilePath,
                        const QString& aprioriPdbFilePath,
                        const QString& alignedPdbFilePath);

  void addRigidBodyModel(const QString& runNumber,
                         const QString& fitFileName,
                         const QString& rigidBodyPdbFileName,
                         const QString& logFileName,
                         const QString& curveConfigFileName,
                         const QString& subunitConfigFileName,
                         const QString& crosscorrConfigFileName,
                         const QString& contactConditionsFileName,
                         const QString& masterSymmetry);

  QVariant getAprioriInformation(const QString& description);

  QVariant getProposals(const QString& userName);

  QVariant getExperimentList();

  QVariant getExperimentDefinition(const QString& id);

  QVariant getBuffers();

  QVariant getMacromolecules();

private:
  ISPyB();
  ISPyB(const ISPyB&);
  ~ISPyB();
  friend ISPyB& ispyb();

  bool submit(QtSoapMessage& request);
  QVariant submitAndWaitForResponse(QtSoapMessage& request);
  

  QStringList copy(const QStringList& srcFilePaths);
  QString copy(const QString& srcFilePath);

  QUrl mUrl;
  QString mUserName;
  QString mPassword;
  QString mProposalCode, mProposalNumber;
  QString mExperimentId;
  QString mDataDestinationPath;

  QStringList messageList;

  QAbstractMessageHandler *mMessageHandler;
};

ISPyB& ispyb();

#endif // !ISPYB_H
