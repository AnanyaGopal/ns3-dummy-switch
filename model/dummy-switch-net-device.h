#ifndef DUMMY_SWITCH_NET_DEVICE_H
#define DUMMY_SWITCH_NET_DEVICE_H

#include "ns3/net-device.h"
#include "ns3/mac48-address.h"
#include "ns3/node.h"
#include "ns3/queue-disc.h"
#include "ns3/traffic-control-layer.h"
#include <vector>

namespace ns3 {

/**
 * \brief Per-port state for the dummy switch.
 *
 * Each port holds a pointer to the NetDevice facing that
 * port's channel, and an optional egress QueueDisc that
 * acts as the pluggable traffic manager for that port.
 */
struct DummySwitchPort
{
    Ptr<NetDevice> device;      ///< NetDevice attached to this port
    Ptr<QueueDisc> queueDisc;   ///< Egress traffic manager (may be null)
    uint32_t portIndex;         ///< Zero-based port number
};

/**
 * \brief Skeleton dummy switch NetDevice for ns-3.
 *
 * Models a multi-port switch at the NetDevice layer.
 * Each egress port can attach an independent QueueDisc
 * (traffic manager). Packet forwarding logic is intentionally
 * left as a stub -- this skeleton focuses on the port-level
 * architecture.
 */
class DummySwitchNetDevice : public NetDevice
{
public:
    static TypeId GetTypeId (void);

    DummySwitchNetDevice ();
    virtual ~DummySwitchNetDevice ();

    /**
     * \brief Add a port to the switch.
     * \param device The NetDevice representing this port.
     * \returns The zero-based index of the newly added port.
     */
    uint32_t AddPort (Ptr<NetDevice> device);

    /**
     * \brief Attach a traffic manager (QueueDisc) to an egress port.
     * \param portIndex Zero-based port index.
     * \param queueDisc The QueueDisc to attach.
     */
    void SetPortQueueDisc (uint32_t portIndex, Ptr<QueueDisc> queueDisc);

    /**
     * \brief Return the number of ports on this switch.
     */
    uint32_t GetNPorts (void) const;

    /**
     * \brief Return the port struct for a given index.
     */
    const DummySwitchPort& GetPort (uint32_t portIndex) const;

    // NetDevice interface
    virtual void SetIfIndex (const uint32_t index) override;
    virtual uint32_t GetIfIndex (void) const override;
    virtual Ptr<Channel> GetChannel (void) const override;
    virtual void SetAddress (Address address) override;
    virtual Address GetAddress (void) const override;
    virtual bool SetMtu (const uint16_t mtu) override;
    virtual uint16_t GetMtu (void) const override;
    virtual bool IsLinkUp (void) const override;
    virtual void AddLinkChangeCallback (Callback<void> callback) override;
    virtual bool IsBroadcast (void) const override;
    virtual Address GetBroadcast (void) const override;
    virtual bool IsMulticast (void) const override;
    virtual Address GetMulticast (Ipv4Address multicastGroup) const override;
    virtual Address GetMulticast (Ipv6Address addr) const override;
    virtual bool IsPointToPoint (void) const override;
    virtual bool IsBridge (void) const override;
    virtual bool Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber) override;
    virtual bool SendFrom (Ptr<Packet> packet, const Address& source,
                           const Address& dest, uint16_t protocolNumber) override;
    virtual Ptr<Node> GetNode (void) const override;
    virtual void SetNode (Ptr<Node> node) override;
    virtual bool NeedsArp (void) const override;
    virtual void SetReceiveCallback (NetDevice::ReceiveCallback cb) override;
    virtual void SetPromiscReceiveCallback (NetDevice::PromiscReceiveCallback cb) override;
    virtual bool SupportsSendFrom (void) const override;

private:
    /**
     * \brief Called when a packet arrives on any port.
     */
    bool ReceiveFromPort (Ptr<NetDevice> device, Ptr<const Packet> packet,
                          uint16_t protocol, const Address& source,
                          const Address& destination, PacketType packetType);

    /**
     * \brief Stub forwarding logic -- override in subclasses.
     */
    void ForwardPacket (Ptr<const Packet> packet, uint16_t protocol,
                        const Address& source, const Address& destination,
                        uint32_t ingressPort);

    std::vector<DummySwitchPort> m_ports;   ///< All switch ports
    Ptr<Node>                    m_node;
    Mac48Address                 m_address;
    uint32_t                     m_ifIndex;
    uint16_t                     m_mtu;
    NetDevice::ReceiveCallback         m_rxCallback;
    NetDevice::PromiscReceiveCallback  m_promiscRxCallback;
};

} // namespace ns3

#endif // DUMMY_SWITCH_NET_DEVICE_H