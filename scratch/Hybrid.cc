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
 * Authors: Shefali Gupta <shefaligups11@gmail.com>
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
#include "ns3/callback.h"

using namespace ns3;

std::vector<std::stringstream> filePlotQueue;
Ptr<UniformRandomVariable> uv = CreateObject<UniformRandomVariable> ();
std::string dir = "results/Hybrid/";
double stopTime = 10;



void
CheckQueueSize (Ptr<QueueDisc> queue,Ptr<OutputStreamWrapper> stream)
{
  uint32_t qSize = queue->GetCurrentSize ().GetValue ();

  *stream->GetStream () << Simulator::Now ().GetSeconds () << " " << qSize << std::endl;
  // check queue size every 1/100 of a second
  Simulator::Schedule (Seconds (0.001), &CheckQueueSize, queue,stream);
 
}

static void
CwndChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
  *stream->GetStream () << Simulator::Now ().GetSeconds () << " " << newCwnd / 1446.0 << std::endl;
}

static void
DropAtQueue (Ptr<OutputStreamWrapper> stream, Ptr<const QueueDiscItem> item)
{
  *stream->GetStream () << Simulator::Now ().GetSeconds () << " 1" << std::endl;
}

static void
MarkAtQueue (Ptr<OutputStreamWrapper> stream, Ptr<const QueueDiscItem> item, const char* reason)
{
  *stream->GetStream () << Simulator::Now ().GetSeconds () << " 1" << std::endl;
}

void
TraceCwnd (uint32_t node, uint32_t cwndWindow,
           Callback <void, uint32_t, uint32_t> CwndTrace)
{
  Config::ConnectWithoutContext ("/NodeList/" + std::to_string (node) + "/$ns3::TcpL4Protocol/SocketList/" + std::to_string (cwndWindow) + "/CongestionWindow", CwndTrace);
}

