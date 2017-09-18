#include <stdlib.h>
#include <iostream>
#include <boost/optional/optional_io.hpp>
#include <boost/optional.hpp>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include "snark.hpp"
#include "utils.cpp"

using namespace libsnark;
using namespace std;

int genProof(r1cs_ppzksnark_proving_key<default_r1cs_ppzksnark_pp> provingKey_in, string proofFileName)
{
  const int unsigned _noPayments = 2;
  int unsigned _counter;
  // Initialize bit_vectors for all of the variables involved.
  bit_vector h_startBalance_bv;
  bit_vector h_endBalance_bv;
  bit_vector h_incoming_bv[_noPayments];
  bit_vector h_outgoing_bv[_noPayments];
  bit_vector r_startBalance_bv;
  bit_vector r_endBalance_bv;
  bit_vector r_incoming_bv[_noPayments];
  bit_vector r_outgoing_bv[_noPayments];

  vector<vector<unsigned long int>> publicValues = fillValuesFromfile("publicInputParameters_multi");
  h_startBalance_bv = int_list_to_bits_local(publicValues[0], 8);
  h_endBalance_bv = int_list_to_bits_local(publicValues[1], 8);
  for (_counter = 0; _counter < noPayments; _counter++)
  {
    h_incoming_bv[_counter] = int_list_to_bits_local(publicValues[_counter+2], 8);
    h_outgoing_bv[_counter] = int_list_to_bits_local(publicValues[_counter+2+_noPayments], 8);
  }

  vector<vector<unsigned long int>> privateValues = fillValuesFromfile("privateInputParameters_multi");
  r_startBalance_bv = int_list_to_bits_local(privateValues[0], 8);
  r_endBalance_bv = int_list_to_bits_local(privateValues[1], 8);
  for (_counter = 0; _counter < noPayments; _counter++)
  {
    r_incoming_bv[_counter] = int_list_to_bits_local(privateValues[_counter+2], 8);
    r_outgoing_bv[_counter] = int_list_to_bits_local(privateValues[_counter+2+_noPayments], 8);
  }

  boost::optional<libsnark::r1cs_ppzksnark_proof<libff::alt_bn128_pp>> proof = generate_payment_multi_proof<default_r1cs_ppzksnark_pp>(provingKey_in, h_startBalance_bv, h_endBalance_bv, h_incoming_bv, h_outgoing_bv, r_startBalance_bv, r_endBalance_bv, r_incoming_bv, r_outgoing_bv);

  if(proof == boost::none)
  {
    return 1;
  } else {
    stringstream proofStream;
    proofStream << proof;

    ofstream fileOut;
    fileOut.open(proofFileName);

    fileOut << proofStream.rdbuf();
    fileOut.close();
    return 0;
  }
}

int main(int argc, char *argv[])
{
  string keyFileName = "provingKey_multi";

  // Initialize the curve parameters.
  default_r1cs_ppzksnark_pp::init_public_params();

  r1cs_ppzksnark_proving_key<default_r1cs_ppzksnark_pp> provingKey_in;

  ifstream fileIn(keyFileName);
  stringstream provingKeyFromFile;
  if (fileIn) {
     provingKeyFromFile << fileIn.rdbuf();
     fileIn.close();
  }
 
  provingKeyFromFile >> provingKey_in;
 
  return genProof(provingKey_in, "proof1");
}
