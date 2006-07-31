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
#include "qmd5.h"

#include <stdio.h>

QMD5::QMD5()
{
    md5_init( &m_state );
}

QMD5::~QMD5()
{
}

void QMD5::add( QString in )
{
    md5_append( &m_state, (const md5_byte_t *)in.ascii(), in.length() ); 
}

QString QMD5::getMD5( void )
{
    md5_state_t t_state;
    t_state = m_state; // let's make a copy so we can get multiple md5's from this obj

    md5_byte_t digest[16];
    char output[33];
    
    md5_finish( &t_state, digest );

    for( int i = 0; i < 16; i++ )
    {
        sprintf( output + i*2, "%02x", digest[i] );
    }

    return output;
}

QString QMD5::MD5( QString in )
{
    QMD5 t;
    t.add( in );
    return t.getMD5();
}

