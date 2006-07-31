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

#include <kmainwindow.h>
#include <klocale.h>

KAudioScrobbler::KAudioScrobbler()
    : KMainWindow( 0, "KAudioScrobbler" )
{
    // set the shell's ui resource file
    setXMLFile("kaudioscrobblerui.rc");

    new QLabel( "Hello World", this, "hello label" );
}

KAudioScrobbler::~KAudioScrobbler()
{
}

#include "kaudioscrobbler.moc"
