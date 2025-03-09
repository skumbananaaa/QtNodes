#include "DefaultVerticalNodeGeometry.hpp"

#include "AbstractGraphModel.hpp"
#include "NodeData.hpp"

#include <QPoint>
#include <QRect>
#include <QWidget>

namespace QtNodes {

DefaultVerticalNodeGeometry::DefaultVerticalNodeGeometry(AbstractGraphModel &graphModel)
    : AbstractNodeGeometry(graphModel)
    , _fontMetrics(QFont())
    , _boldFontMetrics(QFont())
{
    QFont f;
    f.setBold(true);
    _boldFontMetrics = QFontMetrics(f);

    _portSize = _fontMetrics.height();
}

QSize DefaultVerticalNodeGeometry::size(NodeId const nodeId) const
{
    return _graphModel.nodeData<QSize>(nodeId, NodeRole::Size);
}

void DefaultVerticalNodeGeometry::recomputeSize(NodeId const nodeId) const
{
    uint32_t width = 0;
    uint32_t height = 0;

    QRect captionRect = this->captionRect(nodeId);
    uint32_t inPortBoundingTextHeight = this->portBoundingTextHeight(nodeId, PortType::In);
    uint32_t outPortBoundingTextHeight = this->portBoundingTextHeight(nodeId, PortType::Out);

    // Height
    {
        height += _internalMargin;
        
        if (inPortBoundingTextHeight > 0)
        {
            height += inPortBoundingTextHeight;
            height += _internalPadding;
        }

        height += captionRect.height();

        if (auto w = _graphModel.nodeData<QWidget*>(nodeId, NodeRole::Widget))
        {
            height += _internalPadding;
            height += w->height();
        }

        if (outPortBoundingTextHeight > 0)
        {
            height += _internalPadding;
            height += outPortBoundingTextHeight;
        }

        height += _internalMargin;
    }

    // Width
    {
        uint32_t inPortWidth = maxPortsTextAdvance(nodeId, PortType::In);
        uint32_t outPortWidth = maxPortsTextAdvance(nodeId, PortType::Out);

        PortCount nInPorts = _graphModel.nodeData<PortCount>(nodeId, NodeRole::InPortCount);
        PortCount nOutPorts = _graphModel.nodeData<PortCount>(nodeId, NodeRole::OutPortCount);
        uint32_t totalInPortsWidth = nInPorts > 0 ? inPortWidth * nInPorts + _internalPadding * (nInPorts - 1) : 0;
        uint32_t totalOutPortsWidth = nOutPorts > 0 ? outPortWidth * nOutPorts + _internalPadding * (nOutPorts - 1) : 0;

        width = std::max(totalInPortsWidth, totalOutPortsWidth);

        if (auto w = _graphModel.nodeData<QWidget*>(nodeId, NodeRole::Widget))
        {
            width = std::max(width, static_cast<uint32_t>(w->width()));
        }

        width = std::max(width, static_cast<uint32_t>(captionRect.width()));

        width += 2 * _internalMargin;
    }

    _graphModel.setNodeData(nodeId, NodeRole::Size, QSize(width, height));
}

QPointF DefaultVerticalNodeGeometry::portPosition(NodeId const nodeId, PortType const portType, PortIndex const portIndex) const
{
    QSize size = _graphModel.nodeData<QSize>(nodeId, NodeRole::Size);

    qreal x = 0;
    qreal y = 0;
    PortCount nPorts = 0;

    switch (portType) 
    {
    case PortType::In: 
    {
        y = 0.0;
        nPorts = _graphModel.nodeData<PortCount>(nodeId, NodeRole::InPortCount);
        break;
    }
    case PortType::Out: 
    {
        y = size.height();
        nPorts = _graphModel.nodeData<PortCount>(nodeId, NodeRole::OutPortCount);
        break;
    }
    default:
        break;
    }

    uint32_t portWidth = maxPortsTextAdvance(nodeId, portType);
    uint32_t totalCombinedPortsWidth = portWidth * (nPorts - 1) + _internalPadding * (nPorts - 2);
    x = (size.width() - totalCombinedPortsWidth) * 0.5f + portIndex * (portWidth + _internalPadding);

    return QPointF(x, y);
}

QPointF DefaultVerticalNodeGeometry::portTextPosition(NodeId const nodeId, PortType const portType, PortIndex const portIndex) const
{
    QPointF p = portPosition(nodeId, portType, portIndex);
    QRectF rect = portTextRect(nodeId, portType, portIndex);

    p.setX(p.x() - rect.width() / 2.0);

    QSize size = _graphModel.nodeData<QSize>(nodeId, NodeRole::Size);

    switch (portType) 
    {
    case PortType::In:
        p.setY(rect.height() + _internalMargin);
        break;
    case PortType::Out:
        p.setY(size.height() - _internalMargin);
        break;
    default:
        break;
    }

    return p;
}

QRect DefaultVerticalNodeGeometry::captionRect(NodeId const nodeId) const
{
    if (!_graphModel.nodeData<bool>(nodeId, NodeRole::CaptionVisible))
    {
        return QRect();
    }

    return _boldFontMetrics.tightBoundingRect(_graphModel.nodeData<QString>(nodeId, NodeRole::Caption));
}

QPointF DefaultVerticalNodeGeometry::captionPosition(NodeId const nodeId) const
{
    QSize size = _graphModel.nodeData<QSize>(nodeId, NodeRole::Size);
    QRect captionRect = this->captionRect(nodeId);
    uint32_t portBoundingTextHeight = this->portBoundingTextHeight(nodeId, PortType::In);

    uint32_t y = 0;

    y += _internalMargin;
    if (portBoundingTextHeight > 0)
    {
        y += portBoundingTextHeight;
        y += _internalPadding;
    }
    y += captionRect.height();

    return QPointF(0.5 * (size.width() - captionRect.width()), y);
}

QPointF DefaultVerticalNodeGeometry::widgetPosition(NodeId const nodeId) const
{
    QSize size = _graphModel.nodeData<QSize>(nodeId, NodeRole::Size);
    QWidget* pWidget = _graphModel.nodeData<QWidget*>(nodeId, NodeRole::Widget);
    QPointF captionPos = captionPosition(nodeId);

    double x = 0;

    x += 0.5 * size.width();
    if (pWidget)
    {
        x -= 0.5 * pWidget->width();
    }

    double y = 0;
    y += (uint32_t)captionPos.y();
    y += _internalPadding;

    return QPointF(x, y);
}

QRect DefaultVerticalNodeGeometry::resizeHandleRect(NodeId const nodeId) const
{
    QSize size = _graphModel.nodeData<QSize>(nodeId, NodeRole::Size);
    return QRect(size.width() - _resizeHandleWidth, size.height() - _resizeHandleWidth, _resizeHandleWidth, _resizeHandleWidth);
}

QString DefaultVerticalNodeGeometry::portText(NodeId const nodeId, PortType const portType, PortIndex const portIndex) const
{
    if (_graphModel.portData<bool>(nodeId, portType, portIndex, PortRole::CaptionVisible))
    {
        return _graphModel.portData<QString>(nodeId, portType, portIndex, PortRole::Caption);
    }
    else
    {
        return _graphModel.portData<NodeDataType>(nodeId, portType, portIndex, PortRole::DataType).name;
    }
}

QRect DefaultVerticalNodeGeometry::portTextRect(NodeId const nodeId, PortType const portType, PortIndex const portIndex) const
{
    return _fontMetrics.tightBoundingRect(portText(nodeId, portType, portIndex));
}

uint32_t DefaultVerticalNodeGeometry::portBoundingTextHeight(NodeId const nodeId, PortType const portType) const
{
    uint32_t height = 0;

    PortCount nPorts = _graphModel.nodeData<PortCount>(nodeId, portType == PortType::In ? NodeRole::InPortCount : NodeRole::OutPortCount);
    for (PortIndex i = 0; i < nPorts; ++i)
    {
        QRect portTextRect = this->portTextRect(nodeId, portType, i);
        height = std::max(height, (uint32_t)portTextRect.height());
    }

    return height;
}

uint32_t DefaultVerticalNodeGeometry::maxPortsTextAdvance(NodeId const nodeId, PortType const portType) const
{
    uint32_t width = 0;

    PortCount nPorts = _graphModel.nodeData<PortCount>(nodeId, portType == PortType::In ? NodeRole::InPortCount : NodeRole::OutPortCount);
    for (PortIndex portIndex = 0ul; portIndex < nPorts; ++portIndex) 
    {
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
        width = std::max(uint32_t(_fontMetrics.horizontalAdvance(portText(nodeId, portType, portIndex))), width);
#else
        width = std::max(uint32_t(_fontMetrics.width(portText(nodeId, portType, portIndex))), width);
#endif
    }

    return width;
}

} // namespace QtNodes