void
ProbChange0 (double oldP, double newP)
{
  std::ofstream fPlotQueue (dir + "ProbTraces/T1.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newP << std::endl;
  fPlotQueue.close ();
}
void
ProbChange1 (double oldP, double newP)
{
  std::ofstream fPlotQueue (dir + "ProbTraces/T2.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newP << std::endl;
  fPlotQueue.close ();
}
void
ProbChange2 (double oldP, double newP)
{
  std::ofstream fPlotQueue (dir + "ProbTraces/Scorp.plotme", std::ios::out | std::ios::app);
  fPlotQueue << Simulator::Now ().GetSeconds () << " " << newP << std::endl;
  fPlotQueue.close ();
}
void
TraceProb (uint32_t node, uint32_t probability,
           Callback <void, double, double> ProbTrace)
{
  Config::ConnectWithoutContext ("$ns3::NodeListPriv/NodeList/" + std::to_string (node) + "/$ns3::TrafficControlLayer/RootQueueDiscList/" + std::to_string (probability) + "/$ns3::PiQueueDisc/Probability", ProbTrace);
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
  Time timeToStart = Seconds (uv->GetValue (0, 1));
  sourceApps.Start (timeToStart);
  Simulator::Schedule (timeToStart + Seconds (1.0), &TraceCwnd, nodeId, cwndWindow, CwndTrace);
  sourceApps.Stop (Seconds (stopTime));
}



int main (int argc, char *argv[])
{
  uint32_t stream = 1;
  std::string transport_prot = "TcpNewReno";
  std::string queue_disc_type = "FifoQueueDisc";
  bool useEcn = true;
  uint32_t dataSize = 1446;
  uint32_t delAckCount = 2;

  time_t rawtime;
  struct tm * timeinfo;
  char buffer[80];

  time (&rawtime);
  timeinfo = localtime (&rawtime);

  strftime (buffer,sizeof(buffer),"%d-%m-%Y-%I-%M-%S",timeinfo);
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
  cmd.Parse (argc,argv);

  uv->SetStream (stream);
  transport_prot = std::string ("ns3::") + transport_prot;
  queue_disc_type = std::string ("ns3::") + queue_disc_type;

  TypeId qdTid;
  NS_ABORT_MSG_UNLESS (TypeId::LookupByNameFailSafe (queue_disc_type, &qdTid), "TypeId " << queue_disc_type << " not found");

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

  //G1-------G2-----G3------G4------G5------G6--------G7-----------G8----------G9
  //0        1       2       3       4       5         6            7           8
 
 
  NodeContainer G;

  G.Create (9);                                                    //Routers corresponding to whole topology

  std::vector<PointToPointHelper> trunkLink;

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("10000Mbps"));                                       //G0---G1
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("0.05ms"));
  trunkLink.push_back (pointToPoint);

  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("10000Mbps"));                                       //G1---G2
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("0.05ms"));
  trunkLink.push_back (pointToPoint);


  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("1000Mbps"));                                       //G2---G3
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("0.05ms"));
  trunkLink.push_back (pointToPoint);


  pointToPoint.SetDeviceAttribute  ("DataRate", StringValue ("150Mbps"));                                       //G4---G5
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("0.25ms"));
  trunkLink.push_back (pointToPoint);


  pointToPoint.SetDeviceAttribute  ("DataRate", StringValue ("50Mbps"));                                       //G5---G6
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("0.25ms"));
  trunkLink.push_back (pointToPoint);


  pointToPoint.SetDeviceAttribute  ("DataRate", StringValue ("150Mbps"));                                       //G6---G7
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("0.25ms"));
  trunkLink.push_back (pointToPoint);

  pointToPoint.SetDeviceAttribute  ("DataRate", StringValue ("150Mbps"));                                       //G7---G8
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("0.25ms"));
  trunkLink.push_back (pointToPoint);

  pointToPoint.SetDeviceAttribute  ("DataRate", StringValue ("100Mbps"));                                       //G8---G9
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("0.25ms"));
  trunkLink.push_back (pointToPoint);

  //Install pointToPointHelper on all router links and push it to NetDeviceContainer vetcor
  std::vector<NetDeviceContainer> m_switchDevices;
  for (uint32_t i = 0; i < G.GetN () - 1; ++i)
    {
      NetDeviceContainer c = trunkLink [i].Install (G.Get (i), G.Get (i + 1));
      m_switchDevices.push_back (c);
    } 


  NodeContainer S1,S2,S3,R1,R2,R3,RG2,RG1,RG3,SB,RB,SC,RC,SD,RD,SE,RE,SF,RF,RGf2,RGf1,RGf3;

  S1.Create (10);
  S2.Create (20);
  S3.Create (10);

  R2.Create (10);                                                    //receiver for S2
  R1.Create (1);                                                     //receiver for S1
  R3.Create (1);                                                     //receiver for S3

  RGf2.Create (1);                                                   //reciver for s2
  RGf1.Create (1);                                                   //reciver for s1
  RGf3.Create (1);                                                   //reciver for s3

  SB.Create (1);                                                     //sender B
  RB.Create (1);                                                     //Reciever B

  SC.Create (1);                                                     //Sender C
  RC.Create (1);                                                     //Reciever C

  SD.Create (1);                                                     //Sender D
  RD.Create (1);                                                     //Reciever D

  SE.Create (1);                                                     //Sender E
  RE.Create (1);                                                     //Reciever E

  SF.Create (1);                                                     //Sender F
  RF.Create (1);                                                     //Reciever F

  PointToPointHelper pointToPointSR;                                                       //for links corresponding to Datacenter Topo
  pointToPointSR.SetDeviceAttribute ("DataRate", StringValue ("1000Mbps"));
  pointToPointSR.SetChannelAttribute ("Delay", StringValue ("0.05ms"));

  PointToPointHelper pointToPointLeaf;                                                     //for links corresponding to gfc-1 Topo
  pointToPointLeaf.SetDeviceAttribute ("DataRate", StringValue ("150Mbps"));
  pointToPointLeaf.SetChannelAttribute   ("Delay", StringValue ("0.001ms"));
 

  NetDeviceContainer S1G0Dev,S2G0Dev, S3G2Dev,R2G2Dev,R1G3Dev,R3G3Dev,RGf2G4Dev, SDG4Dev, SBG5Dev, RDG5Dev, SFG5Dev, RFG6Dev, SCG6Dev, RGf3G7Dev, RCG7Dev, SEG7Dev, RGf1G8Dev, RBG8Dev, REG8Dev;


  //NetDeviceContainer for S1 and G0 (sender S1 and 0 router)
  for (uint32_t i = 0; i < S1.GetN (); i++)
    {
      S1G0Dev.Add (pointToPointSR.Install (S1.Get (i), G.Get (0)));
    }

  //NetDeviceContainer for S1 and G0 (sender  S2 and 0 router)
  for (uint32_t i = 0; i < S2.GetN (); i++)
    {
      S2G0Dev.Add (pointToPointSR.Install (S2.Get (i), G.Get (0)));
    }


  //NetDeviceContainer for S3 and G2 (sender  S3 and 2 router) 
  for (uint32_t i = 0; i < S3.GetN (); i++)
    {
      S3G2Dev.Add (pointToPointSR.Install (S3.Get (i), G.Get (2)));
    }


  //NetDeviceContainer for R2 and G2 (Reciever  R2 and 2 router)
  for (uint32_t i = 0; i < R2.GetN (); i++)
    {
      R2G2Dev.Add (pointToPointSR.Install (R2.Get (i), G.Get (2)));
    }

 
  //NetDeviceContainer for R1 and G3 (Reciever  R1 and 3 router)
  R1G3Dev.Add (pointToPointSR.Install (R1.Get (0), G.Get (3)));
 
 
  //NetDeviceContainer for R3 and G3 (Reciever  R3 and 3 router)
  R3G3Dev.Add (pointToPointSR.Install (R3.Get (0), G.Get (3)));
   
  //NetDeviceContainer for RGf2 and G4 (Reciever  RGf2 and 4 router)

  RGf2G4Dev.Add (pointToPointSR.Install (RGf2.Get (0), G.Get (4)));
 
  //NetDeviceContainer for R2 and G2 (Reciever  R2 and 2 router)
  SDG4Dev.Add (pointToPointLeaf.Install (SD.Get (0), G.Get (4)));
 
  //NetDeviceContainer for SB and G5 (Sender B and 5 router)
  SBG5Dev.Add (pointToPointLeaf.Install (SB.Get (0), G.Get (5)));
 
  //NetDeviceContainer for RD and G5 (Reciever D and 5 router)
  RDG5Dev.Add (pointToPointLeaf.Install (RD.Get (0), G.Get (5)));
 
  //NetDeviceContainer for SF and G5 (Sender F and 5 router)
  SFG5Dev.Add (pointToPointLeaf.Install (SF.Get (0), G.Get (5)));
 
  //NetDeviceContainer for RF and G6 (Reciever G and 6 router)
  RFG6Dev.Add (pointToPointLeaf.Install (RF.Get (0), G.Get (6)));
 
  //NetDeviceContainer for SC and G6 (Sender C and 6 router)
  SCG6Dev.Add (pointToPointLeaf.Install (SC.Get (0), G.Get (6)));
 
  //NetDeviceContainer for RGf3 and G7 (Reciever for S3 and 7 router)
  RGf3G7Dev.Add (pointToPointSR.Install (RGf3.Get (0), G.Get (7)));
 
  //NetDeviceContainer for RC and G7 (Reciever C and 7 router)
  RCG7Dev.Add (pointToPointLeaf.Install (RC.Get (0), G.Get (7)));
 
  //NetDeviceContainer for SE and G7 (Sender E and 7 router)
  SEG7Dev.Add (pointToPointLeaf.Install (SE.Get (0), G.Get (7)));
 
  //NetDeviceContainer for RGf1 and G8 (Reciever for S1 and 8 router)
  RGf1G8Dev.Add (pointToPointSR.Install (RGf1.Get (0), G.Get (8)));
 
  //NetDeviceContainer for RB and G8 (Reciever B and 8 router)
  RBG8Dev.Add (pointToPointLeaf.Install (RB.Get (0), G.Get (8)));
 
  //NetDeviceContainer for RE and G8 (Reciever E and 8 router)
  REG8Dev.Add (pointToPointLeaf.Install (RE.Get (0), G.Get (8)));
 
  InternetStackHelper stack;

  stack.InstallAll ();	

  Ipv4AddressHelper address;
  Ipv4InterfaceContainer m_switchInt, S1Int, S2Int, S3Int, R2Int, RGf2G4Int,R1G3Int,R3G3Int, SDG4Int, SBG5Int, RDG5Int, SFG5Int, RFG6Int, SCS6Int, RGf3G7Int, RCG7Int, SEG7Int, RGf1G8Int, RBG8Int, REG8Int;
 
  address.SetBase ("10.0.0.0", "255.255.255.0");
 
  //Assign Ip address to all trunkLink
  for (uint32_t i = 0; i < m_switchDevices.size (); i++)
    {
      address.NewNetwork ();
      NetDeviceContainer S1iT1i;
      S1iT1i.Add (m_switchDevices [i].Get (0));
      S1iT1i.Add (m_switchDevices [i].Get (1));
      m_switchInt.Add (address.Assign (S1iT1i));
    }

  address.SetBase ("10.1.0.0", "255.255.255.0");

  //Assign Ip address for S1 and G0 links NIC
  for (uint32_t i = 0; i < S1G0Dev.GetN (); i = i + 2)
    {
      address.NewNetwork ();
      NetDeviceContainer S1iT1i;
      S1iT1i.Add (S1G0Dev.Get (i));
      S1iT1i.Add (S1G0Dev.Get (i + 1));
      S1Int.Add (address.Assign (S1iT1i));
    }
  address.SetBase ("10.2.0.0", "255.255.255.0");

  //Assign Ip address for S2 and G0 links NIC
  for (uint32_t i = 0; i < S2G0Dev.GetN (); i = i + 2)
    {
      address.NewNetwork ();
      NetDeviceContainer S2iT1i;
      S2iT1i.Add (S2G0Dev.Get (i));
      S2iT1i.Add (S2G0Dev.Get (i + 1));
      S2Int.Add (address.Assign (S2iT1i));
    }

  address.SetBase ("10.3.0.0", "255.255.255.0");

  //Assign Ip address for S3 and G2 links NIC
  for (uint32_t i = 0; i < S3G2Dev.GetN (); i = i + 2)
    {
      address.NewNetwork ();
      NetDeviceContainer S3iT2i;
      S3iT2i.Add (S3G2Dev.Get (i));
      S3iT2i.Add (S3G2Dev.Get (i + 1));
      S3Int.Add (address.Assign (S3iT2i));
    }

  address.SetBase ("10.4.0.0", "255.255.255.0");

  //Assign Ip address for R2 and G2 links NIC
  for (uint32_t i = 0; i < R2G2Dev.GetN (); i = i + 2)
    {
      address.NewNetwork ();
      NetDeviceContainer R2iT2i;
      R2iT2i.Add (R2G2Dev.Get (i));
      R2iT2i.Add (R2G2Dev.Get (i + 1));
      R2Int.Add (address.Assign (R2iT2i));
    }

  address.SetBase ("10.6.0.0", "255.255.255.0");

  //Assign Ip address for RGf2 and G4 links NIC
  RGf2G4Int = address.Assign (RGf2G4Dev);

  address.SetBase ("10.7.0.0", "255.255.255.0");
  //Assign Ip address for R1 and G3 links NIC
  R1G3Int = address.Assign (R1G3Dev);

  address.SetBase ("10.8.0.0", "255.255.255.0");
  //Assign Ip address for R3 and G3 links NIC
  R3G3Int = address.Assign (R3G3Dev);

  address.SetBase ("10.9.0.0", "255.255.255.0");
  //Assign Ip address for SD and G4 links NIC
  SDG4Int = address.Assign (SDG4Dev);

  address.SetBase ("10.10.0.0", "255.255.255.0");
  //Assign Ip address for SB and G5 links NIC
  SBG5Int = address.Assign (SBG5Dev);

  address.SetBase ("10.11.0.0", "255.255.255.0");
  //Assign Ip address for RD and G5 links NIC
  RDG5Int = address.Assign (RDG5Dev);

  address.SetBase ("10.12.0.0", "255.255.255.0");
  //Assign Ip address for SF and G5 links NIC
  SFG5Int = address.Assign (SFG5Dev);

  address.SetBase ("10.13.0.0", "255.255.255.0");
  //Assign Ip address for RF and G6 links NIC
  RFG6Int = address.Assign (RFG6Dev);

  address.SetBase ("10.14.0.0", "255.255.255.0");
  //Assign Ip address for SC and G6 links NIC
  SCS6Int = address.Assign (SCG6Dev);

  address.SetBase ("10.15.0.0", "255.255.255.0");
  //Assign Ip address for RGf3 and G7 links NIC
  RGf3G7Int = address.Assign (RGf3G7Dev);

  address.SetBase ("10.16.0.0", "255.255.255.0");
  //Assign Ip address for RC and G7 links NIC
  RCG7Int = address.Assign (RCG7Dev);

  address.SetBase ("10.17.0.0", "255.255.255.0");
  //Assign Ip address for SE and G7 links NIC
  SEG7Int = address.Assign (SEG7Dev);

  address.SetBase ("10.18.0.0", "255.255.255.0");
  //Assign Ip address for RGf1 and G8 links NIC
  RGf1G8Int = address.Assign (RGf1G8Dev);

  address.SetBase ("10.19.0.0", "255.255.255.0");
  //Assign Ip address for RB and G8 links NIC
  RBG8Int = address.Assign (RBG8Dev);

  address.SetBase ("10.20.0.0", "255.255.255.0");
  //Assign Ip address for RE and G8 links NIC
  REG8Int = address.Assign (REG8Dev);
  
  Config::SetDefault ("ns3::TcpSocket::SndBufSize", UintegerValue (1 << 20));
  Config::SetDefault ("ns3::TcpSocket::RcvBufSize", UintegerValue (1 << 20));
  Config::SetDefault ("ns3::TcpSocket::InitialCwnd", UintegerValue (10));
  Config::SetDefault ("ns3::TcpSocket::DelAckCount", UintegerValue (delAckCount));
  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (dataSize));
  Config::SetDefault ("ns3::TcpSocketBase::UseEcn", BooleanValue (useEcn));
  Config::SetDefault ("ns3::PiQueueDisc::UseEcn", BooleanValue (useEcn));
  Config::SetDefault ("ns3::PiQueueDisc::MeanPktSize", UintegerValue (1500));
  Config::SetDefault ("ns3::PiQueueDisc::A", DoubleValue ( 0.00007477268187));
  Config::SetDefault ("ns3::PiQueueDisc::B", DoubleValue ( 0.00006680872759));
  Config::SetDefault ("ns3::PiQueueDisc::W", DoubleValue (4000));
  Config::SetDefault ("ns3::PiQueueDisc::QueueRef", DoubleValue (50));
  Config::SetDefault (queue_disc_type + "::MaxSize", QueueSizeValue (QueueSize ("666p")));

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  dir += (currentTime + "/");
  std::string dirToSave = "mkdir -p " + dir;
  system (dirToSave.c_str ());
  system ((dirToSave + "/pcap/").c_str ());
  system ((dirToSave + "/cwndTraces/").c_str ());
  system ((dirToSave + "/ProbTraces/").c_str ());
  system ((dirToSave + "/markTraces/").c_str ());
  system ((dirToSave + "/queueTraces/").c_str ());
  system (("cp -R plotScript-Hybrid/* " + dir + "/pcap/").c_str ());

  pointToPointSR.EnablePcapAll (dir + "pcap/N", true);

  AsciiTraceHelper asciiTraceHelper;
  Ptr<OutputStreamWrapper> streamWrapper;

  TrafficControlHelper tch;
  // Set root queue_disc
  tch.SetRootQueueDisc (queue_disc_type);

  QueueDiscContainer qdTrunk;
  // Assign root queue_disc to outgoing trunkLink NIC 
  for (uint32_t i = 0; i < m_switchDevices.size (); i++)
    {
      tch.Uninstall (m_switchDevices [i].Get (0));
      qdTrunk.Add (tch.Install (m_switchDevices [i].Get (0)));

      Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream (dir + "queueTraces/queue-" + std::to_string (i + 1) + ".plotme");
      Simulator::ScheduleNow (&CheckQueueSize, qdTrunk.Get (i),stream);
      //Simulator::Schedule (Seconds (2.0), &TraceProb, 0, 0, MakeCallback (&ProbChange0));
      streamWrapper = asciiTraceHelper.CreateFileStream (dir + "/queueTraces/drop-" + std::to_string (i + 1) + ".plotme");
      qdTrunk.Get (i)->TraceConnectWithoutContext ("Drop", MakeBoundCallback (&DropAtQueue, streamWrapper));
      streamWrapper = asciiTraceHelper.CreateFileStream (dir + "/queueTraces/mark" + std::to_string (i + 1) + ".plotme");
      qdTrunk.Get (i)->TraceConnectWithoutContext ("Mark", MakeBoundCallback (&MarkAtQueue, streamWrapper));
    }


  QueueDiscContainer qdR2;
  // Assign root queue_disc to outgoing G2--R2 Link NIC
  for (uint32_t i = 1,j = 0; i < R2G2Dev.GetN (); i = i + 2,j++)
    {
      tch.Uninstall (R2G2Dev.Get (i));
      qdR2.Add (tch.Install (R2G2Dev.Get (i)));

      Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream (dir + "queueTraces/queue-R2-" + std::to_string (j + 1) + ".plotme");
      Simulator::ScheduleNow (&CheckQueueSize, qdR2.Get (j),stream);
      //Simulator::Schedule (Seconds (2.0), &TraceProb, 2, 0, MakeCallback (&ProbChange2));
      streamWrapper = asciiTraceHelper.CreateFileStream (dir + "/queueTraces/drop-R2-" + std::to_string (j + 1) + ".plotme");
      qdR2.Get (j)->TraceConnectWithoutContext ("Drop", MakeBoundCallback (&DropAtQueue, streamWrapper));
      streamWrapper = asciiTraceHelper.CreateFileStream (dir + "/queueTraces/mark-R2-" + std::to_string (j + 1) + ".plotme");
      qdR2.Get (j)->TraceConnectWithoutContext ("Mark", MakeBoundCallback (&MarkAtQueue, streamWrapper));
    }

  QueueDiscContainer qdR3;
  // Assign root queue_disc to outgoing G3--R3 Link NIC
  tch.Uninstall (R3G3Dev);
  qdR3 = tch.Install (R3G3Dev);
  Ptr<OutputStreamWrapper> streamR3 = asciiTraceHelper.CreateFileStream (dir + "queueTraces/queue-R3-" + std::to_string (1) + ".plotme");
  Simulator::ScheduleNow (&CheckQueueSize, qdR3.Get (1),streamR3);
  //Simulator::Schedule (Seconds (2.0), &TraceProb, 1, 1, MakeCallback (&ProbChange1));
  streamWrapper = asciiTraceHelper.CreateFileStream (dir + "/queueTraces/drop-R3-" + std::to_string (1) + ".plotme");
  qdR3.Get (1)->TraceConnectWithoutContext ("Drop", MakeBoundCallback (&DropAtQueue, streamWrapper));
  streamWrapper = asciiTraceHelper.CreateFileStream (dir + "/queueTraces/mark-R3-" + std::to_string (1) + ".plotme");
  qdR3.Get (1)->TraceConnectWithoutContext ("Mark", MakeBoundCallback (&MarkAtQueue, streamWrapper));
  
  QueueDiscContainer qdR1;
  // Assign root queue_disc to outgoing G3--R1 Link NIC
  tch.Uninstall (R1G3Dev);
  qdR1 = tch.Install (R1G3Dev);
  Ptr<OutputStreamWrapper> streamR1 = asciiTraceHelper.CreateFileStream (dir + "queueTraces/queue-R1-" + std::to_string (1) + ".plotme");
  Simulator::ScheduleNow (&CheckQueueSize, qdR1.Get (1),streamR1);
  //Simulator::Schedule (Seconds (2.0), &TraceProb, 1, 1, MakeCallback (&ProbChange1));
  streamWrapper = asciiTraceHelper.CreateFileStream (dir + "/queueTraces/drop-R1-" + std::to_string (1) + ".plotme");
  qdR1.Get (1)->TraceConnectWithoutContext ("Drop", MakeBoundCallback (&DropAtQueue, streamWrapper));
  streamWrapper = asciiTraceHelper.CreateFileStream (dir + "/queueTraces/mark-R1-" + std::to_string (1) + ".plotme");
  qdR1.Get (1)->TraceConnectWithoutContext ("Mark", MakeBoundCallback (&MarkAtQueue, streamWrapper));

  QueueDiscContainer qdRGf2;
  // Assign root queue_disc to outgoing G4--RGf2 Link NIC
  tch.Uninstall (RGf2G4Dev);
  qdRGf2.Add (tch.Install (RGf2G4Dev));
  Ptr<OutputStreamWrapper> streamRGf2 = asciiTraceHelper.CreateFileStream (dir + "queueTraces/queue-RGf2-" + std::to_string (1) + ".plotme");
  Simulator::ScheduleNow (&CheckQueueSize, qdRGf2.Get (1),streamRGf2);
  //Simulator::Schedule (Seconds (2.0), &TraceProb, 1, 1, MakeCallback (&ProbChange1));
  streamWrapper = asciiTraceHelper.CreateFileStream (dir + "/queueTraces/drop-RGf2-" + std::to_string (1) + ".plotme");
  qdRGf2.Get (1)->TraceConnectWithoutContext ("Drop", MakeBoundCallback (&DropAtQueue, streamWrapper));
  streamWrapper = asciiTraceHelper.CreateFileStream (dir + "/queueTraces/mark-RGf2-" + std::to_string (1) + ".plotme");
  qdRGf2.Get (1)->TraceConnectWithoutContext ("Mark", MakeBoundCallback (&MarkAtQueue, streamWrapper));

  QueueDiscContainer qdRBG8;
  // Assign root queue_disc to outgoing G8--RB Link NIC
  tch.Uninstall (RBG8Dev);
  qdRBG8 = tch.Install (RBG8Dev);
  Ptr<OutputStreamWrapper> streamRB = asciiTraceHelper.CreateFileStream (dir + "queueTraces/queue-RB-" + std::to_string (1) + ".plotme");
  Simulator::ScheduleNow (&CheckQueueSize,qdRBG8.Get (1),streamRB);
  //Simulator::Schedule (Seconds (2.0), &TraceProb, 1, 1, MakeCallback (&ProbChange1));
  streamWrapper = asciiTraceHelper.CreateFileStream (dir + "/queueTraces/drop-RB-" + std::to_string (1) + ".plotme");
  qdRBG8.Get (1)->TraceConnectWithoutContext ("Drop", MakeBoundCallback (&DropAtQueue, streamWrapper));
  streamWrapper = asciiTraceHelper.CreateFileStream (dir + "/queueTraces/mark-RB-" + std::to_string (1) + ".plotme");
  qdRBG8.Get (1)->TraceConnectWithoutContext ("Mark", MakeBoundCallback (&MarkAtQueue, streamWrapper));

  QueueDiscContainer qdRCG7;
  // Assign root queue_disc to outgoing G7--RC Link NIC
  tch.Uninstall (RCG7Dev);
  qdRCG7 = tch.Install (RCG7Dev);
  Ptr<OutputStreamWrapper> streamRC = asciiTraceHelper.CreateFileStream (dir + "queueTraces/queue-RC-" + std::to_string (1) + ".plotme");
  Simulator::ScheduleNow (&CheckQueueSize,qdRCG7.Get (1),streamRC);
  //Simulator::Schedule (Seconds (2.0), &TraceProb, 1, 1, MakeCallback (&ProbChange1));
  streamWrapper = asciiTraceHelper.CreateFileStream (dir + "/queueTraces/drop-RC-" + std::to_string (1) + ".plotme");
  qdRCG7.Get (1)->TraceConnectWithoutContext ("Drop", MakeBoundCallback (&DropAtQueue, streamWrapper));
  streamWrapper = asciiTraceHelper.CreateFileStream (dir + "/queueTraces/mark-RC-" + std::to_string (1) + ".plotme");
  qdRCG7.Get (1)->TraceConnectWithoutContext ("Mark", MakeBoundCallback (&MarkAtQueue, streamWrapper));

  QueueDiscContainer qdRDG5;
  // Assign root queue_disc to outgoing G5--RD Link NIC
  tch.Uninstall (RDG5Dev);
  qdRDG5 = tch.Install (RDG5Dev);
  Ptr<OutputStreamWrapper> streamRD = asciiTraceHelper.CreateFileStream (dir + "queueTraces/queue-RD-" + std::to_string (1) + ".plotme");
  Simulator::ScheduleNow (&CheckQueueSize,qdRDG5.Get (1),streamRD);
  //Simulator::Schedule (Seconds (2.0), &TraceProb, 1, 1, MakeCallback (&ProbChange1));
  streamWrapper = asciiTraceHelper.CreateFileStream (dir + "/queueTraces/drop-RD-" + std::to_string (1) + ".plotme");
  qdRDG5.Get (1)->TraceConnectWithoutContext ("Drop", MakeBoundCallback (&DropAtQueue, streamWrapper));
  streamWrapper = asciiTraceHelper.CreateFileStream (dir + "/queueTraces/mark-RD-" + std::to_string (1) + ".plotme");
  qdRDG5.Get (1)->TraceConnectWithoutContext ("Mark", MakeBoundCallback (&MarkAtQueue, streamWrapper));

  QueueDiscContainer qdREG8;
  // Assign root queue_disc to outgoing G8--RE Link NIC
  tch.Uninstall (REG8Dev);
  qdREG8 = tch.Install (REG8Dev);
  Ptr<OutputStreamWrapper> streamRE = asciiTraceHelper.CreateFileStream (dir + "queueTraces/queue-RE-" + std::to_string (1) + ".plotme");
  Simulator::ScheduleNow (&CheckQueueSize,qdREG8.Get (1),streamRE);
  //Simulator::Schedule (Seconds (2.0), &TraceProb, 1, 1, MakeCallback (&ProbChange1));
  streamWrapper = asciiTraceHelper.CreateFileStream (dir + "/queueTraces/drop-RE-" + std::to_string (1) + ".plotme");
  qdREG8.Get (1)->TraceConnectWithoutContext ("Drop", MakeBoundCallback (&DropAtQueue, streamWrapper));
  streamWrapper = asciiTraceHelper.CreateFileStream (dir + "/queueTraces/mark-RE-" + std::to_string (1) + ".plotme");
  qdREG8.Get (1)->TraceConnectWithoutContext ("Mark", MakeBoundCallback (&MarkAtQueue, streamWrapper));

  QueueDiscContainer qdRFG6;
  // Assign root queue_disc to outgoing G6--RF Link NIC
  tch.Uninstall (RFG6Dev);
  qdRFG6 = tch.Install (RFG6Dev);
  Ptr<OutputStreamWrapper> streamRF = asciiTraceHelper.CreateFileStream (dir + "queueTraces/queue-RF-" + std::to_string (1) + ".plotme");
  Simulator::ScheduleNow (&CheckQueueSize,qdRFG6.Get (1),streamRF);
  //Simulator::Schedule (Seconds (2.0), &TraceProb, 1, 1, MakeCallback (&ProbChange1));
  streamWrapper = asciiTraceHelper.CreateFileStream (dir + "/queueTraces/drop-RF-" + std::to_string (1) + ".plotme");
  qdRFG6.Get (1)->TraceConnectWithoutContext ("Drop", MakeBoundCallback (&DropAtQueue, streamWrapper));
  streamWrapper = asciiTraceHelper.CreateFileStream (dir + "/queueTraces/mark-RF-" + std::to_string (1) + ".plotme");
  qdRFG6.Get (1)->TraceConnectWithoutContext ("Mark", MakeBoundCallback (&MarkAtQueue, streamWrapper));

  QueueDiscContainer qdRGf1G8;
  // Assign root queue_disc to outgoing G8--RGf1 Link NIC
  tch.Uninstall (RGf1G8Dev);
  qdRGf1G8 = tch.Install (RGf1G8Dev);
  Ptr<OutputStreamWrapper> streamRGf1 = asciiTraceHelper.CreateFileStream (dir + "queueTraces/queue-RGf1-" + std::to_string (1) + ".plotme");
  Simulator::ScheduleNow (&CheckQueueSize,qdRGf1G8.Get (1),streamRGf1);
  //Simulator::Schedule (Seconds (2.0), &TraceProb, 1, 1, MakeCallback (&ProbChange1));
  streamWrapper = asciiTraceHelper.CreateFileStream (dir + "/queueTraces/drop-RGf1-" + std::to_string (1) + ".plotme");
  qdRGf1G8.Get (1)->TraceConnectWithoutContext ("Drop", MakeBoundCallback (&DropAtQueue, streamWrapper));
  streamWrapper = asciiTraceHelper.CreateFileStream (dir + "/queueTraces/mark-RGf1-" + std::to_string (1) + ".plotme");
  qdRGf1G8.Get (1)->TraceConnectWithoutContext ("Mark", MakeBoundCallback (&MarkAtQueue, streamWrapper));

  QueueDiscContainer qdRGf3G7;
  // Assign root queue_disc to outgoing G7--RGf3 Link NIC
  tch.Uninstall (RGf3G7Dev);
  qdRGf3G7 = tch.Install (RGf3G7Dev);
  Ptr<OutputStreamWrapper> streamRGf3 = asciiTraceHelper.CreateFileStream (dir + "queueTraces/queue-RGf3-" + std::to_string (1) + ".plotme");
  Simulator::ScheduleNow (&CheckQueueSize,qdRGf3G7.Get (1),streamRGf3);
  //Simulator::Schedule (Seconds (2.0), &TraceProb, 1, 1, MakeCallback (&ProbChange1));
  streamWrapper = asciiTraceHelper.CreateFileStream (dir + "/queueTraces/drop-RGf3-" + std::to_string (1) + ".plotme");
  qdRGf3G7.Get (1)->TraceConnectWithoutContext ("Drop", MakeBoundCallback (&DropAtQueue, streamWrapper));
  streamWrapper = asciiTraceHelper.CreateFileStream (dir + "/queueTraces/mark-RGf3-" + std::to_string (1) + ".plotme");
  qdRGf3G7.Get (1)->TraceConnectWithoutContext ("Mark", MakeBoundCallback (&MarkAtQueue, streamWrapper));

