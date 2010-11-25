/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-12-17
 * Description : image file IO threaded interface.
 *
 * Copyright (C) 2005-2008 by Marcel Wiesweg <marcel.wiesweg@gmx.de>
 * Copyright (C) 2005-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "loadsavetask.h"

// Qt includes.

#include <qapplication.h>

// Local includes.

#include "loadsavethread.h"
#include "managedloadsavethread.h"
#include "sharedloadsavethread.h"
#include "loadingcache.h"

namespace Digikam
{

void LoadingTask::execute()
{
    if (m_loadingTaskStatus == LoadingTaskStatusStopping)
        return;
    DImg img(m_loadingDescription.filePath, this, m_loadingDescription.rawDecodingSettings);
    m_thread->taskHasFinished();
    m_thread->imageLoaded(m_loadingDescription, img);
}

LoadingTask::TaskType LoadingTask::type()
{
    return TaskTypeLoading;
}

void LoadingTask::progressInfo(const DImg *, float progress)
{
    if (m_loadingTaskStatus == LoadingTaskStatusLoading)
    {
        if (m_thread->querySendNotifyEvent())
            m_thread->loadingProgress(m_loadingDescription, progress);
    }
}

bool LoadingTask::continueQuery(const DImg *)
{
    return m_loadingTaskStatus != LoadingTaskStatusStopping;
}

void LoadingTask::setStatus(LoadingTaskStatus status)
{
    m_loadingTaskStatus = status;
}

// This is a hack needed to prevent hanging when a K3Process-based loader (raw loader)
// is waiting for the process to finish, but the main thread is waiting
// for the thread to finish and no K3Process events are delivered.
// Remove when porting to Qt4.
bool LoadingTask::isShuttingDown()
{
    return m_thread->isShuttingDown();
}

//---------------------------------------------------------------------------------------------------

void SharedLoadingTask::execute()
{
    if (m_loadingTaskStatus == LoadingTaskStatusStopping)
        return;
    // send StartedLoadingEvent from each single Task, not via LoadingProcess list
    m_thread->imageStartedLoading(m_loadingDescription);

    DImg img;

    LoadingCache *cache = LoadingCache::cache();
    {
        LoadingCache::CacheLock lock(cache);

        // find possible cached images
        DImg *cachedImg = 0;
        QStringList lookupKeys = m_loadingDescription.lookupCacheKeys();
        for ( QStringList::Iterator it = lookupKeys.begin(); it != lookupKeys.end(); ++it ) {
            if ( (cachedImg = cache->retrieveImage(*it)) )
                break;
        }

        if (cachedImg)
        {
            // image is found in image cache, loading is successful
            img = *cachedImg;
            if (accessMode() == LoadSaveThread::AccessModeReadWrite)
                img = img.copy();
            // continues after else clause...
        }
        else
        {
            // find possible running loading process
            m_usedProcess = 0;
            for ( QStringList::Iterator it = lookupKeys.begin(); it != lookupKeys.end(); ++it ) {
                if ( (m_usedProcess = cache->retrieveLoadingProcess(*it)) )
                {
                    break;
                }
            }

            if (m_usedProcess)
            {
                // Other process is right now loading this image.
                // Add this task to the list of listeners and
                // attach this thread to the other thread, wait until loading
                // has finished.
                m_usedProcess->addListener(this);
                // break loop when either the loading has completed, or this task is being stopped
                while ( !m_usedProcess->completed() && m_loadingTaskStatus != LoadingTaskStatusStopping )
                    lock.timedWait();
                // remove listener from process
                m_usedProcess->removeListener(this);
                // wake up the process which is waiting until all listeners have removed themselves
                lock.wakeAll();
                // set to 0, as checked in setStatus
                m_usedProcess = 0;
                //kDebug(50003) << "SharedLoadingTask " << this << ": waited" << endl;
                return;
            }
            else
            {
                // Neither in cache, nor currently loading in different thread.
                // Load it here and now, add this LoadingProcess to cache list.
                cache->addLoadingProcess(this);
                // Add this to the list of listeners
                addListener(this);
                // for use in setStatus
                m_usedProcess = this;
                // Notify other processes that we are now loading this image.
                // They might be interested - see notifyNewLoadingProcess below
                cache->notifyNewLoadingProcess(this, m_loadingDescription);
            }
        }
    }

    if (!img.isNull())
    {
        // following the golden rule to avoid deadlocks, do this when CacheLock is not held
        m_thread->taskHasFinished();
        m_thread->imageLoaded(m_loadingDescription, img);
        return;
    }

    // load image
    img = DImg(m_loadingDescription.filePath, this, m_loadingDescription.rawDecodingSettings);

    bool isCached = false;
    {
        LoadingCache::CacheLock lock(cache);
        // put (valid) image into cache of loaded images
        if (!img.isNull())
            isCached = cache->putImage(m_loadingDescription.cacheKey(), new DImg(img), m_loadingDescription.filePath);
        // remove this from the list of loading processes in cache
        cache->removeLoadingProcess(this);
    }

    // again: following the golden rule to avoid deadlocks, do this when CacheLock is not held
    m_thread->taskHasFinished();

    {
        LoadingCache::CacheLock lock(cache);
        //kDebug(50003) << "SharedLoadingTask " << this << ": image loaded, " << img.isNull() << endl;
        // indicate that loading has finished so that listeners can stop waiting
        m_completed = true;

        // dispatch image to all listeners, including this
        for (int i=0; i<m_listeners.count(); i++)
        {
            LoadingProcessListener *l = m_listeners[i];
            if (l->accessMode() == LoadSaveThread::AccessModeReadWrite)
            {
                // If a listener requested ReadWrite access, it gets a deep copy.
                // DImg is explicitly shared.
                DImg copy = img.copy();
                l->loadSaveNotifier()->imageLoaded(m_loadingDescription, copy);
            }
            else
            {
                l->loadSaveNotifier()->imageLoaded(m_loadingDescription, img);
            }
        }

        // remove myself from list of listeners
        removeListener(this);
        // wake all listeners waiting on cache condVar, so that they remove themselves
        lock.wakeAll();
        // wait until all listeners have removed themselves
        while (m_listeners.count() != 0)
            lock.timedWait();
        // set to 0, as checked in setStatus
        m_usedProcess = 0;
    }
}

void SharedLoadingTask::progressInfo(const DImg *, float progress)
{
    if (m_loadingTaskStatus == LoadingTaskStatusLoading)
    {
        LoadingCache *cache = LoadingCache::cache();
        LoadingCache::CacheLock lock(cache);

        for (int i=0; i<m_listeners.size(); i++)
        {
            LoadingProcessListener *l = m_listeners[i];
            if (l->querySendNotifyEvent())
                l->loadSaveNotifier()->loadingProgress(m_loadingDescription, progress);
        }
    }
}

bool SharedLoadingTask::continueQuery(const DImg *)
{
    // If this is called, the thread is currently loading an image.
    // In shared loading, we cannot stop until all listeners have been removed as well
    return (m_loadingTaskStatus != LoadingTaskStatusStopping) || (m_listeners.count() != 0);
}

void SharedLoadingTask::setStatus(LoadingTaskStatus status)
{
    m_loadingTaskStatus = status;
    if (m_loadingTaskStatus == LoadingTaskStatusStopping)
    {
        LoadingCache *cache = LoadingCache::cache();
        LoadingCache::CacheLock lock(cache);

        // check for m_usedProcess, to avoid race condition that it has finished before
        if (m_usedProcess)
        {
            // remove this from list of listeners - check in continueQuery() of active thread
            m_usedProcess->removeListener(this);
            // set m_usedProcess to 0, signalling that we have detached already
            m_usedProcess = 0;
            // wake all listeners - particularly this - from waiting on cache condvar
            lock.wakeAll();
        }
    }
}

bool SharedLoadingTask::completed()
{
    return m_completed;
}

QString SharedLoadingTask::filePath()
{
    return m_loadingDescription.filePath;
}

QString SharedLoadingTask::cacheKey()
{
    return m_loadingDescription.cacheKey();
}

void SharedLoadingTask::addListener(LoadingProcessListener *listener)
{
    m_listeners << listener;
}

void SharedLoadingTask::removeListener(LoadingProcessListener *listener)
{
    m_listeners.removeAll(listener);
}

void SharedLoadingTask::notifyNewLoadingProcess(LoadingProcess *process, LoadingDescription description)
{
    // Ok, we are notified that another task has been started in another thread.
    // We are of course only interested if the task loads the same file,
    // and we are right now loading a reduced version, and the other task is loading the full version.
    // In this case, we notify our own thread (a signal to the API user is emitted) of this.
    // The fact that we are receiving the method call shows that this task is registered with the LoadingCache,
    // somewhere in between the calls to addLoadingProcess(this) and removeLoadingProcess(this) above.
    if (process != this &&
        m_loadingDescription.isReducedVersion() &&
        m_loadingDescription.equalsIgnoreReducedVersion(description) &&
        !description.isReducedVersion()
       )
    {
        for (int i=0; i<m_listeners.size(); i++)
        {
            m_listeners[i]->loadSaveNotifier()->moreCompleteLoadingAvailable(m_loadingDescription, description);
        }
    }
}

bool SharedLoadingTask::querySendNotifyEvent()
{
    return m_thread->querySendNotifyEvent();
}

LoadSaveNotifier *SharedLoadingTask::loadSaveNotifier()
{
    return m_thread;
}

LoadSaveThread::AccessMode SharedLoadingTask::accessMode()
{
    return m_accessMode;
}

//---------------------------------------------------------------------------------------------------

void SavingTask::execute()
{
    m_thread->imageStartedSaving(m_filePath);
    bool success = m_img.save(m_filePath, m_format, this);
    m_thread->taskHasFinished();
    m_thread->imageSaved(m_filePath, success);
}

LoadingTask::TaskType SavingTask::type()
{
    return TaskTypeSaving;
}

void SavingTask::progressInfo(const DImg *, float progress)
{
    if (m_thread->querySendNotifyEvent())
        m_thread->savingProgress(m_filePath, progress);
}

bool SavingTask::continueQuery(const DImg *)
{
    return m_savingTaskStatus != SavingTaskStatusStopping;
}

void SavingTask::setStatus(SavingTaskStatus status)
{
    m_savingTaskStatus = status;
}

}   //namespace Digikam
