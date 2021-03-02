#include <string.h>
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <fstream>
#include <pthread.h>
#include "zlib.h"
#include <chrono>

#define BLOCK 4000

//tested on linux subsystem on windows 10
//tested on a i7-5960 CPU @ 3.00GHz
//with 64GBs RAM



//compress the individual blocks
//on the separate threads
void compress(char (&input)[BLOCK], char (&output)[BLOCK])
{
  z_stream stream;
  stream.zalloc = Z_NULL;
  stream.zfree  = Z_NULL;
  stream.opaque = Z_NULL;

  stream.avail_in = (uInt)strlen(input)+1;
  stream.next_in = (Bytef *)input;
  stream.avail_out = (uInt)BLOCK;
  stream.next_out = (Bytef *)output;

  deflateInit(&stream, Z_BEST_COMPRESSION);
  deflate(&stream, Z_FINISH);
  deflateEnd(&stream);
}

//decompress the indivual blocks
//on the separate threads
void decompress(char* input, char (&output)[BLOCK])
{
  z_stream stream;
  stream.zalloc = Z_NULL;
  stream.zfree  = Z_NULL;
  stream.opaque = Z_NULL;

  stream.avail_in = (uInt)strlen(input)+1;
  stream.next_in = (Bytef *)input;
  stream.avail_out = (uInt)BLOCK;
  stream.next_out = (Bytef *)output;

  inflateInit(&stream);
  inflate(&stream, Z_NO_FLUSH);
  inflateEnd(&stream);
}

//read in a file
void compressFile(std::string inputName, std::string outputName)
{
  std::ifstream file(inputName, std::ios_base::in);
  std::ofstream out_file(outputName, std::ios::trunc | std::ios_base::out);
  std::ofstream dec_file("Decompress_Block.txt", std::ios::trunc | std::ios_base::out);
  if(file.is_open() && out_file.is_open() && dec_file.is_open())
  {
    char buffer[BLOCK];
    char comp[BLOCK];
    while(file.read(buffer, BLOCK))
    {
      compress(buffer, comp);
      out_file << comp;
      dec_file << strlen(comp) << std::endl;
    }

    compress(buffer, comp);
    out_file.write(comp, (file.gcount()));
    dec_file << file.gcount();

    file.close();
    out_file.close();
    dec_file.close();
  }
}


void decompressFile(std::string inputName, std::string outputName)
{
  std::ifstream file(inputName, std::ios_base::in);
  std::ifstream decomp("Decompress_Block.txt", std::ios_base::in);
  std::ofstream out_file(outputName, std::ios::trunc | std::ios_base::out);
  if(file.is_open() && out_file.is_open() && decomp.is_open())
  {
    int size;
    while(decomp >> size)
    {
      char buffer[size];
      char comp[BLOCK];
      file.read(buffer, size);
      decompress(buffer, comp);
      out_file << comp;
    }
    decomp.close();
    file.close();
    out_file.close();
  }
}

//read the input buffer and block
//the stream into 4KB sizes and
//then thread the blocks
int main(int argc, char *argv[])
{
  //argv[1] = -c (compress) or -d (decompress)
  //argv[2] = input file name
  //argv[3] = output file name
  auto start = std::chrono::high_resolution_clock::now();
  if(argc == 4)
  {
    if(std::string(argv[1]) == "-d")
    {
      decompressFile(std::string(argv[2]), std::string(argv[3]));
    }
    else if(std::string(argv[1]) == "-c")
    {
      compressFile(std::string(argv[2]), std::string(argv[3]));
    }

  }
  auto stop = std::chrono::high_resolution_clock::now();
  double duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();
  duration /= 1e6;

  std::cout << "Compression Runtime: " << std::fixed << duration << std::setprecision(9);
  std::cout << "sec" << std::endl;

  /*
  char in[BLOCK] = "This is a test sting to make sure compress works.";

  std::cout << "INPUT Size: " << strlen(in) << std::endl;
  std::cout << "INPUT: " << in << std::endl;

  char out[BLOCK];
  compress(in, out);

  std::cout << "COMPRESS Size: " << strlen(out) << std::endl;
  std::cout << "COMPRESSED: " << out << std::endl;

  char decomp[BLOCK];
  decompress(out, decomp);

  std::cout << "DECOMPRESS Size: " << strlen(decomp) << std::endl;
  std::cout << "DECOMPRESSED: " << decomp << std::endl;
  */

}
