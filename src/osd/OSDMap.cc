// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab
/*
 * Ceph - scalable distributed file system
 *
 * Copyright (C) 2004-2006 Sage Weil <sage@newdream.net>
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software
 * Foundation.  See file COPYING.
 *
 */

#include "OSDMap.h"

#include "common/config.h"
#include "common/Formatter.h"


// ----------------------------------
// osd_info_t

void osd_info_t::dump(Formatter *f) const
{
  f->dump_int("last_clean_begin", last_clean_begin);
  f->dump_int("last_clean_end", last_clean_end);
  f->dump_int("up_from", up_from);
  f->dump_int("up_thru", up_thru);
  f->dump_int("down_at", down_at);
  f->dump_int("lost_at", lost_at);
}

void osd_info_t::encode(bufferlist& bl) const
{
  __u8 struct_v = 1;
  ::encode(struct_v, bl);
  ::encode(last_clean_begin, bl);
  ::encode(last_clean_end, bl);
  ::encode(up_from, bl);
  ::encode(up_thru, bl);
  ::encode(down_at, bl);
  ::encode(lost_at, bl);
}

void osd_info_t::decode(bufferlist::iterator& bl)
{
  __u8 struct_v;
  ::decode(struct_v, bl);
  ::decode(last_clean_begin, bl);
  ::decode(last_clean_end, bl);
  ::decode(up_from, bl);
  ::decode(up_thru, bl);
  ::decode(down_at, bl);
  ::decode(lost_at, bl);
}

ostream& operator<<(ostream& out, const osd_info_t& info)
{
  out << "up_from " << info.up_from
      << " up_thru " << info.up_thru
      << " down_at " << info.down_at
      << " last_clean_interval [" << info.last_clean_begin << "," << info.last_clean_end << ")";
  if (info.lost_at)
    out << " lost_at " << info.lost_at;
  return out;
}


// ----------------------------------
// OSDMap::Incremental

void OSDMap::Incremental::encode_client_old(bufferlist& bl) const
{
  __u16 v = 5;
  ::encode(v, bl);
  ::encode(fsid, bl);
  ::encode(epoch, bl);
  ::encode(modified, bl);
  int32_t new_t = new_pool_max;
  ::encode(new_t, bl);
  ::encode(new_flags, bl);
  ::encode(fullmap, bl);
  ::encode(crush, bl);

  ::encode(new_max_osd, bl);
  // for ::encode(new_pools, bl);
  __u32 n = new_pools.size();
  ::encode(n, bl);
  for (map<int64_t,pg_pool_t>::const_iterator p = new_pools.begin();
       p != new_pools.end();
       ++p) {
    n = p->first;
    ::encode(n, bl);
    ::encode(p->second, bl, 0);
  }
  // for ::encode(new_pool_names, bl);
  n = new_pool_names.size();
  ::encode(n, bl);
  for (map<int64_t, string>::const_iterator p = new_pool_names.begin(); p != new_pool_names.end(); ++p) {
    n = p->first;
    ::encode(n, bl);
    ::encode(p->second, bl);
  }
  // for ::encode(old_pools, bl);
  n = old_pools.size();
  ::encode(n, bl);
  for (set<int64_t>::iterator p = old_pools.begin(); p != old_pools.end(); ++p) {
    n = *p;
    ::encode(n, bl);
  }
  ::encode(new_up_client, bl);
  ::encode(new_state, bl);
  ::encode(new_weight, bl);
  // for ::encode(new_pg_temp, bl);
  n = new_pg_temp.size();
  ::encode(n, bl);
  for (map<pg_t,vector<int32_t> >::const_iterator p = new_pg_temp.begin();
       p != new_pg_temp.end();
       ++p) {
    old_pg_t opg = p->first.get_old_pg();
    ::encode(opg, bl);
    ::encode(p->second, bl);
  }
}

