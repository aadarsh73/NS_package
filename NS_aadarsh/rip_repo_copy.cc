#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-routing-table-entry.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/mobility-module.h"
#include "ns3/netanim-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

#include <fstream>
using namespace ns3;

NS_LOG_COMPONENT_DEFINE("RipSimpleRouting");

void
LinkUp(Ptr<Node> nodeA, Ptr<Node> nodeB, uint32_t interfaceA, uint32_t interfaceB)
{
    NS_LOG_INFO("hello");
    nodeA->GetObject<Ipv4>()->SetDown(interfaceA);
    nodeB->GetObject<Ipv4>()->SetDown(interfaceB);
}

void
LinkDown(Ptr<Node> nodeA, Ptr<Node> nodeB, uint32_t interfaceA, uint32_t interfaceB)
{
    nodeA->GetObject<Ipv4>()->SetUp(interfaceA);
    nodeB->GetObject<Ipv4>()->SetUp(interfaceB);
}

int
main(int argc, char** argv)
{
    LogComponentEnable("RipSimpleRouting", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    LogComponentEnable("OnOffApplication", LOG_LEVEL_INFO);
    LogComponentEnable("PacketSink", LOG_LEVEL_INFO);
    bool verbose = false;
    bool printRoutingTables = false;
    bool showPings = true;
    std::string SplitHorizon("PoisonReverse");

    CommandLine cmd;
    cmd.AddValue("verbose", "turn on log components", verbose);
    cmd.AddValue("printRoutingTables",
                 "Print routing tables at 30, 60 and 90 seconds",
                 printRoutingTables);
    cmd.AddValue("showPings", "Show Ping6 reception", showPings);
    cmd.AddValue("splitHorizonStrategy",
                 "Split Horizon strategy to use (NoSplitHorizon, SplitHorizon, PoisonReverse)",
                 SplitHorizon);
    cmd.Parse(argc, argv);

    if (verbose)
    {
        LogComponentEnableAll(LogLevel(LOG_PREFIX_TIME | LOG_PREFIX_NODE));
        LogComponentEnable("RipSimpleRouting", LOG_LEVEL_INFO);
        LogComponentEnable("Rip", LOG_LEVEL_ALL);
        LogComponentEnable("Ipv4Interface", LOG_LEVEL_ALL);
        LogComponentEnable("Icmpv4L4Protocol", LOG_LEVEL_ALL);
        LogComponentEnable("Ipv4L3Protocol", LOG_LEVEL_ALL);
        LogComponentEnable("ArpCache", LOG_LEVEL_ALL);
        LogComponentEnable("V4Ping", LOG_LEVEL_ALL);
    }

    if (SplitHorizon == "NoSplitHorizon")
    {
        Config::SetDefault("ns3::Rip::SplitHorizon", EnumValue(Rip::NO_SPLIT_HORIZON));
    }
    else if (SplitHorizon == "SplitHorizon")
    {
        Config::SetDefault("ns3::Rip::SplitHorizon", EnumValue(Rip::SPLIT_HORIZON));
    }
    else
    {
        Config::SetDefault("ns3::Rip::SplitHorizon", EnumValue(Rip::POISON_REVERSE));
    }

    NS_LOG_INFO("Create nodes.");

    NodeContainer c;
    c.Create(11);
    NodeContainer p12 = NodeContainer(c.Get(0), c.Get(1));
    NodeContainer p13 = NodeContainer(c.Get(0), c.Get(2));
    NodeContainer p26 = NodeContainer(c.Get(1), c.Get(5));
    NodeContainer p57 = NodeContainer(c.Get(4), c.Get(6));
    NodeContainer p78 = NodeContainer(c.Get(6), c.Get(7));
    NodeContainer p79 = NodeContainer(c.Get(6), c.Get(8));
    NodeContainer p810 = NodeContainer(c.Get(7), c.Get(9));
    NodeContainer p910 = NodeContainer(c.Get(8), c.Get(9));

    NodeContainer c245 = NodeContainer(c.Get(1), c.Get(3), c.Get(4));
    NodeContainer c1011 = NodeContainer(c.Get(9), c.Get(10));

    /*Ptr<Node> src = CreateObject<Node> ();
    //Names::Add ("SrcNode", src);
    Ptr<Node> dst = CreateObject<Node> ();
    //Names::Add ("DstNode", dst);
    Ptr<Node> a = CreateObject<Node> ();
    //Names::Add ("RouterA", a);
    Ptr<Node> b = CreateObject<Node> ();
    //Names::Add ("RouterB", b);
    Ptr<Node> c = CreateObject<Node> ();
    //Names::Add ("RouterC", c);
    Ptr<Node> d = CreateObject<Node> ();
    //Names::Add ("RouterD", d);
    Ptr<Node> e = CreateObject<Node> ();

    NodeContainer net1 (src, a);
    NodeContainer net2 (a, b);
    NodeContainer net3 (a, c);
    NodeContainer net4 (b, c);
    NodeContainer net5 (c, d);
    NodeContainer net6 (b, d);
    NodeContainer net7 (d, dst);
    NodeContainer net8 (b, e);
    NodeContainer net9 (d, e);
    NodeContainer routers (a, b, c, d, e);
    NodeContainer nodes (src, dst);*/

    NS_LOG_INFO("Create channels.");

    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("1Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("50ms"));
    NetDeviceContainer n1 = p2p.Install(p12);
    NetDeviceContainer n2 = p2p.Install(p13);
    NetDeviceContainer n3 = p2p.Install(p26);
    NetDeviceContainer n4 = p2p.Install(p57);
    NetDeviceContainer n5 = p2p.Install(p78);
    NetDeviceContainer n6 = p2p.Install(p79);
    NetDeviceContainer n7 = p2p.Install(p810);
    NetDeviceContainer n8 = p2p.Install(p910);

    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", DataRateValue(5000000));
    csma.SetChannelAttribute("Delay", TimeValue(MilliSeconds(2)));
    NetDeviceContainer n9 = csma.Install(c245);
    NetDeviceContainer n10 = csma.Install(c1011);

    NS_LOG_INFO("Create IPv4 and routing");
    RipHelper ripRouting;

    // Rule of thumb:
    // Interfaces are added sequentially, starting from 0
    // However, interface 0 is always the loopback...
    /*ripRouting.ExcludeInterface (a, 1);
    ripRouting.ExcludeInterface (d, 3);

    ripRouting.SetInterfaceMetric (c, 3, 10);
    ripRouting.SetInterfaceMetric (d, 1, 10);*/

    Ipv4ListRoutingHelper listRH;
    listRH.Add(ripRouting, 0);

    InternetStackHelper internet;
    internet.SetIpv6StackInstall(false);
    internet.SetRoutingHelper(listRH);
    internet.Install(c);

    /*InternetStackHelper internetNodes;
    internetNodes.SetIpv6StackInstall (false);
    internetNodes.Install (nodes);*/

    NS_LOG_INFO("Assign IPv4 Addresses.");
    Ipv4AddressHelper ipv4;

    ipv4.SetBase(Ipv4Address("10.0.0.0"), Ipv4Mask("255.255.255.0"));
    Ipv4InterfaceContainer iic1 = ipv4.Assign(n1);

    ipv4.SetBase(Ipv4Address("10.0.1.0"), Ipv4Mask("255.255.255.0"));
    Ipv4InterfaceContainer iic2 = ipv4.Assign(n2);

    ipv4.SetBase(Ipv4Address("10.0.2.0"), Ipv4Mask("255.255.255.0"));
    Ipv4InterfaceContainer iic3 = ipv4.Assign(n3);

    ipv4.SetBase(Ipv4Address("10.0.3.0"), Ipv4Mask("255.255.255.0"));
    Ipv4InterfaceContainer iic4 = ipv4.Assign(n4);

    ipv4.SetBase(Ipv4Address("10.0.4.0"), Ipv4Mask("255.255.255.0"));
    Ipv4InterfaceContainer iic5 = ipv4.Assign(n5);

    ipv4.SetBase(Ipv4Address("10.0.5.0"), Ipv4Mask("255.255.255.0"));
    Ipv4InterfaceContainer iic6 = ipv4.Assign(n6);

    ipv4.SetBase(Ipv4Address("10.0.6.0"), Ipv4Mask("255.255.255.0"));
    Ipv4InterfaceContainer iic7 = ipv4.Assign(n7);

    ipv4.SetBase(Ipv4Address("10.0.7.0"), Ipv4Mask("255.255.255.0"));
    Ipv4InterfaceContainer iic8 = ipv4.Assign(n8);

    ipv4.SetBase(Ipv4Address("10.0.8.0"), Ipv4Mask("255.255.255.0"));
    Ipv4InterfaceContainer iic9 = ipv4.Assign(n9);

    ipv4.SetBase(Ipv4Address("10.0.9.0"), Ipv4Mask("255.255.255.0"));
    Ipv4InterfaceContainer iic10 = ipv4.Assign(n10);

    /*Ptr<Ipv4StaticRouting> staticRouting;
    staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (src->GetObject<Ipv4>
    ()->GetRoutingProtocol ()); staticRouting->SetDefaultRoute ("10.0.0.2", 1 ); staticRouting =
    Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (dst->GetObject<Ipv4> ()->GetRoutingProtocol
    ()); staticRouting->SetDefaultRoute ("10.0.6.1", 1 );*/

    /*if (printRoutingTables)
      {
        RipHelper routingHelper;

        Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> (&std::cout);

        routingHelper.PrintRoutingTableAt (Seconds (30.0), a, routingStream);
        routingHelper.PrintRoutingTableAt (Seconds (30.0), b, routingStream);
        routingHelper.PrintRoutingTableAt (Seconds (30.0), c, routingStream);
        routingHelper.PrintRoutingTableAt (Seconds (30.0), d, routingStream);
        routingHelper.PrintRoutingTableAt (Seconds (30.0), e, routingStream);

        routingHelper.PrintRoutingTableAt (Seconds (60.0), a, routingStream);
        routingHelper.PrintRoutingTableAt (Seconds (60.0), b, routingStream);
        routingHelper.PrintRoutingTableAt (Seconds (60.0), c, routingStream);
        routingHelper.PrintRoutingTableAt (Seconds (60.0), d, routingStream);
        routingHelper.PrintRoutingTableAt (Seconds (60.0), e, routingStream);

        routingHelper.PrintRoutingTableAt (Seconds (90.0), a, routingStream);
        routingHelper.PrintRoutingTableAt (Seconds (90.0), b, routingStream);
        routingHelper.PrintRoutingTableAt (Seconds (90.0), c, routingStream);
        routingHelper.PrintRoutingTableAt (Seconds (90.0), d, routingStream);
        routingHelper.PrintRoutingTableAt (Seconds (90.0), e, routingStream);
      }*/

    NS_LOG_INFO("Create Applications.");
    /*uint32_t packetSize = 1024;
    Time interPacketInterval = Seconds (1.0);
    V4PingHelper ping ("10.0.0.1");

    ping.SetAttribute ("Interval", TimeValue (interPacketInterval));
    ping.SetAttribute ("Size", UintegerValue (packetSize));
    if (showPings)
      {
        ping.SetAttribute ("Verbose", BooleanValue (true));
      }
    ApplicationContainer apps = ping.Install (dst);
    apps.Start (Seconds (1.0));
    apps.Stop (Seconds (110.0));*/

    UdpEchoServerHelper echoServer(9);

    ApplicationContainer serverApps = echoServer.Install(c.Get(2));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    UdpEchoClientHelper echoClient(iic2.GetAddress(1), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(100));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(0.25)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps = echoClient.Install(c.Get(9));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(7.0));

    OnOffHelper onoff("ns3::TcpSocketFactory",
                      Address(InetSocketAddress(iic2.GetAddress(1), 3000)));
    onoff.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    onoff.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));

    onoff.SetConstantRate(DataRate("2000b/s"));
    onoff.SetAttribute("PacketSize", UintegerValue(50));

    // onoff.EnableLogComponents();
    ApplicationContainer srcC = onoff.Install(c.Get(5));

    //
    // Tell the application when to start and stop.
    //
    srcC.Start(Seconds(5.0));
    srcC.Stop(Seconds(10.0));

    // Create an optional packet sink to receive these packets
    PacketSinkHelper sink("ns3::TcpSocketFactory", InetSocketAddress(iic3.GetAddress(1), 3000));

    ApplicationContainer sinkC = sink.Install(c.Get(2)); // Node n5
    // Start the sink
    sinkC.Start(Seconds(1.0));
    sinkC.Stop(Seconds(10.0));

    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));
    positionAlloc->Add(Vector(1.0, 0.0, 0.0));
    positionAlloc->Add(Vector(0.0, 1.0, 0.0));
    positionAlloc->Add(Vector(2.0, 0.0, 0.0));
    positionAlloc->Add(Vector(3.0, 0.0, 0.0));
    positionAlloc->Add(Vector(1.0, 1.0, 0.0));
    positionAlloc->Add(Vector(4.0, 0.0, 0.0));
    positionAlloc->Add(Vector(5.0, -1.0, 0.0));
    positionAlloc->Add(Vector(5.0, 1.0, 0.0));
    positionAlloc->Add(Vector(6.0, 0.0, 0.0));
    positionAlloc->Add(Vector(7.0, 0.0, 0.0));
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(c);

    AsciiTraceHelper ascii;
    csma.EnableAsciiAll(ascii.CreateFileStream("rip-simple-routing.tr"));
    csma.EnablePcapAll("rip-simple-routing", true);

    Simulator::Schedule(Seconds(4), &LinkUp, c.Get(4), c.Get(6), 1, 1);
    Simulator::Schedule(Seconds(8), &LinkDown, c.Get(4), c.Get(6), 1, 1);

    Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper>(&std::cout);

    listRH.PrintRoutingTableAt(Seconds(2.0), c.Get(4), routingStream);
    listRH.PrintRoutingTableAt(Seconds(7.5), c.Get(4), routingStream);
    listRH.PrintRoutingTableAt(Seconds(9.5), c.Get(4), routingStream);
    /* Now, do the actual simulation. */
    NS_LOG_INFO("Run Simulation.");
    AnimationInterface anim("l2q2.xml");
    Simulator::Stop(Seconds(15.0));
    Simulator::Run();
    Simulator::Destroy();
    NS_LOG_INFO("Done.");

    return 0;
}
