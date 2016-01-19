//          Copyright Boston University SESA Group 2013 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#include "SerializeEbb.h"

EBBRT_PUBLISH_TYPE(, SerializeEbb);

void SerializeEbb::Send(ebbrt::Messenger::NetworkId nid, const char *str) {
  auto len = strlen(str) + 1;
  auto buf = ebbrt::MakeUniqueIOBuf(len);
  snprintf(reinterpret_cast<char *>(buf->MutData()), len, "%s", str);

  std::cout << "Send(): length of sent iobuf: " << buf->ComputeChainDataLength()
            << " bytes" << std::endl;

  SendMessage(nid, std::move(buf));
}

void SerializeEbb::ReceiveMessage(ebbrt::Messenger::NetworkId nid,
                              std::unique_ptr<ebbrt::IOBuf> &&buffer) {
  auto output = std::string(reinterpret_cast<const char *>(buffer->Data()));
  std::cout << "Received msg: " << nid.ToString() << ": " << output << "\n";
  mypromise.SetValue();
}

void SerializeEbb::runJob() {
  std::cout << "runJob() " << std::endl;

  std::vector<int> v;
  for(int i = 0; i < vecSize; i++)
  {
      v.push_back(i);
  }
  
  std::ostringstream ofs;
  boost::archive::text_oarchive oa(ofs);
  oa & v;
  
  //std::cout << "Sending = " << ofs.str() << std::endl;
  Send(nids[0], ofs.str().c_str());
}