void OSDMap::Incremental::encode(bufferlist& bl, uint64_t features) const
{
  if ((features & CEPH_FEATURE_PGID64) == 0) {
    encode_client_old(bl);
    return;
  }

  // base
  __u16 v = 6;
  ::encode(v, bl);
  ::encode(fsid, bl);
  ::encode(epoch, bl);
  ::encode(modified, bl);
  ::encode(new_pool_max, bl);
  ::encode(new_flags, bl);
  ::encode(fullmap, bl);
  ::encode(crush, bl);

  ::encode(new_max_osd, bl);
  ::encode(new_pools, bl, features);
  ::encode(new_pool_names, bl);
  ::encode(old_pools, bl);
  ::encode(new_up_client, bl);
  ::encode(new_state, bl);
  ::encode(new_weight, bl);
  ::encode(new_pg_temp, bl);

  // extended
  __u16 ev = CEPH_OSDMAP_INC_VERSION_EXT;
  ::encode(ev, bl);
  ::encode(new_hb_up, bl);
  ::encode(new_up_thru, bl);
  ::encode(new_last_clean_interval, bl);
  ::encode(new_lost, bl);
  ::encode(new_blacklist, bl);
  ::encode(old_blacklist, bl);
  ::encode(new_up_internal, bl);
  ::encode(cluster_snapshot, bl);
}

void OSDMap::Incremental::decode(bufferlist::iterator &p)
{
  __u32 n, t;
  // base
  __u16 v;
  ::decode(v, p);
  ::decode(fsid, p);
  ::decode(epoch, p);
  ::decode(modified, p);
  if (v == 4 || v == 5) {
    ::decode(n, p);
    new_pool_max = n;
  } else if (v >= 6)
    ::decode(new_pool_max, p);
  ::decode(new_flags, p);
  ::decode(fullmap, p);
  ::decode(crush, p);

  ::decode(new_max_osd, p);
  if (v < 6) {
    new_pools.clear();
    ::decode(n, p);
    while (n--) {
      ::decode(t, p);
      ::decode(new_pools[t], p);
    }
  } else {
    ::decode(new_pools, p);
  }
  if (v == 5) {
    new_pool_names.clear();
    ::decode(n, p);
    while (n--) {
      ::decode(t, p);
      ::decode(new_pool_names[t], p);
    }
  } else if (v >= 6) {
    ::decode(new_pool_names, p);
  }
  if (v < 6) {
    old_pools.clear();
    ::decode(n, p);
    while (n--) {
      ::decode(t, p);
      old_pools.insert(t);
    }
  } else {
    ::decode(old_pools, p);
  }
  ::decode(new_up_client, p);
  ::decode(new_state, p);
  ::decode(new_weight, p);

  if (v < 6) {
    new_pg_temp.clear();
    ::decode(n, p);
    while (n--) {
      old_pg_t opg;
      ::decode_raw(opg, p);
      ::decode(new_pg_temp[pg_t(opg)], p);
    }
  } else {
    ::decode(new_pg_temp, p);
  }

  // extended
  __u16 ev = 0;
  if (v >= 5)
    ::decode(ev, p);
  ::decode(new_hb_up, p);
  if (v < 5)
    ::decode(new_pool_names, p);
  ::decode(new_up_thru, p);
  ::decode(new_last_clean_interval, p);
  ::decode(new_lost, p);
  ::decode(new_blacklist, p);
  ::decode(old_blacklist, p);
  if (ev >= 6)
    ::decode(new_up_internal, p);
  if (ev >= 7)
    ::decode(cluster_snapshot, p);
}


// ----------------------------------
// OSDMap

bool OSDMap::is_blacklisted(const entity_addr_t& a)
{
  if (blacklist.empty())
    return false;

  // this specific instance?
  if (blacklist.count(a))
    return true;

  // is entire ip blacklisted?
  entity_addr_t b = a;
  b.set_port(0);
  b.set_nonce(0);
  return blacklist.count(b);
}

void OSDMap::set_max_osd(int m)
{
  int o = max_osd;
  max_osd = m;
  osd_state.resize(m);
  osd_weight.resize(m);
  for (; o<max_osd; o++) {
    osd_state[o] = 0;
    osd_weight[o] = CEPH_OSD_OUT;
  }
  osd_info.resize(m);
  osd_addr.resize(m);
  osd_cluster_addr.resize(m);
  osd_hb_addr.resize(m);

  calc_num_osds();
}


