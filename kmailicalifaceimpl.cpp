/*
    This file is part of KMail.
    Copyright (c) 2003 Steffen Hansen <steffen@klaralvdalens-datakonsult.se>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#include "kmailicalifaceimpl.h"

#include <kdebug.h>

#include "kmgroupware.h"

KMailICalIfaceImpl::KMailICalIfaceImpl( KMGroupware* gw )
  : QObject( gw, "KMailICalIfaceImpl"),
    DCOPObject( "KMailICalIface" ),
    mGroupware(gw)
{
  connect( gw, SIGNAL( incidenceAdded( const QString&, const QString& ) ),
	   this, SLOT( slotIncidenceAdded( const QString&, const QString& ) ) );

  connect( gw, SIGNAL( incidenceDeleted( const QString&, const QString& ) ),
	   this, SLOT( slotIncidenceDeleted( const QString&, const QString& ) ) );
}

bool KMailICalIfaceImpl::addIncidence( const QString& folder, 
				       const QString& uid, 
				       const QString& ical )
{
  kdDebug() << "KMailICalIfaceImpl::addIncidence( " << folder << ", "
	    << uid << ", " << ical << " )" << endl;
  return mGroupware->addIncidence( folder, uid, ical );
}

bool KMailICalIfaceImpl::deleteIncidence( const QString& folder, 
					  const QString& uid )
{
  kdDebug() << "KMailICalIfaceImpl::deleteIncidence( " << folder << ", "
	    << uid << " )" << endl;
  return mGroupware->deleteIncidence( folder, uid );
}

QStringList KMailICalIfaceImpl::incidences( const QString& folder )
{
  kdDebug() << "KMailICalIfaceImpl::incidences( " << folder << " )" << endl;
  return mGroupware->incidences( folder );
}

void KMailICalIfaceImpl::slotIncidenceAdded( const QString& folder, const QString& ical )
{
  QByteArray data;
  QDataStream arg(data, IO_WriteOnly );
  arg << folder << ical;
  emitDCOPSignal( "incidenceAdded(QString,QString)", data );
}

void KMailICalIfaceImpl::slotIncidenceDeleted( const QString& folder, const QString& uid )
{
  QByteArray data;
  QDataStream arg(data, IO_WriteOnly );
  arg << folder << uid;
  emitDCOPSignal( "incidenceDeleted(QString,QString)", data );
}

#include "kmailicalifaceimpl.moc"
