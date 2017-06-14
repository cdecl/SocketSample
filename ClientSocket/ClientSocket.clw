; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CClientSocketDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "clientsocket.h"
LastPage=0

ClassCount=3
Class1=CClientSocketApp
Class2=CAboutDlg
Class3=CClientSocketDlg

ResourceCount=2
Resource1=IDD_ABOUTBOX
Resource2=IDD_CLIENTSOCKET_DIALOG

[CLS:CClientSocketApp]
Type=0
BaseClass=CWinApp
HeaderFile=ClientSocket.h
ImplementationFile=ClientSocket.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=ClientSocketDlg.cpp
ImplementationFile=ClientSocketDlg.cpp

[CLS:CClientSocketDlg]
Type=0
BaseClass=CDialog
HeaderFile=ClientSocketDlg.h
ImplementationFile=ClientSocketDlg.cpp
LastObject=IDC_BTN_SEND

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_CLIENTSOCKET_DIALOG]
Type=1
Class=CClientSocketDlg
ControlCount=9
Control1=IDC_EDIT_IP,edit,1350631552
Control2=IDC_EDIT_PORT,edit,1350631554
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_BTN_CONNECT,button,1342242816
Control6=IDC_EDIT_SEND,edit,1350631620
Control7=IDC_BTN_SEND,button,1342242817
Control8=IDC_STATIC,button,1342177287
Control9=IDC_EDIT_LOG,edit,1353781444