int OSDMap::calc_num_osds()
{
  num_osd = 0;
  for (int i=0; i<max_osd; i++)
    if (osd_state[i] & CEPH_OSD_EXISTS)
      num_osd++;
  return num_osd;
}

void OSDMap::adjust_osd_weights(const map<int,double>& weights, Incremental& inc) const
{
  float max = 0;
  for (map<int,double>::const_iterator p = weights.begin();
       p != weights.end(); ++p) {
    if (p->second > max)
      max = p->second;
  }

  for (map<int,double>::const_iterator p = weights.begin();
       p != weights.end(); ++p) {
    inc.new_weight[p->first] = (unsigned)((p->second / max) * CEPH_OSD_IN);
  }
}

int OSDMap::apply_incremental(Incremental &inc)
{
  if (inc.epoch == 1)
    fsid = inc.fsid;
  else
    if (ceph_fsid_compare(&inc.fsid, &fsid) != 0) {
      return -EINVAL;
    }
  assert(inc.epoch == epoch+1);
  epoch++;
  modified = inc.modified;

  // full map?
  if (inc.fullmap.length()) {
    decode(inc.fullmap);
    return 0;
  }

  // nope, incremental.
  if (inc.new_flags >= 0)
    flags = inc.new_flags;

  if (inc.new_max_osd >= 0)
    set_max_osd(inc.new_max_osd);

  if (inc.new_pool_max != -1)
    pool_max = inc.new_pool_max;

  for (set<int64_t>::iterator p = inc.old_pools.begin();
       p != inc.old_pools.end();
       p++) {
    pools.erase(*p);
    name_pool.erase(pool_name[*p]);
    pool_name.erase(*p);
  }
  for (map<int64_t,pg_pool_t>::iterator p = inc.new_pools.begin();
       p != inc.new_pools.end();
       p++) {
    pools[p->first] = p->second;
    pools[p->first].last_change = epoch;
  }
  for (map<int64_t,string>::iterator p = inc.new_pool_names.begin();
       p != inc.new_pool_names.end();
       p++) {
    pool_name[p->first] = p->second;
    name_pool[p->second] = p->first;
  }

  for (map<int32_t,uint32_t>::iterator i = inc.new_weight.begin();
       i != inc.new_weight.end();
       i++)
    set_weight(i->first, i->second);

  // up/down
  for (map<int32_t,uint8_t>::iterator i = inc.new_state.begin();
       i != inc.new_state.end();
       i++) {
    int s = i->second ? i->second : CEPH_OSD_UP;
    if ((osd_state[i->first] & CEPH_OSD_UP) &&
	(s & CEPH_OSD_UP)) {
      osd_info[i->first].down_at = epoch;
    }
    osd_state[i->first] ^= s;
  }
  for (map<int32_t,entity_addr_t>::iterator i = inc.new_up_client.begin();
       i != inc.new_up_client.end();
       i++) {
    osd_state[i->first] |= CEPH_OSD_EXISTS | CEPH_OSD_UP;
    osd_addr[i->first] = i->second;
    if (inc.new_hb_up.empty())
      osd_hb_addr[i->first] = i->second;	//this is a backward-compatibility hack
    else
      osd_hb_addr[i->first] = inc.new_hb_up[i->first];
    osd_info[i->first].up_from = epoch;
  }
  for (map<int32_t,entity_addr_t>::iterator i = inc.new_up_internal.begin();
       i != inc.new_up_internal.end();
       i++)
    osd_cluster_addr[i->first] = i->second;
  // info
  for (map<int32_t,epoch_t>::iterator i = inc.new_up_thru.begin();
       i != inc.new_up_thru.end();
       i++)
    osd_info[i->first].up_thru = i->second;
  for (map<int32_t,pair<epoch_t,epoch_t> >::iterator i = inc.new_last_clean_interval.begin();
       i != inc.new_last_clean_interval.end();
       i++) {
    osd_info[i->first].last_clean_begin = i->second.first;
    osd_info[i->first].last_clean_end = i->second.second;
  }
  for (map<int32_t,epoch_t>::iterator p = inc.new_lost.begin(); p != inc.new_lost.end(); p++)
    osd_info[p->first].lost_at = p->second;

  // pg rebuild
  for (map<pg_t, vector<int> >::iterator p = inc.new_pg_temp.begin(); p != inc.new_pg_temp.end(); p++) {
    if (p->second.empty())
      pg_temp.erase(p->first);
    else
      pg_temp[p->first] = p->second;
  }

  // blacklist
  for (map<entity_addr_t,utime_t>::iterator p = inc.new_blacklist.begin();
       p != inc.new_blacklist.end();
       p++)
    blacklist[p->first] = p->second;
  for (vector<entity_addr_t>::iterator p = inc.old_blacklist.begin();
       p != inc.old_blacklist.end();
       p++)
    blacklist.erase(*p);

  // cluster snapshot?
  if (inc.cluster_snapshot.length()) {
    cluster_snapshot = inc.cluster_snapshot;
    cluster_snapshot_epoch = inc.epoch;
  } else {
    cluster_snapshot.clear();
    cluster_snapshot_epoch = 0;
  }

  // do new crush map last (after up/down stuff)
  if (inc.crush.length()) {
    bufferlist::iterator blp = inc.crush.begin();
    crush.decode(blp);
  }

  calc_num_osds();
  return 0;
}