//T1-scorp G0
  Config::Set ("/$ns3::NodeListPriv/NodeList/0/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/0/$" + queue_disc_type + "/MaxSize", QueueSizeValue (QueueSize ("666p")));
  Config::Set ("/$ns3::NodeListPriv/NodeList/0/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/0/$ns3::PiQueueDisc/QueueRef", DoubleValue (50));
  Config::Set ("/$ns3::NodeListPriv/NodeList/0/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/0/$ns3::PiQueueDisc/A", DoubleValue (0.000000007960779548));
  Config::Set ("/$ns3::NodeListPriv/NodeList/0/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/0/$ns3::PiQueueDisc/B", DoubleValue (0.000000007960678835));
  Config::Set ("/$ns3::NodeListPriv/NodeList/0/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/0/$ns3::PiQueueDisc/W", DoubleValue (4000));

//Scorp-T2  G1
  Config::Set ("/$ns3::NodeListPriv/NodeList/1/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$" + queue_disc_type + "/MaxSize", QueueSizeValue (QueueSize ("666p")));
  Config::Set ("/$ns3::NodeListPriv/NodeList/1/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$ns3::PiQueueDisc/QueueRef", DoubleValue (50));
  Config::Set ("/$ns3::NodeListPriv/NodeList/1/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$ns3::PiQueueDisc/A", DoubleValue (0.000000007960779548));
  Config::Set ("/$ns3::NodeListPriv/NodeList/1/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$ns3::PiQueueDisc/B", DoubleValue (0.000000007960678835));
  Config::Set ("/$ns3::NodeListPriv/NodeList/1/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$ns3::PiQueueDisc/W", DoubleValue (4000));

