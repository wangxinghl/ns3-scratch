#include <time.h>
#include <stdlib.h>
#include "ns3/log.h"
#include "ns3/applications-module.h"
#include "ns3/openflow-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("RunSimulation");  // the define of log component

void GetFlow(std::vector<Flow_t> &flow, std::map<uint16_t, uint16_t> &serverPorts,
             uint32_t numflow, uint16_t numhost, uint64_t bandwidth, ns3::Time simuTime)
{
  flow.resize(numflow);
  uint64_t rate_unit = bandwidth / numflow;

  srand(time(NULL));

  for (uint32_t i = 0; i < numflow; ++i) {
    flow[i].idx = i;

    flow[i].src = rand() % numhost;
    flow[i].dst = rand() % numhost;
    while (flow[i].src == flow[i].dst) {
      flow[i].dst = rand() % numhost;
    }

    if (serverPorts.find(flow[i].dst) == serverPorts.end()) {
      serverPorts[flow[i].dst] = 1 + rand() % 100;  // server port: 1-100
    }
    flow[i].port = serverPorts[flow[i].dst];

    flow[i].rate = rate_unit + rand() % rate_unit;

    uint64_t totalByte = flow[i].rate / 8 * simuTime.GetSeconds();  // bit ---> byte
    // UdpHeader: 8, Ipv4Header: 20, EthernetHeader: 18
    flow[i].size = 954;    // 1000-8-20-18=954 
    flow[i].total = totalByte / flow[i].size;
    flow[i].interval = simuTime.GetMicroSeconds() / flow[i].total;    // unit: us
  }
}

int main(int argc, char const *argv[])
{

  double simuTime = 10.0;
  std::string bandwidth = "100Mbps";
  uint32_t flowNum = 10;


  if (true)
  {
    LogComponentEnable ("RunSimulation", LOG_LEVEL_INFO);
    // LogComponentEnable ("Topology", LOG_LEVEL_FUNCTION);
    // LogComponentEnable ("SimpleController", LOG_LEVEL_FUNCTION);
    // LogComponentEnable ("UdpClient", LOG_LEVEL_INFO);
    // LogComponentEnable ("UdpServer", LOG_LEVEL_INFO);
    // LogComponentEnable ("ControllerServer", LOG_LEVEL_DEBUG);
    // LogComponentEnable ("OpenFlowSwitchNetDevice", LOG_LEVEL_WARN);
    // LogComponentEnable ("CsmaNetDevice", LOG_LEVEL_WARN);
    // LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
    // LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
  }

  // Build Topology
  NS_LOG_INFO ("Build Topology");
  Ptr<Topology> topo = Create<Topology> ();
  topo->BuildTopo("/home/wx/ns-allinone-3.25/ns-3.25/scratch/inputFile.txt", Seconds(simuTime), bandwidth);

  // Get flow
  NS_LOG_INFO ("Get Flow");
  std::vector<Flow_t> flow;
  std::map<uint16_t, uint16_t> serverPorts;
  GetFlow(flow, serverPorts, flowNum, topo->m_numHost, topo->GetBandwidth(), Seconds(simuTime) - Seconds(1));
  
  std::ofstream fout("scratch/default-flow.txt");
  fout << flow.size() << std::endl;
  for (uint32_t i = 0; i < flow.size(); ++i) {
    fout << flow[i].src << " " << flow[i].dst << " " << flow[i].port << " " << flow[i].rate << " "
         << flow[i].size << " " << flow[i].total << " " << flow[i].interval << std::endl; 
  }
  fout << "\n" << serverPorts.size() << std::endl;
  for (std::map<uint16_t, uint16_t>::iterator it = serverPorts.begin(); it != serverPorts.end(); it++) {
    fout << it->first << " " << it->second << std::endl;
  }
  fout.close();

  // Install Application
  NS_LOG_INFO("Install Application");
  // for UdpServer
  for (std::map<uint16_t, uint16_t>::iterator it = serverPorts.begin(); it != serverPorts.end(); it++) {
    UdpServerHelper udpServer(it->second);
    
    ApplicationContainer serverApps = udpServer.Install (topo->GetNode (it->first));
    serverApps.Start (Seconds (0));
    serverApps.Stop (Seconds (simuTime));
  }
  // for UdpClient
  for (uint32_t i = 0; i < flow.size(); ++i) {
    UdpClientHelper udpClient (Ipv4Address (topo->m_ips[flow[i].dst]), flow[i].port);
    udpClient.SetAttribute ("MaxPackets", UintegerValue (flow[i].total));
    udpClient.SetAttribute ("Interval", TimeValue (MicroSeconds (flow[i].interval)));
    udpClient.SetAttribute ("PacketSize", UintegerValue (flow[i].size));

    ApplicationContainer clientApps = udpClient.Install (topo->GetNode (flow[i].src));
    clientApps.Start (Seconds (0.5));
    clientApps.Stop (Seconds (simuTime));
  }

  // Start Simulation
  NS_LOG_INFO ("Run Simulation");
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO("Done");
  return 0;
}