// serialize, unserialize
void OSDMap::encode_client_old(bufferlist& bl) const
{
  __u16 v = 5;
  ::encode(v, bl);

  // base
  ::encode(fsid, bl);
  ::encode(epoch, bl);
  ::encode(created, bl);
  ::encode(modified, bl);

  // for ::encode(pools, bl);
  __u32 n = pools.size();
  ::encode(n, bl);
  for (map<int64_t,pg_pool_t>::const_iterator p = pools.begin();
       p != pools.end();
       ++p) {
    n = p->first;
    ::encode(n, bl);
    ::encode(p->second, bl, 0);
  }
  // for ::encode(pool_name, bl);
  n = pool_name.size();
  ::encode(n, bl);
  for (map<int64_t, string>::const_iterator p = pool_name.begin();
       p != pool_name.end();
       ++p) {
    n = p->first;
    ::encode(n, bl);
    ::encode(p->second, bl);
  }
  // for ::encode(pool_max, bl);
  n = pool_max;
  ::encode(n, bl);

  ::encode(flags, bl);

  ::encode(max_osd, bl);
  ::encode(osd_state, bl);
  ::encode(osd_weight, bl);
  ::encode(osd_addr, bl);

  // for ::encode(pg_temp, bl);
  n = pg_temp.size();
  ::encode(n, bl);
  for (map<pg_t,vector<int32_t> >::const_iterator p = pg_temp.begin();
       p != pg_temp.end();
       ++p) {
    old_pg_t opg = p->first.get_old_pg();
    ::encode(opg, bl);
    ::encode(p->second, bl);
  }

  // crush
  bufferlist cbl;
  crush.encode(cbl);
  ::encode(cbl, bl);
}

void OSDMap::encode(bufferlist& bl, uint64_t features) const
{
  if ((features & CEPH_FEATURE_PGID64) == 0) {
    encode_client_old(bl);
    return;
  }

  __u16 v = 6;
  ::encode(v, bl);

  // base
  ::encode(fsid, bl);
  ::encode(epoch, bl);
  ::encode(created, bl);
  ::encode(modified, bl);

  ::encode(pools, bl, features);
  ::encode(pool_name, bl);
  ::encode(pool_max, bl);

  ::encode(flags, bl);

  ::encode(max_osd, bl);
  ::encode(osd_state, bl);
  ::encode(osd_weight, bl);
  ::encode(osd_addr, bl);

  ::encode(pg_temp, bl);

  // crush
  bufferlist cbl;
  crush.encode(cbl);
  ::encode(cbl, bl);

  // extended
  __u16 ev = CEPH_OSDMAP_VERSION_EXT;
  ::encode(ev, bl);
  ::encode(osd_hb_addr, bl);
  ::encode(osd_info, bl);
  ::encode(blacklist, bl);
  ::encode(osd_cluster_addr, bl);
  ::encode(cluster_snapshot_epoch, bl);
  ::encode(cluster_snapshot, bl);
}

