/***************************************************************************
 *   Copyright (C) 2006 by Vladimir Kuznetsov                              *
 *   vovanec@gmail.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#include <QApplication>
#include <QtGlobal>
#include <QMessageBox>

#include <assert.h>
#include <stdio.h>
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#else
#include "getopt/getopt.h"
#endif // HAVE_GETOPT_H
#include <stdlib.h>
#ifdef HAVE_QDBUS
    #include <QtDBus/QtDBus>
    #include <unistd.h>
    #include "processadaptor.h"
#endif


#include "mainwindow.h"
#include "qterminalapp.h"
#include "terminalconfig.h"

#define out

const char* const short_options = "vhw:e:dp:";

const struct option long_options[] = {
    {"version", 0, NULL, 'v'},
    {"help",    0, NULL, 'h'},
    {"profile", 1, NULL, 'p'},
    {NULL,      0, NULL,  0}
};

QTerminalApp * QTerminalApp::m_instance = NULL;

void print_usage_and_exit(int code)
{
    printf("QTerminal %s\n", STR_VERSION);
    puts("Usage: qterminal [OPTION]...\n");
    puts("  -h,  --help               Print this help");
    puts("  -p,  --profile            Load qterminal with specific options");
    puts("  -v,  --version            Prints application version and exits");
    puts("\nHomepage: <https://github.com/lxde/qterminal>");
    puts("Report bugs to <https://github.com/lxde/qterminal/issues>");
    exit(code);
}

void print_version_and_exit(int code=0)
{
    printf("%s\n", STR_VERSION);
    exit(code);
}

void parse_args(int argc, char* argv[])
{
    int next_option;
    do{
        next_option = getopt_long(argc, argv, short_options, long_options, NULL);
        switch(next_option)
        {
            case 'h':
                print_usage_and_exit(0);
            case 'p':
                Properties::Instance(QString(optarg));
                break;
            case '?':
                print_usage_and_exit(1);
            case 'v':
                print_version_and_exit();
        }
    }
    while(next_option != -1);
}

int main(int argc, char *argv[])
{
#ifdef HAVE_F_SETENV
	setenv("TERM", "xterm", 1); // TODO/FIXME: why?
#endif // HAVE_F_SETENV
    QApplication::setApplicationName("qterminal");
    QApplication::setApplicationVersion(STR_VERSION);
    QApplication::setOrganizationDomain("qterminal.org");
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
    QApplication::setDesktopFileName(QLatin1String("qterminal.desktop"));
#endif
    // Warning: do not change settings format. It can screw bookmarks later.
    QSettings::setDefaultFormat(QSettings::IniFormat);

    QTerminalApp *app = QTerminalApp::Instance(argc, argv);
    #ifdef HAVE_QDBUS
        app->registerOnDbus();
    #endif

    parse_args(argc, argv);

    const QSettings settings;
    const QFileInfo customStyle = QFileInfo(
        QFileInfo(settings.fileName()).canonicalPath() +
        "/style.qss"
    );
    if (customStyle.isFile() && customStyle.isReadable())
    {
        QFile style(customStyle.canonicalFilePath());
        style.open(QFile::ReadOnly);
        QString styleString = QLatin1String(style.readAll());
        app->setStyleSheet(styleString);
    }

    // icons
    /* setup our custom icon theme if there is no system theme (OS X, Windows) */
    QCoreApplication::instance()->setAttribute(Qt::AA_UseHighDpiPixmaps); //Fix for High-DPI systems
    if (QIcon::themeName().isEmpty())
        QIcon::setThemeName("QTerminal");

    // translations
    QString fname = QString("qterminal_%1.qm").arg(QLocale::system().name().left(5));
    QTranslator translator;
#ifdef TRANSLATIONS_DIR
    qDebug() << "TRANSLATIONS_DIR: Loading translation file" << fname << "from dir" << TRANSLATIONS_DIR;
    qDebug() << "load success:" << translator.load(fname, TRANSLATIONS_DIR, "_");
#endif
#ifdef APPLE_BUNDLE
    qDebug() << "APPLE_BUNDLE: Loading translator file" << fname << "from dir" << QApplication::applicationDirPath()+"../translations";
    qDebug() << "load success:" << translator.load(fname, QApplication::applicationDirPath()+"../translations", "_");
#endif
    app->installTranslator(&translator);

    int ret = app->exec();
    delete Properties::Instance();
    app->cleanup();

    return ret;
}

