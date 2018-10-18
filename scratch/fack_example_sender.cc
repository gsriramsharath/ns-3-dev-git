/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2018 NITK Surathkal
 *
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
 * Authors: Shikha Bakshi <shikhabakshi912@gmail.com>
 *          Mohit P. Tahiliani <tahiliani@nitk.edu.in>
 */

// The network topology used in this example is based on the Fig. 17 described in
// Mohammad Alizadeh, Albert Greenberg, David A. Maltz, Jitendra Padhye,
// Parveen Patel, Balaji Prabhakar, Sudipta Sengupta, and Murari Sridharan.
// "Data Center TCP (DCTCP)." In ACM SIGCOMM Computer Communication Review,
// Vol. 40, No. 4, pp. 63-74. ACM, 2010.

#include <iostream>
#include <string>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/applications-module.h"
#include "ns3/packet-sink.h"
#include "ns3/traffic-control-module.h"
#include "ns3/log.h"
#include "ns3/random-variable-stream.h"
//#include "ns3/gtk-config-store.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/callback.h"

using namespace ns3;

Ptr<UniformRandomVariable> uv = CreateObject<UniformRandomVariable> ();
std::string dir = "fack/";
double stopTime = 10;

static void
CwndChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
  *stream->GetStream () << Simulator::Now ().GetSeconds () << " " << newCwnd/1000.0 << std::endl;
}

void
TraceCwnd (uint32_t node, uint32_t cwndWindow,
           Callback <void, uint32_t, uint32_t> CwndTrace)
{
  Config::ConnectWithoutContext ("/NodeList/" + std::to_string (node) + "/$ns3::TcpL4Protocol/SocketList/" + std::to_string (cwndWindow) + "/CongestionWindow", CwndTrace);

}