//T2-switch1  G2
  Config::Set ("/$ns3::NodeListPriv/NodeList/2/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$" + queue_disc_type + "/MaxSize", QueueSizeValue (QueueSize ("666p")));
  Config::Set ("/$ns3::NodeListPriv/NodeList/2/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$ns3::PiQueueDisc/QueueRef", DoubleValue (50));
  Config::Set ("/$ns3::NodeListPriv/NodeList/2/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$ns3::PiQueueDisc/A", DoubleValue ( 0.0000007961232752));
  Config::Set ("/$ns3::NodeListPriv/NodeList/2/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$ns3::PiQueueDisc/B", DoubleValue (0.0000007960225631));
  Config::Set ("/$ns3::NodeListPriv/NodeList/2/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$ns3::PiQueueDisc/W", DoubleValue (4000));

//switch1-switch2  G3
  Config::Set ("/$ns3::NodeListPriv/NodeList/3/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$" + queue_disc_type + "/MaxSize", QueueSizeValue (QueueSize ("666p")));
  Config::Set ("/$ns3::NodeListPriv/NodeList/3/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$ns3::PiQueueDisc/QueueRef", DoubleValue (50));
  Config::Set ("/$ns3::NodeListPriv/NodeList/3/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$ns3::PiQueueDisc/A", DoubleValue (0.00002359397701));
  Config::Set ("/$ns3::NodeListPriv/NodeList/3/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$ns3::PiQueueDisc/B", DoubleValue (0.0000235807145));
  Config::Set ("/$ns3::NodeListPriv/NodeList/3/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$ns3::PiQueueDisc/W", DoubleValue (4000));

