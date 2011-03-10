#include <errno.h>
#include <time.h>

#include "include/types.h"
#include "auth/Crypto.h"

#include "gtest/gtest.h"

TEST(AES, ValidateSecret) {
  CryptoHandler *h = ceph_crypto_mgr.get_crypto(CEPH_CRYPTO_AES);
  int l;

  for (l=0; l<16; l++) {
    bufferptr bp(l);
    int err;
    err = h->validate_secret(bp);
    EXPECT_EQ(-EINVAL, err);
  }

  for (l=16; l<50; l++) {
    bufferptr bp(l);
    int err;
    err = h->validate_secret(bp);
    EXPECT_EQ(0, err);
  }
}

TEST(AES, Encrypt) {
  CryptoHandler *h = ceph_crypto_mgr.get_crypto(CEPH_CRYPTO_AES);
  char secret_s[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
  };
  bufferptr secret(secret_s, sizeof(secret_s));

  char plaintext_s[] = {
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
  };
  bufferlist plaintext;
  plaintext.append(plaintext_s, sizeof(plaintext_s));

  bufferlist cipher;
  int success;
  success = h->encrypt(secret, plaintext, cipher);
  ASSERT_NE(false, success);
  // C defines booleans as non-zero, so the above is sort of enough
  // and more correct, but let's be strict to avoid confusion in
  // callers
  ASSERT_EQ(true, success);

  char want_cipher[] = {
    0xb3, 0x8f, 0x5b, 0xc9, 0x35, 0x4c, 0xf8, 0xc6,
    0x13, 0x15, 0x66, 0x6f, 0x37, 0xd7, 0x79, 0x3a,
    0x11, 0x90, 0x7b, 0xe9, 0xd8, 0x3c, 0x35, 0x70,
    0x58, 0x7b, 0x97, 0x9b, 0x03, 0xd2, 0xa5, 0x01,
  };
  char cipher_s[sizeof(want_cipher)];

  ASSERT_EQ(sizeof(cipher_s), cipher.length());
  cipher.copy(0, sizeof(cipher_s), &cipher_s[0]);

  int err;
  err = memcmp(cipher_s, want_cipher, sizeof(want_cipher));
  ASSERT_EQ(0, err);
}

TEST(AES, Decrypt) {
  CryptoHandler *h = ceph_crypto_mgr.get_crypto(CEPH_CRYPTO_AES);
  char secret_s[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
  };
  bufferptr secret(secret_s, sizeof(secret_s));

  char cipher_s[] = {
    0xb3, 0x8f, 0x5b, 0xc9, 0x35, 0x4c, 0xf8, 0xc6,
    0x13, 0x15, 0x66, 0x6f, 0x37, 0xd7, 0x79, 0x3a,
    0x11, 0x90, 0x7b, 0xe9, 0xd8, 0x3c, 0x35, 0x70,
    0x58, 0x7b, 0x97, 0x9b, 0x03, 0xd2, 0xa5, 0x01,
  };
  bufferlist cipher;
  cipher.append(cipher_s, sizeof(cipher_s));

  char want_plaintext[] = {
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
  };
  char plaintext_s[sizeof(want_plaintext)];

  bufferlist plaintext;
  int err;
  err = h->decrypt(secret, cipher, plaintext);
  ASSERT_EQ((int)sizeof(want_plaintext), err);

  ASSERT_EQ(sizeof(plaintext_s), plaintext.length());
  plaintext.copy(0, sizeof(plaintext_s), &plaintext_s[0]);

  err = memcmp(plaintext_s, want_plaintext, sizeof(want_plaintext));
  ASSERT_EQ(0, err);
}

TEST(AES, Benchmark) {
  int err;

  struct timespec before;
  struct timespec after;

  err = clock_gettime(CLOCK_MONOTONIC, &before);
  ASSERT_EQ(0, err);

  char secret_s[16];
  err = get_random_bytes(secret_s, sizeof(secret_s));
  ASSERT_EQ(0, err);
  bufferptr secret(secret_s, sizeof(secret_s));

  char orig_plaintext_s[1024];
  err = get_random_bytes(orig_plaintext_s, sizeof(orig_plaintext_s));
  ASSERT_EQ(0, err);

  bufferlist plaintext;
  plaintext.append(orig_plaintext_s, sizeof(orig_plaintext_s));

  for (int i=0; i<10000; i++) {
    bufferlist cipher;
    {
      CryptoHandler *h = ceph_crypto_mgr.get_crypto(CEPH_CRYPTO_AES);

      int success;
      success = h->encrypt(secret, plaintext, cipher);
      ASSERT_NE(false, success);
    }
    plaintext.clear();

    {
      CryptoHandler *h = ceph_crypto_mgr.get_crypto(CEPH_CRYPTO_AES);
      int err;
      err = h->decrypt(secret, cipher, plaintext);
      ASSERT_EQ((int)sizeof(orig_plaintext_s), err);
    }
  }

  err = clock_gettime(CLOCK_MONOTONIC, &after);
  ASSERT_EQ(0, err);

  char plaintext_s[sizeof(orig_plaintext_s)];
  plaintext.copy(0, sizeof(plaintext_s), &plaintext_s[0]);
  err = memcmp(plaintext_s, orig_plaintext_s, sizeof(orig_plaintext_s));
  ASSERT_EQ(0, err);

  // 64 bits of nanoseconds a lot, but nothing guarantees what if any
  // epoch CLOCK_MONOTONIC has; shift measurements closer to 0 epoch
  ASSERT_LE(before.tv_sec, after.tv_sec);
  after.tv_sec -= before.tv_sec;
  before.tv_sec = 0;

  u_int64_t before_ns = before.tv_sec*1000000000 + before.tv_nsec;
  u_int64_t after_ns = after.tv_sec*1000000000 + after.tv_nsec;
  ASSERT_LE(before_ns, after_ns);
  u_int64_t duration_ns = after_ns - before_ns;

  // my desktop machine completes this in ~200ms with CryptoPP, ~750ms
  // right now for NSS (too slow!), this allows for 1s before flagging
  // a problem; not really robust but need something to detect really
  // severe regressions
  EXPECT_LT(duration_ns, 1000000000u);

  RecordProperty("durationnanoseconds", duration_ns);
}