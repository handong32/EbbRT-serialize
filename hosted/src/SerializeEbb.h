//          Copyright Boston University SESA Group 2013 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#ifndef SERIALIZE_EBB_H_
#define SERIALIZE_EBB_H_

#include <string>
#include <iostream>
#include <vector>
#include <chrono>

#include <boost/serialization/vector.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <ebbrt/Message.h>
#include <ebbrt/StaticSharedEbb.h>

#include <ebbrt/LocalIdMap.h>
#include <ebbrt/GlobalIdMap.h>
#include <ebbrt/EbbRef.h>
#include <ebbrt/SharedEbb.h>
#include <ebbrt/Message.h>
#include <ebbrt/UniqueIOBuf.h>
#include <ebbrt/EbbRef.h>
#include <ebbrt/IOBuf.h>
#include <ebbrt/Messenger.h>
#include <ebbrt/SpinBarrier.h>

using namespace ebbrt;

class SerializeEbb;
typedef EbbRef<SerializeEbb> SerializeEbbRef;

class SerializeEbb : public ebbrt::SharedEbb<SerializeEbb>,
                     public ebbrt::Messagable<SerializeEbb> {

  EbbId ebbid;
  int counter;
  std::vector<ebbrt::Messenger::NetworkId> nids;
  int vecSize;
  std::chrono::high_resolution_clock::time_point start;

  // this is used to save and load context
  ebbrt::EventManager::EventContext *emec{ nullptr };

  // a void promise where we want to start some computation after some other
  // function has finished
  ebbrt::Promise<void> mypromise;
  ebbrt::Promise<void> nodesinit;

public:
  SerializeEbb(EbbId _ebbid, int num) : Messagable<SerializeEbb>(_ebbid) {
    ebbid = _ebbid;
    counter = 0;
    nids.clear();
    vecSize = num;
    start = std::chrono::high_resolution_clock::now();
  }

  static ebbrt::Future<SerializeEbbRef> Create(int num) {
    auto id = ebbrt::ebb_allocator->Allocate();
    auto ebbref =
        SharedEbb<SerializeEbb>::Create(new SerializeEbb(id, num), id);

    // returns a future for the EbbRef
    return ebbrt::global_id_map
        ->Set(id, ebbrt::messenger->LocalNetworkId().ToBytes())
        // the Future<void> f is returned by the ebbrt::global_id_map
        .Then([ebbref](ebbrt::Future<void> f) {
          // f.Get() ensures the gobal_id_map has completed
          f.Get();
          return ebbref;
        });
  }

  // this uses the void promise to return a future that will
  // get invoked when mypromose gets set with some value
  ebbrt::Future<void> waitReceive() { return std::move(mypromise.GetFuture()); }

  // FIXME
  ebbrt::Future<void> waitNodes() { return std::move(nodesinit.GetFuture()); }

  void Send(ebbrt::Messenger::NetworkId nid, const char *str);

  void ReceiveMessage(ebbrt::Messenger::NetworkId nid,
                      std::unique_ptr<ebbrt::IOBuf> &&buffer);

  // adds a NID to vector and sets promise
  void addNid(ebbrt::Messenger::NetworkId nid) {
    nids.push_back(nid);
    nodesinit.SetValue();
  }

  EbbId getEbbId() { return ebbid; }

  void runJob();

  void destroy() { delete this; }
};

#endif
