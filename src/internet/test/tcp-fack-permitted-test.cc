/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016 Natale Patriciello <natale.patriciello@gmail.com>
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
 */

#include "tcp-general-test.h"
#include "ns3/node.h"
#include "ns3/log.h"
#include "ns3/tcp-header.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FackPermittedTestSuite");

/**
 * \ingroup internet-test
 * \ingroup tests
 *
 * \brief Test case for checking the FACK-PERMITTED option.
 *
 */
class FackPermittedTestCase : public TcpGeneralTest
{
public:
  /** \brief Configuration of the test */
  enum Configuration
  {
    DISABLED,
    ENABLED_RECEIVER,
    ENABLED_SENDER,
    ENABLED
  };

  /**
   * \brief Constructor
   * \param conf Test configuration.
   * */
  FackPermittedTestCase (FackPermittedTestCase::Configuration conf);
protected:
  virtual Ptr<TcpSocketMsgBase> CreateReceiverSocket (Ptr<Node> node);
  virtual Ptr<TcpSocketMsgBase> CreateSenderSocket (Ptr<Node> node);

//  virtual void Tx (const Ptr<const Packet> p, const TcpHeader&h, SocketWho who);

  Configuration m_configuration; //!< The configuration
};

FackPermittedTestCase::FackPermittedTestCase (FackPermittedTestCase::Configuration conf)
  : TcpGeneralTest ("Testing the TCP Fack Permitted option")
{
  m_configuration = conf;
}

Ptr<TcpSocketMsgBase>
FackPermittedTestCase::CreateReceiverSocket (Ptr<Node> node)
{
  Ptr<TcpSocketMsgBase> socket = TcpGeneralTest::CreateReceiverSocket (node);

  switch (m_configuration)
    {
    case DISABLED:
      socket->SetAttribute ("Fack", BooleanValue (false));
      break;

    case ENABLED_RECEIVER:
      socket->SetAttribute ("Fack", BooleanValue (true));
      break;

    case ENABLED_SENDER:
      socket->SetAttribute ("Fack", BooleanValue (false));
      break;

    case ENABLED:
      socket->SetAttribute ("Fack", BooleanValue (true));
      break;
    }

  return socket;
}

Ptr<TcpSocketMsgBase>
FackPermittedTestCase::CreateSenderSocket (Ptr<Node> node)
{
  Ptr<TcpSocketMsgBase> socket = TcpGeneralTest::CreateSenderSocket (node);

  switch (m_configuration)
    {
    case DISABLED:
      socket->SetAttribute ("Fack", BooleanValue (false));
      break;

    case ENABLED_RECEIVER:
      socket->SetAttribute ("Fack", BooleanValue (false));
      break;

    case ENABLED_SENDER:
      socket->SetAttribute ("Fack", BooleanValue (true));
      break;

    case ENABLED:
      socket->SetAttribute ("Fack", BooleanValue (true));
      break;
    }

  return socket;
}

/*void
FackPermittedTestCase::Tx (const Ptr<const Packet> p, const TcpHeader &h, SocketWho who)
{

  if (!(h.GetFlags () & TcpHeader::SYN))
    {
      NS_TEST_ASSERT_MSG_EQ (h.HasOption (TcpOption::SACKPERMITTED), false,
                             "FackPermitted in non-SYN segment");
      return;
    }

  if (m_configuration == DISABLED)
    {
      NS_TEST_ASSERT_MSG_EQ (h.HasOption (TcpOption::SACKPERMITTED), false,
                             "FackPermitted disabled but option enabled");
    }
  else if (m_configuration == ENABLED)
    {
      NS_TEST_ASSERT_MSG_EQ (h.HasOption (TcpOption::SACKPERMITTED), true,
                             "FackPermitted enabled but option disabled");
    }

  NS_LOG_INFO (h);
  if (who == SENDER)
    {
      if (h.GetFlags () & TcpHeader::SYN)
        {
          if (m_configuration == ENABLED_RECEIVER)
            {
              NS_TEST_ASSERT_MSG_EQ (h.HasOption (TcpOption::SACKPERMITTED), false,
                                     "FackPermitted disabled but option enabled");
            }
          else if (m_configuration == ENABLED_SENDER)
            {
              NS_TEST_ASSERT_MSG_EQ (h.HasOption (TcpOption::SACKPERMITTED), true,
                                     "FackPermitted enabled but option disabled");
            }
        }
      else
        {
          if (m_configuration != ENABLED)
            {
              NS_TEST_ASSERT_MSG_EQ (h.HasOption (TcpOption::SACKPERMITTED), false,
                                     "FackPermitted disabled but option enabled");
            }
        }
    }
  else if (who == RECEIVER)
    {
      if (h.GetFlags () & TcpHeader::SYN)
        {
          // Sender has not sent FackPermitted, so implementation should disable ts
          if (m_configuration == ENABLED_RECEIVER)
            {
              NS_TEST_ASSERT_MSG_EQ (h.HasOption (TcpOption::SACKPERMITTED), false,
                                     "sender has not ts, but receiver sent anyway");
            }
          else if (m_configuration == ENABLED_SENDER)
            {
              NS_TEST_ASSERT_MSG_EQ (h.HasOption (TcpOption::SACKPERMITTED), false,
                                     "receiver has not ts enabled but sent anyway");
            }
        }
      else
        {
          if (m_configuration != ENABLED)
            {
              NS_TEST_ASSERT_MSG_EQ (h.HasOption (TcpOption::SACKPERMITTED), false,
                                     "FackPermitted disabled but option enabled");
            }
        }
    }
}
*/
/**
 * \ingroup internet-test
 *  \ingroup tests
 *
 * The test case for testing the TCP SACK PERMITTED option.
 */
class TcpFackPermittedTestSuite : public TestSuite
{
public:
  /** \brief Constructor */
  TcpFackPermittedTestSuite ()
    : TestSuite ("tcp-fack-permitted", UNIT)
  {
    AddTestCase (new FackPermittedTestCase (FackPermittedTestCase::DISABLED), TestCase::QUICK);
    AddTestCase (new FackPermittedTestCase (FackPermittedTestCase::ENABLED_RECEIVER), TestCase::QUICK);
    AddTestCase (new FackPermittedTestCase (FackPermittedTestCase::ENABLED_SENDER), TestCase::QUICK);
    AddTestCase (new FackPermittedTestCase (FackPermittedTestCase::ENABLED), TestCase::QUICK);
  }

};

static TcpFackPermittedTestSuite g_tcpFackPermittedTestSuite; //!< Static variable for test initialization