void OSDMap::decode(bufferlist& bl)
{
  __u32 n, t;
  bufferlist::iterator p = bl.begin();
  __u16 v;
  ::decode(v, p);

  // base
  ::decode(fsid, p);
  ::decode(epoch, p);
  ::decode(created, p);
  ::decode(modified, p);

  int32_t max_pools = 0;
  if (v < 4) {
    ::decode(max_pools, p);
  }
  if (v < 6) {
    pools.clear();
    ::decode(n, p);
    while (n--) {
      ::decode(t, p);
      ::decode(pools[t], p);
    }
  } else {
    ::decode(pools, p);
  }
  if (v == 5) {
    pool_name.clear();
    ::decode(n, p);
    while (n--) {
      ::decode(t, p);
      ::decode(pool_name[t], p);
    }
  } else if (v >= 6) {
    ::decode(pool_name, p);
  }
  if (v == 4 || v == 5) {
    ::decode(n, p);
    pool_max = n;
  } else if (v >= 6) {
    ::decode(pool_max, p);
  } else {
    pool_max = max_pools;
  }

  ::decode(flags, p);

  ::decode(max_osd, p);
  ::decode(osd_state, p);
  ::decode(osd_weight, p);
  ::decode(osd_addr, p);
  if (v <= 5) {
    pg_temp.clear();
    ::decode(n, p);
    while (n--) {
      old_pg_t opg;
      ::decode_raw(opg, p);
      ::decode(pg_temp[pg_t(opg)], p);
    }
  } else {
    ::decode(pg_temp, p);
  }

  // crush
  bufferlist cbl;
  ::decode(cbl, p);
  bufferlist::iterator cblp = cbl.begin();
  crush.decode(cblp);

  // extended
  __u16 ev = 0;
  if (v >= 5)
    ::decode(ev, p);
  ::decode(osd_hb_addr, p);
  ::decode(osd_info, p);
  if (v < 5)
    ::decode(pool_name, p);

  ::decode(blacklist, p);
  if (ev >= 6)
    ::decode(osd_cluster_addr, p);
  else
    osd_cluster_addr.resize(osd_addr.size());

  if (ev >= 7) {
    ::decode(cluster_snapshot_epoch, p);
    ::decode(cluster_snapshot, p);
  }

  // index pool names
  name_pool.clear();
  for (map<int64_t,string>::iterator i = pool_name.begin(); i != pool_name.end(); i++)
    name_pool[i->second] = i->first;

  calc_num_osds();
}



void OSDMap::dump_json(ostream& out) const
{
  JSONFormatter jsf(true);
  jsf.open_object_section("osdmap");
  dump(&jsf);
  jsf.close_section();
  jsf.flush(out);
}

void OSDMap::dump(Formatter *f) const
{
  f->dump_int("epoch", get_epoch());
  f->dump_stream("fsid") << get_fsid();
  f->dump_stream("created") << get_created();
  f->dump_stream("modified") << get_modified();
  f->dump_string("flags", get_flag_string());
  f->dump_string("cluster_snapshot", get_cluster_snapshot());
  f->dump_int("max_osd", get_max_osd());

  f->open_array_section("pools");
  for (map<int64_t,pg_pool_t>::const_iterator p = pools.begin(); p != pools.end(); ++p) {
    f->open_object_section("pool");
    f->dump_int("pool", p->first);
    p->second.dump(f);
    f->close_section();
  }
  f->close_section();

  f->open_array_section("osds");
  for (int i=0; i<get_max_osd(); i++)
    if (exists(i)) {
      f->open_object_section("osd_info");
      f->dump_int("osd", i);
      f->dump_int("up", is_up(i));
      f->dump_int("in", is_in(i));
      get_info(i).dump(f);
      f->dump_stream("public_addr") << get_addr(i);
      f->dump_stream("cluster_addr") << get_cluster_addr(i);
      f->dump_stream("heartbeat_addr") << get_hb_addr(i);
      f->close_section();
    }
  f->close_section();

  f->open_array_section("pg_temp");
  for (map<pg_t,vector<int> >::const_iterator p = pg_temp.begin();
       p != pg_temp.end();
       p++) {
    f->open_array_section("osds");
    for (vector<int>::const_iterator q = p->second.begin(); q != p->second.end(); ++q)
      f->dump_int("osd", *q);
    f->close_section();
  }
  f->close_section();

  f->open_array_section("blacklist");
  for (hash_map<entity_addr_t,utime_t>::const_iterator p = blacklist.begin();
       p != blacklist.end();
       p++) {
    stringstream ss;
    ss << p->first;
    f->dump_stream(ss.str().c_str()) << p->second;
  }
  f->close_section();
}

