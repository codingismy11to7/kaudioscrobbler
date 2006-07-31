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
#include <qtimer.h>

#include <kurl.h>
#include <kdebug.h>
#include <klocale.h>

#include "scrobrequestthread.h"
#include "kascachelist.h"

class KConfigSkeleton;

/**
@author Steven Scott
*/
class AudioScrobbler : public QObject
{
Q_OBJECT
public:
    AudioScrobbler( QWidget *parent, const QString &clientName = "juk", const QString &clientVersion = "0.0.1" );

    ~AudioScrobbler();

    QString handshakeAddress() const
//    { return QString("http://post.audioscrobbler.com?hs=true&p=1.1&c=%1&v=%2&u=").arg(m_clientName).arg(m_clientVersion); }
    { return QString("http://localhost/~progoth/?hs=true&p=1.1&c=%1&v=%2&u=").arg(m_clientName).arg(m_clientVersion); }
    
signals:
    void statusMessage( const QString& );
    void songPlayed( const QString& );

public slots:
    void play( const QString &artist, const QString &songtitle, const QString &album,
               const QString &musicbrainzid, unsigned int length );
    void pause();
    void unpause();
    void seek();
    void stop();

    void doHandshake();
    void gotResponse( const QString &response );
    void gotError();
    
    void setInterval( unsigned int interval );

    void showSettings() const;
    
    void setSettings();

protected slots:        
    void saveCacheToDisk();
    void saveLockedCacheToDisk();
    void loadCacheFromDisk();
    
    void submit();
    void newSong();
    
protected:
    void doRequest( const KURL &address, const QString &postdata = QString::null );
    
    void cacheSettings( QWidget *parent ) const;
    
    void parseResponse( const QString &response );
    
    void setPassword( const QString &password );
    
    inline QString md5Response() const;
    
    KASCacheList m_subcache;
    QMutex m_cache_mutex;
    KConfigSkeleton *m_cachesave;
    QTimer *cacheTimer;
    
    QString m_clientName;
    QString m_clientVersion;
    
    QString m_postaddress;
    QString m_challenge;
    QMutex m_job;
    bool m_handshake_done;
    
    QString m_username;
    QString m_password;
    
    QTimer *hsTimer;
    QTimer *intervalTimer;
    QTimer *songPlayTimer;
    
    ScrobRequestThread *m_job_thread;
    
    unsigned int m_interval;
    unsigned int m_cachesaveinterval;
    
    ScrobSongData m_currentsong;
    QDateTime m_playtime;
    unsigned int m_remainingtime;
    bool m_songplaying;

    static unsigned int MIN_SONG_LENGTH;
    static unsigned int MAX_SONG_LENGTH;
    static unsigned int MIN_PLAY_TIME;
};

#endif
