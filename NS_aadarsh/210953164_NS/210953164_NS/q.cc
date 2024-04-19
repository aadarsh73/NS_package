#include "ns3/animation-interface.h"
#include "ns3/applications-module.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/core-module.h"
#include "ns3/csma-helper.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-helper.h"
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
    nodes.Create(10);
    NodeContainer csmaNode1, csmaNode2;
    csmaNode1.Add(nodes.Get(3));
    csmaNode1.Add(nodes.Get(4));
    csmaNode1.Add(nodes.Get(5));

    csmaNode2.Add(nodes.Get(8));
    csmaNode2.Add(nodes.Get(9));

    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", StringValue("100Mbps"));
    csma.SetChannelAttribute("Delay", TimeValue(NanoSeconds(6560)));

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer device1, device2, device3, device4, device5, device6, device7, device8;
    device1 = pointToPoint.Install(nodes.Get(0), nodes.Get(2));
    device2 = pointToPoint.Install(nodes.Get(1), nodes.Get(2));
    device3 = pointToPoint.Install(nodes.Get(1), nodes.Get(3));
    device4 = csma.Install(csmaNode1);
    device5 = pointToPoint.Install(nodes.Get(5), nodes.Get(6));
    device6 = pointToPoint.Install(nodes.Get(6), nodes.Get(7));
    device7 = pointToPoint.Install(nodes.Get(7), nodes.Get(8));
    device8 = csma.Install(csmaNode2);

    InternetStackHelper stack;
    stack.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");

    Ipv4InterfaceContainer interface1 = address.Assign(device1);
    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer interface2 = address.Assign(device2);
    address.SetBase("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer interface3 = address.Assign(device3);
    address.SetBase("10.1.4.0", "255.255.255.0");
    Ipv4InterfaceContainer interface4 = address.Assign(device4);
    address.SetBase("10.1.5.0", "255.255.255.0");
    Ipv4InterfaceContainer interface5 = address.Assign(device5);
    address.SetBase("10.1.6.0", "255.255.255.0");
    Ipv4InterfaceContainer interface6 = address.Assign(device6);
    address.SetBase("10.1.7.0", "255.255.255.0");
    Ipv4InterfaceContainer interface7 = address.Assign(device7);
    address.SetBase("10.1.8.0", "255.255.255.0");
    Ipv4InterfaceContainer interface8 = address.Assign(device8);

    UdpEchoServerHelper echoServer(9);

    ApplicationContainer serverApps = echoServer.Install(nodes.Get(1));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    UdpEchoClientHelper echoClient(interface3.GetAddress(0), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(10));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps = echoClient.Install(nodes.Get(9));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // Multicast
    NS_LOG_INFO("Configure multicasting.");
    //
    // Now we can configure multicasting.  As described above, the multicast
    // source is at node zero, which we assigned the IP address of 10.1.1.1
    // earlier.  We need to define a multicast group to send packets to.  This
    // can be any multicast address from 224.0.0.0 through 239.255.255.255
    // (avoiding the reserved routing protocol addresses).
    //

    Ipv4Address multicastSource("10.1.1.1");
    Ipv4Address multicastGroup("225.1.2.4");

    // Now, we will set up multicast routing.  We need to do three things:
    // 1) Configure a (static) multicast route on node n2
    // 2) Set up a default multicast route on the sender n0
    // 3) Have node n4 join the multicast group
    // We have a helper that can help us with static multicast
    Ipv4StaticRoutingHelper multicast;

    // 1) Configure a (static) multicast route on node n2 (multicastRouter)
    Ptr<Node> multicastRouter = nodes.Get(3); // The node in question
    Ptr<NetDevice> inputIf = device3.Get(1);  // The input NetDevice
    NetDeviceContainer outputDevices;         // A container of output NetDevices
    outputDevices.Add(device4.Get(0));
    Ptr<Node> multicastRouter1 = nodes.Get(5); // The node in question
    Ptr<NetDevice> inputIf1 = device4.Get(2);  // The input NetDevice
    NetDeviceContainer outputDevices1;         // A container of output NetDevices
    outputDevices1.Add(device5.Get(0));

    // (we only need one NetDevice here)
    Ptr<Node> multicastRouter2 = nodes.Get(6); // The node in question
    Ptr<NetDevice> inputIf2 = device5.Get(1);  // The input NetDevice
    NetDeviceContainer outputDevices2;         // A container of output NetDevices
    outputDevices2.Add(device6.Get(0));

    Ptr<Node> multicastRouter3=nodes.Get(7);
    Ptr<NetDevice> inputIf3=device6.Get(1);
    NetDeviceContainer outputDevices3;
    outputDevices3.Add(device7.Get(0));


    multicast.AddMulticastRoute(multicastRouter,
                                multicastSource,
                                multicastGroup,
                                inputIf,
                                outputDevices);
    multicast.AddMulticastRoute(multicastRouter1,
                                multicastSource,
                                multicastGroup,
                                inputIf1,
                                outputDevices1);
    multicast.AddMulticastRoute(multicastRouter2,
                                multicastSource,
                                multicastGroup,
                                inputIf2,
                                outputDevices2);
    multicast.AddMulticastRoute(multicastRouter3,
                                multicastSource,
                                multicastGroup,
                                inputIf3,
                                outputDevices3);

    // 2) Set up a default multicast route on the sender n0
    Ptr<Node> sender = nodes.Get(1);
    Ptr<NetDevice> senderIf = device3.Get(0);
    multicast.SetDefaultMulticastRoute(sender, senderIf);
    NS_LOG_INFO("Create Applications.");

    uint16_t multicastPort = 9; // Discard port (RFC 863)
    OnOffHelper onoff("ns3::UdpSocketFactory",
                      Address(InetSocketAddress(multicastGroup, multicastPort)));
    onoff.SetConstantRate(DataRate("255b/s"));
    onoff.SetAttribute("PacketSize", UintegerValue(128));

    ApplicationContainer srcC = onoff.Install(nodes.Get(1));

    srcC.Start(Seconds(10.0));
    srcC.Stop(Seconds(15.0));

    PacketSinkHelper sink("ns3::UdpSocketFactory",
                          InetSocketAddress(Ipv4Address::GetAny(), multicastPort));

    ApplicationContainer sinkC = sink.Install(nodes.Get(7));
    // Start the sink
    sinkC.Start(Seconds(10.0));
    sinkC.Stop(Seconds(15.0));

    // end of multicast
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);

    Ptr<ConstantPositionMobilityModel> s1 =
        nodes.Get(0)->GetObject<ConstantPositionMobilityModel>();
    Ptr<ConstantPositionMobilityModel> s2 =
        nodes.Get(1)->GetObject<ConstantPositionMobilityModel>();
    Ptr<ConstantPositionMobilityModel> s3 =
        nodes.Get(2)->GetObject<ConstantPositionMobilityModel>();
    Ptr<ConstantPositionMobilityModel> s4 =
        nodes.Get(3)->GetObject<ConstantPositionMobilityModel>();
    Ptr<ConstantPositionMobilityModel> s5 =
        nodes.Get(4)->GetObject<ConstantPositionMobilityModel>();
    Ptr<ConstantPositionMobilityModel> s6 =
        nodes.Get(5)->GetObject<ConstantPositionMobilityModel>();
    Ptr<ConstantPositionMobilityModel> s7 =
        nodes.Get(6)->GetObject<ConstantPositionMobilityModel>();
    Ptr<ConstantPositionMobilityModel> s8 =
        nodes.Get(7)->GetObject<ConstantPositionMobilityModel>();
    Ptr<ConstantPositionMobilityModel> s9 =
        nodes.Get(8)->GetObject<ConstantPositionMobilityModel>();
    Ptr<ConstantPositionMobilityModel> s10 =
        nodes.Get(9)->GetObject<ConstantPositionMobilityModel>();

    s1->SetPosition(Vector(0, 0, 0));
    s2->SetPosition(Vector(40, 0., 0));
    s3->SetPosition(Vector(20, 20, 0));
    s4->SetPosition(Vector(55, 0, 0));
    s5->SetPosition(Vector(65, 0, 0));
    s6->SetPosition(Vector(75, 0, 0));
    s7->SetPosition(Vector(85, 0, 0));
    s8->SetPosition(Vector(95, 0, 0));
    s9->SetPosition(Vector(105, 0, 0));
    s10->SetPosition(Vector(115, 0, 0));
    AnimationInterface anim("q.xml");

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
