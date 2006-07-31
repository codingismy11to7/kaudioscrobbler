/***************************************************************************
 *   Copyright (C) 2004 by Steven Scott                                    *
 *   progoth@progoth.com                                                   *
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
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "kaudioscrobbler.h"

#include <qlabel.h>
#include <qpushbutton.h>

#include <kmainwindow.h>
#include <klocale.h>

KAudioScrobbler::KAudioScrobbler()
    : QVBox()//KMainWindow( 0, "KAudioScrobbler" )
{
    // set the shell's ui resource file
    //setXMLFile("kaudioscrobblerui.rc");

    resize( 400, 200 );
    
    scrob = new AudioScrobbler( this );
    
    QPushButton *tester = new QPushButton( "Play", this, "testbtn" );    
    
    connect( tester, SIGNAL( clicked() ), this, SLOT(run_test()) );
    
    QPushButton *tester2 = new QPushButton( "Pause", this, "uaoe" );
    connect( tester2, SIGNAL( clicked() ), scrob, SLOT( pause()) );
    
    QPushButton *tester3 = new QPushButton( "Unpause", this, "uheot" );
    connect( tester3, SIGNAL( clicked() ), scrob, SLOT( unpause() ) );
    
    QPushButton *tester4 = new QPushButton( "Stop", this, "ihtaonu" );
    connect( tester4, SIGNAL( clicked() ), scrob, SLOT( stop() ) );
    
    QPushButton *settings = new QPushButton( "Settings", this, "sett" );
    connect( settings, SIGNAL( clicked() ), this, SLOT( showSettings() ) );
    
    QLabel *status = new QLabel( "STATUS", this, "statuslabel" );
    
    /*new QLabel( QString( "'': " ) + QMD5::MD5( "" ), this, "md5label" );
    new QLabel( QString( "'a': " ) + QMD5::MD5( "a" ), this, "md52label" );
    new QLabel( QString( "'abc': " ) + QMD5::MD5( "abc" ), this, "md53label" );*/
    
    connect( scrob, SIGNAL(statusMessage(const QString&)), status, SLOT(setText(const QString&)) );
}

KAudioScrobbler::~KAudioScrobbler()
{
    delete scrob;
}

void KAudioScrobbler::showSettings( void )
{
    scrob->showSettings();
}

void KAudioScrobbler::run_test( void )
{
    scrob->play( "Stairwell", "Disaster", "The Sounds of Change", "", 251 );
}

void KAudioScrobbler::run_test2( void )
{
    scrob->stop();
}


#include "kaudioscrobbler.moc"
