#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("FirstScriptExample");

int main(int argc, char *argv[])
{
    CommandLine cmd(__FILE__);
    cmd.Parse(argc, argv);

    Time::SetResolution(Time::NS);
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    NodeContainer nodes;
    nodes.Create(5);

    PointToPointHelper pointToPoint1;
    pointToPoint1.SetDeviceAttribute("DataRate", StringValue("1Mbps"));
    pointToPoint1.SetChannelAttribute("Delay", StringValue("50ms"));

    NetDeviceContainer device1, device2, device3, device4, device5, device6;
    device1.Add(pointToPoint1.Install(nodes.Get(0), nodes.Get(1)));
    device2.Add(pointToPoint1.Install(nodes.Get(1), nodes.Get(2)));
    device3.Add(pointToPoint1.Install(nodes.Get(1), nodes.Get(3)));
    device4.Add(pointToPoint1.Install(nodes.Get(0), nodes.Get(4)));
    device5.Add(pointToPoint1.Install(nodes.Get(3), nodes.Get(2)));
    device6.Add(pointToPoint1.Install(nodes.Get(2), nodes.Get(4)));

    InternetStackHelper stack;
    stack.Install(nodes);

    Ipv4AddressHelper address1, address2, address3, address4, address5, address6;
    address1.SetBase("10.1.1.0", "255.255.255.0");
    address2.SetBase("10.1.2.0", "255.255.255.0");
    address3.SetBase("10.1.3.0", "255.255.255.0");
    address4.SetBase("10.1.4.0", "255.255.255.0");
    address5.SetBase("10.1.5.0", "255.255.255.0");
    address6.SetBase("10.1.6.0", "255.255.255.0");

    Ipv4InterfaceContainer interface1 = address1.Assign(device1);
    Ipv4InterfaceContainer interface2 = address2.Assign(device2);
    Ipv4InterfaceContainer interface3 = address3.Assign(device3);
    Ipv4InterfaceContainer interface4 = address4.Assign(device4);
    Ipv4InterfaceContainer interface5 = address5.Assign(device5);
    Ipv4InterfaceContainer interface6 = address6.Assign(device6);

    UdpEchoServerHelper echoServer(9);

    ApplicationContainer serverApps = echoServer.Install(nodes.Get(4));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    UdpEchoClientHelper echoClient(interface6.GetAddress(1), 9); // interface number not node number
    echoClient.SetAttribute("MaxPackets", UintegerValue(6));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(5.0));
    ApplicationContainer clientApps1 = echoClient.Install(nodes.Get(3));
    clientApps1.Start(Seconds(5.0));
    clientApps1.Stop(Seconds(10.0));

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    AnimationInterface anim("animFile.XML");

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
