#!/bin/sh

xgettext -d DesktopAppChooser -o ./po/DesktopAppChooser.pot -k_ -s ./CDesktopAppChooser.cpp
cp ./po/DesktopAppChooser.pot ./po/DesktopAppChooser.po
