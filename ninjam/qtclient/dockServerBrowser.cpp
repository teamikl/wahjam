
#include <QString>
#include <QTimer>
#include <QDebug>
#include <QMainWindow>
#include <QDockWidget>
#include <QNetworkAccessManager>
#include <QtScript>
#include "ServerBrowser.h"

void install_dockServerBrowser(QMainWindow *mainWindow)
{
  const QString url = "http://autosong.ninjam.com/serverlist.php";

  // How to get QNetworkAccessManager ?
  // MainWindow::manager was private.

  // this shows two approach both access via QObject tree
  // 1) findChild if QObject::setObjectName.
  // 2) findChildren and get the first object.

  QNetworkAccessManager *netmanager
    = mainWindow->findChild<QNetworkAccessManager*>("network-access-manager");

  if (!netmanager) {
    foreach (QNetworkAccessManager *child,
             mainWindow->findChildren<QNetworkAccessManager*>()) {
      netmanager = child;
      break;
    }
  }

  Q_ASSERT(netmanager);

  QDockWidget *dock = new QDockWidget(mainWindow);
  ServerBrowser *serverBrowser = new ServerBrowser(netmanager, dock);
  QScriptEngine *scriptEngine = new QScriptEngine(serverBrowser);
  QTimer *timer = new QTimer(serverBrowser);

  // dynamic slot by script. (just for demo, qscript was not nessesary yet)
  QScriptValue vars = scriptEngine->globalObject();
  QScriptValue serverBrowser_value = scriptEngine->newQObject(serverBrowser);
  vars.setProperty("url", url);
  vars.setProperty("serverBrowser", serverBrowser_value);

  QString src = "function reload_server_list(){       \n"
                "  serverBrowser.clear();             \n"
                "  serverBrowser.loadServerList(url); \n"
                "}                                    \n";

  QScriptValue result = scriptEngine->evaluate(src);
  if (result.isError()) {
    qDebug() << result.toString();
  }
  else {
    qScriptConnect(timer, SIGNAL(timeout()),
                   serverBrowser_value, vars.property("reload_server_list"));
  }

  dock->setWidget(serverBrowser);
  dock->setAttribute(Qt::WA_DeleteOnClose);
  mainWindow->addDockWidget(Qt::TopDockWidgetArea, dock);

  serverBrowser->loadServerList(url);
  timer->start(30*1000);
}