//switch2-switch3  G4
  Config::Set ("/$ns3::NodeListPriv/NodeList/4/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$" + queue_disc_type + "/MaxSize", QueueSizeValue (QueueSize ("666p")));
  Config::Set ("/$ns3::NodeListPriv/NodeList/4/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$ns3::PiQueueDisc/QueueRef", DoubleValue (50));
  Config::Set ("/$ns3::NodeListPriv/NodeList/4/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$ns3::PiQueueDisc/A", DoubleValue (0.0001699434775));
  Config::Set ("/$ns3::NodeListPriv/NodeList/4/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$ns3::PiQueueDisc/B", DoubleValue (0.0001697143013));
  Config::Set ("/$ns3::NodeListPriv/NodeList/4/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$ns3::PiQueueDisc/W", DoubleValue (4000));

//switch3-switch4  G5
  Config::Set ("/$ns3::NodeListPriv/NodeList/5/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$" + queue_disc_type + "/MaxSize", QueueSizeValue (QueueSize ("666p")));
  Config::Set ("/$ns3::NodeListPriv/NodeList/5/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$ns3::PiQueueDisc/QueueRef", DoubleValue (50));
  Config::Set ("/$ns3::NodeListPriv/NodeList/5/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$ns3::PiQueueDisc/A", DoubleValue (0.0000176942394));
  Config::Set ("/$ns3::NodeListPriv/NodeList/5/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$ns3::PiQueueDisc/B", DoubleValue (0.00001768677923));
  Config::Set ("/$ns3::NodeListPriv/NodeList/5/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$ns3::PiQueueDisc/W", DoubleValue (4000));

