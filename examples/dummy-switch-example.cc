/**
 * Dummy switch example:
 *
 * Two hosts connected to a 2-port dummy switch via CSMA links.
 * Host 0 sends UDP packets to Host 1 through the switch.
 * The switch floods packets out all ports except ingress.
 * Port 1 (egress toward Host 1) has a FqCoDelQueueDisc attached
 * as the pluggable traffic manager.
 *
 *   Host0 ---[csma]--- Switch ---[csma]--- Host1
 *              port0            port1 (FqCoDel)
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/traffic-control-module.h"
#include "ns3/dummy-switch-net-device.h"
#include "ns3/dummy-switch-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("DummySwitchExample");

int
main (int argc, char *argv[])
{
    bool verbose = true;
    uint32_t nPackets = 10;

    CommandLine cmd;
    cmd.AddValue ("verbose",  "Enable logging", verbose);
    cmd.AddValue ("nPackets", "Number of packets to send", nPackets);
    cmd.Parse (argc, argv);

    if (verbose)
    {
        LogComponentEnable ("DummySwitchNetDevice", LOG_LEVEL_INFO);
        LogComponentEnable ("DummySwitchHelper",    LOG_LEVEL_INFO);
        LogComponentEnable ("DummySwitchExample",   LOG_LEVEL_INFO);
    }

    // ---- Nodes ----
    NodeContainer hosts;
    hosts.Create (2);

    NodeContainer switchNode;
    switchNode.Create (1);

    // ---- CSMA links ----
    CsmaHelper csma;
    csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
    csma.SetChannelAttribute ("Delay",    TimeValue (MilliSeconds (1)));

    // host0 <-> switch
    NodeContainer link0 = NodeContainer (hosts.Get (0), switchNode.Get (0));
    NetDeviceContainer devs0 = csma.Install (link0);

    // host1 <-> switch
    NodeContainer link1 = NodeContainer (hosts.Get (1), switchNode.Get (0));
    NetDeviceContainer devs1 = csma.Install (link1);

    // ---- Install switch ----
    // Port devices are the switch-side ends of each CSMA link
    NetDeviceContainer portDevices;
    portDevices.Add (devs0.Get (1)); // switch side of link0
    portDevices.Add (devs1.Get (1)); // switch side of link1

    // Attach FqCoDelQueueDisc on every egress port
    ObjectFactory qdFactory;
    qdFactory.SetTypeId ("ns3::FqCoDelQueueDisc");

    DummySwitchHelper switchHelper;
    // sigabrt signal 6
    // QueueDisc::Initialize() -- FqCoDelQueueDisc requires a
    // NetDeviceQueueInterface to be installed on the device before init
    
    //Ptr<DummySwitchNetDevice> sw =
    //     switchHelper.InstallWithQueueDiscs (switchNode.Get (0), portDevices, qdFactory);

    Ptr<DummySwitchNetDevice> sw =
        switchHelper.Install (switchNode.Get (0), portDevices);

    NS_LOG_INFO ("Switch has " << sw->GetNPorts () << " ports");

    // ---- Internet stack on hosts only ----
    InternetStackHelper internet;
    internet.Install (hosts);

    // Assign IP addresses to host-side devices
    Ipv4AddressHelper ipv4;
    ipv4.SetBase ("10.1.1.0", "255.255.255.0");

    NetDeviceContainer hostDevs;
    hostDevs.Add (devs0.Get (0)); // host0 side
    hostDevs.Add (devs1.Get (0)); // host1 side
    Ipv4InterfaceContainer interfaces = ipv4.Assign (hostDevs);

    // ---- UDP application ----
    uint16_t port = 9;
    UdpEchoServerHelper server (port);
    ApplicationContainer serverApps = server.Install (hosts.Get (1));
    serverApps.Start (Seconds (1.0));
    serverApps.Stop  (Seconds (10.0));

    UdpEchoClientHelper client (interfaces.GetAddress (1), port);
    client.SetAttribute ("MaxPackets", UintegerValue (nPackets));
    client.SetAttribute ("Interval",   TimeValue (Seconds (1.0)));
    client.SetAttribute ("PacketSize", UintegerValue (512));

    ApplicationContainer clientApps = client.Install (hosts.Get (0));
    clientApps.Start (Seconds (2.0));
    clientApps.Stop  (Seconds (10.0));

    // ---- Run ----
    Simulator::Stop (Seconds (11.0));
    Simulator::Run ();
    Simulator::Destroy ();

    return 0;
}