#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("FirstScriptExample");

int
main(int argc, char* argv[])
{
    CommandLine cmd(__FILE__);
    cmd.Parse(argc, argv);

    Time::SetResolution(Time::NS);
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    NodeContainer nodes;
    nodes.Create(4);

    PointToPointHelper pointToPoint1;
    pointToPoint1.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    pointToPoint1.SetChannelAttribute("Delay", StringValue("2ms"));
    pointToPoint1.SetQueue("ns3::DropTailQueue", "MaxSize", StringValue("50p"));

    NetDeviceContainer device1, device2, device3;
    device1.Add(pointToPoint1.Install(nodes.Get(0), nodes.Get(2)));
    device2.Add(pointToPoint1.Install(nodes.Get(1), nodes.Get(2)));
    device3.Add(pointToPoint1.Install(nodes.Get(2), nodes.Get(3)));

    InternetStackHelper stack;
    stack.Install(nodes);

    Ipv4AddressHelper address1, address2, address3;
    address1.SetBase("10.1.1.0", "255.255.255.0");
    address2.SetBase("10.1.2.0", "255.255.255.0");
    address3.SetBase("10.1.3.0", "255.255.255.0");

    Ipv4InterfaceContainer interface1 = address1.Assign(device1);
    Ipv4InterfaceContainer interface2 = address2.Assign(device2);
    Ipv4InterfaceContainer interface3 = address3.Assign(device3);

    UdpEchoServerHelper echoServer(9);

    ApplicationContainer serverApps = echoServer.Install(nodes.Get(1));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(50.0));

    UdpEchoClientHelper echoClient(interface2.GetAddress(0), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(16));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(5.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(50.0));
    Ipv4GlobalRoutingHelper::PopulateRoutingTables(); // this is important to make the nodes
                                                      // communicate with each other
    AnimationInterface anim("q1.xml");
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
