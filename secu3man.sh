#!/bin/sh

#how to install locale:
# sudo locale-gen ru_RU.CP1251
# sudo dpkg-reconfigure locales

#set Russian locale:
LANG="ru_RU.CP1251"
#LANG="ru_RU.UTF8"

#set execution permissions
chmod 755 secu3man.exe

#run application:
wine secu3man.exe
