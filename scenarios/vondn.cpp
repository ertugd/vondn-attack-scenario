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

#include "../extensions/BadGuy.h"
#include "../extensions/CA-cert.hpp"
#include "../extensions/content-store-privacy.hpp"

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"
#include "ns3/point-to-point-module.h"
#include <boost/lexical_cast.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/plus.hpp>
#include <boost/mpl/int.hpp>
namespace mpl = boost::mpl;
#include </home/ertugrul/Desktop/ndnSIM/ns-3/src/core/model/random-variable-stream.h>
#include "ns3/node-container.h"
#include "ns3/ptr.h"
#include <ns3/ndnSIM/NFD/daemon/table/fib.hpp>

using namespace ns3;
using namespace ns3::ndn;
using namespace std;

uint32_t Run = 1;

void PrintTime (Time next, const string name)
{
  cerr << "Simulating= " << name << " " << Simulator::Now ().ToDouble (Time::S) << "s" << endl;
  Simulator::Schedule (next, PrintTime, next, name);
}

int normalNetworkConfiguration(string prefix, NodeContainer nodes) {
	StackHelper helper;
	if (prefix == "default") {
		helper.SetOldContentStore("ns3::ndn::cs::Privacy::Lru","MaxSize", "1000", "CacheProbability", "1", "UpdateInterval", "200", "CHRatioTreshold", "5"); //200=0.2secs.
		helper.Install(nodes);
		StrategyChoiceHelper::Install(nodes, "/vondn",
				"/localhost/nfd/strategy/best-route"); // nfd forwarding strategy
	} else if (prefix == "Lru1") {
		helper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", "1000"); //if 0 it is unlimited
		helper.Install(nodes);
		StrategyChoiceHelper::Install(nodes, "/vondn",
				"/localhost/nfd/strategy/best-route"); // nfd forwarding strategy
	} else if (prefix == "Lru2") {
		helper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", "1000"); //if 0 it is unlimited
		helper.Install(nodes);
		StrategyChoiceHelper::Install(nodes, "/vondn",
				"/localhost/nfd/strategy/multicast"); // nfd forwarding strategy
	} else if (prefix == "Fifo") {
			helper.SetOldContentStore("ns3::ndn::cs::Fifo", "MaxSize", "1000"); //if 0 it is unlimited
			helper.Install(nodes);
			StrategyChoiceHelper::Install(nodes, "/vondn",
					"/localhost/nfd/strategy/best-route"); // nfd forwarding strategy
	} else if (prefix == "Lfu") {
			helper.SetOldContentStore("ns3::ndn::cs::Lfu", "MaxSize", "1000"); //if 0 it is unlimited
			helper.Install(nodes);
			StrategyChoiceHelper::Install(nodes, "/vondn",
					"/localhost/nfd/strategy/best-route"); // nfd forwarding strategy
	} else if (prefix == "Random") {
		helper.SetOldContentStore("ns3::ndn::cs::Random", "MaxSize", "1000");
		helper.Install(nodes);
		StrategyChoiceHelper::Install(nodes, "/vondn",
				"/localhost/nfd/strategy/best-route"); // nfd forwarding strategy
	} else if (prefix == "Probability1") {
		helper.SetOldContentStore("ns3::ndn::cs::Probability::Random","MaxSize", "10", "CacheProbability", "0.01");
		helper.Install(nodes);
		StrategyChoiceHelper::Install(nodes, "/vondn",
				"/localhost/nfd/strategy/best-route"); // nfd forwarding strategy
	} else if (prefix == "Probability2") {
		helper.SetOldContentStore("ns3::ndn::cs::Probability::Random","MaxSize", "10", "CacheProbability", "0.01");
		helper.Install(nodes);
		StrategyChoiceHelper::Install(nodes, "/vondn",
				"/localhost/nfd/strategy/multicast"); // nfd forwarding strategy
	} else if (prefix == "freshness") {
		helper.SetOldContentStore("ns3::ndn::cs::Freshness::Random","MaxSize", "100");
		helper.Install(nodes);
		StrategyChoiceHelper::Install(nodes, "/ndn/video",
				"/localhost/nfd/strategy/multicast"); // nfd forwarding strategy
	} else if (prefix == "dad1") {
		helper.SetOldContentStore("ns3::ndn::cs::Privacy::Lru","MaxSize", "100", "CacheProbability", "1.0", "UpdateInterval", "3500", "CHRatioTreshold", "0.001");
		helper.Install(nodes);
		StrategyChoiceHelper::Install(nodes, "/vondn",
				"/localhost/nfd/strategy/best-route"); // nfd forwarding strategy
	} else if (prefix == "dad2") {
		helper.SetOldContentStore("ns3::ndn::cs::Privacy::Lru","MaxSize", "100", "CacheProbability", "1.0", "UpdateInterval", "3500", "CHRatioTreshold", "0.01");
		helper.Install(nodes);
		StrategyChoiceHelper::Install(nodes, "/vondn",
				"/localhost/nfd/strategy/multicast"); // nfd forwarding strategy
	} else if (prefix == "nocache") {
		helper.SetOldContentStore("ns3::ndn::cs::Nocache");
		helper.Install(nodes);
		StrategyChoiceHelper::Install(nodes, "/vondn",
				"/localhost/nfd/strategy/best-route"); // nfd forwarding strategy
	} else {
		cerr << "Invalid scenario Content Store prefix" << endl;
		return -1;
	}
	return 0;
}

