/*
 * This file is part of KMail.
 * Copyright (c) 2009 Constantin Berzan <exit3219@gmail.com>
 *
 * Parts based on KMail code by:
 * Various authors.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "attachmentcontroller.h"

#include "messagecomposer/attachmentmodel.h"
#include "attachmentview.h"
#include "attachmentfrompublickeyjob.h"
#include "messageviewer/editorwatcher.h"
#include "globalsettings.h"
#include "kmkernel.h"
#include "kmcomposewin.h"
#include "kmcommands.h"
#include "foldercollection.h"

#include <akonadi/itemfetchjob.h>
#include <kio/jobuidelegate.h>

#include <QMenu>
#include <QPointer>

#include <KAction>
#include <KActionCollection>
#include <KDebug>
#include <KEncodingFileDialog>
#include <KFileDialog>
#include <KLocalizedString>
#include <KMessageBox>
#include <KMimeTypeTrader>
#include <KPushButton>
#include <KRun>
#include <KTemporaryFile>

#include "kmreadermainwin.h"
#include <kpimutils/kfileio.h>

#include <libkleo/kleo/cryptobackendfactory.h>
#include <libkleo/ui/keyselectiondialog.h>

#include <messagecore/attachmentcompressjob.h>
#include <messagecore/attachmentfrommimecontentjob.h>
#include <messagecore/attachmentfromurljob.h>
#include <messagecore/attachmentpropertiesdialog.h>

using namespace KMail;
using namespace KPIM;

AttachmentController::AttachmentController( Message::AttachmentModel *model, AttachmentView *view, KMComposeWin *composer )
  : AttachmentControllerBase( model, view, composer, composer->actionCollection() )
{
  mComposer = composer;

  connect( composer, SIGNAL(identityChanged(KPIMIdentities::Identity)),
      this, SLOT(identityChanged()) );
}

AttachmentController::~AttachmentController()
{
//   delete d;
}

void AttachmentController::identityChanged()
{
  const KPIMIdentities::Identity &identity = mComposer->identity();

  // "Attach public key" is only possible if OpenPGP support is available:
  enableAttachPublicKey( Kleo::CryptoBackendFactory::instance()->openpgp() );

  // "Attach my public key" is only possible if OpenPGP support is
  // available and the user specified his key for the current identity:
  enableAttachMyPublicKey( Kleo::CryptoBackendFactory::instance()->openpgp() && !identity.pgpEncryptionKey().isEmpty() );
}

void AttachmentController::attachMyPublicKey()
{
  const KPIMIdentities::Identity &identity = mComposer->identity();
  kDebug() << identity.identityName();
  exportPublicKey( mComposer->identity().pgpEncryptionKey() );
}

void AttachmentController::actionsCreated()
{
  // Disable public key actions if appropriate.
  identityChanged();
}

void AttachmentController::addAttachmentItems( const Akonadi::Item::List &items )
{
  Akonadi::ItemFetchJob *itemFetchJob = new Akonadi::ItemFetchJob( items, this );
  itemFetchJob->fetchScope().fetchFullPayload( true );
  itemFetchJob->fetchScope().setAncestorRetrieval( Akonadi::ItemFetchScope::Parent );
  connect( itemFetchJob, SIGNAL( result( KJob* ) ), this, SLOT( slotFetchJob( KJob* ) ) );
}

void AttachmentController::slotFetchJob( KJob *job )
{
  if ( job->error() ) {
    static_cast<KIO::Job*>(job)->ui()->showErrorMessage();
    return;
  }
  Akonadi::ItemFetchJob *fjob = dynamic_cast<Akonadi::ItemFetchJob*>( job );
  if ( !fjob )
    return;
  Akonadi::Item::List items = fjob->items();

  uint identity = 0;
  if ( items.at( 0 ).isValid() && items.at( 0 ).parentCollection().isValid() ) {
    QSharedPointer<FolderCollection> fd( FolderCollection::forCollection( items.at( 0 ).parentCollection() ) );
    identity = fd->identity();
  }
  KMCommand *command = new KMForwardAttachedCommand( mComposer, items,identity, mComposer );
  command->start();
}


#include "attachmentcontroller.moc"
