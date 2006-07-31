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
#include "scrobsettingsdialog.h"
#include "scroblogindialog.h"
#include "scrobstatus.h"
#include "kasconfig.h"

#include <kconfigdialog.h>
#include <kconfigskeleton.h>

AudioScrobbler::AudioScrobbler( QWidget *parent )
  : QObject(), m_postaddress(QString::null), m_challenge(QString::null), m_handshake_done(false),
    m_interval(0), m_cachesaveinterval(300000)
{
    m_job_thread = new ScrobRequestThread( );
    
    cacheSettings( parent );
    
    m_cachesave = new KConfigSkeleton( "kaudioscrobblercache" );
    m_cachesave->setCurrentGroup( "main" );
    m_cachesave->addItemStringList( "cache", m_subcache );
    
    loadCacheFromDisk();
    
    cacheTimer = new QTimer( this );
    connect( cacheTimer, SIGNAL(timeout()), this, SLOT(saveCacheToDisk()) );
    cacheTimer->start( m_cachesaveinterval );
    
    connect( m_job_thread, SIGNAL(response(QString)), this, SLOT(gotResponse(QString)) );
    connect( m_job_thread, SIGNAL(http_error()), this, SLOT(gotError(void )) );
    
    hsTimer = new QTimer( this );
    connect( hsTimer, SIGNAL(timeout()), this, SLOT(doHandshake()) );

    intervalTimer = new QTimer( this );
    connect( intervalTimer, SIGNAL(timeout()), this, SLOT(submit()) );
        
    setSettings();
}

void AudioScrobbler::submit( void )
{
    if( intervalTimer->isActive() )
        return;
    
    m_cache_mutex.lock();
    if( m_subcache.getSubCount() )
        doRequest( m_postaddress, m_subcache.getPostData( m_username, md5Response() ) );
    else
        m_cache_mutex.unlock();
}

void AudioScrobbler::setSettings( void )
{
    m_handshake_done = false;
    
    m_username = KASConfig::username();
    
    setPassword( KASConfig::password() );
    
    m_cachesaveinterval = KASConfig::cachesaveinterval() * 60 * 1000;
    
    hsTimer->start( 5000, true );
}

void AudioScrobbler::cacheSettings( QWidget *parent )
{
    KConfigDialog *dialog = new KConfigDialog( parent, "settings", KASConfig::self() );
    
    ScrobLoginDialog *loginPage = new ScrobLoginDialog( 0, "AudioScrobbler Login" );
    ScrobSettingsDialog *confWdg = new ScrobSettingsDialog( 0, "AudioScrobbler Settings" );
    ScrobStatus *statusPage = new ScrobStatus( 0, "AudioScrobbler Status" );
    
    connect( this, SIGNAL(statusMessage(const QString&)), statusPage, SLOT(statusMessage(const QString&)) );
    connect( this, SIGNAL(songPlayed(const QString&)), statusPage, SLOT(newSong(const QString&)) );
    
    dialog->addPage( loginPage, "Login", "kaudioscrobbler" );
    dialog->addPage( confWdg, "General", "kaudioscrobbler" );
    dialog->addPage( statusPage, "Status", "kaudioscrobbler" );
    
    connect( dialog, SIGNAL( settingsChanged() ), this, SLOT( setSettings() ) );
}

void AudioScrobbler::showSettings()
{
    KConfigDialog::showDialog( "settings" );
}

