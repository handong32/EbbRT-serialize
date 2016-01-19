#ifndef SERIALIZE_EBB_H_
#define SERIALIZE_EBB_H_

#include <ebbrt/LocalIdMap.h>
#include <ebbrt/GlobalIdMap.h>
#include <ebbrt/EbbRef.h>
#include <ebbrt/SharedEbb.h>
#include <ebbrt/Message.h>
#include <ebbrt/UniqueIOBuf.h>
#include <ebbrt/EbbRef.h>
#include <ebbrt/IOBuf.h>
#include <ebbrt/Messenger.h>
//#include <ebbrt/SpinBarrier.h>

#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

using namespace ebbrt;

class SerializeEbb : public ebbrt::Messagable<SerializeEbb> {

  EbbId ebbid;

  // this is used to save and load context
  ebbrt::EventManager::EventContext *emec{ nullptr };

  // a void promise where we want to start some computation after some other
  // function has finished
  ebbrt::Promise<void> mypromise;

public:
  explicit SerializeEbb(ebbrt::Messenger::NetworkId nid, EbbId _ebbid)
      : Messagable<SerializeEbb>(_ebbid), remote_nid_(std::move(nid)) {
    ebbid = _ebbid;
  }

  static SerializeEbb &HandleFault(ebbrt::EbbId id);

  // this uses the void promise to return a future that will
  // get invoked when mypromise gets set with some value
  ebbrt::Future<void> waitReceive() { return std::move(mypromise.GetFuture()); }

  void Send(const char *str);

  void ReceiveMessage(ebbrt::Messenger::NetworkId nid,
                      std::unique_ptr<ebbrt::IOBuf> &&buffer);

  EbbId getEbbId() { return ebbid; }

  // void runJob(ebbrt::Messenger::NetworkId);

  void destroy() { delete this; }

private:
  ebbrt::Messenger::NetworkId remote_nid_;
};

#endif
