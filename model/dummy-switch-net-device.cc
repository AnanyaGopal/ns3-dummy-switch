#include "dummy-switch-net-device.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/mac48-address.h"
#include "ns3/channel.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("DummySwitchNetDevice");
NS_OBJECT_ENSURE_REGISTERED (DummySwitchNetDevice);

TypeId
DummySwitchNetDevice::GetTypeId (void)
{
    static TypeId tid = TypeId ("ns3::DummySwitchNetDevice")
        .SetParent<NetDevice> ()
        .SetGroupName ("DummySwitch")
        .AddConstructor<DummySwitchNetDevice> ()
        .AddAttribute ("Mtu",
                       "Maximum transmission unit of the switch.",
                       UintegerValue (1500),
                       MakeUintegerAccessor (&DummySwitchNetDevice::m_mtu),
                       MakeUintegerChecker<uint16_t> ())
    ;
    return tid;
}

DummySwitchNetDevice::DummySwitchNetDevice ()
    : m_ifIndex (0),
      m_mtu (1500)
{
    NS_LOG_FUNCTION (this);
    m_address = Mac48Address::Allocate ();
}

DummySwitchNetDevice::~DummySwitchNetDevice ()
{
    NS_LOG_FUNCTION (this);
}

uint32_t
DummySwitchNetDevice::AddPort (Ptr<NetDevice> device)
{
    NS_LOG_FUNCTION (this << device);
    uint32_t index = m_ports.size ();
    DummySwitchPort port;
    port.device    = device;
    port.queueDisc = nullptr;
    port.portIndex = index;
    m_ports.push_back (port);

    // Register receive callback on the underlying device
    device->SetPromiscReceiveCallback(MakeCallback(&DummySwitchNetDevice::ReceiveFromPort, this));

    NS_LOG_INFO ("Added port " << index << " device=" << device);
    return index;
}

void
DummySwitchNetDevice::SetPortQueueDisc (uint32_t portIndex, Ptr<QueueDisc> queueDisc)
{
    NS_LOG_FUNCTION (this << portIndex << queueDisc);
    NS_ASSERT_MSG (portIndex < m_ports.size (), "Port index out of range");
    m_ports[portIndex].queueDisc = queueDisc;
    NS_LOG_INFO ("Attached QueueDisc to port " << portIndex);
}

uint32_t
DummySwitchNetDevice::GetNPorts (void) const
{
    return m_ports.size ();
}

const DummySwitchPort&
DummySwitchNetDevice::GetPort (uint32_t portIndex) const
{
    NS_ASSERT_MSG (portIndex < m_ports.size (), "Port index out of range");
    return m_ports[portIndex];
}

bool
DummySwitchNetDevice::ReceiveFromPort (Ptr<NetDevice> device,
                                       Ptr<const Packet> packet,
                                       uint16_t protocol,
                                       const Address& source,
                                       const Address& destination,
                                       PacketType packetType)
{
    NS_LOG_FUNCTION (this << device << packet << protocol);

    // Find ingress port index
    uint32_t ingressPort = 0;
    for (uint32_t i = 0; i < m_ports.size (); i++)
    {
        if (m_ports[i].device == device)
        {
            ingressPort = i;
            break;
        }
    }

    NS_LOG_INFO ("Packet received on port " << ingressPort
                 << " size=" << packet->GetSize () << " bytes");

    ForwardPacket (packet, protocol, source, destination, ingressPort);
    return true;
}

void
DummySwitchNetDevice::ForwardPacket (Ptr<const Packet> packet,
                                     uint16_t protocol,
                                     const Address& source,
                                     const Address& destination,
                                     uint32_t ingressPort)
{
    NS_LOG_FUNCTION(this << ingressPort);

    // Stub: flood to all ports except ingress
    for (uint32_t i = 0; i < m_ports.size (); i++)
    {
        if (i == ingressPort) continue;

        DummySwitchPort& port = m_ports[i];

        if (port.queueDisc)
        {
            // Traffic manager is attached -- enqueue through it
            NS_LOG_INFO ("Port " << i << ": forwarding via QueueDisc");
            // QueueDisc::Enqueue expects a non-const packet
            Ptr<Packet> copy = packet->Copy();
            // port.queueDisc->Enqueue (copy);
            port.device->Send(copy, destination, protocol);
        }
        else
        {
            // No traffic manager -- send directly
            NS_LOG_INFO ("Port " << i << ": forwarding directly");
            Ptr<Packet> copy = packet->Copy();
            port.device->Send(copy, destination, protocol);
        }
    }
}

// ---- NetDevice interface stubs ----

void
DummySwitchNetDevice::SetIfIndex (const uint32_t index)
{
    m_ifIndex = index;
}

uint32_t
DummySwitchNetDevice::GetIfIndex (void) const
{
    return m_ifIndex;
}

Ptr<Channel>
DummySwitchNetDevice::GetChannel (void) const
{
    return nullptr;
}

void
DummySwitchNetDevice::SetAddress (Address address)
{
    m_address = Mac48Address::ConvertFrom (address);
}

Address
DummySwitchNetDevice::GetAddress (void) const
{
    return m_address;
}

bool
DummySwitchNetDevice::SetMtu (const uint16_t mtu)
{
    m_mtu = mtu;
    return true;
}

uint16_t
DummySwitchNetDevice::GetMtu (void) const
{
    return m_mtu;
}

bool
DummySwitchNetDevice::IsLinkUp (void) const
{
    return true;
}

void
DummySwitchNetDevice::AddLinkChangeCallback (Callback<void> callback)
{
    // Not implemented in skeleton
}

bool
DummySwitchNetDevice::IsBroadcast (void) const
{
    return true;
}

Address
DummySwitchNetDevice::GetBroadcast (void) const
{
    return Mac48Address::GetBroadcast ();
}

bool
DummySwitchNetDevice::IsMulticast (void) const
{
    return false;
}

Address
DummySwitchNetDevice::GetMulticast (Ipv4Address multicastGroup) const
{
    return Mac48Address::GetMulticast (multicastGroup);
}

Address
DummySwitchNetDevice::GetMulticast (Ipv6Address addr) const
{
    return Mac48Address::GetMulticast (addr);
}

bool
DummySwitchNetDevice::IsPointToPoint (void) const
{
    return false;
}

bool
DummySwitchNetDevice::IsBridge (void) const
{
    return true;
}

bool
DummySwitchNetDevice::Send (Ptr<Packet> packet, const Address& dest,
                             uint16_t protocolNumber)
{
    return false;
}

bool
DummySwitchNetDevice::SendFrom (Ptr<Packet> packet, const Address& source,
                                 const Address& dest, uint16_t protocolNumber)
{
    return false;
}

Ptr<Node>
DummySwitchNetDevice::GetNode (void) const
{
    return m_node;
}

void
DummySwitchNetDevice::SetNode (Ptr<Node> node)
{
    m_node = node;
}

bool
DummySwitchNetDevice::NeedsArp (void) const
{
    return false;
}

void
DummySwitchNetDevice::SetReceiveCallback (NetDevice::ReceiveCallback cb)
{
    m_rxCallback = cb;
}

void
DummySwitchNetDevice::SetPromiscReceiveCallback (NetDevice::PromiscReceiveCallback cb)
{
    m_promiscRxCallback = cb;
}

bool
DummySwitchNetDevice::SupportsSendFrom (void) const
{
    return false;
}

} // namespace ns3