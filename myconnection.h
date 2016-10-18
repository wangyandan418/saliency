
// this is the custom ConnectionGenerator
// for the dct interconnections.. this is really clunky
class MyConnection : public ConnectionGenerator {
  
  int L1_id, L2_id, L3_id;
  int sizeR,sizeC;
  float **weights_01;

 public:
  MyConnection(int ID1, int ID2, int ID3,  int rows, int cols) { 
    L1_id = ID1; // input
    L2_id = ID2; // dctplis
    L3_id = ID3; // dctminus

    // need to know the geometry to resolive nod IDs
    sizeR = rows;
    sizeC = cols;
    
    weights_01 = new float*[sizeR];
    for (int i = 0; i<sizeR; i++) weights_01[i] = new float[sizeC];
      
    // set up the weights array.. 
    float incr_01 = M_PI /(2. * (float) sizeC);
    float incr_12 = M_PI /(2. * (float) sizeR);
    float c1_01 = 1./sqrt(sizeC);
    float c1_12 = 1./sqrt(sizeR);
    float c2_01 = sqrt(2./(float) sizeC);
    float c2_12 = sqrt(2./(float) sizeR);

    // for the weight array
    // j is the presynaptic neuron
    // i is the post synaptic neuron
    // compute the weights for C_01
    for (int i=0; i<sizeR; i++) 
    	for (int j=0; j<sizeC; j++)
    	{
    		float rads = incr_01 * (float) ((2*j+1)*i);
    		if (i == 0)
    			weights_01[i][j] = (float) cos(rads)*c1_01;
	        else
	        	weights_01[i][j] = (float) cos(rads)*c2_01;
        }
    dumpWeights();
  }
  ~MyConnection() {}
    
  void dumpWeights() {

    std::ofstream fd_wt01;
    fd_wt01.open("results/weights_01.csv");

    for (int i = 0; i<sizeR; i++) {
      for (int j=0; j<(sizeC-1); j++)
      {
    	  fd_wt01  << weights_01[i][j] << ",";
      }
      fd_wt01  << weights_01[i][sizeC-1] <<  "\n";
    }
    fd_wt01.close();
  }

  // the pure virtual function inherited from base class
  // note that weight, maxWt, delay, and connected are passed by reference
  void connect(CARLsim* sim, int srcGrp, int i, int destGrp, int j, float& weight, float& maxWt,
	       float& delay, bool& connected) {
  
    
    // make it 2d
    int src_row = i/sizeC;
    int src_col = i%sizeC;
    int dest_row = j/sizeC;
    int dest_col = j%sizeC;


    // connection setups
    /**********************  dct2 layers ******************************************/
    // input to dct2 positive cosines layer
    if ((srcGrp == L1_id) && (destGrp == L2_id)) { // con_01 (why not use the connection id???)
      weight = (weights_01[dest_row][src_row]*weights_01[dest_col][src_col]);
      if(weight > 0) 
	connected = true;
      else {
	connected = false;
        weight = 0.;
      }
      maxWt = 1.0f;
      delay = 1;
      //cout <<"+"<< weight<<std::endl;
      //if(i==2303)
      //	  cout << std::endl;
      }


    // input to dct2 negative cosines layer
    else if ((srcGrp == L1_id) && (destGrp == L3_id)) { // con_01 (why not use the connection id???)
//      weight = (weights_01[dest_row][src_row]*weights_01[dest_col][src_col]);
      weight = (weights_01[dest_row][src_row]*weights_01[dest_col][src_col]);
      if(weight < 0) {
	connected = true;
	weight = -weight;
      }
      else {
	connected = false;
        weight = 0;
      }
      maxWt = 10.f;
      delay = 1;
      //cout << "_"<<weight<<std::endl;
      //if(j==2303)
      //   cout << std::endl;
    }

    else
      printf("*Connectivity setup error*\nsrcGrp %d srcID %d destGrp %d destID %d - %d %d %d\n",
	     srcGrp,i,destGrp,j,L1_id,L2_id,L3_id);
  }
};