//switch4-switch5  G6
  Config::Set ("/$ns3::NodeListPriv/NodeList/6/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$" + queue_disc_type + "/MaxSize", QueueSizeValue (QueueSize ("666p")));
  Config::Set ("/$ns3::NodeListPriv/NodeList/6/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$ns3::PiQueueDisc/QueueRef", DoubleValue (50));
  Config::Set ("/$ns3::NodeListPriv/NodeList/6/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$ns3::PiQueueDisc/A", DoubleValue (0.00001887412061));
  Config::Set ("/$ns3::NodeListPriv/NodeList/6/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$ns3::PiQueueDisc/B", DoubleValue (0.0000188656326));
  Config::Set ("/$ns3::NodeListPriv/NodeList/6/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$ns3::PiQueueDisc/W", DoubleValue (4000));

//switch5-switch6  G7
  Config::Set ("/$ns3::NodeListPriv/NodeList/7/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$" + queue_disc_type + "/MaxSize", QueueSizeValue (QueueSize ("666p")));
  Config::Set ("/$ns3::NodeListPriv/NodeList/7/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$ns3::PiQueueDisc/QueueRef", DoubleValue (50));
  Config::Set ("/$ns3::NodeListPriv/NodeList/7/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$ns3::PiQueueDisc/A", DoubleValue (0.000037161036));
  Config::Set ("/$ns3::NodeListPriv/NodeList/7/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$ns3::PiQueueDisc/B", DoubleValue (0.00003713910312));
  Config::Set ("/$ns3::NodeListPriv/NodeList/7/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$ns3::PiQueueDisc/W", DoubleValue (4000));

//switch6-RB G8---RB
  Config::Set ("/$ns3::NodeListPriv/NodeList/8/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/2/$" + queue_disc_type + "/MaxSize", QueueSizeValue (QueueSize ("666p")));
  Config::Set ("/$ns3::NodeListPriv/NodeList/8/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/2/$ns3::PiQueueDisc/QueueRef", DoubleValue (50));
  Config::Set ("/$ns3::NodeListPriv/NodeList/8/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/2/$ns3::PiQueueDisc/A", DoubleValue (0.000003538251066));
  Config::Set ("/$ns3::NodeListPriv/NodeList/8/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/2/$ns3::PiQueueDisc/B", DoubleValue (0.00000353795266));
  Config::Set ("/$ns3::NodeListPriv/NodeList/8/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/2/$ns3::PiQueueDisc/W", DoubleValue (4000));

// T2 - S2 (10 Links)  G2---R2
  for (int i = 0,j = 12; i < 10; i++,j++)
    {
      Config::Set ("/$ns3::NodeListPriv/NodeList/2/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/"+std::to_string (j)+"/$" + queue_disc_type + "/MaxSize", QueueSizeValue (QueueSize ("666p")));
      Config::Set ("/$ns3::NodeListPriv/NodeList/2/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/"+std::to_string (j)+"/$ns3::PiQueueDisc/QueueRef", DoubleValue (50));
      Config::Set ("/$ns3::NodeListPriv/NodeList/2/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/"+std::to_string (j)+"/$ns3::PiQueueDisc/A", DoubleValue ( 0.00000002653581992));
      Config::Set ("/$ns3::NodeListPriv/NodeList/2/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/"+std::to_string (j)+"/$ns3::PiQueueDisc/B", DoubleValue (0.00000002653570802));
      Config::Set ("/$ns3::NodeListPriv/NodeList/2/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/"+std::to_string (j)+"/$ns3::PiQueueDisc/W", DoubleValue (4000));
    }

