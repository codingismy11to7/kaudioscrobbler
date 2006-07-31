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
#include "scrobrequestthread.h"

ScrobRequestThread::ScrobRequestThread( /*QWaitCondition *job_done*/ )
 : QObject()//, QThread()//, m_jobdone( job_done )
{
}


ScrobRequestThread::~ScrobRequestThread()
{
}

void ScrobRequestThread::setJob( const KURL &url, const QByteArray &postdata )
{
    m_current_url = url;
    m_current_post = postdata;
}

void ScrobRequestThread::dataReceived( KIO::Job *, const QByteArray &data )
{
    if( data.isEmpty() )
        return;
    m_tmpdata->writeBlock( data );
}

void ScrobRequestThread::transferResult( KIO::Job *job )
{
    KIO::TransferJob *j = (KIO::TransferJob*)job;
    m_tmpdata->close();
    if( j->error() )
    {
        //j->showErrorDialog(); // prolly bad
        emit http_error();
        return;
    }   
    
    QString result = m_tmpdata->buffer();
     
    m_current_post.resize( 0 );
     
    //m_jobdone->wakeAll();
    emit response( result );
    delete m_tmpdata;
}

void ScrobRequestThread::run( void )
{
    //add error checking!
    m_tmpdata = new QBuffer();
    m_tmpdata->open( IO_WriteOnly );

    KIO::TransferJob *job;
    if( m_current_post.isEmpty() )
        job = KIO::get( m_current_url, true, false );
    else
    {
        job = KIO::http_post( m_current_url, m_current_post, false );
        job->addMetaData( "content-type", "Content-Type: application/x-www-form-urlencoded" );
    }
    connect( job, SIGNAL(result(KIO::Job*)), this, SLOT(transferResult(KIO::Job*)) );
    connect( job, SIGNAL(data(KIO::Job*,const QByteArray&)),
             this, SLOT(dataReceived(KIO::Job*, const QByteArray& )) );
    
}

#include "scrobrequestthread.moc"
