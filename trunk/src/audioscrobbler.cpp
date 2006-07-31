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
#include "audioscrobbler.h"

AudioScrobbler::AudioScrobbler( QString username, QString password )
  : QObject(), m_postaddress(QString::null), m_challenge(QString::null), m_handshake_done(false),
    m_username( username ), m_interval(0)
{
    m_job_thread = new ScrobRequestThread( /*&m_jobwait*/ );
    
    setPassword( password );
    
    connect( m_job_thread, SIGNAL(response(QString)), this, SLOT(gotResponse(QString)) );
}


AudioScrobbler::~AudioScrobbler()
{
    /*if( m_job_thread->running() )
        m_job_thread->wait();*/
    delete m_job_thread;
}

void AudioScrobbler::setPassword( QString password )
{
    m_password = QMD5::MD5( password );
}

void AudioScrobbler::gotResponse( QString response )
{
    m_job.unlock();
    kdDebug(100) << response << endl;
    
    emit statusMessage( "got response" );
    
    parseResponse( response );
}

inline QString AudioScrobbler::md5Response( void )
{
    return QMD5::MD5( m_password + m_challenge );
}

void AudioScrobbler::setInterval( unsigned int interval )
{
    // need to do something here?
    m_interval = interval;
}

void AudioScrobbler::parseResponse( QString response )
{
    static QRegExp uptodate( "^UPTODATE\\s*(\\S{32})\\s*(\\S+)\\s*(INTERVAL\\s+(\\d+))?", false );
    static QRegExp update( "^UPDATE\\s+(\\S+)\\s*(\\S{32})\\s*(\\S+)\\s*(INTERVAL\\s+(\\d+))?", false );
    static QRegExp failed( "^FAILED\\s+([^\\n]*)\\s*(INTERVAL\\s+(\\d+))?", false );
    static QRegExp baduser( "^BADUSER\\s*(INTERVAL\\s+(\\d+))?", false );
    static QRegExp badauth( "^BADAUTH\\s*(INTERVAL\\s+(\\d+))?", false );
    static QRegExp ok( "^OK\\s*(INTERVAL\\s+(\\d+))?", false );
    
    if( !m_handshake_done )
    {
        if( response.find( uptodate ) >= 0 )
        {
            m_handshake_done = true;
            QStringList re = uptodate.capturedTexts();
            m_challenge = re[1];
            m_postaddress = re[2];
            if( re.count() > 3 )
                setInterval( re[5].toUInt() );
            emit statusMessage( "successful handshake" );
        }
        else if( response.find( update ) >= 0 )
        {
            m_handshake_done = true;
            QStringList re = update.capturedTexts();
            m_challenge = re[2];
            m_postaddress = re[3];
            if( re.count() > 4 )
                setInterval( re[6].toUInt() );
            emit statusMessage( "update, url: " + re[1] );
        }
        else if( response.find( failed ) >= 0 )
        {
            QStringList re = update.capturedTexts();
            
            if( re.count() > 2 )
                setInterval( re[3].toUInt() );
            
            emit statusMessage( "handshake failed: " + re[1] );
        }
        else if( response.find( baduser ) )
        {
            QStringList re = update.capturedTexts();
            if( re.count() > 1 )
                setInterval( re[2].toUInt() );
            // let's do something to not submit again until the user is reset
            
            emit statusMessage( "error: bad username" );
        }
        else
        {
            emit statusMessage( "unparseable response from server" );
            // let's do something here
        }
    }
}

void AudioScrobbler::gotError( void )
{
    m_job.unlock();
    // the request errored out
}

void AudioScrobbler::run_test( void )
{
    doRequest( KURL(HANDSHAKE_ADDR + m_username) );
}
void AudioScrobbler::run_test2(void)
{
    QString postdata = "u=" + m_username + "&s=" + md5Response() + 
       "&a[0]=Stairwell&t[0]=Disaster&b[0]=The%20Sounds%20of%20Change&m[0]=&l[0]=251&i[0]=";
    
    QDateTime cur = QDateTime::currentDateTime( Qt::UTC );
    postdata += cur.toString( "yyyy-MM-dd hh:mm:ss" );
    //postdata += "&"; //in example but I don't know yet i'll try this
    
    doRequest( m_postaddress, postdata );
    
//    doRequest( KURL("http://post.audioscrobbler.com?hs=true&p=1.1&c=juk&v=0.0.1&u=progothdevtest") );
}

/*QString*/void AudioScrobbler::doRequest( const KURL &address, QString postdata )
{
    kdDebug(100) << "firing request" << endl;
    emit statusMessage( "firing request" );

    m_job.lock();
    QCString tmp;
    if( !postdata.isEmpty() )
        tmp = postdata.ascii();//utf8();
    kdDebug(100) << tmp.count() << endl;
    
    //QByteArray tosend = tmp;
    //kdDebug(100) << tosend.count() << endl;
    //tmp.truncate( tmp.count() - 2 );
    //kdDebug(100) << tmp.count() << endl;
    
    m_job_thread->setJob( address, tmp );
    m_job_thread->run/*start*/();
    
    //m_jobwait.wait();
    //m_job_thread->wait();
    
    //return m_job_thread->result;
}