string OSDMap::get_flag_string() const
{
  string s;
  if (test_flag(CEPH_OSDMAP_NEARFULL))
    s += ",nearfull";
  if (test_flag(CEPH_OSDMAP_FULL))
    s += ",full";
  if (test_flag(CEPH_OSDMAP_PAUSERD))
    s += ",pauserd";
  if (test_flag(CEPH_OSDMAP_PAUSEWR))
    s += ",pausewr";
  if (test_flag(CEPH_OSDMAP_PAUSEREC))
    s += ",pauserec";
  if (s.length())
    s = s.erase(0, 1);
  return s;
}

struct qi {
  int item;
  int depth;
  float weight;
  qi() {}
  qi(int i, int d, float w) : item(i), depth(d), weight(w) {}
};

void OSDMap::print(ostream& out) const
{
  out << "epoch " << get_epoch() << "\n"
      << "fsid " << get_fsid() << "\n"
      << "created " << get_created() << "\n"
      << "modifed " << get_modified() << "\n";

  out << "flags " << get_flag_string() << "\n";
  if (get_cluster_snapshot().length())
    out << "cluster_snapshot " << get_cluster_snapshot() << "\n";
  out << "\n";

  for (map<int64_t,pg_pool_t>::const_iterator p = pools.begin(); p != pools.end(); ++p) {
    std::string name("<unknown>");
    map<int64_t,string>::const_iterator pni = pool_name.find(p->first);
    if (pni != pool_name.end())
      name = pni->second;
    out << "pool " << p->first
	<< " '" << name
	<< "' " << p->second << "\n";
    for (map<snapid_t,pool_snap_info_t>::const_iterator q = p->second.snaps.begin();
	 q != p->second.snaps.end();
	 q++)
      out << "\tsnap " << q->second.snapid << " '" << q->second.name << "' " << q->second.stamp << "\n";
    if (!p->second.removed_snaps.empty())
      out << "\tremoved_snaps " << p->second.removed_snaps << "\n";
  }
  out << std::endl;

  out << "max_osd " << get_max_osd() << "\n";
  for (int i=0; i<get_max_osd(); i++) {
    if (exists(i)) {
      out << "osd." << i;
      out << (is_up(i) ? " up  ":" down");
      out << (is_in(i) ? " in ":" out");
      out << " weight " << get_weightf(i);
      const osd_info_t& info(get_info(i));
      out << " " << info;
      out << " " << get_addr(i) << " " << get_cluster_addr(i) << " " << get_hb_addr(i);
      out << "\n";
    }
  }
  out << std::endl;

  for (map<pg_t,vector<int> >::const_iterator p = pg_temp.begin();
       p != pg_temp.end();
       p++)
    out << "pg_temp " << p->first << " " << p->second << "\n";

  for (hash_map<entity_addr_t,utime_t>::const_iterator p = blacklist.begin();
       p != blacklist.end();
       p++)
    out << "blacklist " << p->first << " expires " << p->second << "\n";

  // ignore pg_swap_primary
}

void OSDMap::print_osd_line(int cur, ostream& out) const
{
  out << "osd." << cur << "\t";
  if (!exists(cur))
    out << "DNE\t\t";
  else {
    if (is_up(cur))
      out << "up\t";
    else
      out << "down\t";
    out << (exists(cur) ? get_weightf(cur):0) << "\t";
  }
}

