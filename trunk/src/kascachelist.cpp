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
#include "kascachelist.h"

#include <qstring.h>
#include <qdatetime.h>

#include <kurl.h>

/*KASCacheList::KASCacheList(QObject *parent, const char *name)
 : QStringList(parent, name)
{
}*/


/*KASCacheList::~KASCacheList()
{
}*/

/**
 * gets the number of cached audioscrobbler entries
 */
int KASCacheList::getSubCount( void )
{
    return count() / 6;
}

void KASCacheList::addSubmission( QString artist, QString songtitle, QString album, QString mbid, int seconds/*, QString time*/ )
{
    append( KURL::encode_string_no_slash( artist ).utf8() );
    append( KURL::encode_string_no_slash( songtitle ).utf8() );
    append( KURL::encode_string_no_slash( album ).utf8() );
    append( mbid );
    append( QString::number( seconds ) );
    
    QDateTime cur = QDateTime::currentDateTime( Qt::UTC );
    QString time = cur.toString( "yyyy-MM-dd hh:mm:ss" );
    
    append( KURL::encode_string_no_slash( time ) );
}

QCString KASCacheList::getPostData( QString username, QString md5response )
{
    username = KURL::encode_string_no_slash( username );
    
    QString ret;
    ret += "u=" + username + "&s=" + md5response;
    int total = getSubCount();
    for( int i = 0; i < total; i++ )
    {
        QString n = QString::number( i );
        
        ret += "&a[" + n + "]=" + (*this)[0 + i * 6]
            +  "&t[" + n + "]=" + (*this)[1 + i * 6]
            +  "&b[" + n + "]=" + (*this)[2 + i * 6]
            +  "&m[" + n + "]=" + (*this)[3 + i * 6]
            +  "&l[" + n + "]=" + (*this)[4 + i * 6]
            +  "&i[" + n + "]=" + (*this)[5 + i * 6];
    }
    
    return ret.utf8();
}

#include "kascachelist.moc"
