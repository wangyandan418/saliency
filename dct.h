
class spiking_dct  {

  // neuron group handles
  int in0E, dct2plus, dct2minus;

  // connection handles
  int cid_dct2plus, cid_dct2minus;

  std::vector<float> fr_dct2p,fr_dct2m;
  
  // spike monitor handles
  SpikeMonitor* in0E_mon;
  SpikeMonitor* dct2p_mon;
  SpikeMonitor* dct2m_mon;

 public:

// constructor
  spiking_dct(CARLsim *sim) {

  // configure the network
  // separate layers for pos and neg consne weights, give pos and neg partitioned result
  in0E =sim->createSpikeGeneratorGroup("IN0E", Grid3D(sz_R,sz_C), EXCITATORY_NEURON);
  dct2plus =sim->createGroup("dct2plus",   Grid3D(sz_R,sz_C), EXCITATORY_NEURON);
  dct2minus=sim->createGroup("dct2minus",  Grid3D(sz_R,sz_C), EXCITATORY_NEURON);

  sim->setNeuronParameters(dct2plus,  0.1f, 0.2f, -65.0f, 2.0f);
  sim->setNeuronParameters(dct2minus, 0.1f, 0.2f, -65.0f, 2.0f);

  // constructor for custion pattern connector object
  MyConnection dct2DPattern(in0E,dct2plus,dct2minus, sz_R,sz_C);
  
  // make the connections here
  int cid_dct2plus  = sim->connect(in0E,       dct2plus,   &dct2DPattern, gain_AMPA, gain_NMDA, SYN_FIXED, 0);
  int cid_dct2minus = sim->connect(in0E,       dct2minus,  &dct2DPattern, gain_AMPA, gain_NMDA, SYN_FIXED, 0);
  
  // use coba neurons
  sim->setConductances(true);
  sim->setIntegrationMethod(FORWARD_EULER, 2);
  
  // ---------------- SETUP STATE -------------------
  // build the network
  sim->setupNetwork();

  // set up some monitors
  in0E_mon   = sim->setSpikeMonitor(in0E     ,"DEFAULT");
  dct2p_mon  = sim->setSpikeMonitor(dct2plus ,"DEFAULT");
  dct2m_mon  = sim->setSpikeMonitor(dct2minus,"DEFAULT");
 }

  void rundct(CARLsim *sim, vector<float> inrates) {

   PoissonRate rate_inE(sz_R*sz_C,1);
//   PoissonRate rate_inE(sz_R*sz_C,0);
   rate_inE.setRates(inrates);
   sim->setSpikeRate(in0E,&rate_inE);

   // run the first stage of the pipeline
   in0E_mon -> startRecording();
   dct2p_mon -> startRecording();
   dct2m_mon -> startRecording();

   //sim->runNetwork(1,500);
   sim->runNetwork(1,500);

   in0E_mon -> stopRecording();
   dct2p_mon -> stopRecording();
   dct2m_mon -> stopRecording();

   fr_dct2p = dct2p_mon->getAllFiringRates();
   fr_dct2m = dct2m_mon->getAllFiringRates();

   //reset the rates
   //rate_inE.setRates(0.0f);
   //sim->setSpikeRate(in0E,&rate_inE);                     // need to call setSpikeRate again
   //sim->runNetwork(0,10);

   //~PoissonRate();
}

vector<float> getpos()
{
  return(fr_dct2p);
}

vector<float> getneg()
{
   return(fr_dct2m);
}
};