//Choosing forwarding strategy
	//ndn::StrategyChoiceHelper::InstallAll("/ndn_voice_packet11", "/localhost/nfd/strategy/multicast");
	//ndn::StrategyChoiceHelper::InstallAll("/ndn_voice_packet11", "/localhost/nfd/strategy/best-route");
	//ndn::StrategyChoiceHelper::InstallAll("/ndn_voice_packet", "/localhost/nfd/strategy/broadcast");
	//ndn::StrategyChoiceHelper::InstallAll("/ndn_voice_packet", "/localhost/nfd/strategy/client-control");
	//ndn::StrategyChoiceHelper::InstallAll("/ndn_voice_packet", "/localhost/nfd/strategy/ncc");




int specialNetworkConfiguration(string prefix, NodeContainer nodes) {
	StackHelper helper;
	// bad guys will use their own cache...
	helper.SetOldContentStore("ns3::ndn::cs::Nocache");
	helper.Install(nodes);
	StrategyChoiceHelper::Install(nodes, "/vondn",
			"/localhost/nfd/strategy/best-route"); //defult nfd forwarding strategy
	return 0;
}




int
main(int argc, char* argv[])
{
	  string topology = "tree";
	  string prefix = "default";
	  string producerLocation = "bb";
	  string consumerLocation = "leaf";
	  Time evilGap = Time::FromDouble (0.02, Time::MS);
	  Time defaultRtt = Seconds (0.25);
	  uint32_t badCount = 2;
	  uint32_t goodCount = 3;
	  uint32_t prodCount = 42; //amount of topology bb counts.// small-tree=3, tree=9, testbed=42, 7018.r0= 221
	  string folder = "tmp";

  // 1. Process command line arguments...
  CommandLine cmd;
  cmd.AddValue ("topology", "Topology", topology);
  cmd.AddValue ("run", "Run", Run);
  cmd.AddValue ("algorithm", "Side-Channel Timing attack mitigation algorithms", prefix);
  cmd.AddValue ("producer", "Producer location: gw or bb", producerLocation);
  cmd.AddValue ("consumer", "Consumer location: leaf or ...", consumerLocation);
  cmd.AddValue ("badCount", "Number of bad guys", badCount);
  cmd.AddValue ("goodCount", "Number of good guys", goodCount);
  cmd.AddValue ("prodCount", "Number of producer guys", prodCount);
  cmd.AddValue ("folder", "Folder where results will be placed", folder);
  cmd.AddValue ("defaultRtt", "Default RTT for BDP limits", defaultRtt);
  cmd.Parse (argc, argv);

  Config::SetGlobal ("RngRun", IntegerValue (Run));

  // 2. Define the name of the files to output the results...
  string name = prefix;
  name += "-topo-" + topology;
  name += "-evil-" + boost::lexical_cast<string> (badCount);
  name += "-good-" + boost::lexical_cast<string> (goodCount);
  name += "-producer-" + producerLocation;
  name += "-run-"  + boost::lexical_cast<string> (Run);

  string results_file = "results/" + folder + "/" + name + ".txt";
  string meta_file    = "results/" + folder + "/" + name + ".meta";
  string graph_dot_file    = "results/" + folder + "/" + name + ".dot";
  string graph_pdf_file    = "results/" + folder + "/" + name + ".pdf";

  system (("mkdir -p \"results/" + folder + "\"").c_str ());
  ofstream os (meta_file.c_str(), ios_base::out | ios_base::trunc);
  os << "VoNDN Attack Scenario run: " << Run << " is starting..." << endl;


  // 3. Read the topology file to memory...
  AnnotatedTopologyReader topologyReader ("", 1.0);
  topologyReader.SetFileName ("topology/" + topology + ".txt");
  os << "Reading topology file for VoNDN scenario: " << "topology/" + topology + ".txt" << endl;
  topologyReader.Read ();
  //os << "The Topology file is successfully read for the attack scenario." << endl;
  topologyReader.ApplyOspfMetric ();

  // 4. Create a random number generator
  Ptr<UniformRandomVariable> rand = CreateObject<UniformRandomVariable>();

  // 5. Create containers and select potential consumers and producers using names...
  NodeContainer allNodes = topologyReader.GetNodes();
  vector<Ptr<Node>> producerCandidates;
  vector<Ptr<Node>> consumerCandidates;
  vector<Ptr<Node>> consumersToAttack;
  set<Ptr<Node>> producers;
  set<Ptr<Node>> consumers;
  set<Ptr<Node>> badGuys;
  for (NodeContainer::Iterator node = allNodes.Begin (); node != allNodes.End (); node++) {
      if (Names::FindName (*node).find(producerLocation)!=std::string::npos) {
    	  // found a node with name containing the string defined in producerLocation
    	  producerCandidates.push_back(*node);
      }
      else if (Names::FindName (*node).find(consumerLocation)!=std::string::npos) {
    	  // found a node with name containing the string defined in consumerLocation
    	  consumerCandidates.push_back(*node);
      }
  }
  os << "THE TOTAL NUMBER OF THE NODE IS:           " << allNodes.GetN()  << endl;
  os << "NUMBER OF BACKBONE NODE CANDIDATE IS:      " << producerCandidates.size()  << endl;
  os << "NUMBER OF CONSUMER NODE CANDIDATE IS:      " << consumerCandidates.size()  << endl;

  // when goodCount is zero that means that all nodes are good guys except the evil ones
  if ( goodCount == 0 )
	  goodCount = consumerCandidates.size() - badCount;

  // sanity check to avoid problems...
  if (producerCandidates.size() < prodCount) {
	  os << "Error: not enough BACKBONE are candidates available." << endl;
	  exit (1);
  }
  if (consumerCandidates.size() < goodCount + badCount) {
	  os << "Error: not enough CONSUMER are candidates available for good and bad guys." << endl;
	  exit (1);
  }

  // 6. Place the producers randomly...
  while (producers.size () < prodCount) {
	  uint32_t node_number = rand->GetValue(0,producerCandidates.size());
	//  os << "PRODUCER: Random value between 0 and " << producerCandidates.size() << " --> " << node_number << endl;
	  Ptr<Node> node = producerCandidates.at(node_number);
	  if (producers.find(node) != producers.end ())
		  continue;
	  producers.insert (node);
	  string name = Names::FindName (node);
	  //Names::Rename (name, "producer-"+name);
	  os << "Selected BACKBONE Node is: \t " << name << endl;
  }

  // 7. Place the bad guys randomly...
  while (badGuys.size () < badCount) {
	  uint32_t node_number = rand->GetValue(0,consumerCandidates.size());
	//  os << "BADGUY: Random value between 0 and " << consumerCandidates.size() << " --> " << node_number << endl;
	  Ptr<Node> node = consumerCandidates.at(node_number);
	  if (badGuys.find(node) != badGuys.end ())
		  continue;
	  badGuys.insert (node);
	  string name = Names::FindName (node);
	  Names::Rename (name, "evil-"+name);
	  os << "Randomly selected EVIL node is: \t " << name << " ---> Adversary node for the side-channel timing attack" << endl;
  }

  // 8. Place the good guys randomly...
  while (consumers.size () < goodCount) {
	  uint32_t node_number = rand->GetValue(0,consumerCandidates.size());
	//  os << "CONSUMER: Random value between 0 and " << consumerCandidates.size() << " --> " << node_number << endl;
	  Ptr<Node> node = consumerCandidates.at (node_number);
      if (consumers.find (node) != consumers.end () ||
          badGuys.find (node) != badGuys.end ())
        continue;

      consumers.insert (node);
      consumersToAttack.push_back(node);
      string name = Names::FindName (node);
      Names::Rename (name, "good-"+name);
      os << "Randomly selected GOOD node is: \t " << name << " ---> Legitimate consumer node for a VoNDN conversation" << endl;
    }

  // 9. Network Configuration: special config for badGusy, normal config for everybody else
  NodeContainer normalNodes;
  NodeContainer specialNodes;
  for (NodeContainer::Iterator node = allNodes.Begin (); node != allNodes.End (); node++) {
	  if (badGuys.find(*node) != badGuys.end ())
		  specialNodes.Add(*node);
	  else
		  normalNodes.Add(*node);

  }
  normalNetworkConfiguration(prefix, normalNodes);
  specialNetworkConfiguration(prefix, specialNodes);

  // 10. Installing global routing interface on all nodes
  GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll();

  // place the CA entity in the a node... like a producer (a special kind of)
  AppHelper CACert ("ns3::ndn::CA_Cert");
  CACert.SetPrefix ("/ndn/domain/vondn/KEY");
  CACert.SetAttribute ("PayloadSize", StringValue("1024"));
  CACert.SetAttribute("Freshness", StringValue("100ms"));
  for (set<Ptr<Node>>::iterator node = producers.begin(); node != producers.end(); node++) {
		ApplicationContainer certApp;
		ndnGlobalRoutingHelper.AddOrigins ("/ndn/domain/vondn/KEY", *node);
		CACert.SetAttribute("Signature", UintegerValue(1));
		CACert.SetAttribute("KeyLocator", StringValue("/ndn/domain/vondn/KEY/"+Names::FindName(*node)));
		certApp.Add(CACert.Install(*node));
		//os << "Adding a good guy: install on node " << endl;
		certApp.Start(Seconds(0.0));
   }

  // 11. Application configurations: BadGuys
  AppHelper BadGuy("ns3::ndn::BadGuy");
  BadGuy.SetAttribute("Frequency", StringValue("100"));
  BadGuy.SetAttribute("BadBehavior", BooleanValue(true));
  //BadGuy.SetAttribute ("LifeTime", StringValue ("2s"));
  BadGuy.SetAttribute("SpySeq", IntegerValue(1));

  for (set<Ptr<Node>>::iterator node = badGuys.begin(); node != badGuys.end(); node++)
    {
      ApplicationContainer evilApp;
      //BadGuy.SetAttribute("SpyPrefix", StringValue("/evil"+Names::FindName (*node)));
      //BadGuy.SetAttribute("SpyPrefix", StringValue("/vondn/good-leaf-2/%FE%02%81"));
    //  BadGuy.SetAttribute("SpyPrefix", StringValue("/vondn"));

     // BadGuy.SetAttribute("SpyPrefix", StringValue("/vondn/good-leaf-1"));
     // BadGuy.SetPrefix("/vondn");

      //BadGuy.SetAttribute("SpyPrefix", StringValue("/ndn/domain/vondn/KEY"));
      BadGuy.SetAttribute("SpyPrefix", StringValue("/vondn"));
      BadGuy.SetAttribute("SpyLeafName", StringValue("good"));
      BadGuy.SetAttribute("SpyLeafCount", IntegerValue(goodCount));
      //BadGuy.SetAttribute("SpyAllNames", ObjectVectorValue(consumersToAttack));
      BadGuy.SetAttribute("BruteForce", IntegerValue(1));
      BadGuy.SetAttribute("InterestRepeat", IntegerValue(4));

      evilApp.Add (BadGuy.Install (*node));
    // evilApp.Start (Seconds (300.0));
     //evilApp.Stop (Seconds (600.0));
      evilApp.Start (Seconds (21.0) + Time::FromDouble (rand->GetValue (0, 1), Time::MS));//MS>S
      evilApp.Stop (Seconds (40.0) + Time::FromDouble (rand->GetValue (0, 1), Time::MS));//MS>S
    }

  // 12. Application configurations: Goodguys as consumers and producers in a talk...
  AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
  //consumerHelper.SetAttribute ("LifeTime", StringValue ("2s"));
  consumerHelper.SetAttribute("Frequency", StringValue("100"));
  AppHelper producerHelper ("ns3::ndn::Producer");
  producerHelper.SetAttribute ("PayloadSize", StringValue("64"));
  producerHelper.SetAttribute("Freshness", StringValue("100ms"));

  for (set<Ptr<Node>>::iterator node = consumers.begin(); node != consumers.end(); node++) {
	    // need another node to pair in for a voicre conversation, get the next one if available..
	    set<Ptr<Node>>::iterator one = node;
	    set<Ptr<Node>>::iterator two = ++node;
	    // if we don't have two, leave the loop...
	    if (two == consumers.end()) break;

	    os << "Caller first pair is: \t "  << Names::FindName(*one) << ", prefix ="<< "/vondn/" + Names::FindName(*one) + "/call-id/"<< rand->GetInteger(100000,9000000) << endl;
	    os << "Caller second pair is: \t " << Names::FindName(*two) << ", prefix ="<< "/vondn/" + Names::FindName(*two) + "/call-id/"<< rand->GetInteger(100000,9000000) << endl;
	    os << "Callee first pair is: \t "  << Names::FindName(*one) << ", prefix ="<< "/vondn/" + Names::FindName(*two) + "/call-id/" << rand->GetInteger(100000,9000000)<< endl;
	    os << "Callee second pair is: \t " << Names::FindName(*two) << ", prefix ="<< "/vondn/" + Names::FindName(*one) + "/call-id/" << rand->GetInteger(100000,9000000)<< endl;

		ApplicationContainer producerOne;
		ndnGlobalRoutingHelper.AddOrigins ("/vondn", *one); //for adversary
		ndnGlobalRoutingHelper.AddOrigins ("/vondn/" + Names::FindName(*one) + "/call-id/", *one); //burada random numara ile aranan cevap verebilir yada vermeyebilir olabilir.
		producerHelper.SetPrefix ("/vondn/" + Names::FindName(*one) + "/call-id/");
		producerHelper.SetAttribute("Signature", UintegerValue(1));
		producerHelper.SetAttribute("KeyLocator", StringValue("/ndn/domain/vondn/KEY/"+Names::FindName(*one)));

		producerOne.Add(producerHelper.Install(*one));
		//os << "Adding a good guy: install on node " << endl;
		producerOne.Start(Seconds(0.0));
		ApplicationContainer producerTwo;
		ndnGlobalRoutingHelper.AddOrigins ("/vondn", *two); //burada random numara ile aranan cevap verebilir yada vermeyebilir olabilir.
		ndnGlobalRoutingHelper.AddOrigins ("/vondn/" + Names::FindName(*two) + "/call-id/", *two);
		producerHelper.SetPrefix ("/vondn/" + Names::FindName(*two) + "/call-id/");
		producerHelper.SetAttribute("Signature", UintegerValue(1));
		producerHelper.SetAttribute("KeyLocator", StringValue("/ndn/domain/vondn/KEY/"+Names::FindName(*two)));
		producerTwo.Add(producerHelper.Install(*two));
		//os << "Adding a good guy: install on node " << endl;
		producerTwo.Start(Seconds(0.0));

		ApplicationContainer consumerOne;
		consumerHelper.SetPrefix("/vondn/" + Names::FindName(*two) + "/call-id/");
		//goodAppHelper.SetAttribute ("AvgGap", TimeValue (Seconds (1.100 / maxNonCongestionShare)));
		consumerOne.Add(consumerHelper.Install(*one));
		//os << "Adding a good guy: install on node " << endl;
		consumerOne.Start(Seconds(0.0) + Time::FromDouble(rand->GetValue(0, 1), Time::S));
		ApplicationContainer consumerTwo;
		consumerHelper.SetPrefix("/vondn/" + Names::FindName(*one)+ "/call-id/");
		//goodAppHelper.SetAttribute ("AvgGap", TimeValue (Seconds (1.100 / maxNonCongestionShare)));
		consumerTwo.Add(consumerHelper.Install(*two));
		//os << "Adding a good guy: install on node " << endl;
		consumerTwo.Start(Seconds(0.0) + Time::FromDouble(rand->GetValue(0, 1), Time::S));
   }

  // 14. Calculate and install FIBs
  ndnGlobalRoutingHelper.CalculateRoutes ();   // Calculate and install FIBs


  // 15. Output information only...

   // os << "Number of EVIL nodes: " << badCount << endl;
   //os << "Number of GOOD nodes: " << goodCount << endl;

  os << "EVIL node quantity is:  " << badGuys.size() << endl;
  for (set<Ptr<Node>>::iterator node = badGuys.begin(); node != badGuys.end(); node++) {
	  os << Names::FindName(*node) << " ";
  }
  os << endl;
  os << "GOOD node quantity is:  " << consumers.size() << endl;
  for (set<Ptr<Node>>::iterator node = consumers.begin(); node != consumers.end(); node++) {
	  os << Names::FindName(*node) << " ";
  }
  os << endl;
  os << "BACKBONE node quantity is:  " << producers.size() << endl;
  for (set<Ptr<Node>>::iterator node = producers.begin(); node != producers.end(); node++) {
	  os << Names::FindName(*node) << " ";
  }
  os << endl;


    // 	double maxNonCongestionShare = 0.8 * calculateNonCongestionFlows (goodNodes, producerNodes);
    // os << "maxNonCongestionShare   " << maxNonCongestionShare << endl;

     // saveActualGraph(graph_dot_file, NodeContainer (goodNodes, evilNodes));

     	// system (("twopi -Tpdf \"" + graph_dot_file + "\" > \"" + graph_pdf_file + "\"").c_str ());
      	 // cout << "Write effective topology graph to: " << graph_pdf_file << endl;
      	 // cout << "Max non-congestion share:   " << maxNonCongestionShare << endl;

  // 16. Trace and simulate ...


 //L3RateTracer::InstallAll (results_file, Seconds (1.0));
  // CsTracer::InstallAll(results_file, Seconds (0.1));
 AppDelayTracer::InstallAll (results_file);
 //L2RateTracer::InstallAll(results_file);
 //L3RateTracer::InstallAll(results_file);

  Simulator::Schedule (Seconds (10.0), PrintTime, Seconds (10.0), name);

  Simulator::Stop (Seconds (60.0));
  Simulator::Run ();
  Simulator::Destroy ();
  AppDelayTracer::Destroy ();
 // L3RateTracer::Destroy ();
//CsTracer::Destroy();

  cerr << "Archiving to: " << results_file << ".bz2" << endl;
  system (("rm -f \"" + results_file + ".bz2" + "\"").c_str() );
  system (("bzip2 \"" + results_file + "\"").c_str() );

  return 0;
}
