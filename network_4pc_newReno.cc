/*
@Copyrighted by CAUCSE-Team5.2
@author : 강동원, 공찬형, 금강현, 이동현
@date : 2019.06.06
@GNU-Licence
*/
 
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
 #if 0 
   LogComponentEnable ("SimpleGlobalRoutingExample", LOG_LEVEL_INFO);
 #endif
 
   // Set up some default values for the simulation.  Use the 
   Config::SetDefault ("ns3::OnOffApplication::PacketSize", UintegerValue (210));
   Config::SetDefault ("ns3::OnOffApplication::DataRate", StringValue ("448kb/s"));
   //Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue ("ns3::TcpNewReno"));
   //DefaultValue::Bind ("DropTailQueue::m_maxPackets", 30);
 
   // Allow the user to override any of the defaults and the above
   // DefaultValue::Bind ()s at run-time, via command-line arguments
   CommandLine cmd;
   bool enableFlowMonitor = false;
   cmd.AddValue ("EnableMonitor", "Enable Flow Monitor", enableFlowMonitor);
   cmd.Parse (argc, argv);

   /*

  <HOST>

   c_0 : sugang.cau.ac.kr
   c_1 : Internal Router at Univ
   c_2 : 4PC server
   c_3~22 : Individual 4PC host
   c_23 : outer gateway
   c_24 : univ_base_route_1
   c_25 : univ_base_route_2
   c_26 : univ_base_route_3
   c_27 : base_station
   c_28 : pc_central
   c_29~32 : pc_1 ~ pc_4
   c_33 : home_base_route_1
   c_34 : home_base_route_2
   c_35 : home_base_route_3
   c_36 : home

  <Connections>

   sugang : c_0 - c_1
   innerUniv : c_1 - c_2
   pc4 : c_2 ~ c_22
   outer_gateway : c_1 - c_23
   univ_base_route_1 : c_23 - c_24
   univ_base_route_2 : c_24 - c_25
   univ_base_route_3 : c_25 - c_26
   univ_base_route_4 : c_26 - c_27
   pc_base_route : c_27 - c_28
   pc_inner_route_1 : c_28 - c_29
   pc_inner_route_2 : c_28 - c_30
   pc_inner_route_3 : c_28 - c_31
   pc_inner_route_4 : c_28 - c_32
   home_base_route_1 : c_27 - c_33
   home_base_route_2 : c_33 - c_34
   home_base_route_3 : c_34 - c_35
   home_base_route_4 : c_35 - c_36

   */

   NS_LOG_INFO ("Create nodes.");
   NodeContainer c;
   c.Create (37);
   NodeContainer nc_sugang = NodeContainer (c.Get (0), c.Get (1));
   NodeContainer nc_innerUniv = NodeContainer (c.Get (1), c.Get (2));
   NodeContainer nc_pc4 = NodeContainer ();
   NodeContainer nc_outer_gateway = NodeContainer (c.Get (1), c.Get (23));
   NodeContainer nc_univ_base_route_1 = NodeContainer (c.Get (23), c.Get (24));
   NodeContainer nc_univ_base_route_2 = NodeContainer (c.Get (24), c.Get (25));
   NodeContainer nc_univ_base_route_3 = NodeContainer (c.Get (25), c.Get (26));
   NodeContainer nc_univ_base_route_4 = NodeContainer (c.Get (26), c.Get (27));
   NodeContainer nc_pc_base_route = NodeContainer (c.Get (27), c.Get (28));
   NodeContainer nc_pc_inner_route_1 = NodeContainer (c.Get (28), c.Get (29));
   NodeContainer nc_pc_inner_route_2 = NodeContainer (c.Get (28), c.Get (30));
   NodeContainer nc_pc_inner_route_3 = NodeContainer (c.Get (28), c.Get (31));
   NodeContainer nc_pc_inner_route_4 = NodeContainer (c.Get (28), c.Get (32));
   NodeContainer nc_home_base_route_1 = NodeContainer (c.Get (27), c.Get (33));
   NodeContainer nc_home_base_route_2 = NodeContainer (c.Get (33), c.Get (34));
   NodeContainer nc_home_base_route_3 = NodeContainer (c.Get (34), c.Get (35));
   NodeContainer nc_home_base_route_4 = NodeContainer (c.Get (35), c.Get (36));
   

   for(int i = 0; i < 21; i++) {
     nc_pc4.Add (c.Get(i+2));
   }
 
   InternetStackHelper internet;
   internet.Install (c);
 
   // We create the channels first without any IP addressing information
   NS_LOG_INFO ("Create channels.");
   PointToPointHelper p2p;
   CsmaHelper csma;

   //sugang
   p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
   p2p.SetChannelAttribute ("Delay", StringValue ("0.5ms"));
   NetDeviceContainer ndc_sugang = p2p.Install (nc_sugang);
 
   //innerUniv
   p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
   p2p.SetChannelAttribute ("Delay", StringValue ("0.5ms"));
   NetDeviceContainer ndc_innerUniv = p2p.Install (nc_innerUniv);
 
   //pc4
   csma.SetChannelAttribute ("DataRate", StringValue ("1500kbps"));
   csma.SetChannelAttribute ("Delay", StringValue ("0.5ms"));
   NetDeviceContainer ndc_pc4 = csma.Install (nc_pc4);

   //outer gateway
   p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
   p2p.SetChannelAttribute ("Delay", StringValue ("0.5ms"));
   NetDeviceContainer ndc_outer_gateway = p2p.Install (nc_outer_gateway);

   //univ_base_route_1~4
   p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
   p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
   NetDeviceContainer ndc_univ_base_route_1 = p2p.Install (nc_univ_base_route_1);
   NetDeviceContainer ndc_univ_base_route_2 = p2p.Install (nc_univ_base_route_2);
   NetDeviceContainer ndc_univ_base_route_3 = p2p.Install (nc_univ_base_route_3);
   NetDeviceContainer ndc_univ_base_route_4 = p2p.Install (nc_univ_base_route_4); // must have higher specs.

   //pc_base_route
   p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
   p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
   NetDeviceContainer ndc_pc_base_route = p2p.Install (nc_pc_base_route); // must have higher specs.

   //pc_inner_route
   p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
   p2p.SetChannelAttribute ("Delay", StringValue ("0.5ms"));
   NetDeviceContainer ndc_pc_inner_route_1 = p2p.Install (nc_pc_inner_route_1); // must have higher specs.
   NetDeviceContainer ndc_pc_inner_route_2 = p2p.Install (nc_pc_inner_route_2);
   NetDeviceContainer ndc_pc_inner_route_3 = p2p.Install (nc_pc_inner_route_3);
   NetDeviceContainer ndc_pc_inner_route_4 = p2p.Install (nc_pc_inner_route_4);

   //home_base_route
   p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
   p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
   NetDeviceContainer ndc_home_base_route_1 = p2p.Install (nc_home_base_route_1);
   NetDeviceContainer ndc_home_base_route_2 = p2p.Install (nc_home_base_route_2);
   NetDeviceContainer ndc_home_base_route_3 = p2p.Install (nc_home_base_route_3);
   NetDeviceContainer ndc_home_base_route_4 = p2p.Install (nc_home_base_route_4);

   // Later, we add IP addresses.
   NS_LOG_INFO ("Assign IP Addresses.");
   Ipv4AddressHelper ipv4;
   ipv4.SetBase ("165.192.200.0", "255.255.255.0");
   Ipv4InterfaceContainer iic_sugang = ipv4.Assign (ndc_sugang);
 
   ipv4.SetBase ("165.192.1.0", "255.255.255.0");
   Ipv4InterfaceContainer iic_innerUniv = ipv4.Assign (ndc_innerUniv);

   ipv4.SetBase ("165.192.254.0", "255.255.255.0");
   Ipv4InterfaceContainer iic_outer_gateway = ipv4.Assign (ndc_outer_gateway);
 
   ipv4.SetBase ("165.192.104.0", "255.255.255.0");
   Ipv4InterfaceContainer iic_pc4 = ipv4.Assign (ndc_pc4);

   ipv4.SetBase ("10.1.1.0", "255.255.255.0");
   Ipv4InterfaceContainer iic_univ_base_route_1 = ipv4.Assign (ndc_univ_base_route_1);

   ipv4.SetBase ("10.1.2.0", "255.255.255.0");
   Ipv4InterfaceContainer iic_univ_base_route_2 = ipv4.Assign (ndc_univ_base_route_2);

   ipv4.SetBase ("10.1.3.0", "255.255.255.0");
   Ipv4InterfaceContainer iic_univ_base_route_3 = ipv4.Assign (ndc_univ_base_route_3);

   ipv4.SetBase ("10.1.4.0", "255.255.255.0");
   Ipv4InterfaceContainer iic_univ_base_route_4 = ipv4.Assign (ndc_univ_base_route_4);

   ipv4.SetBase ("10.2.1.0", "255.255.255.0");
   Ipv4InterfaceContainer iic_pc_base_route = ipv4.Assign (ndc_pc_base_route);
   
   ipv4.SetBase ("192.168.151.0", "255.255.255.0");
   Ipv4InterfaceContainer iic_pc_inner_route_1 = ipv4.Assign (ndc_pc_inner_route_1);
   Ipv4InterfaceContainer iic_pc_inner_route_2 = ipv4.Assign (ndc_pc_inner_route_2);
   Ipv4InterfaceContainer iic_pc_inner_route_3 = ipv4.Assign (ndc_pc_inner_route_3);
   Ipv4InterfaceContainer iic_pc_inner_route_4 = ipv4.Assign (ndc_pc_inner_route_4);

   ipv4.SetBase ("10.3.1.0", "255.255.255.0");
   Ipv4InterfaceContainer iic_home_base_route_1 = ipv4.Assign (ndc_home_base_route_1);

   ipv4.SetBase ("10.3.2.0", "255.255.255.0");
   Ipv4InterfaceContainer iic_home_base_route_2 = ipv4.Assign (ndc_home_base_route_2);

   ipv4.SetBase ("10.3.3.0", "255.255.255.0");
   Ipv4InterfaceContainer iic_home_base_route_3 = ipv4.Assign (ndc_home_base_route_3);

   ipv4.SetBase ("192.168.152.0", "255.255.255.0");
   Ipv4InterfaceContainer iic_home_base_route_4 = ipv4.Assign (ndc_home_base_route_4);
 

 
   // Create router nodes, initialize routing database and set up the routing
   // tables in the nodes.
   Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
 
   // Create the OnOff application to send TCP datagrams of size
   // 210 bytes at a rate of 448 Kb/s
   NS_LOG_INFO ("Create Applications.");
   uint16_t port = 9;   // Discard port (RFC 863)


   //4PC_room
   for (int i = 0; i < 20; i++) 
   {
      OnOffHelper onoff ("ns3::TcpSocketFactory", 
                      Address (InetSocketAddress (iic_sugang.GetAddress (0), port)));
      onoff.SetConstantRate (DataRate ("448kb/s"));
      ApplicationContainer apps = onoff.Install (c.Get (i+3));
      apps.Start (Seconds (1.0));
      apps.Stop (Seconds (100.0));
   }

   //PC_room
   for (int i = 29; i < 33; i++) 
   {
      OnOffHelper onoff ("ns3::TcpSocketFactory", 
                      Address (InetSocketAddress (iic_sugang.GetAddress (0), port)));
      onoff.SetConstantRate (DataRate ("448kb/s"));
      ApplicationContainer apps = onoff.Install (c.Get (i));
      apps.Start (Seconds (1.0));
      apps.Stop (Seconds (10.0));
   }

   //Home
   OnOffHelper onoff ("ns3::TcpSocketFactory", 
                      Address (InetSocketAddress (iic_sugang.GetAddress (0), port)));
   onoff.SetConstantRate (DataRate ("448kb/s"));
   ApplicationContainer apps = onoff.Install (c.Get (36));
   apps.Start (Seconds (1.0));
   apps.Stop (Seconds (10.0));
 
   // Create a packet sink to receive these packets
   PacketSinkHelper sink ("ns3::TcpSocketFactory",
                          Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
   ApplicationContainer apps_sink = sink.Install (c.Get (0));
   apps_sink.Start (Seconds (1.0));
   apps_sink.Stop (Seconds (10.0));
 
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