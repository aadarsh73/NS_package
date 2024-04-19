#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/mobility-helper.h"
#include "ns3/constant-position-mobility-model.h"

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
    nodes.Create(3);

    PointToPointHelper pointToPoint1;
    pointToPoint1.SetDeviceAttribute("DataRate", StringValue("1Mbps"));
    pointToPoint1.SetChannelAttribute("Delay", StringValue("50ms"));

    PointToPointHelper pointToPoint2;
    pointToPoint2.SetDeviceAttribute("DataRate", StringValue("100kbps"));
    pointToPoint2.SetChannelAttribute("Delay", StringValue("5ms"));

    NetDeviceContainer device1, device2;
    device1.Add(pointToPoint1.Install(nodes.Get(0), nodes.Get(1)));
    device2.Add(pointToPoint2.Install(nodes.Get(1), nodes.Get(2)));

    InternetStackHelper stack;
    stack.Install(nodes);

    Ipv4AddressHelper address1, address2;
    address1.SetBase("10.1.1.0", "255.255.255.0");
    address2.SetBase("10.1.2.0", "255.255.255.0");

    Ipv4InterfaceContainer interface1 = address1.Assign(device1);
    Ipv4InterfaceContainer interface2 = address2.Assign(device2);

    UdpEchoServerHelper echoServer(9);              

    ApplicationContainer serverApps = echoServer.Install(nodes.Get(2));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    UdpEchoClientHelper echoClient(interface2.GetAddress(1), 9); // interface number not node number
    echoClient.SetAttribute("MaxPackets", UintegerValue(5));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    AsciiTraceHelper ascii;
    Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream("mixed-global-routing.tr");
    pointToPoint1.EnableAsciiAll(stream);
    // pointToPoint2.EnableAsciiAll(stream);
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);

    Ptr<ConstantPositionMobilityModel> s1 = nodes.Get(0)->GetObject<ConstantPositionMobilityModel>();
    Ptr<ConstantPositionMobilityModel> s2 = nodes.Get(1)->GetObject<ConstantPositionMobilityModel>();
    Ptr<ConstantPositionMobilityModel> s3 = nodes.Get(2)->GetObject<ConstantPositionMobilityModel>();

    s1->SetPosition(Vector(0, 0, 0));
    s2->SetPosition(Vector(30, 90., 0));
    s3->SetPosition(Vector(90, 30, 0));
    AnimationInterface anim("animFile.XML");
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