void OSDMap::print_tree(ostream& out) const
{
  out << "# id\tweight\ttype name\tup/down\treweight\n";
  set<int> touched;
  set<int> roots;
  crush.find_roots(roots);
  for (set<int>::iterator p = roots.begin(); p != roots.end(); p++) {
    list<qi> q;
    q.push_back(qi(*p, 0, crush.get_bucket_weight(*p) / (float)0x10000));
    while (!q.empty()) {
      int cur = q.front().item;
      int depth = q.front().depth;
      float weight = q.front().weight;
      q.pop_front();

      out << cur << "\t" << weight << "\t";
      for (int k=0; k<depth; k++)
	out << "\t";

      if (cur >= 0) {
	print_osd_line(cur, out);
	out << "\n";
	touched.insert(cur);
	continue;
      }

      int type = crush.get_bucket_type(cur);
      out << crush.get_type_name(type) << " " << crush.get_item_name(cur) << "\n";

      // queue bucket contents...
      int s = crush.get_bucket_size(cur);
      for (int k=s-1; k>=0; k--)
	q.push_front(qi(crush.get_bucket_item(cur, k), depth+1,
			(float)crush.get_bucket_item_weight(cur, k) / (float)0x10000));
    }
  }

  set<int> stray;
  for (int i=0; i<max_osd; i++)
    if (exists(i) && touched.count(i) == 0)
      stray.insert(i);

  if (!stray.empty()) {
    out << "\n";
    for (set<int>::iterator p = stray.begin(); p != stray.end(); ++p) {
      out << *p << "\t0\t";
      print_osd_line(*p, out);
      out << "\n";
    }
  }

}

void OSDMap::print_summary(ostream& out) const
{
  out << "e" << get_epoch() << ": "
      << get_num_osds() << " osds: "
      << get_num_up_osds() << " up, "
      << get_num_in_osds() << " in";
  if (test_flag(CEPH_OSDMAP_FULL))
    out << " full";
  else if (test_flag(CEPH_OSDMAP_NEARFULL))
    out << " nearfull";
}


void OSDMap::build_simple(CephContext *cct, epoch_t e, ceph_fsid_t &fsid,
			  int nosd, int ndom, int pg_bits, int pgp_bits, int lpg_bits)
{
  ldout(cct, 10) << "build_simple on " << num_osd
	   << " osds with " << pg_bits << " pg bits per osd, "
	   << lpg_bits << " lpg bits" << dendl;
  epoch = e;
  set_fsid(fsid);
  created = modified = ceph_clock_now(cct);

  set_max_osd(nosd);

  // pgp_num <= pg_num
  if (pgp_bits > pg_bits)
    pgp_bits = pg_bits;

  // crush map
  map<int, const char*> rulesets;
  rulesets[CEPH_DATA_RULE] = "data";
  rulesets[CEPH_METADATA_RULE] = "metadata";
  rulesets[CEPH_RBD_RULE] = "rbd";

  for (map<int,const char*>::iterator p = rulesets.begin(); p != rulesets.end(); p++) {
    int64_t pool = ++pool_max;
    pools[pool].type = pg_pool_t::TYPE_REP;
    pools[pool].size = cct->_conf->osd_pool_default_size;
    pools[pool].crush_ruleset = p->first;
    pools[pool].object_hash = CEPH_STR_HASH_RJENKINS;
    pools[pool].pg_num = nosd << pg_bits;
    pools[pool].pgp_num = nosd << pgp_bits;
    pools[pool].lpg_num = lpg_bits ? (1 << (lpg_bits-1)) : 0;
    pools[pool].lpgp_num = lpg_bits ? (1 << (lpg_bits-1)) : 0;
    pools[pool].last_change = epoch;
    if (p->first == CEPH_DATA_RULE)
      pools[pool].crash_replay_interval = cct->_conf->osd_default_data_pool_replay_window;
    pool_name[pool] = p->second;
  }

  build_simple_crush_map(cct, crush, rulesets, nosd, ndom);

  for (int i=0; i<nosd; i++) {
    set_state(i, 0);
    set_weight(i, CEPH_OSD_OUT);
  }
}