static void
RxDrop (Ptr<const Packet> p)
{
  std::ofstream fPlotQueue (std::stringstream (dir + "app.plotme").str ().c_str (), std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " 1" << std::endl;
}

static void
DropAtQueue (Ptr<OutputStreamWrapper> stream, Ptr<const QueueDiscItem> item)
{
  *stream->GetStream () << Simulator::Now ().GetSeconds () << " 1" << std::endl;
}

void
CheckQueueSize (Ptr<QueueDisc> queue)
{
  uint32_t qSize = queue->GetCurrentSize ().GetValue ();

  // check queue size every 1/100 of a second
  Simulator::Schedule (Seconds (0.001), &CheckQueueSize, queue);

  std::ofstream fPlotQueue (std::stringstream (dir + "queue.plotme").str ().c_str (), std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << qSize << std::endl;
  fPlotQueue.close ();
}

void InstallPacketSink (Ptr<Node> node, uint16_t port)
{
  PacketSinkHelper sink ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer sinkApps = sink.Install (node);
  sinkApps.Start (Seconds (0));
  sinkApps.Stop (Seconds (stopTime));
}

void InstallBulkSend (Ptr<Node> node, Ipv4Address address, uint16_t port, 
                      uint32_t nodeId, uint32_t cwndWindow,
                      Callback <void, uint32_t, uint32_t> CwndTrace)
{

  BulkSendHelper source ("ns3::TcpSocketFactory", 
                         InetSocketAddress (address, port));

  source.SetAttribute ("MaxBytes", UintegerValue (0));
  ApplicationContainer sourceApps = source.Install (node);
  Ptr< Socket > socket = sourceApps.Get (0)->GetObject<BulkSendApplication> ()->GetSocket ();
  Time timeToStart = Seconds (uv->GetValue (0, 1));
  sourceApps.Start (timeToStart);
  Simulator::Schedule (timeToStart + Seconds (0.001), &TraceCwnd, nodeId, cwndWindow, CwndTrace);
  sourceApps.Stop (Seconds (stopTime));
}
int main (int argc, char *argv[])
{
  uint32_t stream = 1;
  std::string transport_prot = "TcpNewReno";
  std::string queue_disc_type = "FifoQueueDisc";
  bool fack = false;
  bool useEcn = true;
  uint32_t dataSize = 1000;
  uint32_t delAckCount = 2;

  time_t rawtime;
  struct tm * timeinfo;
  char buffer[80];

  time (&rawtime);
  timeinfo = localtime(&rawtime);

  strftime(buffer,sizeof(buffer),"%d-%m-%Y-%I-%M-%S",timeinfo);
  std::string currentTime (buffer);

  CommandLine cmd;
  cmd.AddValue ("stream", "Seed value for random variable", stream);
  cmd.AddValue ("transport_prot", "Transport protocol to use: TcpNewReno, "
                "TcpHybla, TcpHighSpeed, TcpHtcp, TcpVegas, TcpScalable, TcpVeno, "
                "TcpBic, TcpYeah, TcpIllinois, TcpWestwood, TcpWestwoodPlus, TcpLedbat, "
                "TcpLp", transport_prot);
  cmd.AddValue ("queue_disc_type", "Queue disc type for gateway (e.g. ns3::CoDelQueueDisc)", queue_disc_type);
  cmd.AddValue ("useEcn", "Use ECN", useEcn);
  cmd.AddValue ("dataSize", "Data packet size", dataSize);
  cmd.AddValue ("delAckCount", "Delayed ack count", delAckCount);
  cmd.AddValue ("stopTime", "Stop time for applications / simulation time will be stopTime", stopTime);
  cmd.AddValue ("fack", "Enable/Disable Fack", fack);
  cmd.Parse (argc,argv);

  uv->SetStream (stream);
  transport_prot = std::string ("ns3::") + transport_prot;
  queue_disc_type = std::string ("ns3::") + queue_disc_type;

  TypeId qdTid;
  NS_ABORT_MSG_UNLESS (TypeId::LookupByNameFailSafe (queue_disc_type, &qdTid), "TypeId " << queue_disc_type << " not found");

  Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpNewReno::GetTypeId()));
  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (dataSize));
  Config::SetDefault ("ns3::TcpSocket::SndBufSize", UintegerValue (1 << 20));
  Config::SetDefault ("ns3::TcpSocket::RcvBufSize", UintegerValue (1 << 20));
  Config::SetDefault ("ns3::TcpSocket::InitialCwnd", UintegerValue (10));
  Config::SetDefault("ns3::TcpSocketBase::Sack",BooleanValue(true));
  Config::SetDefault("ns3::TcpSocketBase::Fack",BooleanValue(fack));
  Config::SetDefault (queue_disc_type + "::MaxSize", QueueSizeValue (QueueSize ("17p")));
  Config::SetDefault ("ns3::BurstErrorModel::ErrorRate", DoubleValue (0.01));
//  Config::SetDefault ("ns3::RateErrorModel::ErrorUnit", StringValue ("ERROR_UNIT_PACKET"));



  AsciiTraceHelper asciiTraceHelper;
  Ptr<OutputStreamWrapper> streamWrapper;

   // Select TCP variant
  if (transport_prot.compare ("ns3::TcpWestwoodPlus") == 0)
    {
      // TcpWestwoodPlus is not an actual TypeId name; we need TcpWestwood here
      Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpWestwood::GetTypeId ()));
      // the default protocol type in ns3::TcpWestwood is WESTWOOD
      Config::SetDefault ("ns3::TcpWestwood::ProtocolType", EnumValue (TcpWestwood::WESTWOODPLUS));
    }
  else
    {
      TypeId tcpTid;
      NS_ABORT_MSG_UNLESS (TypeId::LookupByNameFailSafe (transport_prot, &tcpTid), "TypeId " << transport_prot << " not found");
      Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TypeId::LookupByName (transport_prot)));
    }

  //create nodes
  NodeContainer nodes, routers, receiver;
  nodes.Create(4);
  routers.Create(2);
  receiver.Create(1);

  //Create point-to-point channels
  PointToPointHelper p2pSR1, p2pR, p2pSR2;
  p2pSR1.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
  p2pSR1.SetChannelAttribute ("Delay", StringValue ("2ms"));

  p2pR.SetDeviceAttribute ("DataRate", StringValue ("1.5Mbps"));
  p2pR.SetChannelAttribute ("Delay", StringValue ("5ms"));

  p2pSR2.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
  p2pSR2.SetChannelAttribute ("Delay", StringValue ("33ms"));

  //Create netdevices
  NetDeviceContainer S1R1, R1R2, R2S2;
  S1R1 = p2pSR1.Install(nodes.Get (0), routers.Get (0));
  S1R1.Add(p2pSR1.Install(nodes.Get (1), routers.Get (0)));
  S1R1.Add(p2pSR1.Install(nodes.Get (2), routers.Get (0)));
  S1R1.Add(p2pSR1.Install(nodes.Get (3), routers.Get (0)));
  R1R2 = p2pR.Install(routers.Get (0), routers.Get (1));
  R2S2 = p2pSR2.Install(routers.Get (1), receiver.Get (0));

