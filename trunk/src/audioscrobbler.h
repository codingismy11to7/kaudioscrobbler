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

#include "qmd5.h"
#include "scrobrequestthread.h"
#include "kascachelist.h"

#define MIN_SONG_LEN 30
#define MAX_SONG_LEN (30 * 60)
#define MIN_PLAY_TIME 240

#define MIN(a,b) ((a)<(b))?(a):(b)

//#define HANDSHAKE_ADDR "http://post.audioscrobbler.com?hs=true&p=1.1&c=juk&v=0.0.1&u="
#define HANDSHAKE_ADDR "http://localhost/~progoth/?hs=true&p=1.1&c=juk&v=0.0.1&u="

class KConfigSkeleton;

/**
@author Steven Scott
*/
class AudioScrobbler : public QObject
{
Q_OBJECT
public:
    AudioScrobbler( QWidget *parent );

    ~AudioScrobbler();

signals:
    void statusMessage( const QString& );
    void songPlayed( const QString& );

public slots:
    void play( const QString &artist, const QString &songtitle, const QString &album,
               const QString &musicbrainzid, unsigned int length );
    void pause( void );
    void unpause( void );
    void seek( void );
    void stop( void );

    void doHandshake();
    void gotResponse( QString response );
    void gotError( void );
    
    void setInterval( unsigned int interval );

    void showSettings();
    
    void setSettings( void );

protected slots:        
    void saveCacheToDisk( void );
    void saveLockedCacheToDisk( void );
    void loadCacheFromDisk( void );
    
    void submit( void );
    void newSong( void );
    
protected:
    /*QString*/void doRequest( const KURL &address, QString postdata = QString::null );
    
    void cacheSettings( QWidget *parent );
    
    void parseResponse( QString response );
    
    void setPassword( QString password );
    
    inline QString md5Response( void );
    
    KASCacheList m_subcache;
    QMutex m_cache_mutex;
    KConfigSkeleton *m_cachesave;
    QTimer *cacheTimer;
    
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
};

#endif