//Switch 1 - S1   G3--R1 
  Config::Set ("/$ns3::NodeListPriv/NodeList/3/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/3/$" + queue_disc_type + "/MaxSize", QueueSizeValue (QueueSize ("666p")));
  Config::Set ("/$ns3::NodeListPriv/NodeList/3/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/3/$ns3::PiQueueDisc/QueueRef", DoubleValue (50));
  Config::Set ("/$ns3::NodeListPriv/NodeList/3/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/3/$ns3::PiQueueDisc/A", DoubleValue (0.0000001326802186));
  Config::Set ("/$ns3::NodeListPriv/NodeList/3/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/3/$ns3::PiQueueDisc/B", DoubleValue (0.0000001326774211));
  Config::Set ("/$ns3::NodeListPriv/NodeList/3/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/3/$ns3::PiQueueDisc/W", DoubleValue (4000));

//Switch 1 - S3   G3--R3
  Config::Set ("/$ns3::NodeListPriv/NodeList/3/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/2/$" + queue_disc_type + "/MaxSize", QueueSizeValue (QueueSize ("666p")));
  Config::Set ("/$ns3::NodeListPriv/NodeList/3/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/2/$ns3::PiQueueDisc/QueueRef", DoubleValue (50));
  Config::Set ("/$ns3::NodeListPriv/NodeList/3/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/2/$ns3::PiQueueDisc/A", DoubleValue (0.0000001326802186));
  Config::Set ("/$ns3::NodeListPriv/NodeList/3/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/2/$ns3::PiQueueDisc/B", DoubleValue (0.0000001326774211));
  Config::Set ("/$ns3::NodeListPriv/NodeList/3/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/2/$ns3::PiQueueDisc/W", DoubleValue (4000));

//Switch 2 - S2     G4---R2
  Config::Set ("/$ns3::NodeListPriv/NodeList/4/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/2/$" + queue_disc_type + "/MaxSize", QueueSizeValue (QueueSize ("666p")));
  Config::Set ("/$ns3::NodeListPriv/NodeList/4/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/2/$ns3::PiQueueDisc/QueueRef", DoubleValue (50));
  Config::Set ("/$ns3::NodeListPriv/NodeList/4/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/2/$ns3::PiQueueDisc/A", DoubleValue (0.0000002653632348));
  Config::Set ("/$ns3::NodeListPriv/NodeList/4/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/2/$ns3::PiQueueDisc/B", DoubleValue (0.0000002653520446));
  Config::Set ("/$ns3::NodeListPriv/NodeList/4/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/2/$ns3::PiQueueDisc/W", DoubleValue (4000));

//Switch 3 - D  G5----RD
  Config::Set ("/$ns3::NodeListPriv/NodeList/5/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/3/$" + queue_disc_type + "/MaxSize", QueueSizeValue (QueueSize ("666p")));
  Config::Set ("/$ns3::NodeListPriv/NodeList/5/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/3/$ns3::PiQueueDisc/QueueRef", DoubleValue (50));
  Config::Set ("/$ns3::NodeListPriv/NodeList/5/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/3/$ns3::PiQueueDisc/A", DoubleValue (0.000007076800539));
  Config::Set ("/$ns3::NodeListPriv/NodeList/5/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/3/$ns3::PiQueueDisc/B", DoubleValue (0.000007075606913));
  Config::Set ("/$ns3::NodeListPriv/NodeList/5/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/3/$ns3::PiQueueDisc/W", DoubleValue (4000));

//Switch 4 - F      G6--RF 
  Config::Set ("/$ns3::NodeListPriv/NodeList/6/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/2/$" + queue_disc_type + "/MaxSize", QueueSizeValue (QueueSize ("666p")));
  Config::Set ("/$ns3::NodeListPriv/NodeList/6/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/2/$ns3::PiQueueDisc/QueueRef", DoubleValue (50));
  Config::Set ("/$ns3::NodeListPriv/NodeList/6/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/2/$ns3::PiQueueDisc/A", DoubleValue (0.000002358800888));
  Config::Set ("/$ns3::NodeListPriv/NodeList/6/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/2/$ns3::PiQueueDisc/B", DoubleValue (0.000002358668263));
  Config::Set ("/$ns3::NodeListPriv/NodeList/6/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/2/$ns3::PiQueueDisc/W", DoubleValue (4000));

//Switch 5 - S3   G7--R3
  Config::Set ("/$ns3::NodeListPriv/NodeList/7/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/2/$" + queue_disc_type + "/MaxSize", QueueSizeValue (QueueSize ("666p")));
  Config::Set ("/$ns3::NodeListPriv/NodeList/7/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/2/$ns3::PiQueueDisc/QueueRef", DoubleValue (50));
  Config::Set ("/$ns3::NodeListPriv/NodeList/7/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/2/$ns3::PiQueueDisc/A", DoubleValue (0.0000001326802186));
  Config::Set ("/$ns3::NodeListPriv/NodeList/7/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/2/$ns3::PiQueueDisc/B", DoubleValue (0.0000001326774211));
  Config::Set ("/$ns3::NodeListPriv/NodeList/7/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/2/$ns3::PiQueueDisc/W", DoubleValue (4000));

//Switch 5 - C   G7---RC
  Config::Set ("/$ns3::NodeListPriv/NodeList/7/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/3/$" + queue_disc_type + "/MaxSize", QueueSizeValue (QueueSize ("666p")));
  Config::Set ("/$ns3::NodeListPriv/NodeList/7/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/3/$ns3::PiQueueDisc/QueueRef", DoubleValue (50));
  Config::Set ("/$ns3::NodeListPriv/NodeList/7/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/3/$ns3::PiQueueDisc/A", DoubleValue (0.000003538251066));
  Config::Set ("/$ns3::NodeListPriv/NodeList/7/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/3/$ns3::PiQueueDisc/B", DoubleValue (0.00000353795266));
  Config::Set ("/$ns3::NodeListPriv/NodeList/7/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/3/$ns3::PiQueueDisc/W", DoubleValue (4000));

//Switch 6 - S1    G8----R1
  Config::Set ("/$ns3::NodeListPriv/NodeList/8/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$" + queue_disc_type + "/MaxSize", QueueSizeValue (QueueSize ("666p")));
  Config::Set ("/$ns3::NodeListPriv/NodeList/8/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$ns3::PiQueueDisc/QueueRef", DoubleValue (50));
  Config::Set ("/$ns3::NodeListPriv/NodeList/8/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$ns3::PiQueueDisc/A", DoubleValue (0.0000001326802186));
  Config::Set ("/$ns3::NodeListPriv/NodeList/8/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$ns3::PiQueueDisc/B", DoubleValue (0.0000001326774211));
  Config::Set ("/$ns3::NodeListPriv/NodeList/8/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/1/$ns3::PiQueueDisc/W", DoubleValue (4000));

