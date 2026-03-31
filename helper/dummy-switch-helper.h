#ifndef DUMMY_SWITCH_HELPER_H
#define DUMMY_SWITCH_HELPER_H

#include "ns3/dummy-switch-net-device.h"
#include "ns3/net-device-container.h"
#include "ns3/node.h"
#include "ns3/object-factory.h"
#include "ns3/queue-disc.h"

namespace ns3 {

/**
 * \brief Helper to install a DummySwitchNetDevice on a node
 *        and attach port devices and optional traffic managers.
 *
 * Typical usage:
 * \code
 *   DummySwitchHelper switchHelper;
 *   switchHelper.SetDeviceAttribute ("Mtu", UintegerValue (1500));
 *   Ptr<DummySwitchNetDevice> sw = switchHelper.Install (switchNode, portDevices);
 * \endcode
 */
class DummySwitchHelper
{
public:
    DummySwitchHelper ();

    /**
     * \brief Set an attribute on the DummySwitchNetDevice before installation.
     */
    void SetDeviceAttribute (std::string name, const AttributeValue& value);

    /**
     * \brief Install a DummySwitchNetDevice on \p node,
     *        adding each device in \p ports as a switch port.
     * \returns The installed DummySwitchNetDevice.
     */
    Ptr<DummySwitchNetDevice> Install (Ptr<Node> node,
                                       NetDeviceContainer ports);

    /**
     * \brief Install and also attach a QueueDisc to every egress port.
     * \param node        The switch node.
     * \param ports       The port devices to add.
     * \param qdFactory   An ObjectFactory configured to produce QueueDisc objects.
     * \returns The installed DummySwitchNetDevice.
     */
    Ptr<DummySwitchNetDevice> InstallWithQueueDiscs (Ptr<Node> node,
                                                      NetDeviceContainer ports,
                                                      ObjectFactory qdFactory);

private:
    ObjectFactory m_deviceFactory; ///< Factory for DummySwitchNetDevice
};

} // namespace ns3

#endif // DUMMY_SWITCH_HELPER_H