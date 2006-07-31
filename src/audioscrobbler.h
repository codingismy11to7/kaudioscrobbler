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
#ifndef AUDIOSCROBBLER_H
#define AUDIOSCROBBLER_H

#include <qstring.h>
#include <qwaitcondition.h>
#include <qmutex.h>
#include <qregexp.h>
#include <qdatetime.h>

#include <kurl.h>
#include <kdebug.h>

#include "qmd5.h"
#include "scrobrequestthread.h"

#define HANDSHAKE_ADDR "http://post.audioscrobbler.com?hs=true&p=1.1&c=juk&v=0.0.1&u="

/**
@author Steven Scott
*/
class AudioScrobbler : public QObject
{
Q_OBJECT
public:
    AudioScrobbler( QString username, QString password );

    ~AudioScrobbler();

    void setPassword( QString password );
    
signals:
    void statusMessage( const QString& );

public slots:
    void run_test( void );
    void run_test2( void );
    void gotResponse( QString response );
    void gotError( void );
    
    void setInterval( unsigned int interval );
    
protected:
    /*QString*/void doRequest( const KURL &address, QString postdata = QString::null );
    
    void parseResponse( QString response );
    
    inline QString md5Response( void );
    
    QString m_postaddress;
    QString m_challenge;
    QMutex m_job;
    bool m_handshake_done;
    
    QString m_username;
    QString m_password;
    
    ScrobRequestThread *m_job_thread;
    
    unsigned int m_interval;
};

#endif
