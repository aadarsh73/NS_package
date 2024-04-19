/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/applications-module.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/mobility-helper.h"
#include "ns3/netanim-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

// Default Network Topology
//
//       10.1.1.0
// n0 -------------- n1   n2   n3   n4
//    point-to-point  |    |    |    |
//                    ================
//                      LAN 10.1.2.0

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("SecondScriptExample");

int main(int argc, char *argv[])
{
    bool verbose = true;
    uint32_t nCsma = 4;

    CommandLine cmd(__FILE__);
    cmd.AddValue("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
    cmd.AddValue("verbose", "Tell echo applications to log if true", verbose);

    cmd.Parse(argc, argv);

    if (verbose)
    {
        LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
        LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

    nCsma = nCsma == 0 ? 1 : nCsma;

    NodeContainer p2pNodes;
    p2pNodes.Create(3);

    NodeContainer csmaNodes;
    csmaNodes.Add(p2pNodes.Get(1));
    csmaNodes.Add(p2pNodes.Get(2));
    csmaNodes.Create(nCsma);

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer device1, device2;
    device1 = pointToPoint.Install(p2pNodes.Get(0), p2pNodes.Get(1));
    device2 = pointToPoint.Install(p2pNodes.Get(1), p2pNodes.Get(2));

    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", StringValue("100Mbps"));
    csma.SetChannelAttribute("Delay", TimeValue(NanoSeconds(6560)));

    NetDeviceContainer csmaDevices;
    csmaDevices = csma.Install(csmaNodes);

    InternetStackHelper stack;
    stack.Install(p2pNodes.Get(0));
    stack.Install(csmaNodes);

    Ipv4AddressHelper address1, address2, address3;
    address1.SetBase("10.1.1.0", "255.255.255.0");
    address2.SetBase("10.1.2.0", "255.255.255.0");

    Ipv4InterfaceContainer p2pInterface1, p2pInterface2;
    p2pInterface1 = address1.Assign(device1);
    p2pInterface2 = address2.Assign(device2);

    address3.SetBase("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer csmaInterfaces;
    csmaInterfaces = address3.Assign(csmaDevices);

    UdpEchoServerHelper echoServer(9);

    ApplicationContainer serverApps = echoServer.Install(csmaNodes.Get(nCsma));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    UdpEchoClientHelper echoClient(csmaInterfaces.GetAddress(nCsma), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(6));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps = echoClient.Install(p2pNodes.Get(0));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    pointToPoint.EnablePcapAll("second");
    csma.EnablePcap("second", csmaDevices.Get(1), true);
    AsciiTraceHelper ascii;
    Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream("mixed-global-routing.tr");
    pointToPoint.EnableAsciiAll(stream);
    csma.EnableAsciiAll(stream);

    MobilityHelper mobility1, mobility2;
    mobility1.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility1.Install(p2pNodes);
    mobility2.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility2.Install(csmaNodes);

    Ptr<ConstantPositionMobilityModel> s1 =
        p2pNodes.Get(0)->GetObject<ConstantPositionMobilityModel>();
    Ptr<ConstantPositionMobilityModel> s2 =
        p2pNodes.Get(1)->GetObject<ConstantPositionMobilityModel>();
    Ptr<ConstantPositionMobilityModel> s3 =
        p2pNodes.Get(2)->GetObject<ConstantPositionMobilityModel>();

    Ptr<ConstantPositionMobilityModel> s4 =
        csmaNodes.Get(0)->GetObject<ConstantPositionMobilityModel>();
    Ptr<ConstantPositionMobilityModel> s5 =
        csmaNodes.Get(1)->GetObject<ConstantPositionMobilityModel>();
    Ptr<ConstantPositionMobilityModel> s6 =
        csmaNodes.Get(2)->GetObject<ConstantPositionMobilityModel>();
    Ptr<ConstantPositionMobilityModel> s7 =
        csmaNodes.Get(3)->GetObject<ConstantPositionMobilityModel>();
    Ptr<ConstantPositionMobilityModel> s8 =
        csmaNodes.Get(4)->GetObject<ConstantPositionMobilityModel>();
    s1->SetPosition(Vector(0, 0, 0));
    s2->SetPosition(Vector(30, 90., 0));
    s3->SetPosition(Vector(90, 30, 0));
    s5->SetPosition(Vector(120, 120, 0));
    s4->SetPosition(Vector(120, 60, 0));
    s6->SetPosition(Vector(60, 120, 0));
    s7->SetPosition(Vector(60, 60, 0));
    s8->SetPosition(Vector(60, 90, 0));
    AnimationInterface anim("q1.xml");
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
