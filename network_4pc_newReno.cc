/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
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
  *
  */
 
 //
 // Network topology
 //
 //  n0
 //     \ 5 Mb/s, 2ms
 //      \          1.5Mb/s, 10ms
 //       n2 -------------------------n3
 //      /
 //     / 5 Mb/s, 2ms
 //   n1
 //
 // - all links are point-to-point links with indicated one-way BW/delay
 // - CBR/UDP flows from n0 to n3, and from n3 to n1
 // - FTP/TCP flow from n0 to n3, starting at time 1.2 to time 1.35 sec.
 // - UDP packet size of 210 bytes, with per-packet interval 0.00375 sec.
 //   (i.e., DataRate of 448,000 bps)
 // - DropTail queues 
 // - Tracing of queues and packet receptions to file "simple-global-routing.tr"
 
 #include <iostream>
 #include <fstream>
 #include <string>
 #include <cassert>
 
 #include "ns3/core-module.h"
 #include "ns3/network-module.h"
 #include "ns3/internet-module.h"
 #include "ns3/csma-module.h"
 #include "ns3/point-to-point-module.h"
 #include "ns3/applications-module.h"
 #include "ns3/flow-monitor-helper.h"
 #include "ns3/ipv4-global-routing-helper.h"
 
 using namespace ns3;
 
 NS_LOG_COMPONENT_DEFINE ("SimpleGlobalRoutingExample");
 
 int 
 main (int argc, char *argv[])
 {
   // Users may find it convenient to turn on explicit debugging
   // for selected modules; the below lines suggest how to do this
 #if 0 
   LogComponentEnable ("SimpleGlobalRoutingExample", LOG_LEVEL_INFO);
 #endif
 
   // Set up some default values for the simulation.  Use the 
   Config::SetDefault ("ns3::OnOffApplication::PacketSize", UintegerValue (210));
   Config::SetDefault ("ns3::OnOffApplication::DataRate", StringValue ("448kb/s"));
 
   Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue ("ns3::TcpNewReno"));
   //DefaultValue::Bind ("DropTailQueue::m_maxPackets", 30);
 
   // Allow the user to override any of the defaults and the above
   // DefaultValue::Bind ()s at run-time, via command-line arguments
   CommandLine cmd;
   bool enableFlowMonitor = false;
   cmd.AddValue ("EnableMonitor", "Enable Flow Monitor", enableFlowMonitor);
   cmd.Parse (argc, argv);
 
   // Here, we will explicitly create four nodes.  In more sophisticated
   // topologies, we could configure a node factory.


   /*

  NAMES

  <HOST>

   c_0 : sugang.cau.ac.kr
   c_1 : Internal Router at Univ
   c_2 : 4PC server
   c_3~22 : Individual 4PC host

  <Connections>

   sugang : c_0 - c_1
   innerUniv : c_1 - c_2
   pc4 : c_2 ~ c_23

   */

   NS_LOG_INFO ("Create nodes.");
   NodeContainer c;
   c.Create (23);
   NodeContainer nc_sugang = NodeContainer (c.Get (0), c.Get (1));
   NodeContainer nc_innerUniv = NodeContainer (c.Get (1), c.Get (2));
   NodeContainer nc_pc4 = NodeContainer (
      c.Get (2), c.Get (3), c.Get (4), c.Get (5), c.Get (6), c.Get (7), c.Get (8), c.Get (9), c.Get (10), 
      c.Get (11), c.Get (12), c.Get (13), c.Get (14), c.Get (15), c.Get (16), c.Get (17), c.Get (18), c.Get (19), c.Get (20),
      c.Get (21), c.Get (22), c.Get (23));

  //  NodeContainer n0n2 = NodeContainer (c.Get (0), c.Get (2));
  //  NodeContainer n1n2 = NodeContainer (c.Get (1), c.Get (2));
  //  NodeContainer n3n2 = NodeContainer (c.Get (3), c.Get (2));
  //  NodeContainer n4n2 = NodeContainer (c.Get (4), c.Get (2));
 
   InternetStackHelper internet;
   internet.Install (c);
 
   // We create the channels first without any IP addressing information
   NS_LOG_INFO ("Create channels.");
   PointToPointHelper p2p;
   CsmaHelper csma;

   //sugang
   p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
   p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
   NetDeviceContainer ndc_sugang = p2p.Install (nc_sugang);
 
   //innerUniv
   p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
   p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
   NetDeviceContainer ndc_innerUniv = p2p.Install (nc_innerUniv);
 
   //pc4
   csma.SetChannelAttribute ("DataRate", StringValue ("1500kbps"));
   csma.SetChannelAttribute ("Delay", StringValue ("2ms"));
   NetDeviceContainer ndc_pc4 = csma.Install (nc_pc4);
 
   // Later, we add IP addresses.
   NS_LOG_INFO ("Assign IP Addresses.");
   Ipv4AddressHelper ipv4;
   ipv4.SetBase ("10.1.1.0", "255.255.255.0");
   Ipv4InterfaceContainer iic_sugang = ipv4.Assign (ndc_sugang);
 
   ipv4.SetBase ("10.1.2.0", "255.255.255.0");
   Ipv4InterfaceContainer iic_innerUniv = ipv4.Assign (ndc_innerUniv);
 
   ipv4.SetBase ("10.4.1.0", "255.255.255.0");
   Ipv4InterfaceContainer iic_pc4 = ipv4.Assign (ndc_pc4);
 
   // Create router nodes, initialize routing database and set up the routing
   // tables in the nodes.
   Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
 
   // Create the OnOff application to send TCP datagrams of size
   // 210 bytes at a rate of 448 Kb/s
   NS_LOG_INFO ("Create Applications.");
   uint16_t port = 9;   // Discard port (RFC 863)

   for (int i = 0; i < 20; i++) 
   {
      OnOffHelper onoff ("ns3::TcpSocketFactory", 
                      Address (InetSocketAddress (iic_pc4.GetAddress (i), port)));
      onoff.SetConstantRate (DataRate ("448kb/s"));
      ApplicationContainer apps = onoff.Install (c.Get (i+3));
      apps.Start (Seconds (1.0));
      apps.Stop (Seconds (10.0));
   }
 
   // Create a packet sink to receive these packets
   PacketSinkHelper sink ("ns3::TcpSocketFactory",
                          Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
   apps = sink.Install (c.Get (0));
   apps.Start (Seconds (1.0));
   apps.Stop (Seconds (10.0));
 
   AsciiTraceHelper ascii;
   p2p.EnableAsciiAll (ascii.CreateFileStream ("cau_4_pc_simulation.tr"));
   p2p.EnablePcapAll ("cau_4_pc_simulation");
 
   // Flow Monitor
   FlowMonitorHelper flowmonHelper;
   if (enableFlowMonitor)
     {
       flowmonHelper.InstallAll ();
     }
 
   NS_LOG_INFO ("Run Simulation.");
   Simulator::Stop (Seconds (11));
   Simulator::Run ();
   NS_LOG_INFO ("Done.");
 
   if (enableFlowMonitor)
     {
       flowmonHelper.SerializeToXmlFile ("cau_4_pc_simulation.flowmon", false, false);
     }
 
   Simulator::Destroy ();
   return 0;
 }