#include "dummy-switch-helper.h"
#include "ns3/log.h"
#include "ns3/node.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("DummySwitchHelper");

DummySwitchHelper::DummySwitchHelper ()
{
    m_deviceFactory.SetTypeId ("ns3::DummySwitchNetDevice");
}

void
DummySwitchHelper::SetDeviceAttribute (std::string name, const AttributeValue& value)
{
    m_deviceFactory.Set (name, value);
}

Ptr<DummySwitchNetDevice>
DummySwitchHelper::Install (Ptr<Node> node, NetDeviceContainer ports)
{
    NS_LOG_FUNCTION (this << node);

    Ptr<DummySwitchNetDevice> sw =
        m_deviceFactory.Create<DummySwitchNetDevice> ();

    sw->SetNode (node);
    node->AddDevice (sw);

    for (uint32_t i = 0; i < ports.GetN (); i++)
    {
        Ptr<NetDevice> port = ports.Get (i);
        sw->AddPort (port);
        NS_LOG_INFO ("Installed port " << i << " on switch node " << node->GetId ());
    }

    return sw;
}

Ptr<DummySwitchNetDevice>
DummySwitchHelper::InstallWithQueueDiscs (Ptr<Node> node,
                                           NetDeviceContainer ports,
                                           ObjectFactory qdFactory)
{
    NS_LOG_FUNCTION (this << node);

    Ptr<DummySwitchNetDevice> sw = Install (node, ports);

    for (uint32_t i = 0; i < sw->GetNPorts (); i++)
    {
        Ptr<QueueDisc> qd = qdFactory.Create<QueueDisc> ();
        qd->Initialize ();
        sw->SetPortQueueDisc (i, qd);
        NS_LOG_INFO ("Attached QueueDisc to port " << i);
    }

    return sw;
}

} // namespace ns3