AudioScrobbler::~AudioScrobbler()
{
    saveCacheToDisk();
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
            //hsTimer->stop();
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
            //hsTimer->stop();
            emit statusMessage( "update, url: " + re[1] );
        }
        else if( response.find( failed ) >= 0 )
        {
            QStringList re = failed.capturedTexts();
            
            if( re.count() > 2 )
                setInterval( re[3].toUInt() );
            
            //hsTimer->changeInterval(60000);
            emit statusMessage( "handshake failed: " + re[1] );
        }
        else if( response.find( baduser ) >= 0 )
        {
            QStringList re = baduser.capturedTexts();
            if( re.count() > 1 )
                setInterval( re[2].toUInt() );
            // let's do something to not submit again until the user is reset
            
            //hsTimer->stop();
            emit statusMessage( "error: bad username" );
        }
        else
        {
            //hsTimer->changeInterval(60000);
            emit statusMessage( "unparseable response from server" );
            // let's do something here
        }
    }
    else // we've already handshaken
    {
        if( response.find( failed ) >= 0 )
        {
            QStringList re = failed.capturedTexts();
            if( re.count() > 2 )
                setInterval( re[3].toUInt() );
            
            emit statusMessage( "submit failed: " + re[1] );
        }
        else if( response.find( badauth ) >= 0 )
        {
            QStringList re = badauth.capturedTexts();
            if( re.count() > 1 )
                setInterval( re[2].toUInt() );
            emit statusMessage( "bad password" );
        }
        else if( response.find( ok ) >= 0 )
        {
            QStringList re = ok.capturedTexts();
            if( re.count() > 1 )
                setInterval( re[2].toUInt() );
            m_subcache.clear();
            saveLockedCacheToDisk();
            emit statusMessage( "submit succeeded" );
        }
        else
        {
            emit statusMessage( "unparseable response from server" );
            // let's do something here like set a delay or something
        }
        m_cache_mutex.unlock();
    }
    if( m_interval ) intervalTimer->start( m_interval, true );
}

void AudioScrobbler::gotError( void )
{
    m_job.unlock();
    
    emit statusMessage( "error trying to talk to server" );
    
    if( m_handshake_done )
        m_cache_mutex.unlock();
    else
        hsTimer->start( 60000, true );
    
    //if( !m_handshake_done )
    //    hsTimer->changeInterval(60000);
}

void AudioScrobbler::saveCacheToDisk( void )
{
    m_cache_mutex.lock();
    m_cachesave->writeConfig();
    m_cache_mutex.unlock();
}

void AudioScrobbler::saveLockedCacheToDisk( void )
{
    m_cachesave->writeConfig();
}

void AudioScrobbler::loadCacheFromDisk( void )
{
    m_cache_mutex.lock();
    m_cachesave->readConfig();
    m_cache_mutex.unlock();
}

void AudioScrobbler::doHandshake()
{
    if( !m_handshake_done || KASConfig::username() != m_username )
        doRequest( KURL(HANDSHAKE_ADDR + m_username) );
}
void AudioScrobbler::run_test2(void)
{

    //QString postdata = "u=" + m_username + "&s=" + md5Response() + 
    //   "&a[0]=Stairwell&t[0]=Disaster&b[0]=The%20Sounds%20of%20Change&m[0]=&l[0]=251&i[0]=";
    
    //QDateTime cur = QDateTime::currentDateTime( Qt::UTC );
    //QString tm = cur.toString( "yyyy-MM-dd hh:mm:ss" );

    m_cache_mutex.lock();
    
    m_subcache.addSubmission( "Stairwell", "Disaster", "The Sounds of Change", "", 251/*, tm*/ );
    
    doRequest( m_postaddress, m_subcache.getPostData( m_username, md5Response() ) );
    
    emit songPlayed( "Stairwell - Disaster" );
    //kdDebug(100) << m_subcache.getPostData( m_username, md5Response() ) << endl;
    
//    doRequest( KURL("http://post.audioscrobbler.com?hs=true&p=1.1&c=juk&v=0.0.1&u=progothdevtest") );
}

void AudioScrobbler::doRequest( const KURL &address, QString postdata )
{
    emit statusMessage( "firing request" );

    m_job.lock();
    QCString tmp;
    if( !postdata.isEmpty() )
        tmp = postdata.utf8();//ascii();//utf8();
    
    m_job_thread->setJob( address, tmp );
    m_job_thread->run/*start*/();
    
}
