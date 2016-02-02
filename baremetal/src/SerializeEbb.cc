#include "SerializeEbb.h"

EBBRT_PUBLISH_TYPE(, SerializeEbb);

using namespace ebbrt;

void AppMain() {}

void SerializeEbb::Send(const char *str) {
  auto len = strlen(str) + 1;
  auto buf = ebbrt::MakeUniqueIOBuf(len);
  snprintf(reinterpret_cast<char *>(buf->MutData()), len, "%s", str);

  SendMessage(remote_nid_, std::move(buf));
}

struct membuf : std::streambuf {
  membuf(char *begin, char *end) { this->setg(begin, begin, end); }
};

void SerializeEbb::ReceiveMessage(ebbrt::Messenger::NetworkId nid,
                                  std::unique_ptr<ebbrt::IOBuf> &&buffer) {
    //auto output = std::string(reinterpret_cast<const char *>(buffer->Data()));
  //ebbrt::kprintf("reached while loop\n");
  //int a = 1;
  //while (a) {
  //}
  ebbrt::kprintf("Received %d bytes\n", buffer->ComputeChainDataLength());
  //ebbrt::IOBuf::DataPointer dp = buffer->GetDataPointer();
  //auto output = std::string(
  //    reinterpret_cast<const char *>(dp.Get(buffer->ComputeChainDataLength())));

  //std::vector<int> v;
  //std::istringstream ifs;
  //ifs.str(output);
  //boost::archive::text_iarchive ia(ifs);
  //ia &v;

  std::string tt = "abc";
  Send(tt.c_str());
}

SerializeEbb &SerializeEbb::HandleFault(ebbrt::EbbId id) {
  {
    ebbrt::LocalIdMap::ConstAccessor accessor;
    auto found = ebbrt::local_id_map->Find(accessor, id);
    if (found) {
      auto &pr = *boost::any_cast<SerializeEbb *>(accessor->second);
      ebbrt::EbbRef<SerializeEbb>::CacheRef(id, pr);
      return pr;
    }
  }

  ebbrt::EventManager::EventContext context;
  auto f = ebbrt::global_id_map->Get(id);
  SerializeEbb *p;
  f.Then([&f, &context, &p, id](ebbrt::Future<std::string> inner) {
    p = new SerializeEbb(ebbrt::Messenger::NetworkId(inner.Get()), id);
    ebbrt::event_manager->ActivateContext(std::move(context));
  });
  ebbrt::event_manager->SaveContext(context);
  auto inserted = ebbrt::local_id_map->Insert(std::make_pair(id, p));
  if (inserted) {
    ebbrt::EbbRef<SerializeEbb>::CacheRef(id, *p);
    return *p;
  }

  delete p;
  // retry reading
  ebbrt::LocalIdMap::ConstAccessor accessor;
  ebbrt::local_id_map->Find(accessor, id);
  auto &pr = *boost::any_cast<SerializeEbb *>(accessor->second);
  ebbrt::EbbRef<SerializeEbb>::CacheRef(id, pr);
  return pr;
}
