// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*- 
// vim: ts=8 sw=2 smarttab
/*
 * Ceph - scalable distributed file system
 *
 * Copyright (C) 2011 New Dream Network
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software 
 * Foundation.  See file COPYING.
 * 
 */

#ifndef CEPH_MDSFINDINOREPLY_H
#define CEPH_MDSFINDINOREPLY_H

#include "msg/Message.h"
#include "include/filepath.h"

struct MMDSFindInoReply : public Message {
  tid_t tid;
  filepath path;

  MMDSFindInoReply() : Message(MSG_MDS_FINDINOREPLY) {}
  MMDSFindInoReply(tid_t t) : Message(MSG_MDS_FINDINOREPLY), tid(t) {}

  const char *get_type_name() { return "findinoreply"; }
  void print(ostream &out) {
    out << "findinoreply(" << tid << " " << path << ")";
  }
  
  void encode_payload(CephContext *cct) {
    ::encode(tid, payload);
    ::encode(path, payload);
  }
  void decode_payload(CephContext *cct) {
    bufferlist::iterator p = payload.begin();
    ::decode(tid, p);
    ::decode(path, p);
  }
};

#endif
