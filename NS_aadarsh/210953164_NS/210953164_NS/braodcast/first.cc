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

//
// Example of the sending of a datagram to a broadcast address
//
// Network topology
//     ==============
//       |          |
//       n0    n1   n2
//       |     |
//     ==========
//
//   n0 originates UDP broadcast to 255.255.255.255/discard port, which
//   is replicated and received on both n1 and n2

#include "ns3/animation-interface.h"
#include "ns3/applications-module.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-helper.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("CsmaBroadcastExample");

int
main(int argc, char* argv[])
{
    // Users may find it convenient to turn on explicit debugging
    // for selected modules; the below lines suggest how to do this
#if 0
  LogComponentEnable ("CsmaBroadcastExample", LOG_LEVEL_INFO);
#endif
    LogComponentEnable("CsmaBroadcastExample", LOG_PREFIX_TIME);

    // Allow the user to override any of the defaults and the above
    // Bind()s at run-time, via command-line arguments
    CommandLine cmd(__FILE__);
    cmd.Parse(argc, argv);

    NS_LOG_INFO("Create nodes.");
    NodeContainer c;
    c.Create(8);
    NodeContainer c0 = NodeContainer(c.Get(0), c.Get(1), c.Get(2));
    NodeContainer c1 = NodeContainer(c.Get(2), c.Get(3), c.Get(4));
    NodeContainer c2 = NodeContainer(c.Get(4), c.Get(5));
    NodeContainer c3 = NodeContainer(c.Get(5), c.Get(6), c.Get(7));

    NS_LOG_INFO("Build Topology.");
    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", DataRateValue(DataRate(5000000)));
    csma.SetChannelAttribute("Delay", TimeValue(MilliSeconds(2)));

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", DataRateValue(DataRate("5Mbps")));
    pointToPoint.SetChannelAttribute("Delay", TimeValue(NanoSeconds(6560)));

    // We will use these NetDevice containers later, for IP addressing
    NetDeviceContainer n0 = csma.Install(c0);
    NetDeviceContainer n1 = csma.Install(c1);
    NetDeviceContainer n2 = pointToPoint.Install(c2);
    NetDeviceContainer n3 = csma.Install(c3);

    NS_LOG_INFO("Add IP Stack.");
    InternetStackHelper internet;
    internet.Install(c);

    NS_LOG_INFO("Assign IP Addresses.");
    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.1.1.0", "255.255.255.0");
    ipv4.Assign(n0);
    ipv4.SetBase("10.1.2.0", "255.255.255.0");
    ipv4.Assign(n1);
    ipv4.SetBase("10.1.3.0", "255.255.255.0");
    ipv4.Assign(n2);
    ipv4.SetBase("10.1.4.0", "255.255.255.0");
    ipv4.Assign(n3);
    // RFC 863 discard port ("9") indicates packet should be thrown away
    // by the system.  We allow this silent discard to be overridden
    // by the PacketSink application.
    uint16_t port = 9;

    // Create the OnOff application to send UDP datagrams of size
    // 512 bytes (default) at a rate of 500 Kb/s (default) from n0
    NS_LOG_INFO("Create Applications.");
    OnOffHelper onoff("ns3::UdpSocketFactory",
                      Address(InetSocketAddress(Ipv4Address("255.255.255.255"), port)));
    onoff.SetConstantRate(DataRate("500kb/s"));

    ApplicationContainer app = onoff.Install(c0.Get(2));
    ApplicationContainer app1 = onoff.Install(c1.Get(2));
    // ApplicationContainer app2 = onoff.Install(c2.Get(1));
    // Start the application
    app.Start(Seconds(1.0));
    app.Stop(Seconds(10.0));
    app1.Start(Seconds(3.0));
    app1.Stop(Seconds(10.0));
    // app2.Start(Seconds(1.0));
    // app2.Stop(Seconds(10.0));

    // Create an optional packet sink to receive these packets
    PacketSinkHelper sink("ns3::UdpSocketFactory",
                          Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
    PacketSinkHelper sink1("ns3::UdpSocketFactory",
                           Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
    app = sink.Install(c0.Get(0));
    app.Add(sink.Install(c0.Get(1)));
    app.Add(sink.Install(c1.Get(1)));
    app.Add(sink.Install(c1.Get(2)));
    app1 = sink1.Install(c2.Get(1));
    app1.Add(sink1.Install(c1.Get(0)));




    // Configure ascii tracing of all enqueue, dequeue, and NetDevice receive
    // events on all devices.  Trace output will be sent to the file
    // "csma-one-subnet.tr"
    AsciiTraceHelper ascii;
    csma.EnableAsciiAll(ascii.CreateFileStream("csma-broadcast.tr"));

    // Also configure some tcpdump traces; each interface will be traced
    // The output files will be named
    // csma-broadcast-<nodeId>-<interfaceId>.pcap
    // and can be read by the "tcpdump -tt -r" command
    csma.EnablePcapAll("csma-broadcast", false);
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(c);
    Ptr<ConstantPositionMobilityModel> s1 = c.Get(0)->GetObject<ConstantPositionMobilityModel>();
    Ptr<ConstantPositionMobilityModel> s2 = c.Get(1)->GetObject<ConstantPositionMobilityModel>();
    Ptr<ConstantPositionMobilityModel> s3 = c.Get(2)->GetObject<ConstantPositionMobilityModel>();
    Ptr<ConstantPositionMobilityModel> s4 = c.Get(3)->GetObject<ConstantPositionMobilityModel>();
    Ptr<ConstantPositionMobilityModel> s5 = c.Get(4)->GetObject<ConstantPositionMobilityModel>();
    Ptr<ConstantPositionMobilityModel> s6 = c.Get(5)->GetObject<ConstantPositionMobilityModel>();
    Ptr<ConstantPositionMobilityModel> s7 = c.Get(6)->GetObject<ConstantPositionMobilityModel>();
    Ptr<ConstantPositionMobilityModel> s8 = c.Get(7)->GetObject<ConstantPositionMobilityModel>();
    s1->SetPosition(Vector(0, 0, 0));
    s2->SetPosition(Vector(10, 1, 0));
    s3->SetPosition(Vector(20, 1, 0));
    s4->SetPosition(Vector(30, 2, 0));
    s5->SetPosition(Vector(40, 2, 0));
    s6->SetPosition(Vector(50, 3, 0));
    s7->SetPosition(Vector(60, 3, 0));
    s8->SetPosition(Vector(70, 4, 0));
    AnimationInterface anim("first.xml");
    NS_LOG_INFO("Run Simulation.");
    Simulator::Run();
    Simulator::Destroy();
    NS_LOG_INFO("Done.");

    return 0;
}
