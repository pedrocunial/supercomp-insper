#include <iostream>
#include <cstdlib>
#include <boost/mpi.hpp>

#include "parser.hpp"

#define MASTER 0

namespace mpi = boost::mpi;

/*
  mpiexec -n 2 ./aps02 <file01> <file02> <ngram-size> <result-size>
                       n files (e.g. 2)
 */


int main(int argc, char **argv)
{
  mpi::environment env(argc, argv);
  mpi::communicator world;
  auto size = world.size();
  auto rank = world.rank();
  if (argc < size + 3) {
    std::cout << "[ERR] Too few arguments " << argc << std::endl;
    return 1;
  } else if (argc > size + 3) {
    std::cout << "[ERR] Too many arguments " << argc << std::endl;
    return 1;
  }

#ifdef DEBUG
  if (rank == MASTER) {
    puts("I'm the master");
    for (auto i=1; i<=size; i++) {
      std::cout << argv[i] << std::endl;
    }
  } else {
    puts("I'm not the master");
  }
#endif

  Parser *parser = new Parser(argv[rank+1],
                              std::strtoul(argv[argc-2],
                                           nullptr, 0));
  parser->run();

  puts("Generating text...");
  std::string text = parser->generate_text(std::strtoul(argv[argc-1], nullptr, 0));
  puts("DONE!\nGenerated text:");
  if (rank == MASTER)
    std::cout << text << std::endl;

  delete parser;
  return 0;
}
