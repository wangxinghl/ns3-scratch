#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

void generateTopoLevel2(ofstream &fout, char** argv)
{
  int numSw_top, numSw_bottom, hostNum, numHost, ctrlPos;

  numSw_top = atoi(argv[2]);      // Get the number of switch at top layer
  numSw_bottom = atoi(argv[3]);   // Get the number of switch at bottom layer
  hostNum = atoi(argv[4]);        // Get the number of host connected to each switch
  ctrlPos = atoi(argv[5]);        // Get the id of host which will install the controller server

  numHost = hostNum * numSw_bottom;   // Calculate the total number of hosts

  // Generate topology
  fout << "Layer "<< 2 << endl;
  fout << "numHost " << numHost << endl;
  fout << "Controller " << ctrlPos << endl;
  fout << "numSw_bottom "<< numSw_bottom << endl;
  fout << "numSw_top "<< numSw_top << endl << endl;
  

  int linkNum = 0;

  // Links between switch and switch
  for (int i = 0; i < numSw_bottom; ++i)
  {
    for (int j = 0; j < numSw_top; ++j)
    {
      fout << linkNum << " " << numHost + i << " " << numHost + numSw_bottom + j << endl;
      ++linkNum;    // Update link id
    }
  }
  // Links between host and switch
  for (int i = 0; i < numSw_bottom; ++i)
  {
    for (int j = 0; j < hostNum; ++j)
    {
      fout << linkNum << " " << i * hostNum + j << " " << numHost + i << endl;
      ++linkNum;    // Update link id
    }
  }

  // Comments
  fout << endl << endl;
  fout << "**********************explain for 2-layer topology*********************" << endl;
  fout << " the 1st line is the layer of topology" << endl;
  fout << " the 2rd line is the number of hosts" << endl;
  fout << " the 3th line is the id of host which will install the controller server" << endl;
  fout << " the 4th line is the number of switches at bottom layer" << endl;
  fout << " the 5th line is the number of switches at top layer" << endl;
  fout << " the 6th line is empty" << endl;
  fout << " the following lines describe the links on the network(id, src, dst)" << endl;
  fout << "***********************************************************************" << endl;
}


void generateTopoLevel3(ofstream &fout, char **argv)
{
  int numCoreSw, numPOD, numSwPOD, numSwitchPOD, hostNum, numHost, ctrlPos;

  numCoreSw = atoi(argv[2]);    // Get the number of core switch
  numPOD = atoi(argv[3]);       // Get the number of POD
  numSwPOD = atoi(argv[4]);     // Get the number of switch in each POD
  hostNum = atoi(argv[5]);      // Get the number of host connected to each switch
  ctrlPos = atoi(argv[6]);      // Get the id of host which will install the controller server
  
  numSwitchPOD = numSwPOD / 2 ;   // Calculate the number of switch at each layer in POD
  numHost = hostNum * numPOD * numSwitchPOD ;  // Calculate the total number of host

  // Generate topology
  fout << "Layer " << 3 << endl;
  fout << "numCoreSw " << numCoreSw << endl;
  fout << "numPOD " << numPOD << endl;
  fout << "numSwPOD " << numSwPOD << endl;
  fout << "numHost " << numHost << endl;
  fout << "Controller " << ctrlPos << endl << endl;
  
  int linkNum = 0;

  // Links between POD switch and core switch
  for (int i = 0; i < numSwitchPOD * numPOD; ++i)
  {
    for (int j = i % numSwitchPOD % 2; j < numCoreSw; j += 2)
    {
      fout << linkNum << " " << numHost + numSwitchPOD * numPOD + i << " " << numHost + numSwPOD * numPOD + j << endl;
      ++linkNum;  // Update link id
    }
  }
  // Links between switch and switch in POD
  for (int i = 0; i < numPOD; ++i)
  {
    for (int j = 0; j < numSwitchPOD; ++j)
    {
      for (int k = 0; k < numSwitchPOD; ++k)
      {
        fout << linkNum << " " << numHost + numSwitchPOD * i + j << " " << numHost + numSwitchPOD * numPOD + numSwitchPOD * i + k << endl;
        ++linkNum;  // Update link id
      }
    }
  }
  // Links between host and switch in PDO
  for (int i = 0; i < numPOD * numSwitchPOD; ++i)
  {
    for (int j = 0; j < hostNum; ++j)
    {
      fout << linkNum << " " << hostNum * i + j << " " << numHost + i << endl;
      ++linkNum;
    }
  }

  // Comments
  fout << endl << endl;
  fout << "**********************explain for 3-layer topology*********************" << endl;
  fout << " the 1st line is the layer of topology" << endl;
  fout << " the 2nd line is the number of core switches" << endl;
  fout << " the 3rd line is the number of PODs" << endl;
  fout << " the 4th line is the number of switches in each POD" << endl;
  fout << " the 5rd line is the number of hosts" << endl;
  fout << " the 6th line is the id of host which will install the controller server" << endl;
  fout << " the 7th line is empty" << endl;
  fout << " the following lines describe the links on the network(id, src, dst)" << endl;
  fout << "***********************************************************************" << endl;

}

int main(int argc, char** argv)
{
  ofstream fout("inputFile.txt");   // Set output topo file

  int level = atoi(argv[1]);   // Reading topo parameter

  if (level == 2)
    generateTopoLevel2(fout, argv);
  else if (level == 3)
    generateTopoLevel3(fout, argv);
  else
    cout << "Error";

  return 0;
}
