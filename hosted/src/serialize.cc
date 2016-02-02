//          Copyright Boston University SESA Group 2013 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <signal.h>
#include <thread>
#include <chrono>

#include <boost/filesystem.hpp>

#include <ebbrt/Context.h>
#include <ebbrt/ContextActivation.h>
#include <ebbrt/GlobalIdMap.h>
#include <ebbrt/StaticIds.h>
#include <ebbrt/NodeAllocator.h>
#include <ebbrt/Runtime.h>

#include "SerializeEbb.h"

void run(int argc, char **argv) {
  auto bindir = boost::filesystem::system_complete(argv[0]).parent_path() /
                "/bm/SerializeEbb.elf32";

  static ebbrt::Runtime runtime;
  static ebbrt::Context c(runtime);
  ebbrt::ContextActivation activation(c);

  int num;
  if (argc == 1) {
    num = 1;
  } else {
    num = atoi(argv[1]);
  }
  std::cout << "num = " << num << std::endl;

  ebbrt::event_manager->Spawn([bindir, num]() {
    SerializeEbb::Create(num).Then([bindir, num](
        ebbrt::Future<SerializeEbbRef> f) {
      SerializeEbbRef ref = f.Get();

      std::cout << "#######################################EbbId: "
                << ref->getEbbId() << std::endl;

      ebbrt::NodeAllocator::NodeDescriptor nd =
          // default
          // (binary string path, num cpus = 2, num nodes = 2, RAM = 2)
          ebbrt::node_allocator->AllocateNode(bindir.string());

      nd.NetworkId().Then([ref](ebbrt::Future<ebbrt::Messenger::NetworkId> f) {
        ebbrt::Messenger::NetworkId nid = f.Get();
        std::cout << nid.ToString() << std::endl;
        ref->addNid(nid);
      });

      // waiting for all nodes to be initialized
      ref->waitNodes().Then([ref, num](ebbrt::Future<void> f) {
        f.Get();
        std::cout << "all nodes initialized" << std::endl;
        ebbrt::event_manager->Spawn([ref]() { ref->runJob(); });
      });

      // waiting to receive messages from all nodes
      /*ref->waitReceive().Then([ref](ebbrt::Future<void> f) {
        f.Get();
        std::cout << "runJob() ended" << std::endl;
        ebbrt::active_context->io_service_.stop();
	});*/
    });
  });

  c.Deactivate();
  c.Run();
  c.Reset();

  std::cout << "Finished" << std::endl;
}

int main(int argc, char **argv) {
  run(argc, argv);
  return 0;
}