MainWindow *QTerminalApp::newWindow(TerminalConfig &cfg)
{
    MainWindow *window = new MainWindow(cfg, false);
    window->show();
    return window;
}

QTerminalApp *QTerminalApp::Instance()
{
    assert(m_instance != NULL);
    return m_instance;
}

QTerminalApp *QTerminalApp::Instance(int &argc, char **argv)
{
    assert(m_instance == NULL);
    m_instance = new QTerminalApp(argc, argv);
    return m_instance;
}

namespace Konsole {
    __declspec(dllimport) short TargetPtyTcpPort;
}

QTerminalApp::QTerminalApp(int &argc, char **argv)
    :QApplication(argc, argv)
{
    bridge = new QProcess();
    connect(bridge, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(bridgeErrorOccurred(QProcess::ProcessError)));  // connect process signals with your code
    connect(bridge, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(bridgeFinished(int, QProcess::ExitStatus)));  // connect process signals with your code
    connect(bridge, SIGNAL(readyReadStandardOutput()), this, SLOT(bridgeOutput()));  // connect process signals with your code
    bridge->start("WSL.exe", {"./tcppty"});
}

QTerminalApp::~QTerminalApp() {
    bridge->disconnect();
    bridge->close();
    delete bridge;
}

void QTerminalApp::bridgeErrorOccurred(QProcess::ProcessError error) {
	QMessageBox msgBox;
	QString msg;
	msg.sprintf("Bridge failed (%d), Check your WSL installation.\n\n", error);
	msgBox.setIcon(QMessageBox::Critical);
	msgBox.setText(msg + bridge->readAllStandardError());
	msgBox.exec();
}

void QTerminalApp::bridgeFinished(int exitCode, QProcess::ExitStatus exitStatus) {
	QMessageBox msgBox;
	QString msg;
	msg.sprintf("Bridge exited (%d/%d), Check your WSL installation.\n\n", exitCode, exitStatus);
	msgBox.setIcon(QMessageBox::Critical);
	msgBox.setText(msg + bridge->readAllStandardError());
	msgBox.exec();
}

void QTerminalApp::bridgeOutput() {
	auto port = bridge->readLine().trimmed().toInt();
	if (port > 0) {
		Konsole::TargetPtyTcpPort = port;
        newWindow(TerminalConfig());
    }
}

QString &QTerminalApp::getWorkingDirectory()
{
    return m_workDir;
}

void QTerminalApp::setWorkingDirectory(const QString &wd)
{
    m_workDir = wd;
}

void QTerminalApp::cleanup() {
    delete m_instance;
    m_instance = NULL;
}


void QTerminalApp::addWindow(MainWindow *window)
{
    m_windowList.append(window);
}

void QTerminalApp::removeWindow(MainWindow *window)
{
    m_windowList.removeOne(window);
}

QList<MainWindow *> QTerminalApp::getWindowList()
{
    return m_windowList;
}

#ifdef HAVE_QDBUS
void QTerminalApp::registerOnDbus()
{
    if (!QDBusConnection::sessionBus().isConnected())
    {
        fprintf(stderr, "Cannot connect to the D-Bus session bus.\n"
                "To start it, run:\n"
                "\teval `dbus-launch --auto-syntax`\n");
        return;
    }
    QString serviceName = QStringLiteral("org.lxqt.QTerminal-%1").arg(getpid());
    if (!QDBusConnection::sessionBus().registerService(serviceName))
    {
        fprintf(stderr, "%s\n", qPrintable(QDBusConnection::sessionBus().lastError().message()));
        return;
    }
    new ProcessAdaptor(this);
    QDBusConnection::sessionBus().registerObject("/", this);
}

QList<QDBusObjectPath> QTerminalApp::getWindows()
{
    QList<QDBusObjectPath> windows;
    foreach (MainWindow *wnd, m_windowList)
    {
        windows.push_back(wnd->getDbusPath());
    }
    return windows;
}

QDBusObjectPath QTerminalApp::newWindow(const QHash<QString,QVariant> &termArgs)
{
    TerminalConfig cfg = TerminalConfig::fromDbus(termArgs);
    MainWindow *wnd = newWindow(false, cfg);
    assert(wnd != NULL);
    return wnd->getDbusPath();
}

QDBusObjectPath QTerminalApp::getActiveWindow()
{
    QWidget *aw = activeWindow();
    if (aw == NULL)
        return QDBusObjectPath("/");
    return qobject_cast<MainWindow*>(aw)->getDbusPath();
}

#endif
