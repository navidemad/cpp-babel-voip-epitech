#pragma once

#include <qlist.h>
#include <QTimer>
#include <qstring.h>
#include <QPainter>
#include "Contact.hpp"
#include "BabelFlyer.hpp"
#include "BabelInscription.hpp"
#include "BabelSetting.hpp"
#include "BabelMain.hpp"
#include "BabelDialog.hpp"
#include "qmainwindow.h"
#include "ErrorStatus.hpp"
#include "BabelUpdate.hpp"
#include <QStackedWidget>

class BabelMainWindow : public QMainWindow {

	Q_OBJECT

	// ctor - dtor
	public:
		BabelMainWindow(void);
		~BabelMainWindow(void);

	// coplien form
	private:
		BabelMainWindow(const BabelMainWindow &) : QMainWindow() {}
		const BabelMainWindow &operator=(const BabelMainWindow &) { return *this; }

	// attributes
	private:
		QStackedWidget		*mCentralWidget;
		BabelFlyer			*mFlyer;
		BabelInscription	*mSignup;
		BabelSetting		*mSetting;
		BabelMain			*mMain;
		BabelUpdate			*mUpdate;
		BabelDialog			mDialog;
		Contact				mContact;

	// public slots
	public slots:
		void	updateContactList(const QList<Contact> &contacts);
		void	newContactInvitation(const Contact &contact);
		void	newMessage(const Contact &contact, const QString &message);
		void	newCallInvitation(const Contact &contact);
		void	startingCommunication(const Contact &contact, bool hasAccepted);
		void	terminatingCommunication(const Contact &contact);
		void	updateInfo(const Contact &contact);
		void	createAccountSuccess(const ErrorStatus &errorStatus);
		void	authenticateSuccess(const ErrorStatus &errorStatus);
		void	sendInvitationSuccess(const ErrorStatus &errorStatus);
		void	acceptContactSuccess(const ErrorStatus &errorStatus);
		void	deleteContactSuccess(const ErrorStatus &errorStatus);
		void	disconnectSuccess(const ErrorStatus &errorStatus);
		void	sendMessageSuccess(const ErrorStatus &errorStatus);
		void	updateInfoSuccess(const ErrorStatus &errorStatus);
		void	callContactSuccess(const ErrorStatus &errorStatus);
		void	acceptCallSuccess(const ErrorStatus &errorStatus);
		void	terminateCallSuccess(const ErrorStatus &errorStatus);
		void	connectToServerSuccess(const ErrorStatus &errorStatus);
		void	disconnectedFromServer(void);

	// actions - requests
	private slots:
		void	connectionToServer(const QString &host, int port);
		void	createAccount(const Contact &contact);
		void	connexionToAccount(const Contact &contact);
		void	addNewContact(const Contact &contact);
		void	sendMessage(const Contact &contact, const QString &message);
		void	callContact(const Contact &contact);
		void	closeCall(const Contact &contact);
		void	disconnectionToAccount(void);
		void	deleteContact(const Contact &contact);
		void	displayInformation(const QString &message);

	// slots display
	private slots:
		void	displayOptions(void);
		void	displaySignUp(void);
		void	displayFlyer(void);
		void	displayHome(void);
		void	displayUpdate(void);
		void	updateContactInfo(Contact &contact);

	// signals
	signals:
		void	askForRegistration(const Contact &contact);
		void	askForAuthentication(const Contact &contact);
		void	askForAddingContact(const Contact &contact);
		void	askForAcceptingContact(const Contact &contact, bool hasAccepted);
		void	askForDeletingContact(const Contact &contact);
		void	askForSendingMessage(const Contact &contact, const QString &message);
		void	askForDisconnection(void);
		void	askForUpdatingInfo(const Contact &contact);
		void	askForCalling(const Contact &contact);
		void	askForAcceptingCall(const Contact &contact, bool hasAccepted);
		void	askForTerminatingCall(const Contact &contact);
		void	askForConnectionToServer(const QString &addr, int port);

	// member function
	public:
		void	show();
		void	updateContent(QWidget *widget);

};