void OSDMap::build_simple_crush_map(CephContext *cct, CrushWrapper& crush,
	map<int, const char*>& rulesets, int nosd, int ndom)
{
  // new
  crush.create();

  crush.set_type_name(0, "osd");
  crush.set_type_name(1, "domain");
  crush.set_type_name(2, "pool");

  const md_config_t *conf = cct->_conf;
  int minrep = conf->osd_min_rep;
  int maxrep = conf->osd_max_rep;
  assert(maxrep >= minrep);
  if (!ndom)
    ndom = MAX(maxrep, conf->osd_max_raid_width);
  if (ndom > 1 &&
      nosd >= ndom*3 &&
      nosd > 8) {
    int ritems[ndom];
    int rweights[ndom];

    int nper = ((nosd - 1) / ndom) + 1;
    ldout(cct, 0) << ndom << " failure domains, " << nper << " osds each" << dendl;

    int o = 0;
    for (int i=0; i<ndom; i++) {
      int items[nper], weights[nper];
      int j;
      rweights[i] = 0;
      for (j=0; j<nper; j++, o++) {
	if (o == nosd) break;
	ldout(cct, 20) << "added osd." << o << dendl;
	items[j] = o;
	weights[j] = 0x10000;
	//w[j] = weights[o] ? (0x10000 - (int)(weights[o] * 0x10000)):0x10000;
	//rweights[i] += w[j];
	rweights[i] += 0x10000;
      }

      crush_bucket *domain = crush_make_bucket(CRUSH_BUCKET_STRAW, CRUSH_HASH_DEFAULT, 1, j, items, weights);
      ritems[i] = crush_add_bucket(crush.crush, 0, domain);
      ldout(cct, 20) << "added domain bucket i " << ritems[i] << " of size " << j << dendl;

      char bname[10];
      snprintf(bname, sizeof(bname), "dom%d", i);
      crush.set_item_name(ritems[i], bname);
    }

    // root
    crush_bucket *root = crush_make_bucket(CRUSH_BUCKET_STRAW, CRUSH_HASH_DEFAULT, 2, ndom, ritems, rweights);
    int rootid = crush_add_bucket(crush.crush, 0, root);
    crush.set_item_name(rootid, "root");

    // rules
    for (map<int,const char*>::iterator p = rulesets.begin(); p != rulesets.end(); p++) {
      int ruleset = p->first;
      crush_rule *rule = crush_make_rule(3, ruleset, pg_pool_t::TYPE_REP, minrep, maxrep);
      crush_rule_set_step(rule, 0, CRUSH_RULE_TAKE, rootid, 0);
      crush_rule_set_step(rule, 1, CRUSH_RULE_CHOOSE_LEAF_FIRSTN, CRUSH_CHOOSE_N, 1); // choose N domains
      crush_rule_set_step(rule, 2, CRUSH_RULE_EMIT, 0, 0);
      int rno = crush_add_rule(crush.crush, rule, -1);
      crush.set_rule_name(rno, p->second);
    }

  } else {
    // one bucket

    int items[nosd];
    int weights[nosd];
    for (int i=0; i<nosd; i++) {
      items[i] = i;
      weights[i] = 0x10000;
    }

    crush_bucket *b = crush_make_bucket(CRUSH_BUCKET_STRAW, CRUSH_HASH_DEFAULT, 1, nosd, items, weights);
    int rootid = crush_add_bucket(crush.crush, 0, b);
    crush.set_item_name(rootid, "root");

    // replication
    for (map<int,const char*>::iterator p = rulesets.begin(); p != rulesets.end(); p++) {
      int ruleset = p->first;
      crush_rule *rule = crush_make_rule(3, ruleset, pg_pool_t::TYPE_REP, conf->osd_min_rep, maxrep);
      crush_rule_set_step(rule, 0, CRUSH_RULE_TAKE, rootid, 0);
      crush_rule_set_step(rule, 1, CRUSH_RULE_CHOOSE_FIRSTN, CRUSH_CHOOSE_N, 0);
      crush_rule_set_step(rule, 2, CRUSH_RULE_EMIT, 0, 0);
      int rno = crush_add_rule(crush.crush, rule, -1);
      crush.set_rule_name(rno, p->second);
    }

  }

  crush.finalize();

  ldout(cct, 20) << "crush max_devices " << crush.crush->max_devices << dendl;
}

