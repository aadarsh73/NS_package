#include <fstream>
#include "ns3/constant-position-mobility-model.h"
#include "ns3/mobility-helper.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/animation-interface.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-routing-table-entry.h"

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

    RipHelper ripRouting;

    Ipv4ListRoutingHelper listRH;
    listRH.Add(ripRouting, 0);
    Ipv4StaticRoutingHelper staticRh;
    listRH.Add(staticRh, 5);

    InternetStackHelper stack;
    stack.SetRoutingHelper(listRH);
    stack.SetIpv6StackInstall(false);
    stack.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase(Ipv4Address("10.1.1.0"), Ipv4Mask("255.255.255.0"));
    Ipv4InterfaceContainer interface1 = address.Assign(device1);
    address.SetBase(Ipv4Address("10.1.2.0"), Ipv4Mask("255.255.255.0"));
    Ipv4InterfaceContainer interface2 = address.Assign(device2);
    address.SetBase(Ipv4Address("10.1.3.0"), Ipv4Mask("255.255.255.0"));
    Ipv4InterfaceContainer interface3 = address.Assign(device3);
    address.SetBase(Ipv4Address("10.1.4.0"), Ipv4Mask("255.255.255.0"));
    Ipv4InterfaceContainer interface4 = address.Assign(device4);
    address.SetBase(Ipv4Address("10.1.5.0"), Ipv4Mask("255.255.255.0"));
    Ipv4InterfaceContainer interface5 = address.Assign(device5);
    address.SetBase(Ipv4Address("10.1.6.0"), Ipv4Mask("255.255.255.0"));
    Ipv4InterfaceContainer interface6 = address.Assign(device6);
    address.SetBase(Ipv4Address("10.1.7.0"), Ipv4Mask("255.255.255.0"));
    Ipv4InterfaceContainer interface7 = address.Assign(device7);
    address.SetBase(Ipv4Address("10.1.8.0"), Ipv4Mask("255.255.255.0"));
    Ipv4InterfaceContainer interface8 = address.Assign(device8);

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    OnOffHelper onoff("ns3::UdpSocketFactory",
                      Address(InetSocketAddress(interface3.GetAddress(1), 9)));
    onoff.SetConstantRate(DataRate("448kb/s"));
    ApplicationContainer apps1 = onoff.Install(nodes.Get(0));
    apps1.Start(Seconds(15.0));
    apps1.Stop(Seconds(25.0));

    // Create a packet sink to receive these packets
    PacketSinkHelper sink("ns3::UdpSocketFactory",
                          Address(InetSocketAddress(Ipv4Address::GetAny(), 9)));
    apps1 = sink.Install(nodes.Get(3));
    apps1.Start(Seconds(15.0));
    apps1.Stop(Seconds(25.0));

    Ptr<Ipv4StaticRouting> staticRouting;
    staticRouting = Ipv4RoutingHelper::GetRouting<Ipv4StaticRouting>(nodes.Get(0)->GetObject<Ipv4>()->GetRoutingProtocol());
    staticRouting->SetDefaultRoute("10.1.1.2", 1);
    staticRouting = Ipv4RoutingHelper::GetRouting<Ipv4StaticRouting>(nodes.Get(3)->GetObject<Ipv4>()->GetRoutingProtocol());
    staticRouting->SetDefaultRoute("10.1.3.1", 1);

    NS_LOG_INFO("Create Applications.");

    NS_LOG_INFO("Create Applications.");
    uint32_t packetSize = 1024;
    Time interPacketInterval = Seconds(1.0);
    V4PingHelper ping("10.0.3.2");

    ping.SetAttribute("Interval", TimeValue(interPacketInterval));
    ping.SetAttribute("Size", UintegerValue(packetSize));
    ApplicationContainer apps = ping.Install(nodes.Get(0));
    apps.Start(Seconds(1.0));
    apps.Stop(Seconds(10.0));

    NS_LOG_INFO("Configure multicasting.");
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
    Simulator::Stop(Seconds(40.0));
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
