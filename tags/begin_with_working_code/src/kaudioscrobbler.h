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

#ifndef _KAUDIOSCROBBLER_H_
#define _KAUDIOSCROBBLER_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

//#include <kmainwindow.h>
#include <qvbox.h>

#include "audioscrobbler.h"

/**
 * @short Application Main Window
 * @author Steven Scott <progoth@progoth.com>
 * @version 0.1
 */
class KAudioScrobbler : public QVBox//KMainWindow
{
    Q_OBJECT
public:
    /**
     * Default Constructor
     */
    KAudioScrobbler();

    /**
     * Default Destructor
     */
    virtual ~KAudioScrobbler();
    
signals:
    void exitCalled();
    
private:
    AudioScrobbler *scrob;
    
private slots:

    void run_test();
    void run_test2();
};

#endif // _KAUDIOSCROBBLER_H_