//Switch 6 - E     G8----RE
  Config::Set ("/$ns3::NodeListPriv/NodeList/8/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/3/$" + queue_disc_type + "/MaxSize", QueueSizeValue (QueueSize ("666p")));
  Config::Set ("/$ns3::NodeListPriv/NodeList/8/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/3/$ns3::PiQueueDisc/QueueRef", DoubleValue (50));
  Config::Set ("/$ns3::NodeListPriv/NodeList/8/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/3/$ns3::PiQueueDisc/A", DoubleValue (0.000007076800539));
  Config::Set ("/$ns3::NodeListPriv/NodeList/8/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/3/$ns3::PiQueueDisc/B", DoubleValue (0.000007075606913));
  Config::Set ("/$ns3::NodeListPriv/NodeList/8/$ns3::Node/$ns3::TrafficControlLayer/RootQueueDiscList/3/$ns3::PiQueueDisc/W", DoubleValue (4000));

  uint16_t port = 50000;
  //Install sink on R2
  for (uint32_t i = 0; i < R2.GetN (); i++)
    {
      InstallPacketSink (R2.Get (i),port);
    }

  //Install sink on R1
  for (int i = 0; i < 5; i++)
    {
      InstallPacketSink (R1.Get (0),port + i);
    }

  //Install sink on R3
  for (int i = 0; i < 5; i++)
    {
      InstallPacketSink (R3.Get (0),port + i);
    }

  //Install sink on RGf2
  for (uint32_t i = 0; i < 10; i++)
    {
      InstallPacketSink (RGf2.Get (0),port + i);
    }

  //Install sink on RD
  for (int i = 0; i < 6; i++)
    {
      InstallPacketSink (RD.Get (0),port + i);
    }

  //Install sink on RF
  for (int i = 0; i < 2; i++)
    {
      InstallPacketSink (RF.Get (0),port + i);
    }

  //Install sink on RGf3
  for (int i = 0; i < 5; i++)
    {
      InstallPacketSink (RGf3.Get (0),port + i);
    }

  //Install sink on RC
  for (int i = 0; i < 3; i++)
    {
      InstallPacketSink (RC.Get (0),port + i);
    }

  //Install sink on RE
  for (int i = 0; i < 6; i++)
    {
      InstallPacketSink (RE.Get (0),port + i);
    }

  //Install sink on RGf1
  for (int i = 0; i < 5; i++)
    {
      InstallPacketSink (RGf1.Get (0),port + i);
    }

  //Install sink on RB
  for (int i = 0; i < 3; i++)
    {
      InstallPacketSink (RB.Get (0),port + i);
    }

  //Install sender on S1 with sink R1
  for (int i = 0; i < 5; i++)
    {
      Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream (dir + "cwndTraces/S1-" + std::to_string (i + 1) + ".plotme");
      InstallBulkSend (S1.Get (i), R1G3Int.GetAddress (0), port + i, 9 + i, 0, MakeBoundCallback (&CwndChange, stream));
    }

  //Install sender on S1 with sink RGf1
  for (int i = 5; i < 10; i++)
    {
      Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream (dir + "cwndTraces/S1-" + std::to_string (i + 1) + ".plotme");
      InstallBulkSend (S1.Get (i), RGf1G8Int.GetAddress (0), port + i - 5, 9 + i, 0, MakeBoundCallback (&CwndChange, stream));
    }

  //Install sender on S2 with sink R2
  for (int i = 0; i < 10; i++)
    {
      Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream (dir + "cwndTraces/S2-" + std::to_string (i + 1) + ".plotme");
      InstallBulkSend (S2.Get (i), R2Int.GetAddress (2 * i), port, 19 + i, 0, MakeBoundCallback (&CwndChange, stream));
    }

  //Install sender on S2 with sink RGf2
  for (int i = 10; i < 20; i++)
    {
      Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream (dir + "cwndTraces/S2-" + std::to_string (i + 1) + ".plotme");
      InstallBulkSend (S2.Get (i), RGf2G4Int.GetAddress (0), port + i - 10, 19 + i, 0, MakeBoundCallback (&CwndChange, stream));
    }

  //Install sender on S3 with sink R3
  for (int i = 0; i < 5; i++)
    {
      Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream (dir + "cwndTraces/S3-" + std::to_string (i + 1) + ".plotme");
      InstallBulkSend (S3.Get (i), R3G3Int.GetAddress (0), port + i, 39 + i, 0, MakeBoundCallback (&CwndChange, stream));
    }

  //Install sender on S3 with sink RGf3
  for (int i = 5; i < 10; i++)
    {
      Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream (dir + "cwndTraces/S3-" + std::to_string (i + 1) + ".plotme");
      InstallBulkSend (S3.Get (i), RGf3G7Int.GetAddress (0), port + i - 5, 39 + i, 0, MakeBoundCallback (&CwndChange, stream));
    }

  //Install sender on SD with sink RD
  for (int i = 0; i < 6; i++)
    {
      Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream (dir + "cwndTraces/SD-" + std::to_string (i + 1) + ".plotme");
      InstallBulkSend (SD.Get (0), RDG5Int.GetAddress (0), port + i, 68, i, MakeBoundCallback (&CwndChange, stream));
    }

  //Install sender on SB with sink RB
  for (int i = 0; i < 3; i++)
    {
      Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream (dir + "cwndTraces/SB-" + std::to_string (i + 1) + ".plotme");
      InstallBulkSend (SB.Get (0), RBG8Int.GetAddress (0), port + i, 64, i, MakeBoundCallback (&CwndChange, stream));
    }

  //Install sender on SF with sink RF
  for (int i = 0; i < 2; i++)
    {
      Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream (dir + "cwndTraces/SF-" + std::to_string (i + 1) + ".plotme");
      InstallBulkSend (SF.Get (0), RFG6Int.GetAddress (0), port + i, 72, i, MakeBoundCallback (&CwndChange, stream));
    }

  //Install sender on SC with sink RC
  for (int i = 0; i < 3; i++)
    {
      Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream (dir + "cwndTraces/SC-" + std::to_string (i + 1) + ".plotme");
      InstallBulkSend (SC.Get (0), RCG7Int.GetAddress (0), port + i, 66, i, MakeBoundCallback (&CwndChange, stream));
    }

  //Install sender on SE with sink RE
  for (int i = 0; i < 6; i++)
    {
      Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream (dir + "cwndTraces/SE-" + std::to_string (i + 1) + ".plotme");
      InstallBulkSend (SE.Get (0), REG8Int.GetAddress (0), port + i, 70, i, MakeBoundCallback (&CwndChange, stream));
    }

  Simulator::Stop (Seconds (stopTime));
  Simulator::Run ();

  std::ofstream myfile;
  myfile.open (dir + "queueStats.txt", std::fstream::in | std::fstream::out | std::fstream::app);
  myfile << std::endl;
   for (uint32_t i = 0; i < qdTrunk.GetN (); i++)
      {
        myfile << "Stat for Queue " + std::to_string (i + 1);
        myfile << qdTrunk.Get (i)->GetStats ();
      }
     myfile.close ();

  myfile.open (dir + "config.txt", std::fstream::in | std::fstream::out | std::fstream::app);
  myfile << "useEcn " << useEcn << "\n";
  myfile << "queue_disc_type " << queue_disc_type << "\n";
  myfile << "stream  " << stream << "\n";
  myfile << "transport_prot " << transport_prot << "\n";
  myfile << "dataSize " << dataSize << "\n";
  myfile << "delAckCount " << delAckCount << "\n";
  myfile << "stopTime " << stopTime << "\n";
  myfile.close ();

  Simulator::Destroy ();
  return 0;

}
