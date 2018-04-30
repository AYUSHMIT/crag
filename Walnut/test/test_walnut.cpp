#include <gtest/gtest.h>

#include "walnut.h"

namespace crag {
namespace walnut {
namespace {

using ZZ5 = finitefield::ZZ<5>;
using GF32 = finitefield::FieldElement<finitefield::IdealGeneratedByPolynomial<finitefield::ZZ<2>, 1, 0, 1, 0, 0, 1>>;
using GF256 =
    finitefield::FieldElement<finitefield::IdealGeneratedByPolynomial<finitefield::ZZ<2>, 1, 1, 0, 1, 1, 0, 0, 0, 1>>;

TEST(Walnut, Encoding_1) {
  EXPECT_EQ(Word({1, 1}), encode(0));
  EXPECT_EQ(Word({1, 2}), encode(1));

  EXPECT_EQ(Word({4, 4, 3, 3, 3}), encode(0b01111010));

  EXPECT_EQ(Word({4, 4, 3, 3, 3, 2, 1, 1}), encode({0b01111010, 0b00010100}));
}

TEST(Walnut, Encoding_2) {
  std::vector<uint8_t> message = {0, 1};
  EXPECT_EQ(Word({1, 1, 1, 2}), encode(message));

  EXPECT_EQ(Word({7, 7, 7, 7, 7, 7, 7, 6, 5, 4, 3, 2, 2, -3, -4, -5, -6, -7}), encode(8, message, {7, 2, 1, 5}));
}

TEST(Walnut, Encoding_3) {
  const size_t n = 8;

  std::mt19937_64 g(0);

  // 256 random bits
  const auto msg_hash = randomMessageHash(256, g);

  const auto encoded_msg = encode(n, msg_hash, random::subsetVector<size_t>(4, 1, n - 1, g));

  const auto sigma = coloredburau::permutation(n, encoded_msg);

  EXPECT_TRUE(sigma.isTrivial());
}

TEST(Walnut, FreePureBraidSubgroupGen) {
  EXPECT_EQ(Word({6, 6}), getFreePureBraidSubgroupGen(7, 6));
  EXPECT_EQ(Word({6, 5, 4, 4, -5, -6}), getFreePureBraidSubgroupGen(7, 4));
  EXPECT_EQ(Word({6, 5, 4, 3, 2, 1, 1, -2, -3, -4, -5, -6}), getFreePureBraidSubgroupGen(7, 1));
}

TEST(Walnut, Sign_1) {
  using FF = ZZ5;
  const size_t n = 11;

  // generate random parameters using seed = 0
  const auto random_parameters = randomParameters<FF, StabilizerSquare>(n, 0);
  const auto random_encoder = randomEncoder(n, 512, 0);
  const auto stabilizer = StabilizerSquare(random_parameters, 15);

  const auto protocol = getProtocol(random_parameters, random_encoder, stabilizer);

  // generate random private key using seed = 123
  const auto private_key = protocol.generatePrivateKey(123);
  const auto public_key = protocol.computePublicKey(private_key);

  for (size_t i = 0; i < 5; ++i) {
    // generate random message hash using seed = i + 10
    const auto random_msg_hash = randomMessageHash(256, i + 10);

    // generate random signature using seed = i
    const auto signature = protocol.sign(random_msg_hash, private_key, i);

    EXPECT_TRUE(protocol.verify(signature, public_key));
  }
}

TEST(Walnut, Sign_2) {
  using FF = GF256;
  const size_t n = 11;

  // generate random parameters using seed = 0
  const auto random_parameters = randomParameters<FF, StabilizerDoubleSquare>(n, 0);
  const auto random_encoder = randomEncoder(n, 512, 0);
  const auto stabilizer = StabilizerDoubleSquare(random_parameters, 30);

  const auto protocol = getProtocol(random_parameters, random_encoder, stabilizer);

  // generate random private key using seed = 123
  const auto private_key = protocol.generatePrivateKey(123);
  const auto public_key = protocol.computePublicKey(private_key);

  for (size_t i = 0; i < 5; ++i) {
    // generate random message hash using seed = i + 10
    const auto random_msg_hash = randomMessageHash(512, i + 10);

    // generate random signature using seed = i
    const auto signature = protocol.sign(random_msg_hash, private_key, i);

    EXPECT_TRUE(protocol.verify(signature, public_key));
  }
}
} // namespace
} // namespace walnut
} // namespace crag
