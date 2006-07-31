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
#ifndef SCROBREQUESTTHREAD_H
#define SCROBREQUESTTHREAD_H

#include <qobject.h>
//#include <qthread.h>
//#include <qwaitcondition.h>
#include <qcstring.h>
#include <qbuffer.h>

#include <kurl.h>
#include <kio/global.h>
#include <kio/job.h>

#include <kdebug.h>

/**
@author Steven Scott
*/
class ScrobRequestThread : public QObject//, public QThread
{
Q_OBJECT
public:
    ScrobRequestThread(  );

    ~ScrobRequestThread();

    void run( void );
    
    void setJob( const KURL &url, const QByteArray &postdata );
    
    //QString result;
    
signals:
    void response( const QString& );
    void http_error();
    
private slots:
    void transferResult( KIO::Job *job );
    void dataReceived( KIO::Job *job, const QByteArray &data );
    
private:
    //QWaitCondition *m_jobdone;
    
    QBuffer *m_tmpdata;
    
    KURL m_current_url;
    QByteArray m_current_post;
};

#endif
