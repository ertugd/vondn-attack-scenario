/* -*- Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil -*- */
/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Ertugrul Dogruluk, Antonio Costa <d7474@di.uminho.pt>, Centro Algorithmi, University of Minho
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"


namespace ns3 {

int
main(int argc, char* argv[])
{
CommandLine cmd;
cmd.Parse(argc, argv);

//Topology Reader
	AnnotatedTopologyReader topologyReader("", 1);
	topologyReader.SetFileName("/home/ertugrul/Desktop/ndnSIM/scenario/topology/vondn_topology.txt");
	topologyReader.Read();

//Install NDN stack on all nodes
	ndn::StackHelper ndnHelper;
	ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", "0"); // no limit for CS
	ndnHelper.InstallAll();

//Choosing forwarding strategy
	ndn::StrategyChoiceHelper::InstallAll("/ndn_cxx_voice1,/ndn_cxx_voice2", "/localhost/nfd/strategy/broadcast");

//Installing global routing interface on all nodes
	ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
	ndnGlobalRoutingHelper.InstallAll();

//Getting containers for the consumer/producer
	Ptr<Node> consumers[2] = {Names::Find<Node>("callee"), Names::Find<Node>("caller")
								};
	Ptr<Node> producer1 = Names::Find<Node>("callee");
	Ptr<Node> producer2 = Names::Find<Node>("caller");


	ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
	consumerHelper.SetAttribute("Frequency", StringValue("100"));// 100 interests a second "callee"
	consumerHelper.SetPrefix("/ndn_cxx_voice2");
	consumerHelper.SetAttribute("Randomize",StringValue("exponential"));
	consumerHelper.Install(consumers[0]);

	consumerHelper.SetAttribute("Frequency", StringValue("100"));//100 interests a second "caller"
	consumerHelper.SetPrefix("/ndn_cxx_voice1");
	consumerHelper.SetAttribute("Randomize",StringValue("uniform"));
	consumerHelper.Install(consumers[1]);



      /****************************************************************************/
      ndn::AppHelper producerHelper("ns3::ndn::Producer");
      producerHelper.SetAttribute("PayloadSize", StringValue("1024"));

      /****************************************************************************/
      // Register /producer1 to /producer2 prefix with global routing controller and
      // install producer that will satisfy Interests in /caller to /callee namespace
      ndnGlobalRoutingHelper.AddOrigins("/ndn_cxx_voice1", producer1);
      producerHelper.SetPrefix("/ndn_cxx_voice1");
      producerHelper.SetAttribute("Signature", UintegerValue(100));
      producerHelper.SetAttribute("KeyLocator", StringValue("/unique/key/locator"));
      producerHelper.Install(producer1).Start(Seconds(5.0));

      ndnGlobalRoutingHelper.AddOrigins("/ndn_cxx_voice2", producer2);
      producerHelper.SetPrefix("/ndn_cxx_voice2");
      producerHelper.SetAttribute("Signature", UintegerValue(100));
      producerHelper.SetAttribute("KeyLocator", StringValue("/unique/key/locator"));
      producerHelper.Install(producer2).Start(Seconds(7.0));


  // Calculate and install FIBs
  ndn::GlobalRoutingHelper::CalculateRoutes();

  	  	  	  	  	  	  	  	  	  	  	  	  //Tracers
  ndn::CsTracer::InstallAll("/home/ertugrul/Desktop/ndnSIM/scenario/results/vondn_Cs_tracer.txt", Seconds(1));
  ndn::L3RateTracer::InstallAll("/home/ertugrul/Desktop/ndnSIM/scenario/results/vondn_L3-rate-trace.txt");
  ndn::AppDelayTracer::InstallAll("/home/ertugrul/Desktop/ndnSIM/scenario/results/vondn_app-delays-trace.txt");
  L2RateTracer::InstallAll("/home/ertugrul/Desktop/ndnSIM/scenario/results/vondn_drop-trace.txt", Seconds(0.5));

  Simulator::Stop(Seconds(30));
  Simulator::Run();
  Simulator::Destroy();

  return 0;
}

} // namespace ns3

int
main(int argc, char* argv[])
{
  return ns3::main(argc, argv);
}
