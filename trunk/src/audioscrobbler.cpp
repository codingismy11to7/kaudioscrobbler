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
    m_interval(0), m_cachesaveinterval(300000), m_songplaying(false)
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
    
    songPlayTimer = new QTimer( this );
    connect( songPlayTimer, SIGNAL(timeout()), this, SLOT(newSong()) );
    
    hsTimer = new QTimer( this );
    connect( hsTimer, SIGNAL(timeout()), this, SLOT(doHandshake()) );

    intervalTimer = new QTimer( this );
    connect( intervalTimer, SIGNAL(timeout()), this, SLOT(submit()) );
        
    setSettings();
}

void AudioScrobbler::play( const QString &artist, const QString &songtitle, const QString &album,
               const QString &musicbrainzid, unsigned int length )
{
    if( songPlayTimer->isActive() ) songPlayTimer->stop();
    
    emit statusMessage( tr2i18n( "Playing" ) );
    
    if( length < MIN_SONG_LEN || length > MAX_SONG_LEN ) return;
    
    m_currentsong.artist = artist;
    m_currentsong.songtitle = songtitle;
    m_currentsong.album = album;
    m_currentsong.mbid = musicbrainzid;
    m_currentsong.length = length;
    
    m_playtime = QDateTime::currentDateTime();
    m_songplaying = true;
    m_remainingtime = MIN( 1 + length / 2, MIN_PLAY_TIME );
    songPlayTimer->start( m_remainingtime * 1000, true );
}

void AudioScrobbler::pause( void )
{
    if( !songPlayTimer->isActive() || !m_songplaying ) return;
    songPlayTimer->stop();
    QDateTime cur = QDateTime::currentDateTime();
    
    m_remainingtime -= m_playtime.secsTo( cur );
    
    emit statusMessage( i18n( "Paused" ) );    
    m_songplaying = false;
}

void AudioScrobbler::unpause( void )
{
    if( songPlayTimer->isActive() || m_songplaying ) return;
    m_songplaying = true;
    m_playtime = QDateTime::currentDateTime();
    songPlayTimer->start( m_remainingtime * 1000, true );

    emit statusMessage( i18n( "Playing" ) );
}

void AudioScrobbler::seek( void )
{
    if( songPlayTimer->isActive() )
    {
        songPlayTimer->stop();
        emit statusMessage( i18n( "Seek; submission cancelled" ) );
    }
}

void AudioScrobbler::stop( void )
{
    if( songPlayTimer->isActive() )
        songPlayTimer->stop();
    m_songplaying = false;
    emit statusMessage( i18n( "Stopped" ) );
}

void AudioScrobbler::newSong( void )
{
    m_cache_mutex.lock();
    m_subcache.addSubmission( m_currentsong );
    saveLockedCacheToDisk();
    m_cache_mutex.unlock();
    emit songPlayed( m_currentsong.artist + " - " + m_currentsong.songtitle );
    submit();
}

void AudioScrobbler::submit( void )
{
    if( intervalTimer->isActive() || !m_handshake_done )
        return;
    
    m_cache_mutex.lock();
    if( m_subcache.getSubCount() )
        doRequest( m_postaddress, m_subcache.getPostData( m_username, md5Response() ) );
    else
    {
        m_cache_mutex.unlock();
    }
}

void AudioScrobbler::setSettings( void )
{
    if( hsTimer->isActive() ) hsTimer->stop();
    if( intervalTimer->isActive() ) intervalTimer->stop();
    
    m_handshake_done = false;
    
    m_username = KASConfig::username();
    
    setPassword( KASConfig::password() );
    
    m_cachesaveinterval = KASConfig::cachesaveinterval() * 60 * 1000;
    
    cacheTimer->changeInterval( m_cachesaveinterval );
    
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
    
    dialog->addPage( loginPage, i18n("Login"), "kaudioscrobbler" );
    dialog->addPage( confWdg, i18n("General"), "kaudioscrobbler" );
    dialog->addPage( statusPage, i18n("Status"), "kaudioscrobbler" );
    
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
    
    emit statusMessage( i18n("Got response") );
    
    parseResponse( response );
}

inline QString AudioScrobbler::md5Response( void )
{
    return QMD5::MD5( m_password + m_challenge );
}

void AudioScrobbler::setInterval( unsigned int interval )
{
    // need to do something here?
    m_interval = interval * 1000;
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
            emit statusMessage( i18n("Successful handshake") );
            if( !m_interval ) intervalTimer->start( 2 * 1000, true );
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
            emit statusMessage( i18n("Update needed: %1").arg( re[1] ) );
            if( !m_interval ) intervalTimer->start( 2 * 1000, true );
        }
        else if( response.find( failed ) >= 0 )
        {
            QStringList re = failed.capturedTexts();
            
            if( re.count() > 2 )
                setInterval( re[3].toUInt() );
            
            hsTimer->start( 10 * 60 * 1000, true );
            emit statusMessage( i18n("Handshake failed: %1").arg( re[1] ) );
        }
        else if( response.find( baduser ) >= 0 )
        {
            QStringList re = baduser.capturedTexts();
            if( re.count() > 1 )
                setInterval( re[2].toUInt() );
            // let's do something to not submit again until the user is reset
            
            intervalTimer->start( 24 * 60 * 60 * 1000, true );
            emit statusMessage( i18n("Error: Bad username") );
        }
        else
        {
            emit statusMessage( i18n("Could not parse server response") );
            hsTimer->start( 120000 );
        }
    }
    else // we've already handshaken
    {
        if( response.find( failed ) >= 0 )
        {
            QStringList re = failed.capturedTexts();
            if( re.count() > 2 )
                setInterval( re[3].toUInt() );
            
            emit statusMessage( i18n("Submission failed: %1").arg( re[1] ) );
        }
        else if( response.find( badauth ) >= 0 )
        {
            QStringList re = badauth.capturedTexts();
            if( re.count() > 1 )
                setInterval( re[2].toUInt() );
            emit statusMessage( i18n("Bad password") );
        }
        else if( response.find( ok ) >= 0 )
        {
            QStringList re = ok.capturedTexts();
            if( re.count() > 1 )
                setInterval( re[2].toUInt() );
            m_subcache.clear();
            saveLockedCacheToDisk();
            emit statusMessage( i18n("Submit succeeded") );
        }
        else
        {
            emit statusMessage( i18n("Can't parse server response") );
            intervalTimer->start( 2 * 60 * 1000, true );
        }
        m_cache_mutex.unlock();
    }
    if( m_interval && !intervalTimer->isActive() ) intervalTimer->start( m_interval, true );
}

void AudioScrobbler::gotError( void )
{
    m_job.unlock();
    
    emit statusMessage( i18n("Error trying to talk to server") );
    
    if( m_handshake_done )
    {
        m_cache_mutex.unlock();
    }
    else
        hsTimer->start( 120000, true );
    
    //if( !m_handshake_done )
    //    hsTimer->changeInterval(60000);
}

void AudioScrobbler::saveCacheToDisk( void )
{
    m_cache_mutex.lock();
    saveLockedCacheToDisk();
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

void AudioScrobbler::doRequest( const KURL &address, QString postdata )
{
    emit statusMessage( i18n("Firing request") );

    m_job.lock();
    QCString tmp;
    if( !postdata.isEmpty() )
        tmp = postdata.utf8();//ascii();//utf8();
    
    m_job_thread->setJob( address, tmp );
    m_job_thread->run/*start*/();
}
