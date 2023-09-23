/*
    KWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2017 Roman Gilg <subdiff@gmail.com>
    SPDX-FileCopyrightText: 2015 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "drm_buffer_gbm.h"
#include "gbm_surface.h"

#include "logging.h"

// system
#include <sys/mman.h>
// c++
#include <cerrno>
// drm
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <gbm.h>
// KWaylandServer
#include "KWaylandServer/buffer_interface.h"

namespace KWin
{

// DrmSurfaceBuffer
DrmSurfaceBuffer::DrmSurfaceBuffer(int fd, const std::shared_ptr<GbmSurface> &surface)
    : DrmBuffer(fd)
    , m_surface(surface)
{
    m_bo = m_surface->lockFrontBuffer();
    if (!m_bo) {
        qCWarning(KWIN_DRM) << "Locking front buffer failed";
        return;
    }
    m_size = QSize(gbm_bo_get_width(m_bo), gbm_bo_get_height(m_bo));
    if (drmModeAddFB(fd, m_size.width(), m_size.height(), 24, 32, gbm_bo_get_stride(m_bo), gbm_bo_get_handle(m_bo).u32, &m_bufferId) != 0) {
        qCWarning(KWIN_DRM) << "drmModeAddFB failed";
    }
    gbm_bo_set_user_data(m_bo, this, nullptr);
}

DrmSurfaceBuffer::DrmSurfaceBuffer(int fd, gbm_bo *buffer, KWaylandServer::BufferInterface *bufferInterface)
    : DrmBuffer(fd)
    , m_bo(buffer)
    , m_bufferInterface(bufferInterface)
{
    if (m_bufferInterface) {
        m_bufferInterface->ref();
        connect(m_bufferInterface, &KWaylandServer::BufferInterface::aboutToBeDestroyed, this, &DrmSurfaceBuffer::clearBufferInterface);
    }
    m_size = QSize(gbm_bo_get_width(m_bo), gbm_bo_get_height(m_bo));
    if (drmModeAddFB(fd, m_size.width(), m_size.height(), 24, 32, gbm_bo_get_stride(m_bo), gbm_bo_get_handle(m_bo).u32, &m_bufferId) != 0) {
        qCWarning(KWIN_DRM) << "drmModeAddFB failed";
    }
    gbm_bo_set_user_data(m_bo, this, nullptr);
}

DrmSurfaceBuffer::~DrmSurfaceBuffer()
{
    if (m_bufferId) {
        drmModeRmFB(fd(), m_bufferId);
    }
    releaseGbm();
    if (m_bufferInterface) {
        clearBufferInterface();
    }
}

void DrmSurfaceBuffer::releaseGbm()
{
    if (m_surface) {
        m_surface->releaseBuffer(m_bo);
    }
    m_bo = nullptr;
}

void DrmSurfaceBuffer::clearBufferInterface()
{
    disconnect(m_bufferInterface, &KWaylandServer::BufferInterface::aboutToBeDestroyed, this, &DrmSurfaceBuffer::clearBufferInterface);
    m_bufferInterface->unref();
    m_bufferInterface = nullptr;
}

}