/*  Ptr<BurstErrorModel> em = CreateObject<BurstErrorModel> ();
  em->SetAttribute ("ErrorRate", DoubleValue (0.5));
  R2S2.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em));*/

  //Install Internet stack on the end nodes
  InternetStackHelper stack;
  stack.Install (nodes);
  stack.Install (routers);
  stack.Install (receiver);

  //Assign IP addresses
  Ipv4AddressHelper address;
  Ipv4InterfaceContainer S1Int, S2Int, routerInt;

  address.SetBase("10.1.0.0", "255.255.255.0");
  
 for(uint32_t i=0; i<S1R1.GetN (); i = i+2)
  {
     address.NewNetwork ();
     NetDeviceContainer SiR1;
     SiR1.Add(S1R1.Get (i));
     SiR1.Add(S1R1.Get (i+1));
     S1Int.Add (address.Assign (SiR1));
  }
  address.SetBase("10.2.0.0", "255.255.255.252");
  routerInt = address.Assign(R1R2);
  address.SetBase("10.3.0.0", "255.255.255.252");
  S2Int = address.Assign(R2S2);

  dir += (currentTime + "/");
  std::string dirToSave = "mkdir -p " + dir;
  system (dirToSave.c_str ());
  system ((dirToSave + "/cwndTraces/").c_str ());
  system ((dirToSave + "/queueTraces/").c_str ());

//  p2pSR1.EnablePcapAll ();

  //Set Queue Disc
  TrafficControlHelper tch;
  tch.SetRootQueueDisc (queue_disc_type);
  
  QueueDiscContainer qd;
  tch.Uninstall (R1R2);
  qd = tch.Install (R1R2);
  Simulator::ScheduleNow (&CheckQueueSize, qd.Get (0));

  streamWrapper = asciiTraceHelper.CreateFileStream (dir + "queueTraces/drop.plotme");
  qd.Get (0)->TraceConnectWithoutContext ("Drop", MakeBoundCallback (&DropAtQueue, streamWrapper));

  uint16_t port = 50000;

  //Install Sink application
   for(uint32_t i=0; i<nodes.GetN ()-3; i++)
    {
      InstallPacketSink (receiver.Get (0), port+i);
    }
  //Install BulkSend applications on S1 and S3
  for (uint32_t i = 0; i < nodes.GetN ()-3; i++)
    {
      Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream (dir + "cwndTraces/S" + std::to_string (i+1) + ".plotme");
      InstallBulkSend (nodes.Get (i), S2Int.GetAddress (1), port + i, i, 0, MakeBoundCallback (&CwndChange, stream));
    }
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  R2S2.Get (1)->TraceConnectWithoutContext ("PhyRxDrop", MakeCallback (&RxDrop));


  Simulator::Stop (Seconds (stopTime));
  Simulator::Run ();

  Simulator::Destroy ();
  return 0;
}

 
  

  



