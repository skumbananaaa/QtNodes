#pragma once

#include "AbstractNodeGeometry.hpp"

#include <QtGui/QFontMetrics>

namespace QtNodes {

class AbstractGraphModel;
class BasicGraphicsScene;

class NODE_EDITOR_PUBLIC DefaultVerticalNodeGeometry : public AbstractNodeGeometry
{
public:
    DefaultVerticalNodeGeometry(AbstractGraphModel &graphModel);

public:
    QSize size(NodeId const nodeId) const override;

    void recomputeSize(NodeId const nodeId) const override;

    QPointF portPosition(NodeId const nodeId,
                         PortType const portType,
                         PortIndex const index) const override;

    QPointF portTextPosition(NodeId const nodeId,
                             PortType const portType,
                             PortIndex const PortIndex) const override;

    QPointF captionPosition(NodeId const nodeId) const override;

    QRect captionRect(NodeId const nodeId) const override;

    QPointF widgetPosition(NodeId const nodeId) const override;

    QRect resizeHandleRect(NodeId const nodeId) const override;

private:
    QString portText(NodeId const nodeId, PortType const portType, PortIndex const portIndex) const;
    QRect portTextRect(NodeId const nodeId, PortType const portType, PortIndex const portIndex) const;
    uint32_t portBoundingTextHeight(NodeId const nodeId, PortType const portType) const;
    uint32_t maxPortsTextAdvance(NodeId const nodeId, PortType const portType) const;


private:
    static constexpr uint32_t _internalMargin = 10;
    static constexpr uint32_t _internalPadding = 5;
    static constexpr uint32_t _resizeHandleWidth = 7;
    static constexpr uint32_t _minPortDistance = 35;

    // Some variables are mutable because we need to change drawing
    // metrics corresponding to fontMetrics but this doesn't change
    // constness of the Node.

    uint32_t _portSize;
    mutable QFontMetrics _fontMetrics;
    mutable QFontMetrics _boldFontMetrics;
};

} // namespace QtNodes
