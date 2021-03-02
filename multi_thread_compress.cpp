#include <string.h>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <pthread.h>
#include "zlib.h"
#include <vector>
#include <iomanip>
#include <chrono>


#define BLOCK 4000

//tested on linux subsystem on windows 10
//tested on a i7-5960 CPU @ 3.00GHz
//with 64GBs RAM


//compress the individual blocks
//on the separate threads
void* compress(void* thread_args)
{
  char tmp[BLOCK];
  char *input = (char *) thread_args;

  z_stream stream;
  stream.zalloc = Z_NULL;
  stream.zfree  = Z_NULL;
  stream.opaque = Z_NULL;

  stream.avail_in = (uInt)strlen(input)+1;
  stream.next_in = (Bytef *)input;
  stream.avail_out = (uInt)BLOCK;
  stream.next_out = (Bytef *)tmp;

  deflateInit(&stream, Z_BEST_COMPRESSION);
  deflate(&stream, Z_FINISH);
  deflateEnd(&stream);

  pthread_exit(tmp);
}

//decompress the indivual blocks
//on the separate threads
std::string decompress(char* input)
{
  char out[BLOCK];

  z_stream stream;
  stream.zalloc = Z_NULL;
  stream.zfree  = Z_NULL;
  stream.opaque = Z_NULL;

  stream.avail_in = (uInt)strlen(input)+1;
  stream.next_in = (Bytef *)input;
  stream.avail_out = (uInt) BLOCK;
  stream.next_out = (Bytef *) out;

  inflateInit(&stream);
  inflate(&stream, Z_NO_FLUSH);
  inflateEnd(&stream);

  return std::string(out);
}

//read in a file and determine if it needs the additional
//decompression block file
std::vector<std::vector<char>> getFile(std::string filename, std::string type)
{
  std::ifstream file(filename, std::ios_base::in);
  std::vector<std::vector<char>> test;

  if(type == "-d")
  {
    std::ifstream decomp("Decompress_Block.txt", std::ios_base::in);
    if(file.is_open() && decomp.is_open())
    {
      int blockSize;
      while(decomp >> blockSize)
      {
        char buffer[blockSize];
        file.read(buffer, blockSize);
        test.push_back(std::vector<char>(buffer, buffer+blockSize));

      }
      decomp.close();
      file.close();
    }
  }
  else if(type == "-c")
  {
    if(file.is_open())
    {
      char buffer[BLOCK];
      while(file.read(buffer, BLOCK))
      {
        test.push_back(std::vector<char>(buffer, buffer+BLOCK));
      }
      test.push_back(std::vector<char>(buffer, buffer+file.gcount()));

      file.close();
    }
  }

  return test;
}


//compress the file that was read in
std::vector<std::string> compressFile(int nThreads, std::vector<std::vector<char>> file)
{
  std::ofstream out_file("Decompress_Block.txt", std::ios::trunc | std::ios_base::out);
  std::vector<std::string> output;
  if(out_file.is_open())
  {
    char comp[BLOCK];
    pthread_t threads[nThreads];
    for(int i = 0; i < file.size(); i+=nThreads)
    {
      if((file.size()-i) < nThreads)
      {
        nThreads = (file.size()-i);
      }
      void *vptr;
      for(int j = 0; j < nThreads; j++)
      {
        pthread_create(&threads[j], NULL, compress, (void *) reinterpret_cast<char*>(file[i+j].data()));
      }
      for(int j = 0; j < nThreads; j++)
      {
        pthread_join(threads[j], &vptr);
        std::string s((char *)vptr);
        output.push_back(s);
        out_file << s.length() << std::endl;
      }
    }
    out_file.close();
  }

  return output;
}


//decompress a file that was read in
std::vector<std::string> decompressFile(std::vector<std::vector<char>> file)
{

  std::vector<std::string> output;

  for(int i = 0; i < file.size(); i++)
  {
    std::string s = decompress(reinterpret_cast<char*>(file[i].data()));
    output.push_back(s);
  }

  return output;
}

//write the file to the output
void writeFile(std::vector<std::string> output, std::string filename)
{
  std::ofstream out_file(filename, std::ios::trunc | std::ios_base::out);
  if(out_file.is_open())
  {
    for(int i = 0; i < output.size(); i++)
    {
      out_file << output[i];
    }
    out_file.close();
  }
}

//read the input buffer and block
//the stream into 4KB sizes and
//then thread the blocks
int main(int argc, char *argv[])
{
  //argv[1] = -c (compress) or -d (decompress)
  //argv[2] = # of threads
  //argv[3] = input file name
  //argv[4] = output file name
  auto start = std::chrono::high_resolution_clock::now();
  if(argc == 5)
  {
    if(std::string(argv[1]) == "-d")
    {
      std::vector<std::vector<char>> file = getFile(std::string(argv[3]), std::string(argv[1]));
      std::vector<std::string> output = decompressFile(file);

      writeFile(output, std::string(argv[4]));
    }
    else if(std::string(argv[1]) == "-c")
    {
      std::vector<std::vector<char>> file = getFile(std::string(argv[3]), std::string(argv[1]));
      std::vector<std::string> output = compressFile(std::stoi(std::string(argv[2])), file);
      writeFile(output, std::string(argv[4]));
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
