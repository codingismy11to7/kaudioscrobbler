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
#ifndef KASCACHELIST_H
#define KASCACHELIST_H

#include <qstringlist.h>

typedef struct {
    QString artist;
    QString songtitle;
    QString album;
    QString mbid;
    unsigned int length;
} ScrobSongData;

/**
@author Steven Scott
*/
class KASCacheList : public QStringList
{
//Q_OBJECT
public:
    //KASCacheList(QObject *parent = 0, const char *name = 0);

    //~KASCacheList();

    int submissionCount( void ) const;
    
    void addSubmission( const ScrobSongData & );
    void addSubmission( const QString &artist, const QString &songtitle, const QString &album,
                        const QString &mbid, int seconds );

    QCString postData( const QString &username, const QString &md5response ) const; 
        
};

#endif
