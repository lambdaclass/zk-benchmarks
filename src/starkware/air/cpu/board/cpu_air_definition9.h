// Copyright 2023 StarkWare Industries Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License").
// You may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// https://www.starkware.co/open-source-license/
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions
// and limitations under the License.

#ifndef STARKWARE_AIR_CPU_BOARD_CPU_AIR9_H_
#define STARKWARE_AIR_CPU_BOARD_CPU_AIR9_H_

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "third_party/gsl/gsl-lite.hpp"

#include "starkware/air/air.h"
#include "starkware/air/compile_time_optional.h"
#include "starkware/air/components/ecdsa/ecdsa.h"
#include "starkware/air/components/trace_generation_context.h"
#include "starkware/air/cpu/board/memory_segment.h"
#include "starkware/air/cpu/component/cpu_component.h"
#include "starkware/air/trace.h"
#include "starkware/algebra/elliptic_curve/elliptic_curve.h"
#include "starkware/algebra/elliptic_curve/elliptic_curve_constants.h"
#include "starkware/algebra/polymorphic/field_element.h"
#include "starkware/crypt_tools/hash_context/pedersen_hash_context.h"
#include "starkware/error_handling/error_handling.h"
#include "starkware/math/math.h"

namespace starkware {
namespace cpu {

template <typename FieldElementT>
class CpuAirDefinition<FieldElementT, 9> : public Air {
 public:
  using FieldElementT_ = FieldElementT;
  using Builder = typename CompositionPolynomialImpl<CpuAirDefinition>::Builder;

  std::unique_ptr<CompositionPolynomial> CreateCompositionPolynomial(
      const FieldElement& trace_generator,
      const ConstFieldElementSpan& random_coefficients) const override;

  uint64_t GetCompositionPolynomialDegreeBound() const override {
    return kConstraintDegree * TraceLength();
  }

  std::vector<std::pair<int64_t, uint64_t>> GetMask() const override;

  uint64_t NumRandomCoefficients() const override { return kNumConstraints; }

  uint64_t NumColumns() const override { return kNumColumns; }

  std::vector<std::vector<FieldElementT>> PrecomputeDomainEvalsOnCoset(
      const FieldElementT& point, const FieldElementT& generator,
      gsl::span<const uint64_t> point_exponents, gsl::span<const FieldElementT> shifts) const;

  FractionFieldElement<FieldElementT> ConstraintsEval(
      gsl::span<const FieldElementT> neighbors, gsl::span<const FieldElementT> periodic_columns,
      gsl::span<const FieldElementT> random_coefficients, const FieldElementT& point,
      gsl::span<const FieldElementT> shifts, gsl::span<const FieldElementT> precomp_domains) const;

  std::vector<FieldElementT> DomainEvalsAtPoint(
      gsl::span<const FieldElementT> point_powers, gsl::span<const FieldElementT> shifts) const;

  TraceGenerationContext GetTraceGenerationContext() const;

  void BuildAutoPeriodicColumns(const FieldElementT& gen, Builder* builder) const;

  virtual void BuildPeriodicColumns(const FieldElementT& gen, Builder* builder) const = 0;

  std::optional<InteractionParams> GetInteractionParams() const override {
    InteractionParams interaction_params{kNumColumnsFirst, kNumColumnsSecond, 6};
    return interaction_params;
  }

  static constexpr uint64_t kNumColumnsFirst = 8;
  static constexpr uint64_t kNumColumnsSecond = 3;

  static constexpr uint64_t kPublicMemoryStep = 16;
  static constexpr bool kHasDilutedPool = true;
  static constexpr uint64_t kDilutedSpacing = 4;
  static constexpr uint64_t kDilutedNBits = 16;
  static constexpr uint64_t kPedersenBuiltinRatio = 256;
  static constexpr uint64_t kPedersenBuiltinRepetitions = 1;
  static constexpr uint64_t kRcBuiltinRatio = 8;
  static constexpr uint64_t kRcNParts = 8;
  static constexpr uint64_t kEcdsaBuiltinRatio = 2048;
  static constexpr uint64_t kEcdsaBuiltinRepetitions = 1;
  static constexpr uint64_t kEcdsaElementBits = 251;
  static constexpr uint64_t kEcdsaElementHeight = 256;
  static constexpr uint64_t kBitwiseRatio = 16;
  static constexpr uint64_t kBitwiseTotalNBits = 251;
  static constexpr uint64_t kEcOpBuiltinRatio = 1024;
  static constexpr uint64_t kEcOpScalarHeight = 256;
  static constexpr uint64_t kEcOpNBits = 252;
  static constexpr uint64_t kKeccakRatio = 2048;
  static constexpr uint64_t kPoseidonRatio = 256;
  static constexpr uint64_t kPoseidonM = 3;
  static constexpr uint64_t kPoseidonRoundsFull = 8;
  static constexpr uint64_t kPoseidonRoundsPartial = 83;
  static constexpr std::array<uint64_t, 2> kPoseidonRPPartition = {64, 22};
  static constexpr std::array<std::array<FieldElementT, 3>, 3> kPoseidonMds = {
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(0x3_Z), FieldElementT::ConstexprFromBigInt(0x1_Z),
          FieldElementT::ConstexprFromBigInt(0x1_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(0x1_Z), -FieldElementT::ConstexprFromBigInt(0x1_Z),
          FieldElementT::ConstexprFromBigInt(0x1_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(0x1_Z), FieldElementT::ConstexprFromBigInt(0x1_Z),
          -FieldElementT::ConstexprFromBigInt(0x2_Z)}};
  static constexpr std::array<std::array<FieldElementT, 3>, 91> kPoseidonArk = {
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x6861759ea556a2339dd92f9562a30b9e58e2ad98109ae4780b7fd8eac77fe6f_Z),
          FieldElementT::ConstexprFromBigInt(
              0x3827681995d5af9ffc8397a3d00425a3da43f76abf28a64e4ab1a22f27508c4_Z),
          FieldElementT::ConstexprFromBigInt(
              0x3a3956d2fad44d0e7f760a2277dc7cb2cac75dc279b2d687a0dbe17704a8309_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x626c47a7d421fe1f13c4282214aa759291c78f926a2d1c6882031afe67ef4cd_Z),
          FieldElementT::ConstexprFromBigInt(
              0x78985f8e16505035bd6df5518cfd41f2d327fcc948d772cadfe17baca05d6a6_Z),
          FieldElementT::ConstexprFromBigInt(
              0x5427f10867514a3204c659875341243c6e26a68b456dc1d142dcf34341696ff_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x5af083f36e4c729454361733f0883c5847cd2c5d9d4cb8b0465e60edce699d7_Z),
          FieldElementT::ConstexprFromBigInt(
              0x7d71701bde3d06d54fa3f74f7b352a52d3975f92ff84b1ac77e709bfd388882_Z),
          FieldElementT::ConstexprFromBigInt(
              0x603da06882019009c26f8a6320a1c5eac1b64f699ffea44e39584467a6b1d3e_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x4332a6f6bde2f288e79ce13f47ad1cdeebd8870fd13a36b613b9721f6453a5d_Z),
          FieldElementT::ConstexprFromBigInt(
              0x53d0ebf61664c685310a04c4dec2e7e4b9a813aaeff60d6c9e8caeb5cba78e7_Z),
          FieldElementT::ConstexprFromBigInt(
              0x5346a68894845835ae5ebcb88028d2a6c82f99f928494ee1bfc2d15eaabfebc_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x550a9e24176509ea7631ccaecb7a4ab8694ab61f238797098147e69dd91e5a3_Z),
          FieldElementT::ConstexprFromBigInt(
              0x219dcccb783b1cbaa62773fedd3570e0f48ad3ed77c8b262b5794daa2687000_Z),
          FieldElementT::ConstexprFromBigInt(
              0x4b085eb1df4258c3453cc97445954bf3433b6ab9dd5a99592864c00f54a3f9a_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x53e8a8e8a404c503af2bf3c03e420ea5a465939d04b6c72e2da084e5aabb78d_Z),
          FieldElementT::ConstexprFromBigInt(
              0x5ca045c1312c09d1bd14d2537fe5c19fb4049cb137faf5df4f9ada962be8ca8_Z),
          FieldElementT::ConstexprFromBigInt(
              0x7c74922a456802c44997e959f27a5b06820b1ed97596a969939c46c162517f4_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0xc0bba6880d2e686bf5088614b9684ff2526a20f91670435dc6f519bb7ab83f_Z),
          FieldElementT::ConstexprFromBigInt(
              0x4526bcaec43e8ebd708dd07234c1b2dc1a6203741decd72843849cd0f87934a_Z),
          FieldElementT::ConstexprFromBigInt(
              0x1cc9a17b00d3607d81efaea5a75a434bef44d92edc6d5b0bfe1ec7f01d613ed_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x28b1e269b84c4012aa8cdbead0bc1ce1eb7284e2b28ed90bc7b4a4fde8f01f_Z),
          FieldElementT::ConstexprFromBigInt(
              0x62af2f41d76c4ad1d9a2482fbdaf6590c19656bcb945b58bb724dc7a994498d_Z),
          FieldElementT::ConstexprFromBigInt(
              0x5cfd7e44946daa6b2618213b0d1bf4a2269bed2dc0d4dbf59e285eee627df1a_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x7ff2afb40f3300856fdd1b94da8d3bbcf0312ab9f16ac9bc31955dc8386a747_Z),
          FieldElementT::ConstexprFromBigInt(
              0x5cd236bdc15b54183e90bab8ae37f8aab40efae6fa9cd919b3248ee326e929c_Z),
          FieldElementT::ConstexprFromBigInt(
              0x5463841390e22d60c946418bf0e5822bd999084e30688e741a90bbd53a698a_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x24c940fff3fe8c8b2021f13eb4d71747efd44a4e51890ae8226e7406144f805_Z),
          FieldElementT::ConstexprFromBigInt(
              0x4e50cb07b3873268dc88f05393d9d03153ca4c02172dd1d7fc77d45e1b04555_Z),
          FieldElementT::ConstexprFromBigInt(
              0x62ca053e4da0fc87b430e53238d2bab1d9b499c35f375d7d0b32e1189b6dcb5_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x719f20ac59d1ebcaaf37fe0b851bc2419cd89100adff965951bff3d3d7e1191_Z),
          FieldElementT::ConstexprFromBigInt(
              0x7645ca5e87a9f916a82fe5bb90807f44050ac92ca52f5c798935cf47d55a8fd_Z),
          FieldElementT::ConstexprFromBigInt(
              0x15b8aeaca96ab53200eed38d248ecda23d4b71d17133438015391ca63663767_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x53d94dbbca7cb2aa8252f106292ac3b98799e908f928c196c1b658bf10b2e2_Z),
          FieldElementT::ConstexprFromBigInt(
              0x28f90b403e240f1c6f4c0a3b70edbb3942b447c615c0f033913831c34de2d1e_Z),
          FieldElementT::ConstexprFromBigInt(
              0x2485167dc233ba6e1161c4d0bf025159699dd2feb36e3e5b70ae6e770e22081_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x1c8b08a90d6ee46ff7de548541dd26988f7fdaacdd58698e938607a5feca6e8_Z),
          FieldElementT::ConstexprFromBigInt(
              0x105c3bf5cba256466b75e79d146f9880c7c4df5ecdad643ce05b16901c4881e_Z),
          FieldElementT::ConstexprFromBigInt(
              0x238019787f4cc0b627a65a21bef2106d5015b85dfbd77b2965418b02dbc6bd7_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x15e624d7698fdf9b73dce29a5f24c465c15b52dec8172923a6ebc99a6ddc5e1_Z),
          FieldElementT::ConstexprFromBigInt(
              0x5d3688ba56f34fdf56bc056ad8bf740ca0c2efef23b04a479f612fde5800a0a_Z),
          FieldElementT::ConstexprFromBigInt(
              0x229abdef3fef7ae9e67ed336e82dc6c2e26d872d98b3cce811c69ae363b444d_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x3e8096ecfcbcde2ee400801a56f236db2c43d1e33c92b57ac58daf2d3fc44db_Z),
          FieldElementT::ConstexprFromBigInt(
              0x3ad5fec670d7039108d605aae834c7ce6a7cd4e1b47bf6a02265352c57db9bd_Z),
          FieldElementT::ConstexprFromBigInt(
              0x7cf4598c0cf143875877afdbb4df6794ef597fff1f98557adca32046aeaef0a_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x58aecc0081b55134a4d1c4c8f27932e4170c37841fef49aca0ec7a123c00ad6_Z),
          FieldElementT::ConstexprFromBigInt(
              0x757b4b7ee98e0a15460b71995790396e4ef3c859db5b714ec09308d65d2ca61_Z),
          FieldElementT::ConstexprFromBigInt(
              0x6b82800937f8981f3cd974f43322169963d2b54fd2b7ed348dc6cc226718b5d_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x3a915b1814707273427e34ab8fbb7ca044f14088fedae9606b34a60b1e9c64_Z),
          FieldElementT::ConstexprFromBigInt(
              0x54afbf1bd990043f9bc01028ff44195c0bb609d367b76269a627689547bfbef_Z),
          FieldElementT::ConstexprFromBigInt(
              0x5e1ceb846fe1422b9524c7d014931072c3852df2d991470b08375edf6e762bb_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x7f751f98968212ebe5dff3ce06e8cb916709e0c48e3020c6b2b01c1bec0814b_Z),
          FieldElementT::ConstexprFromBigInt(
              0x36f6b64463f7c29fc3180616e340536bea7f01d226b68b6d45cd6dfbff811e4_Z),
          FieldElementT::ConstexprFromBigInt(
              0x61135c9846faf39b4511d74fe8de8b48dd4d0e469d6703d7ed4fe4fe8e0dbac_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0xb58921a3fbdbb559b78f6acfca9a21a4ba83cc6e0ae3527fbaad907fc912b8_Z),
          FieldElementT::ConstexprFromBigInt(
              0x22a4f8a5cdc7474b9d16b61c2973847211d84eb2fb27b816e52821c2e2b1b1e_Z),
          FieldElementT::ConstexprFromBigInt(
              0x41cf6db5d6145edfeccbbc9a50b2ceedeb1765c61516ffcb112f810ad67036f_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0xbe44689973db2b1cfc05fa8f4aec6fac6a0ff2fdfab744ade9de11416b6831_Z),
          FieldElementT::ConstexprFromBigInt(
              0x39bf209c4e117e16489cda45128096d6d148a237142dc4951df0b8239be148b_Z),
          FieldElementT::ConstexprFromBigInt(
              0x209cf541e5f74fc2b93310b8ce37b092a58282643860b5707c7eb980ea03a06_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x6b562e6005f34ee0bdc218ba681b6ba7232e122287036d18c22dd5afa95326d_Z),
          FieldElementT::ConstexprFromBigInt(
              0xe8103a23902be5dc6d5f59253a627a2a39c8aca11a914670e7a35dea38c8f_Z),
          FieldElementT::ConstexprFromBigInt(
              0x6a3725548c664fd06bdc1b4d5f9bed83ef8ca7468d68f4fbbf345de2d552f72_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x67fcd6997472e8e605d0f01a8eccc5f11a45c0aa21eb4ebb447b4af006a4a37_Z),
          FieldElementT::ConstexprFromBigInt(
              0x26144c95c8de3634075784d28c06c162a44366f77792d4064c95db6ecb5cff0_Z),
          FieldElementT::ConstexprFromBigInt(
              0x5b173c8b0eb7e9c4b3a874eb6307cda6fd875e3725061df895dc1466f350239_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x7e1c2d6fde8ac9f87bae06ad491d391c448f877e53298b6370f2165c3d54ddb_Z),
          FieldElementT::ConstexprFromBigInt(
              0x4db779f3e5b7424996f451b156fe4e28f74d61e7771f9e3fa433b57ca6627a9_Z),
          FieldElementT::ConstexprFromBigInt(
              0xbb930d8a6c6583713435ec06b6fed7825c3f71114acb93e240eed6970993dd_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x4472d73b2830565d708467e9296fb5599d3a08814c31c4189e9579c046e878f_Z),
          FieldElementT::ConstexprFromBigInt(
              0x7ba9c303dfee2d89e10e3c883ca5ce5614d23739b7cb2052cc23612b11170e2_Z),
          FieldElementT::ConstexprFromBigInt(
              0x21c0e3319ede47f0425dc9b2c1ed30e6356cb133e97579b822548eb9c4dc4b7_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x2cfd61139e50ddd37b09933816e2a0932e53b7dc4f4947565c1d41e877eb191_Z),
          FieldElementT::ConstexprFromBigInt(
              0x5abea18941a4976844544d92ee0eca65bdd10b3f170b0dc2f30acd37e26d8e7_Z),
          FieldElementT::ConstexprFromBigInt(
              0x77088fdb015c7947a6265e44fef6f724ea28ae28b26e6eee5a751b7ce6bcc21_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x3abdc9d677231325b3e3c43cfd443076b4ce33cddbc8446120dce84e6122b73_Z),
          FieldElementT::ConstexprFromBigInt(
              0x2250f430b7fe7d12e5d00b6b83e52a52ca94879ccfab81a7a602662c2d62c4d_Z),
          FieldElementT::ConstexprFromBigInt(
              0x5c92ef479c11bb51fb24ef76d57912b12660e7bd156d6cabbb1efb79a25861b_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x235ec597391648b510f616fa8b87900fd08fd4208a785cffcf784a63a0fd5c6_Z),
          FieldElementT::ConstexprFromBigInt(
              0x4ed4e872eb7e736207be77e9d11e38f396b5c0ba3376e855523c00b372cc668_Z),
          FieldElementT::ConstexprFromBigInt(
              0x5f9406febca3879b756ef3f6331890b3d46afa705908f68fb7d861c4f275a1b_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x1d9c501d9ff1fba621a9f61b68873c05f17b0384661f06d97edf441abdaa49d_Z),
          FieldElementT::ConstexprFromBigInt(
              0x4b0de22bbd0a58534982c8e28d2f6e169e37ba694774c4dfa530f41c535952e_Z),
          FieldElementT::ConstexprFromBigInt(
              0x1b4d48bd38a3f8602186aabb291eca0d319f0e3648b2574c49d6fd1b033d903_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x7558bbea55584bf1725d8aa67ddba626b6596bbd2f4e65719702cefcead4bab_Z),
          FieldElementT::ConstexprFromBigInt(
              0x1108f1a9500a52f561ea174600e266a70b157d56ece95b60a44cf7a3eef17be_Z),
          FieldElementT::ConstexprFromBigInt(
              0x8913d96a4f36b12becb92b4b6ae3f8c209fb90caab6668567289b67087bf60_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x6502262c51ad8f616926346857dec8cca2e99f5742b6bf223f4d8a6f32867a6_Z),
          FieldElementT::ConstexprFromBigInt(
              0x7cb5fcdc00892812889280505c915bde962ea034378b343cd3a5931d2ec0e52_Z),
          FieldElementT::ConstexprFromBigInt(
              0x2eb919524a89a26f90be9781a1515145baea3bc96b8cd1f01b221c4d2a1ce87_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x58efb6272921bc5eada46635e3567dced0662c0161223e3c1c63e8de3ec3d73_Z),
          FieldElementT::ConstexprFromBigInt(
              0x62fcd49ca9c7587b436d205ffc2a39594254a1ac34acd46d6955e7844d4f88e_Z),
          FieldElementT::ConstexprFromBigInt(
              0x635895330838846e62d9acce0b625f885e5941e54bd3a2106fcf837aef5313b_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x7da445b81e9b3d36d47a5f4d23b92a378a17f119d5e6e70629f8b41fefb12e3_Z),
          FieldElementT::ConstexprFromBigInt(
              0x2b22dab62f0817e9fc5737e189d5096a9027882bef1738943b7016256118343_Z),
          FieldElementT::ConstexprFromBigInt(
              0x1af01472348f395bacdfed1d27664d0d5bdea769be8fcb8fbef432b790e50d5_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x76b172dbbeec5a31de313b9390f79ec9284163c8e4986bc5b682e5ac6360309_Z),
          FieldElementT::ConstexprFromBigInt(
              0x70efaeae36f6af0f362f6cb423d2009b30ddb4178d46def0bdb2905b3e0862_Z),
          FieldElementT::ConstexprFromBigInt(
              0x6cb99b36e521ac0a39872686b84ee1d28c4942b8036a1c25a0e4117ccaeedf_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x29fd44305a5a9a70bbf9674e544bda0fb3d0fe5bb3aa743fd1b8a4fc1dc6055_Z),
          FieldElementT::ConstexprFromBigInt(
              0x6b447ded1046e83629b184d8c36db3a11a6778d8848142aa6363d6619f9764_Z),
          FieldElementT::ConstexprFromBigInt(
              0x642a8b4be4ba812cbfcf55a77339b5d357cceb6946fdc51c14b58f5b8989b59_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x489e0a26f65a1eecc6cc6aa5b6e775cbc51a73700bd794a7acd79ae1d95882a_Z),
          FieldElementT::ConstexprFromBigInt(
              0x3b19d4ef195975bbf78ab5dc2fd1d24816428f45a06293c1b9d57b9a02e9200_Z),
          FieldElementT::ConstexprFromBigInt(
              0x7d2dd994756eacba576b74790b2194971596f9cd59e55ad2884c52039013df5_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x1922810cc08f50bf300df869823b9f18b3327e29e9e765002970ef0f2e8c5f3_Z),
          FieldElementT::ConstexprFromBigInt(
              0x52f3afaf7c9102f1d46e1d79a70745b39c04376aafff05771cbd4a88ed418ac_Z),
          FieldElementT::ConstexprFromBigInt(
              0x7ccfc88e44a0507a95260f44203086e89552bbe53dcc46b376c5bcab6ea788e_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x2949125939e6ad94100228beff83823f5157dd8e067bc8819e40a1ab008dd9c_Z),
          FieldElementT::ConstexprFromBigInt(
              0x6cb64e3a0d37a6a4273ce4ee6929ba372d6811dde135af4078ba6e1912e1014_Z),
          FieldElementT::ConstexprFromBigInt(
              0xd63b53707acf8962f05f688129bf30ad43714257949cd9ded4bf5953837fae_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0xbcb1549c9cabb5d13bb968b4ea22d0bb7d7460a6965702942092b32ef152d4_Z),
          FieldElementT::ConstexprFromBigInt(
              0x3d1c5233657ce31f5ead698fe76f6492792a7205ba0531a0ca25b8d8fe798c1_Z),
          FieldElementT::ConstexprFromBigInt(
              0x2240b9755182ee9066c2808b1e16ea448e26a83074558d9279f450b79f97516_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0xcc203d8b0f90e30fe8e54f343cef59fe8d70882137de70c9b43ab6615a646c_Z),
          FieldElementT::ConstexprFromBigInt(
              0x310c6cc475d9346e061bacdc175ea9e119e937dea9d2100fa68e03c1f77910b_Z),
          FieldElementT::ConstexprFromBigInt(
              0x7f84b639f52e57420bc947defced0d8cbdbe033f578699397b83667049106c7_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x584ca7f01262c5bd89c4562f57139f47e9f038cb32ec35abe4e1da8de3e164a_Z),
          FieldElementT::ConstexprFromBigInt(
              0x1135eefaf69b6e4af7d02f562868be3e02fdc72e01e9510531f9afa78abbbde_Z),
          FieldElementT::ConstexprFromBigInt(
              0x372082b8a6c07100a50a3d33805827ad350c88b56f62c6d36a0d876856a99e8_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x7c3c12b819a8aad87499bac1a143fc59674f132e33898f0c119e3d12462dfe6_Z),
          FieldElementT::ConstexprFromBigInt(
              0x4f1354c51e8f6905b84157cfeff6822c056ce9e29d602eb46bd9b75a23836cf_Z),
          FieldElementT::ConstexprFromBigInt(
              0x2da9f26a8271659075739ba206507a08ac360150e849950ef3973548fbd2fca_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x287173956a2beb111b5ec29195e38cc3f6a65ff50801aa75fd78dd550702843_Z),
          FieldElementT::ConstexprFromBigInt(
              0x7273101c190ff64212420095a51c8411c7f3227f6a7a4a64ae6ba7f9201e126_Z),
          FieldElementT::ConstexprFromBigInt(
              0x2dbf2a6b56b26d23ebeb61e500687de749b03d3d349169699258ee4c98005fc_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x85b6cbb29739a6808e67f00ab89b52ab89ef8d92530394e4b910efd706c7fb_Z),
          FieldElementT::ConstexprFromBigInt(
              0x3d55b5f1171efda1dacbcbadfd5b910b493fa9589fd937e3e06ce26b08925a3_Z),
          FieldElementT::ConstexprFromBigInt(
              0xaaedaa6ef2fa707d16b3b295410c0e44f7a2f8135c207824f6ae2a9b16e90c_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x6aca6ebf70b1cb46c6331e9f1a5c4cc89b80f8adc5d18915c1cd0d496ccf5e1_Z),
          FieldElementT::ConstexprFromBigInt(
              0x1678602af36c28abb010f831d403d94d5e90003e6d37c677e9dd157fb27761_Z),
          FieldElementT::ConstexprFromBigInt(
              0x2022036bdf687f041b547fefdf36d4c2cd3f4b0526a88aafe60a0a8f508bad2_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x7bfc350957c968ca664397414bdfb8f9b8dfe49fb63e32353d4e2e8d1d4af6_Z),
          FieldElementT::ConstexprFromBigInt(
              0x2d639cbd418cb9fc24ea29ccd1d15ab81f43a499b27a06d3c5e2176f7ad79af_Z),
          FieldElementT::ConstexprFromBigInt(
              0xecdea7f959a4d488403d5b39687a1fe0dee3369e5fbc0f4779569f64506e0c_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x3f656bdc4fefd92b70658e2f1992ef9f22e5f2d28c490e21d4e34357154b558_Z),
          FieldElementT::ConstexprFromBigInt(
              0xd1b8cb1561eed32319638ccab9033dfec47596f8a6f4ce6594e19fddd59254_Z),
          FieldElementT::ConstexprFromBigInt(
              0x758ffc77c62e3e0f86ef6ea01545ad76f281ec2941da7222d1e8b4e2ec1f192_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x20315ca079570df995386e96aeaa1b4596aacd28f83c32f29a591c95e6fcac5_Z),
          FieldElementT::ConstexprFromBigInt(
              0x3e55cf341e7c280cb05f3d6ff9c8d9f2cfe76b84a9d1b0f54884b316b740d8d_Z),
          FieldElementT::ConstexprFromBigInt(
              0x4d56feb32cde74feede9749739be452e92c029007a06f6e67c81203bf650c68_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x4ee807aa678a9a433b6171eaa6a2544497f7599fb8145d7e8089f465403c89b_Z),
          FieldElementT::ConstexprFromBigInt(
              0x25d2bacc8f1ee7548cb5f394de2cb6e1f365e56a1bc579d0f9a8ad2ef2b3821_Z),
          FieldElementT::ConstexprFromBigInt(
              0x5f573de597ce1709fc20051f6501268cd4b278811924af1f237d15feb17bd49_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x30297c3c54a505f5826a280e053cf7a3c1e84a1dcf8b33c682cf85ddac86deb_Z),
          FieldElementT::ConstexprFromBigInt(
              0x2f5e9c47c9a86e043c7526a59783f03c6bc79b69b8709fe6a052b93a8339ae8_Z),
          FieldElementT::ConstexprFromBigInt(
              0x1bf75c7a739da8d29f9c23065ff8ccb1da7deec83e130bcd4a27a416c72b84b_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x60563d5f852ae875989017bd5c4cfdc29cd27fc4e91eeabdb8e864df3c3c675_Z),
          FieldElementT::ConstexprFromBigInt(
              0x7a4b1d70885aa820969635468daec94f8156c20e3131bd71005be1cd16ccf9e_Z),
          FieldElementT::ConstexprFromBigInt(
              0x347bb025695e497f1e201cd62aa4600b8b85cf718cd1d400f39c10e59cc5852_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x6783ab1e1ef97bb9e7f9381eb6ab0de2c4c9c2de413691ba8aa666292e9e217_Z),
          FieldElementT::ConstexprFromBigInt(
              0x133e0280c6de90e7b3870a07823c081fd9c4cb99d534debd6a7bfb4e5b0dd46_Z),
          FieldElementT::ConstexprFromBigInt(
              0x865d450ce29dc42fb5db72460b3560a2f093695573dff94fd0216eb925beec_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x1de023f840e054a35526dabacf0dee948efba06bcbb414ecd81a6b301664e57_Z),
          FieldElementT::ConstexprFromBigInt(
              0x55fc1e341bfdf7805015a96f724c5ac7cc7b892a292d38190631ab1a5388c4_Z),
          FieldElementT::ConstexprFromBigInt(
              0x2df6557bfd4a4e7e7b27bf51552d2b5162706a3e624faca01a307ef8d532858_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x113a8a66962ce08d92a6bd3e9c1d55ef8f226da95e4d629046d73d0507f6271_Z),
          FieldElementT::ConstexprFromBigInt(
              0x271577d6ee9fa377f2c889874ba5b44ca1076033db5c2de4f3367b08c008e53_Z),
          FieldElementT::ConstexprFromBigInt(
              0x3396b33911219b6b0365c09348a561ef1ccb956fc673bc5291d311866538574_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x1e1392f2da08549c8a7d89e899189306170baa3c3436e6a5398f69c8f321636_Z),
          FieldElementT::ConstexprFromBigInt(
              0x661545081032013df118e1d6e7c61a333e313b1a9a5b6d69c876bd2e7d694ca_Z),
          FieldElementT::ConstexprFromBigInt(
              0x6b14294e71cd7fb776edbd432d20eb8f66d00533574e46573516f0cacdeec88_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x7252fbbb06c2848338b1c41df31e4e51fe2a18e2406c671915cab6eb1a1d4f2_Z),
          FieldElementT::ConstexprFromBigInt(
              0x3ccf71be7cc2a9abcf5a09807c69679430c03645747621b7f5327cb00ff99da_Z),
          FieldElementT::ConstexprFromBigInt(
              0x29778dc707504fa6a9f7c97b4ceef0a9b39001d034441617757cd816dac919a_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x39473f6f06bb99e33590d34e3bae36e491f7bbf86a26aa55a8f5b27bb98d4c5_Z),
          FieldElementT::ConstexprFromBigInt(
              0x7ba7c32f875b71b895caa0215f996fd4ad92bab187e81417063dde91c08c027_Z),
          FieldElementT::ConstexprFromBigInt(
              0x37c1367e49cbfc403b22aac82abf83b0ed083148a5f4c92839e5d769bdab6b6_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x5c9eb899931d2f4b53ffcf833cdfa05c2068375ff933eb37ae34157c0b2d951_Z),
          FieldElementT::ConstexprFromBigInt(
              0x5f6054a4d48698ec27772fb50a7d2e5c1557ffdc1ffd07331f2ca26c6e3b661_Z),
          FieldElementT::ConstexprFromBigInt(
              0x20e6d62a2fe0fe9b0fab83e8c7d1e8bfd0fec827960e40a91df64664dcd7774_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x6290a56a489ad52120c426fe0e409c2ff17adf51f528cafb0d026d14ffd6aac_Z),
          FieldElementT::ConstexprFromBigInt(
              0x3703f16f990342c2267a6f7ece342705a32ca4c101417286279f6fc315edc7c_Z),
          FieldElementT::ConstexprFromBigInt(
              0x5194962daf6679b9a0c32b5a9a307ba92e2c630f70e439195b680dd296df3fd_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0xe8eae20a79a7c1242c34617b01340fb5fd4bea2aa58b98d2400d9b515ee5e2_Z),
          FieldElementT::ConstexprFromBigInt(
              0x369058169d63091ae28bfb28def7cd8d00dd7c2894fae4ffec65242afa5cd45_Z),
          FieldElementT::ConstexprFromBigInt(
              0x418c963bc97195a74077503ee472f22cfdff0973190ab189c7b93103fd78167_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x68d07a3eefc78dc5b28b3f4dc93167fb8c97112d14a25b4d4db559720156386_Z),
          FieldElementT::ConstexprFromBigInt(
              0x517e892228df2d4f15a3c4241c98ba25ba0b5557375003f8748583a61836372_Z),
          FieldElementT::ConstexprFromBigInt(
              0x5cc0f0f6cf9be94a150116e7932f8fe74ac20ad8100c41dc9c99538792e279b_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x53d5d7863434c6629bdb1f8a648e4820883543e821f0f5c1668884c0be41ec8_Z),
          FieldElementT::ConstexprFromBigInt(
              0xa158126b89e6b0a600bf53f8101707b072218912dd0d9df2528f67de24fdf5_Z),
          FieldElementT::ConstexprFromBigInt(
              0x6b53b807265387ee582069a698323d44c204bed60672b8d8d073bed2fede503_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x1097fb448406b7a6de0877efd58c01be53be83bde9601a9acc9e0ca2091fda0_Z),
          FieldElementT::ConstexprFromBigInt(
              0xcbc0ff7239d3763902396389d67b3049ce1fefde66333ce37ca441f5a31bec_Z),
          FieldElementT::ConstexprFromBigInt(
              0x79a3d91dd8a309c632eb43d57b5c5d838ceebd64603f68a8141ebef84280e72_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x23fb472fe575135300f74e8f6de8fe1185078218eceb938900e7598a368db9_Z),
          FieldElementT::ConstexprFromBigInt(
              0x7ac73134016d2a8a4c63a6b9494c0bd7a6ba87cc33e8a8e23ebda18bfb67c2a_Z),
          FieldElementT::ConstexprFromBigInt(
              0x19a16068c3eac9c03f1b5c5ee2485ccc163d9ab17bb035d5df6e31c3dcf8f14_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x1f24b4356a6bbfd4d4ef9fd1634752820ee86a925725ac392134d90def073ea_Z),
          FieldElementT::ConstexprFromBigInt(
              0x3e44e7f7aeea6add59b6b4d11c60a528fb70727f35d817305971592333d36_Z),
          FieldElementT::ConstexprFromBigInt(
              0x5f93b02f826741414535a511ed3eb4fe85987ae57bc9807cbd94cd7513d394e_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0xf0a0a88db99247d71c3d51d4197fa3fd1cc76e670607e35ca2d3bada29523a_Z),
          FieldElementT::ConstexprFromBigInt(
              0x3432226916d31f3acac1e211431fd4cd2b6f2e80626af6564bdde3e77608db0_Z),
          FieldElementT::ConstexprFromBigInt(
              0x55625941bfea6f48175192845a7ad74b0b82940ef5f393ca3830528d59cf919_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0xddf48695b204477dfe4f8cb3ef1b39783e9b92f9276b858e2e585e318e20a4_Z),
          FieldElementT::ConstexprFromBigInt(
              0x260730a657ff8f38851a679ab2a1490434ee50d4953e7c5d3194578b08ae8e3_Z),
          FieldElementT::ConstexprFromBigInt(
              0x4cfd231373aa46d96283840bdb79ba6d7132775b398d324bcd206842b961aa9_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x3203843c41cd453f14fa0bc0b2191a27ebc659e74fd48f981e963de57eff25d_Z),
          FieldElementT::ConstexprFromBigInt(
              0x2c2f6ae5624d1fb8435d1c86bf76c260f5e77a54b006293705872e647cc46_Z),
          FieldElementT::ConstexprFromBigInt(
              0x780225456e63903b3e561384ef2e73a85b0e142b69752381535022014765f06_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x7f602ec1a80a051fd21b07f8e2960613082fc954b9a9ff641cc432a75c81887_Z),
          FieldElementT::ConstexprFromBigInt(
              0x62561b0a0a72239b60f6aaf7022b7d323fe77cd7c1ab432f0c8c118ca7e6bca_Z),
          FieldElementT::ConstexprFromBigInt(
              0x604fe5a6a22344aa69b05dea16b1cf22450c186d093754cb9b84a8a03b70bc8_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x1cf9987a4044716d3dc140bf5f9b76f6eada5995905189f8682eaf88aef2b7b_Z),
          FieldElementT::ConstexprFromBigInt(
              0x6bc0b2487c1eece3db47a4bdd60cf69debee233e91b50e9ee42ce22cbfbacbf_Z),
          FieldElementT::ConstexprFromBigInt(
              0x2f5dbb5055eb749a11403b93e90338b7620c51356d2c6adcbf87ab7ea0792e6_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x446328f4dddae6529743c43883d59c45f63b8a623a9cf318489e5fc4a550f61_Z),
          FieldElementT::ConstexprFromBigInt(
              0x4ba30c5240cde5bca6c4010fb4b481a25817b43d358399958584d2c48f5af25_Z),
          FieldElementT::ConstexprFromBigInt(
              0x5f5275f76425b15c89209117734ae85708351d2cf19af5fe39a32f89c2c8a89_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x576f3b5156f4763e18c7f98df3b2f7b993cdda4eb8cb92415e1be8e6af2fc17_Z),
          FieldElementT::ConstexprFromBigInt(
              0x11dc3f15cba928aed5a44b55a5b026df84a61719ed5adbb93c0e8e12d35ef3d_Z),
          FieldElementT::ConstexprFromBigInt(
              0x44c40e6bd52e91ad9896403ae4f543ae1c1d9ea047d75f8a6442b8feda04dca_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x1836d733a54013ebd0ccbf4974e80ac1954bf90fe9ea4e2c914ad01166026d8_Z),
          FieldElementT::ConstexprFromBigInt(
              0x3c553be9776b628a8159d306ef084727611df8037761f00f84ca02ce731b3ac_Z),
          FieldElementT::ConstexprFromBigInt(
              0x6ce94781c1a23fda1c7b87e0436b1b401ae11a6d757843e342f5017076a059_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x381ec71fbdef3160253be9f00f4e6b9e107f457812effb7371cc2daa0acd0ed_Z),
          FieldElementT::ConstexprFromBigInt(
              0x1844da9cc0eeadc6490d847320d9f3cd4fb574aa687bafdfe0ffa7bf2a8f1a1_Z),
          FieldElementT::ConstexprFromBigInt(
              0x7a8bf471f902d5abb27fea5b401483dedf97101047459682acfd7f9b65a812f_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x633b6fb004de62441915fb51ac174456f5a9cdff7aecb6e6b0d063839e56327_Z),
          FieldElementT::ConstexprFromBigInt(
              0x179ee5cec496194771200382bfc6d17bbe546ba88fed8b17535fd70fbc50ab6_Z),
          FieldElementT::ConstexprFromBigInt(
              0x2806c0786185986ea9891b42d565256b0312446f07435ac2cae194330bf8c42_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x438703d948708ae90c7a6b8af194b8b603bb2cdfd26bfa356ac9bb6ee041393_Z),
          FieldElementT::ConstexprFromBigInt(
              0x24446628f56029d7153bd3a482b7f6e1c56f4e02225c628a585d58a920035af_Z),
          FieldElementT::ConstexprFromBigInt(
              0x4c2a76e5ce832e8b0685cdeeea3a253ae48f6606790d817bd96025e5435e259_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x78a23323520994592933c079b148aed57d5e4ce1ab122d370983b8caa0e0300_Z),
          FieldElementT::ConstexprFromBigInt(
              0x79ca6c5e1025b2151144ea5937dd07cadce1aa691b19e6db87070ba51ec22c0_Z),
          FieldElementT::ConstexprFromBigInt(
              0x6b2e4a46e37af3cf952d9d34f8d6bd84a442ebfd1ac5d17314e48922af79c5d_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x305d6cd95cc2eab6805d93d3d8d74e1ca7d443f11e34a18e3529e0d03435c2_Z),
          FieldElementT::ConstexprFromBigInt(
              0x6097b4b8b90db14b39743ed23f8956cabb7aea70cc624a415c7c17b37fbf9a9_Z),
          FieldElementT::ConstexprFromBigInt(
              0x64e1b3f16c26c8845bdb98373e77dad3bdcc90865b0f0af96288707c18893f_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x649fafe673f21e623384d841221b73421c56014af2ffdf57f1579ae911fd335_Z),
          FieldElementT::ConstexprFromBigInt(
              0x7d806dccbf1a2696b294404e849722f2baa2f4d19005a49d1ba288a77fefe30_Z),
          FieldElementT::ConstexprFromBigInt(
              0x5951a37da53e3bbc0b3e2db1a9a235d7a03f48f443be6d659119c44aafc7522_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x6d87fa479fb59524d1912c3554ae3d010496a31bdacb542c816a1607a907731_Z),
          FieldElementT::ConstexprFromBigInt(
              0x1451cccd4200fa9d473ad73466b4e8c0a712a0b12bb6fc9462a3ac892acc9b2_Z),
          FieldElementT::ConstexprFromBigInt(
              0x3ca1b6400b3e51007642535f1ca9b03832ca0faa15e1c4ed82dd1efdc0763da_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x52c55735b2f0a6560ad1516a8f13592b0dd024ff4162539f993a99c7a1a4d95_Z),
          FieldElementT::ConstexprFromBigInt(
              0x7e04de60aa80132f0149d1dee29617de750bd5ce3e9fa5e62951d65f6b924cd_Z),
          FieldElementT::ConstexprFromBigInt(
              0x271784e6920a68e47c4c8fab71c8f8303ef29e26f289223edf63291c0a5495_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x5c7c19061a84d5960a04b8f0adaa603c8afe93f17b7f0e56b49514af43d0c69_Z),
          FieldElementT::ConstexprFromBigInt(
              0x172db5affe783af419da337cb79061e090943c2959dea1b38e4436f5482eafe_Z),
          FieldElementT::ConstexprFromBigInt(
              0x518b7975a6d8d310eac9fe4082916f021a7ecbadf18809746a9e061a2cb9456_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x20c5539dc45dd56d4bbc2440a9f5061d74b8ae5e37b34e8755a0315f1e196db_Z),
          FieldElementT::ConstexprFromBigInt(
              0x1ea6f5fb309fa4a08bc7d516e80efc3a977b47208283cf35a9d8bc213b90b14_Z),
          FieldElementT::ConstexprFromBigInt(
              0x50ce323c5128dc7fdd8ddd8ba9cfe2efd424b5de167c7257d1f766541e29ded_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x401e37d0e276547695538b41d3c28215b865f5b7d1b497a8919284c613cb7d8_Z),
          FieldElementT::ConstexprFromBigInt(
              0x645a0de30acc3117f2893056fc5880255daa12cc61261cc0fab9cf57c57397b_Z),
          FieldElementT::ConstexprFromBigInt(
              0x69bc3841eb0a310d9e988d75f09f698d4fdc9d0d69219f676b66ae7fa3d495b_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x2684bbe315ad2c4bdd47c38fe72db47cf0ae0c455cda5484baf523f136bdc6_Z),
          FieldElementT::ConstexprFromBigInt(
              0x11e0f83c547ca5c68202e8d34e5595a88858c2afa664365e4acb821fd8a13ee_Z),
          FieldElementT::ConstexprFromBigInt(
              0x4af4a7635f8c7515966567ceec34315d0f86ac66c1e5a5ecac945f1097b82ef_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x4fba58cf8aaf4893cb7158908ccc18b1dc48894d2bb46225c72b11f4c74b271_Z),
          FieldElementT::ConstexprFromBigInt(
              0x397c4c169115b468cc90da2e664f8c29a7f89be0ead679a38b0f44c8a2a0e20_Z),
          FieldElementT::ConstexprFromBigInt(
              0x6563b9ebb6450dbad397fa5dd13c501f326dd7f32be22e20998f59ec7bacff_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x376edb238f7b630ea81d307f4c79f9afec48562076dd09c36cd79e9cb817165_Z),
          FieldElementT::ConstexprFromBigInt(
              0x60d4208bb50eb15f29ed22addcd50a1b337504039690eb858584cda96e2e061_Z),
          FieldElementT::ConstexprFromBigInt(
              0x6a37d569d2fbc73dbff1019dc3465ec0f30da46918ab020344a52f1df9a9210_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0xd3b174c7290c6bf412083ff35d23821dc512f1df073c1b429130371ac63b1a_Z),
          FieldElementT::ConstexprFromBigInt(
              0x226ed3d763477454b46eb2a5c3b814634d974919689fb489fe55e525b980373_Z),
          FieldElementT::ConstexprFromBigInt(
              0x5f3997e7dafcb2de0e7a23d33d2fd9ef06f4d79bd7ffa1930e8b0080d218513_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x7c5eec716d94634434df335a10bbac504f886f7f9d3c1648348c3fae8fdf14d_Z),
          FieldElementT::ConstexprFromBigInt(
              0x53cc30d7fe0f84e7e24fd22c0f9ad68a89da85553f871ef63d2f55f57e1a7c_Z),
          FieldElementT::ConstexprFromBigInt(
              0x368821ee335d71819b95769f47418569474a24f6e83b268fefa4cd58c4ec8fa_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x5334f75b052c0235119816883040da72c6d0a61538bdfff46d6a242bfeb7a1_Z),
          FieldElementT::ConstexprFromBigInt(
              0x5d0af4fcbd9e056c1020cca9d871ae68f80ee4af2ec6547cd49d6dca50aa431_Z),
          FieldElementT::ConstexprFromBigInt(
              0x30131bce2fba5694114a19c46d24e00b4699dc00f1d53ba5ab99537901b1e65_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x5646a95a7c1ae86b34c0750ed2e641c538f93f13161be3c4957660f2e788965_Z),
          FieldElementT::ConstexprFromBigInt(
              0x4b9f291d7b430c79fac36230a11f43e78581f5259692b52c90df47b7d4ec01a_Z),
          FieldElementT::ConstexprFromBigInt(
              0x5006d393d3480f41a98f19127072dc83e00becf6ceb4d73d890e74abae01a13_Z)},
      std::array<FieldElementT, 3>{
          FieldElementT::ConstexprFromBigInt(
              0x62c9d42199f3b260e7cb8a115143106acf4f702e6b346fd202dc3b26a679d80_Z),
          FieldElementT::ConstexprFromBigInt(
              0x51274d092db5099f180b1a8a13b7f2c7606836eabd8af54bf1d9ac2dc5717a5_Z),
          FieldElementT::ConstexprFromBigInt(
              0x61fc552b8eb75e17ad0fb7aaa4ca528f415e14f0d9cdbed861a8db0bfff0c5b_Z)}};
  static constexpr bool kHasOutputBuiltin = true;
  static constexpr bool kHasPedersenBuiltin = true;
  static constexpr bool kHasRangeCheckBuiltin = true;
  static constexpr bool kHasEcdsaBuiltin = true;
  static constexpr bool kHasBitwiseBuiltin = true;
  static constexpr bool kHasEcOpBuiltin = true;
  static constexpr bool kHasKeccakBuiltin = true;
  static constexpr bool kHasPoseidonBuiltin = true;
  static constexpr char kLayoutName[] = "all_cairo";
  static constexpr BigInt<4> kLayoutCode = 0x616c6c5f636169726f_Z;
  static constexpr uint64_t kConstraintDegree = 2;
  static constexpr uint64_t kCpuComponentHeight = 16;
  static constexpr uint64_t kLogCpuComponentHeight = 4;
  static constexpr uint64_t kMemoryStep = 2;
  static constexpr std::array<std::string_view, 10> kSegmentNames = {
      "program", "execution", "output", "pedersen", "range_check",
      "ecdsa",   "bitwise",   "ec_op",  "keccak",   "poseidon"};

  enum Columns {
    kColumn0Column,
    kColumn1Column,
    kColumn2Column,
    kColumn3Column,
    kColumn4Column,
    kColumn5Column,
    kColumn6Column,
    kColumn7Column,
    kColumn8Inter1Column,
    kColumn9Inter1Column,
    kColumn10Inter1Column,
    // Number of columns.
    kNumColumns,
  };

  enum PeriodicColumns {
    kPedersenPointsXPeriodicColumn,
    kPedersenPointsYPeriodicColumn,
    kEcdsaGeneratorPointsXPeriodicColumn,
    kEcdsaGeneratorPointsYPeriodicColumn,
    kKeccakKeccakKeccakRoundKey0PeriodicColumn,
    kKeccakKeccakKeccakRoundKey1PeriodicColumn,
    kKeccakKeccakKeccakRoundKey3PeriodicColumn,
    kKeccakKeccakKeccakRoundKey7PeriodicColumn,
    kKeccakKeccakKeccakRoundKey15PeriodicColumn,
    kKeccakKeccakKeccakRoundKey31PeriodicColumn,
    kKeccakKeccakKeccakRoundKey63PeriodicColumn,
    kPoseidonPoseidonFullRoundKey0PeriodicColumn,
    kPoseidonPoseidonFullRoundKey1PeriodicColumn,
    kPoseidonPoseidonFullRoundKey2PeriodicColumn,
    kPoseidonPoseidonPartialRoundKey0PeriodicColumn,
    kPoseidonPoseidonPartialRoundKey1PeriodicColumn,
    // Number of periodic columns.
    kNumPeriodicColumns,
  };

  enum Neighbors {
    kColumn0Row0Neighbor,
    kColumn0Row1Neighbor,
    kColumn0Row2Neighbor,
    kColumn0Row3Neighbor,
    kColumn0Row4Neighbor,
    kColumn0Row5Neighbor,
    kColumn0Row6Neighbor,
    kColumn0Row7Neighbor,
    kColumn0Row8Neighbor,
    kColumn0Row9Neighbor,
    kColumn0Row10Neighbor,
    kColumn0Row11Neighbor,
    kColumn0Row12Neighbor,
    kColumn0Row13Neighbor,
    kColumn0Row14Neighbor,
    kColumn0Row15Neighbor,
    kColumn1Row0Neighbor,
    kColumn1Row1Neighbor,
    kColumn1Row2Neighbor,
    kColumn1Row3Neighbor,
    kColumn1Row4Neighbor,
    kColumn1Row5Neighbor,
    kColumn1Row6Neighbor,
    kColumn1Row8Neighbor,
    kColumn1Row12Neighbor,
    kColumn1Row14Neighbor,
    kColumn1Row16Neighbor,
    kColumn1Row20Neighbor,
    kColumn1Row24Neighbor,
    kColumn1Row28Neighbor,
    kColumn1Row32Neighbor,
    kColumn1Row36Neighbor,
    kColumn1Row40Neighbor,
    kColumn1Row44Neighbor,
    kColumn1Row48Neighbor,
    kColumn1Row52Neighbor,
    kColumn1Row56Neighbor,
    kColumn1Row60Neighbor,
    kColumn1Row64Neighbor,
    kColumn1Row66Neighbor,
    kColumn1Row128Neighbor,
    kColumn1Row130Neighbor,
    kColumn1Row176Neighbor,
    kColumn1Row180Neighbor,
    kColumn1Row184Neighbor,
    kColumn1Row188Neighbor,
    kColumn1Row192Neighbor,
    kColumn1Row194Neighbor,
    kColumn1Row195Neighbor,
    kColumn1Row198Neighbor,
    kColumn1Row240Neighbor,
    kColumn1Row244Neighbor,
    kColumn1Row248Neighbor,
    kColumn1Row252Neighbor,
    kColumn1Row259Neighbor,
    kColumn1Row262Neighbor,
    kColumn1Row451Neighbor,
    kColumn1Row515Neighbor,
    kColumn1Row518Neighbor,
    kColumn1Row707Neighbor,
    kColumn1Row769Neighbor,
    kColumn1Row771Neighbor,
    kColumn1Row773Neighbor,
    kColumn1Row774Neighbor,
    kColumn1Row782Neighbor,
    kColumn1Row963Neighbor,
    kColumn1Row1025Neighbor,
    kColumn1Row1027Neighbor,
    kColumn1Row1029Neighbor,
    kColumn1Row1030Neighbor,
    kColumn1Row1038Neighbor,
    kColumn1Row1219Neighbor,
    kColumn1Row1283Neighbor,
    kColumn1Row1286Neighbor,
    kColumn1Row1475Neighbor,
    kColumn1Row1539Neighbor,
    kColumn1Row1542Neighbor,
    kColumn1Row1731Neighbor,
    kColumn1Row1795Neighbor,
    kColumn1Row1798Neighbor,
    kColumn1Row1987Neighbor,
    kColumn1Row2051Neighbor,
    kColumn1Row2054Neighbor,
    kColumn1Row2118Neighbor,
    kColumn1Row2182Neighbor,
    kColumn1Row2243Neighbor,
    kColumn1Row2307Neighbor,
    kColumn1Row2310Neighbor,
    kColumn1Row2499Neighbor,
    kColumn1Row2563Neighbor,
    kColumn1Row2566Neighbor,
    kColumn1Row2755Neighbor,
    kColumn1Row2819Neighbor,
    kColumn1Row2822Neighbor,
    kColumn1Row3011Neighbor,
    kColumn1Row3075Neighbor,
    kColumn1Row3078Neighbor,
    kColumn1Row3331Neighbor,
    kColumn1Row3334Neighbor,
    kColumn1Row3587Neighbor,
    kColumn1Row3590Neighbor,
    kColumn1Row3654Neighbor,
    kColumn1Row3718Neighbor,
    kColumn1Row3843Neighbor,
    kColumn1Row3846Neighbor,
    kColumn1Row3910Neighbor,
    kColumn1Row3974Neighbor,
    kColumn1Row4099Neighbor,
    kColumn1Row4102Neighbor,
    kColumn1Row4355Neighbor,
    kColumn1Row4358Neighbor,
    kColumn1Row4611Neighbor,
    kColumn1Row4614Neighbor,
    kColumn1Row4867Neighbor,
    kColumn1Row4870Neighbor,
    kColumn1Row5123Neighbor,
    kColumn1Row5126Neighbor,
    kColumn1Row5379Neighbor,
    kColumn1Row5382Neighbor,
    kColumn1Row5443Neighbor,
    kColumn1Row5446Neighbor,
    kColumn1Row5507Neighbor,
    kColumn1Row5510Neighbor,
    kColumn1Row5635Neighbor,
    kColumn1Row5638Neighbor,
    kColumn1Row5699Neighbor,
    kColumn1Row5763Neighbor,
    kColumn1Row5891Neighbor,
    kColumn1Row5894Neighbor,
    kColumn1Row5955Neighbor,
    kColumn1Row6019Neighbor,
    kColumn1Row6147Neighbor,
    kColumn1Row6150Neighbor,
    kColumn1Row6211Neighbor,
    kColumn1Row6275Neighbor,
    kColumn1Row6401Neighbor,
    kColumn1Row6403Neighbor,
    kColumn1Row6405Neighbor,
    kColumn1Row6406Neighbor,
    kColumn1Row6469Neighbor,
    kColumn1Row6470Neighbor,
    kColumn1Row6533Neighbor,
    kColumn1Row6534Neighbor,
    kColumn1Row6593Neighbor,
    kColumn1Row6595Neighbor,
    kColumn1Row6597Neighbor,
    kColumn1Row6598Neighbor,
    kColumn1Row6657Neighbor,
    kColumn1Row6662Neighbor,
    kColumn1Row6721Neighbor,
    kColumn1Row6726Neighbor,
    kColumn1Row6785Neighbor,
    kColumn1Row6787Neighbor,
    kColumn1Row6789Neighbor,
    kColumn1Row6790Neighbor,
    kColumn1Row6977Neighbor,
    kColumn1Row6979Neighbor,
    kColumn1Row6981Neighbor,
    kColumn1Row6982Neighbor,
    kColumn1Row7169Neighbor,
    kColumn1Row7171Neighbor,
    kColumn1Row7173Neighbor,
    kColumn1Row7174Neighbor,
    kColumn1Row7361Neighbor,
    kColumn1Row7363Neighbor,
    kColumn1Row7365Neighbor,
    kColumn1Row7366Neighbor,
    kColumn1Row7553Neighbor,
    kColumn1Row7555Neighbor,
    kColumn1Row7557Neighbor,
    kColumn1Row7558Neighbor,
    kColumn1Row7745Neighbor,
    kColumn1Row7747Neighbor,
    kColumn1Row7749Neighbor,
    kColumn1Row7750Neighbor,
    kColumn1Row7937Neighbor,
    kColumn1Row7939Neighbor,
    kColumn1Row7941Neighbor,
    kColumn1Row7942Neighbor,
    kColumn1Row8193Neighbor,
    kColumn1Row8195Neighbor,
    kColumn1Row8197Neighbor,
    kColumn1Row8206Neighbor,
    kColumn1Row8451Neighbor,
    kColumn1Row8707Neighbor,
    kColumn1Row10755Neighbor,
    kColumn1Row15941Neighbor,
    kColumn1Row16902Neighbor,
    kColumn1Row18883Neighbor,
    kColumn1Row19139Neighbor,
    kColumn1Row19395Neighbor,
    kColumn1Row22531Neighbor,
    kColumn1Row22595Neighbor,
    kColumn1Row22659Neighbor,
    kColumn1Row22785Neighbor,
    kColumn1Row24577Neighbor,
    kColumn1Row24579Neighbor,
    kColumn1Row24581Neighbor,
    kColumn1Row24590Neighbor,
    kColumn1Row24835Neighbor,
    kColumn1Row25091Neighbor,
    kColumn1Row26371Neighbor,
    kColumn1Row30214Neighbor,
    kColumn1Row30977Neighbor,
    kColumn1Row31171Neighbor,
    kColumn1Row51971Neighbor,
    kColumn1Row55939Neighbor,
    kColumn1Row57345Neighbor,
    kColumn1Row57347Neighbor,
    kColumn1Row57349Neighbor,
    kColumn1Row57358Neighbor,
    kColumn1Row57603Neighbor,
    kColumn1Row57859Neighbor,
    kColumn1Row68867Neighbor,
    kColumn1Row71430Neighbor,
    kColumn1Row71941Neighbor,
    kColumn1Row73473Neighbor,
    kColumn1Row75782Neighbor,
    kColumn1Row75846Neighbor,
    kColumn1Row75910Neighbor,
    kColumn1Row80133Neighbor,
    kColumn1Row80197Neighbor,
    kColumn1Row80261Neighbor,
    kColumn1Row86275Neighbor,
    kColumn1Row89283Neighbor,
    kColumn1Row115715Neighbor,
    kColumn1Row122246Neighbor,
    kColumn1Row122881Neighbor,
    kColumn1Row122883Neighbor,
    kColumn1Row122885Neighbor,
    kColumn1Row122894Neighbor,
    kColumn1Row123139Neighbor,
    kColumn1Row123395Neighbor,
    kColumn1Row127491Neighbor,
    kColumn1Row130435Neighbor,
    kColumn1Row151043Neighbor,
    kColumn1Row155397Neighbor,
    kColumn1Row159750Neighbor,
    kColumn1Row162054Neighbor,
    kColumn1Row165379Neighbor,
    kColumn1Row165382Neighbor,
    kColumn1Row170246Neighbor,
    kColumn1Row171397Neighbor,
    kColumn1Row172803Neighbor,
    kColumn1Row175110Neighbor,
    kColumn1Row178433Neighbor,
    kColumn1Row178435Neighbor,
    kColumn1Row192262Neighbor,
    kColumn1Row192326Neighbor,
    kColumn1Row192390Neighbor,
    kColumn1Row195009Neighbor,
    kColumn1Row195073Neighbor,
    kColumn1Row195137Neighbor,
    kColumn1Row207875Neighbor,
    kColumn1Row208390Neighbor,
    kColumn1Row208454Neighbor,
    kColumn1Row208518Neighbor,
    kColumn1Row211398Neighbor,
    kColumn1Row211462Neighbor,
    kColumn1Row211526Neighbor,
    kColumn1Row212742Neighbor,
    kColumn1Row225027Neighbor,
    kColumn1Row228163Neighbor,
    kColumn1Row230659Neighbor,
    kColumn1Row230662Neighbor,
    kColumn1Row235969Neighbor,
    kColumn1Row236929Neighbor,
    kColumn1Row253953Neighbor,
    kColumn1Row253955Neighbor,
    kColumn1Row253957Neighbor,
    kColumn1Row253966Neighbor,
    kColumn1Row254211Neighbor,
    kColumn1Row254467Neighbor,
    kColumn1Row295686Neighbor,
    kColumn1Row299011Neighbor,
    kColumn1Row301317Neighbor,
    kColumn1Row302083Neighbor,
    kColumn1Row304134Neighbor,
    kColumn1Row309702Neighbor,
    kColumn1Row320451Neighbor,
    kColumn1Row320707Neighbor,
    kColumn1Row320963Neighbor,
    kColumn1Row322822Neighbor,
    kColumn1Row325123Neighbor,
    kColumn1Row325187Neighbor,
    kColumn1Row325251Neighbor,
    kColumn1Row325893Neighbor,
    kColumn1Row337603Neighbor,
    kColumn1Row337859Neighbor,
    kColumn1Row338115Neighbor,
    kColumn1Row341763Neighbor,
    kColumn1Row341827Neighbor,
    kColumn1Row341891Neighbor,
    kColumn1Row352771Neighbor,
    kColumn1Row356870Neighbor,
    kColumn1Row358661Neighbor,
    kColumn1Row359621Neighbor,
    kColumn1Row360707Neighbor,
    kColumn1Row362758Neighbor,
    kColumn1Row367046Neighbor,
    kColumn1Row367809Neighbor,
    kColumn1Row370691Neighbor,
    kColumn1Row376390Neighbor,
    kColumn1Row381958Neighbor,
    kColumn1Row383425Neighbor,
    kColumn1Row405766Neighbor,
    kColumn1Row407809Neighbor,
    kColumn1Row415750Neighbor,
    kColumn1Row416198Neighbor,
    kColumn1Row445190Neighbor,
    kColumn1Row448774Neighbor,
    kColumn1Row450755Neighbor,
    kColumn1Row451011Neighbor,
    kColumn1Row451267Neighbor,
    kColumn1Row455939Neighbor,
    kColumn1Row456003Neighbor,
    kColumn1Row456067Neighbor,
    kColumn1Row463619Neighbor,
    kColumn1Row463622Neighbor,
    kColumn1Row465350Neighbor,
    kColumn1Row466499Neighbor,
    kColumn1Row476934Neighbor,
    kColumn1Row481537Neighbor,
    kColumn1Row502019Neighbor,
    kColumn1Row502278Neighbor,
    kColumn1Row506305Neighbor,
    kColumn1Row507457Neighbor,
    kColumn1Row513027Neighbor,
    kColumn1Row513286Neighbor,
    kColumn1Row513350Neighbor,
    kColumn1Row513414Neighbor,
    kColumn1Row514310Neighbor,
    kColumn1Row514374Neighbor,
    kColumn1Row514438Neighbor,
    kColumn1Row515843Neighbor,
    kColumn1Row516097Neighbor,
    kColumn1Row516099Neighbor,
    kColumn1Row516101Neighbor,
    kColumn1Row516102Neighbor,
    kColumn1Row516110Neighbor,
    kColumn1Row516294Neighbor,
    kColumn1Row516355Neighbor,
    kColumn1Row516358Neighbor,
    kColumn1Row516611Neighbor,
    kColumn1Row522497Neighbor,
    kColumn1Row522501Neighbor,
    kColumn1Row522502Neighbor,
    kColumn1Row522689Neighbor,
    kColumn1Row522694Neighbor,
    kColumn2Row0Neighbor,
    kColumn2Row1Neighbor,
    kColumn3Row0Neighbor,
    kColumn3Row1Neighbor,
    kColumn3Row2Neighbor,
    kColumn3Row3Neighbor,
    kColumn3Row4Neighbor,
    kColumn3Row5Neighbor,
    kColumn3Row6Neighbor,
    kColumn3Row7Neighbor,
    kColumn3Row8Neighbor,
    kColumn3Row9Neighbor,
    kColumn3Row10Neighbor,
    kColumn3Row11Neighbor,
    kColumn3Row12Neighbor,
    kColumn3Row13Neighbor,
    kColumn3Row14Neighbor,
    kColumn3Row15Neighbor,
    kColumn3Row16Neighbor,
    kColumn3Row32Neighbor,
    kColumn3Row256Neighbor,
    kColumn3Row264Neighbor,
    kColumn3Row272Neighbor,
    kColumn3Row288Neighbor,
    kColumn3Row512Neighbor,
    kColumn3Row520Neighbor,
    kColumn3Row1876Neighbor,
    kColumn3Row1940Neighbor,
    kColumn3Row2004Neighbor,
    kColumn3Row3924Neighbor,
    kColumn3Row3988Neighbor,
    kColumn3Row4052Neighbor,
    kColumn3Row5972Neighbor,
    kColumn3Row6036Neighbor,
    kColumn3Row6100Neighbor,
    kColumn3Row6416Neighbor,
    kColumn3Row6432Neighbor,
    kColumn3Row7568Neighbor,
    kColumn3Row7760Neighbor,
    kColumn3Row7824Neighbor,
    kColumn3Row7888Neighbor,
    kColumn3Row8208Neighbor,
    kColumn3Row8224Neighbor,
    kColumn3Row8448Neighbor,
    kColumn3Row8456Neighbor,
    kColumn3Row10068Neighbor,
    kColumn3Row12116Neighbor,
    kColumn3Row14164Neighbor,
    kColumn3Row15760Neighbor,
    kColumn3Row15952Neighbor,
    kColumn3Row16144Neighbor,
    kColumn3Row16145Neighbor,
    kColumn3Row16146Neighbor,
    kColumn3Row16147Neighbor,
    kColumn3Row16148Neighbor,
    kColumn3Row16149Neighbor,
    kColumn3Row16150Neighbor,
    kColumn3Row16151Neighbor,
    kColumn3Row16160Neighbor,
    kColumn3Row16161Neighbor,
    kColumn3Row16162Neighbor,
    kColumn3Row16163Neighbor,
    kColumn3Row16164Neighbor,
    kColumn3Row16165Neighbor,
    kColumn3Row16166Neighbor,
    kColumn3Row16167Neighbor,
    kColumn3Row16176Neighbor,
    kColumn3Row16192Neighbor,
    kColumn3Row16208Neighbor,
    kColumn3Row16224Neighbor,
    kColumn3Row16240Neighbor,
    kColumn3Row16256Neighbor,
    kColumn3Row16272Neighbor,
    kColumn3Row16288Neighbor,
    kColumn3Row16304Neighbor,
    kColumn3Row16320Neighbor,
    kColumn3Row16336Neighbor,
    kColumn3Row16352Neighbor,
    kColumn3Row16368Neighbor,
    kColumn3Row16384Neighbor,
    kColumn3Row23952Neighbor,
    kColumn3Row32144Neighbor,
    kColumn3Row32768Neighbor,
    kColumn3Row65536Neighbor,
    kColumn3Row66320Neighbor,
    kColumn3Row66336Neighbor,
    kColumn3Row71508Neighbor,
    kColumn3Row79700Neighbor,
    kColumn3Row79764Neighbor,
    kColumn3Row79828Neighbor,
    kColumn3Row98304Neighbor,
    kColumn3Row131072Neighbor,
    kColumn3Row132624Neighbor,
    kColumn3Row132640Neighbor,
    kColumn3Row157524Neighbor,
    kColumn3Row163840Neighbor,
    kColumn3Row165716Neighbor,
    kColumn3Row179600Neighbor,
    kColumn3Row196176Neighbor,
    kColumn3Row196240Neighbor,
    kColumn3Row196304Neighbor,
    kColumn3Row196608Neighbor,
    kColumn3Row198928Neighbor,
    kColumn3Row198944Neighbor,
    kColumn3Row208724Neighbor,
    kColumn3Row208788Neighbor,
    kColumn3Row208852Neighbor,
    kColumn3Row229376Neighbor,
    kColumn3Row237136Neighbor,
    kColumn3Row262144Neighbor,
    kColumn3Row265232Neighbor,
    kColumn3Row265248Neighbor,
    kColumn3Row294912Neighbor,
    kColumn3Row300884Neighbor,
    kColumn3Row307028Neighbor,
    kColumn3Row325460Neighbor,
    kColumn3Row327680Neighbor,
    kColumn3Row331536Neighbor,
    kColumn3Row331552Neighbor,
    kColumn3Row358228Neighbor,
    kColumn3Row360448Neighbor,
    kColumn3Row364372Neighbor,
    kColumn3Row384592Neighbor,
    kColumn3Row393216Neighbor,
    kColumn3Row397840Neighbor,
    kColumn3Row397856Neighbor,
    kColumn3Row408976Neighbor,
    kColumn3Row413524Neighbor,
    kColumn3Row425984Neighbor,
    kColumn3Row444244Neighbor,
    kColumn3Row458752Neighbor,
    kColumn3Row462676Neighbor,
    kColumn3Row464144Neighbor,
    kColumn3Row464160Neighbor,
    kColumn3Row482704Neighbor,
    kColumn3Row491520Neighbor,
    kColumn3Row507472Neighbor,
    kColumn3Row509780Neighbor,
    kColumn3Row509844Neighbor,
    kColumn3Row509908Neighbor,
    kColumn3Row516112Neighbor,
    kColumn3Row516128Neighbor,
    kColumn3Row516352Neighbor,
    kColumn3Row516360Neighbor,
    kColumn3Row517972Neighbor,
    kColumn4Row0Neighbor,
    kColumn4Row1Neighbor,
    kColumn4Row2Neighbor,
    kColumn4Row3Neighbor,
    kColumn4Row4Neighbor,
    kColumn4Row5Neighbor,
    kColumn4Row8Neighbor,
    kColumn4Row9Neighbor,
    kColumn4Row10Neighbor,
    kColumn4Row11Neighbor,
    kColumn4Row12Neighbor,
    kColumn4Row13Neighbor,
    kColumn4Row16Neighbor,
    kColumn4Row42Neighbor,
    kColumn4Row43Neighbor,
    kColumn4Row74Neighbor,
    kColumn4Row75Neighbor,
    kColumn4Row106Neighbor,
    kColumn4Row138Neighbor,
    kColumn4Row139Neighbor,
    kColumn4Row171Neighbor,
    kColumn4Row202Neighbor,
    kColumn4Row234Neighbor,
    kColumn4Row235Neighbor,
    kColumn4Row266Neighbor,
    kColumn4Row267Neighbor,
    kColumn4Row298Neighbor,
    kColumn4Row522Neighbor,
    kColumn4Row523Neighbor,
    kColumn4Row778Neighbor,
    kColumn4Row779Neighbor,
    kColumn4Row1034Neighbor,
    kColumn4Row1035Neighbor,
    kColumn4Row1291Neighbor,
    kColumn4Row1546Neighbor,
    kColumn4Row1547Neighbor,
    kColumn4Row1803Neighbor,
    kColumn4Row2058Neighbor,
    kColumn4Row2059Neighbor,
    kColumn4Row2315Neighbor,
    kColumn4Row2826Neighbor,
    kColumn4Row2827Neighbor,
    kColumn4Row3082Neighbor,
    kColumn4Row3083Neighbor,
    kColumn4Row3594Neighbor,
    kColumn4Row3595Neighbor,
    kColumn4Row4106Neighbor,
    kColumn4Row4362Neighbor,
    kColumn4Row4618Neighbor,
    kColumn4Row4619Neighbor,
    kColumn4Row5643Neighbor,
    kColumn4Row7178Neighbor,
    kColumn4Row7179Neighbor,
    kColumn4Row7691Neighbor,
    kColumn4Row8714Neighbor,
    kColumn4Row8715Neighbor,
    kColumn4Row9739Neighbor,
    kColumn4Row11274Neighbor,
    kColumn4Row11275Neighbor,
    kColumn4Row11787Neighbor,
    kColumn4Row12810Neighbor,
    kColumn4Row12811Neighbor,
    kColumn4Row13835Neighbor,
    kColumn4Row15370Neighbor,
    kColumn4Row15371Neighbor,
    kColumn4Row15883Neighbor,
    kColumn4Row17931Neighbor,
    kColumn4Row19466Neighbor,
    kColumn4Row19467Neighbor,
    kColumn4Row19979Neighbor,
    kColumn4Row22027Neighbor,
    kColumn4Row24075Neighbor,
    kColumn4Row26123Neighbor,
    kColumn4Row27658Neighbor,
    kColumn4Row28171Neighbor,
    kColumn4Row30219Neighbor,
    kColumn4Row32267Neighbor,
    kColumn4Row35850Neighbor,
    kColumn5Row0Neighbor,
    kColumn5Row1Neighbor,
    kColumn5Row2Neighbor,
    kColumn5Row3Neighbor,
    kColumn6Row0Neighbor,
    kColumn6Row1Neighbor,
    kColumn6Row2Neighbor,
    kColumn6Row3Neighbor,
    kColumn6Row4Neighbor,
    kColumn6Row5Neighbor,
    kColumn6Row6Neighbor,
    kColumn6Row7Neighbor,
    kColumn6Row8Neighbor,
    kColumn6Row9Neighbor,
    kColumn6Row11Neighbor,
    kColumn6Row12Neighbor,
    kColumn6Row13Neighbor,
    kColumn6Row28Neighbor,
    kColumn6Row44Neighbor,
    kColumn6Row60Neighbor,
    kColumn6Row76Neighbor,
    kColumn6Row92Neighbor,
    kColumn6Row108Neighbor,
    kColumn6Row124Neighbor,
    kColumn6Row1539Neighbor,
    kColumn6Row1547Neighbor,
    kColumn6Row1571Neighbor,
    kColumn6Row1579Neighbor,
    kColumn6Row2011Neighbor,
    kColumn6Row2019Neighbor,
    kColumn6Row2041Neighbor,
    kColumn6Row2045Neighbor,
    kColumn6Row2047Neighbor,
    kColumn6Row2049Neighbor,
    kColumn6Row2051Neighbor,
    kColumn6Row2053Neighbor,
    kColumn6Row4089Neighbor,
    kColumn7Row0Neighbor,
    kColumn7Row1Neighbor,
    kColumn7Row2Neighbor,
    kColumn7Row4Neighbor,
    kColumn7Row5Neighbor,
    kColumn7Row6Neighbor,
    kColumn7Row8Neighbor,
    kColumn7Row9Neighbor,
    kColumn7Row10Neighbor,
    kColumn7Row12Neighbor,
    kColumn7Row13Neighbor,
    kColumn7Row14Neighbor,
    kColumn7Row16Neighbor,
    kColumn7Row17Neighbor,
    kColumn7Row21Neighbor,
    kColumn7Row22Neighbor,
    kColumn7Row24Neighbor,
    kColumn7Row25Neighbor,
    kColumn7Row29Neighbor,
    kColumn7Row30Neighbor,
    kColumn7Row33Neighbor,
    kColumn7Row37Neighbor,
    kColumn7Row38Neighbor,
    kColumn7Row41Neighbor,
    kColumn7Row45Neighbor,
    kColumn7Row46Neighbor,
    kColumn7Row49Neighbor,
    kColumn7Row53Neighbor,
    kColumn7Row54Neighbor,
    kColumn7Row57Neighbor,
    kColumn7Row62Neighbor,
    kColumn7Row65Neighbor,
    kColumn7Row70Neighbor,
    kColumn7Row73Neighbor,
    kColumn7Row77Neighbor,
    kColumn7Row78Neighbor,
    kColumn7Row81Neighbor,
    kColumn7Row85Neighbor,
    kColumn7Row86Neighbor,
    kColumn7Row94Neighbor,
    kColumn7Row97Neighbor,
    kColumn7Row102Neighbor,
    kColumn7Row109Neighbor,
    kColumn7Row110Neighbor,
    kColumn7Row113Neighbor,
    kColumn7Row117Neighbor,
    kColumn7Row118Neighbor,
    kColumn7Row137Neighbor,
    kColumn7Row149Neighbor,
    kColumn7Row157Neighbor,
    kColumn7Row173Neighbor,
    kColumn7Row177Neighbor,
    kColumn7Row181Neighbor,
    kColumn7Row205Neighbor,
    kColumn7Row213Neighbor,
    kColumn7Row237Neighbor,
    kColumn7Row241Neighbor,
    kColumn7Row285Neighbor,
    kColumn7Row301Neighbor,
    kColumn7Row333Neighbor,
    kColumn7Row365Neighbor,
    kColumn7Row461Neighbor,
    kColumn7Row493Neighbor,
    kColumn7Row621Neighbor,
    kColumn7Row749Neighbor,
    kColumn7Row877Neighbor,
    kColumn7Row1565Neighbor,
    kColumn7Row1645Neighbor,
    kColumn7Row1773Neighbor,
    kColumn7Row1821Neighbor,
    kColumn7Row1901Neighbor,
    kColumn7Row2029Neighbor,
    kColumn7Row2157Neighbor,
    kColumn7Row2285Neighbor,
    kColumn7Row2413Neighbor,
    kColumn7Row2477Neighbor,
    kColumn7Row2509Neighbor,
    kColumn7Row2605Neighbor,
    kColumn7Row2637Neighbor,
    kColumn7Row2733Neighbor,
    kColumn7Row2765Neighbor,
    kColumn7Row3613Neighbor,
    kColumn7Row3693Neighbor,
    kColumn7Row3821Neighbor,
    kColumn7Row3869Neighbor,
    kColumn7Row3949Neighbor,
    kColumn7Row3953Neighbor,
    kColumn7Row4017Neighbor,
    kColumn7Row4077Neighbor,
    kColumn7Row4081Neighbor,
    kColumn7Row12305Neighbor,
    kColumn7Row12369Neighbor,
    kColumn7Row12561Neighbor,
    kColumn7Row12625Neighbor,
    kColumn7Row16081Neighbor,
    kColumn7Row16145Neighbor,
    kColumn7Row16321Neighbor,
    kColumn7Row16325Neighbor,
    kColumn7Row16342Neighbor,
    kColumn7Row16345Neighbor,
    kColumn7Row16353Neighbor,
    kColumn7Row16357Neighbor,
    kColumn7Row16361Neighbor,
    kColumn7Row16374Neighbor,
    kColumn7Row16377Neighbor,
    kColumn7Row16390Neighbor,
    kColumn7Row16422Neighbor,
    kColumn7Row32653Neighbor,
    kColumn7Row32661Neighbor,
    kColumn7Row32709Neighbor,
    kColumn7Row32725Neighbor,
    kColumn7Row32726Neighbor,
    kColumn7Row32741Neighbor,
    kColumn7Row32757Neighbor,
    kColumn7Row32758Neighbor,
    kColumn8Inter1Row0Neighbor,
    kColumn8Inter1Row1Neighbor,
    kColumn9Inter1Row0Neighbor,
    kColumn9Inter1Row1Neighbor,
    kColumn10Inter1Row0Neighbor,
    kColumn10Inter1Row1Neighbor,
    kColumn10Inter1Row2Neighbor,
    kColumn10Inter1Row5Neighbor,
    // Number of neighbors.
    kNumNeighbors,
  };

  static constexpr std::array<FieldElementT, 32> kKeccakKeccakKeccakRoundKey0PeriodicColumnData = {
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
  };
  static constexpr std::array<FieldElementT, 32> kKeccakKeccakKeccakRoundKey1PeriodicColumnData = {
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
  };
  static constexpr std::array<FieldElementT, 32> kKeccakKeccakKeccakRoundKey3PeriodicColumnData = {
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
  };
  static constexpr std::array<FieldElementT, 32> kKeccakKeccakKeccakRoundKey7PeriodicColumnData = {
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
  };
  static constexpr std::array<FieldElementT, 32> kKeccakKeccakKeccakRoundKey15PeriodicColumnData = {
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
  };
  static constexpr std::array<FieldElementT, 32> kKeccakKeccakKeccakRoundKey31PeriodicColumnData = {
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
  };
  static constexpr std::array<FieldElementT, 32> kKeccakKeccakKeccakRoundKey63PeriodicColumnData = {
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(0x2222222222222222_Z),
      FieldElementT::Zero(),
  };
  static constexpr std::array<FieldElementT, 8> kPoseidonPoseidonFullRoundKey0PeriodicColumnData = {
      FieldElementT::ConstexprFromBigInt(
          0x626c47a7d421fe1f13c4282214aa759291c78f926a2d1c6882031afe67ef4cd_Z),
      FieldElementT::ConstexprFromBigInt(
          0x5af083f36e4c729454361733f0883c5847cd2c5d9d4cb8b0465e60edce699d7_Z),
      FieldElementT::ConstexprFromBigInt(
          0x4332a6f6bde2f288e79ce13f47ad1cdeebd8870fd13a36b613b9721f6453a5d_Z),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(
          0x5334f75b052c0235119816883040da72c6d0a61538bdfff46d6a242bfeb7a1_Z),
      FieldElementT::ConstexprFromBigInt(
          0x5646a95a7c1ae86b34c0750ed2e641c538f93f13161be3c4957660f2e788965_Z),
      FieldElementT::ConstexprFromBigInt(
          0x62c9d42199f3b260e7cb8a115143106acf4f702e6b346fd202dc3b26a679d80_Z),
      FieldElementT::Zero(),
  };
  static constexpr std::array<FieldElementT, 8> kPoseidonPoseidonFullRoundKey1PeriodicColumnData = {
      FieldElementT::ConstexprFromBigInt(
          0x78985f8e16505035bd6df5518cfd41f2d327fcc948d772cadfe17baca05d6a6_Z),
      FieldElementT::ConstexprFromBigInt(
          0x7d71701bde3d06d54fa3f74f7b352a52d3975f92ff84b1ac77e709bfd388882_Z),
      FieldElementT::ConstexprFromBigInt(
          0x53d0ebf61664c685310a04c4dec2e7e4b9a813aaeff60d6c9e8caeb5cba78e7_Z),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(
          0x5d0af4fcbd9e056c1020cca9d871ae68f80ee4af2ec6547cd49d6dca50aa431_Z),
      FieldElementT::ConstexprFromBigInt(
          0x4b9f291d7b430c79fac36230a11f43e78581f5259692b52c90df47b7d4ec01a_Z),
      FieldElementT::ConstexprFromBigInt(
          0x51274d092db5099f180b1a8a13b7f2c7606836eabd8af54bf1d9ac2dc5717a5_Z),
      FieldElementT::Zero(),
  };
  static constexpr std::array<FieldElementT, 8> kPoseidonPoseidonFullRoundKey2PeriodicColumnData = {
      FieldElementT::ConstexprFromBigInt(
          0x5427f10867514a3204c659875341243c6e26a68b456dc1d142dcf34341696ff_Z),
      FieldElementT::ConstexprFromBigInt(
          0x603da06882019009c26f8a6320a1c5eac1b64f699ffea44e39584467a6b1d3e_Z),
      FieldElementT::ConstexprFromBigInt(
          0x5346a68894845835ae5ebcb88028d2a6c82f99f928494ee1bfc2d15eaabfebc_Z),
      FieldElementT::Zero(),
      FieldElementT::ConstexprFromBigInt(
          0x30131bce2fba5694114a19c46d24e00b4699dc00f1d53ba5ab99537901b1e65_Z),
      FieldElementT::ConstexprFromBigInt(
          0x5006d393d3480f41a98f19127072dc83e00becf6ceb4d73d890e74abae01a13_Z),
      FieldElementT::ConstexprFromBigInt(
          0x61fc552b8eb75e17ad0fb7aaa4ca528f415e14f0d9cdbed861a8db0bfff0c5b_Z),
      FieldElementT::Zero(),
  };
  static constexpr std::array<FieldElementT, 64>
      kPoseidonPoseidonPartialRoundKey0PeriodicColumnData = {
          FieldElementT::ConstexprFromBigInt(
              0x715b300ba7e9ce470fe5ba34f18ed3d7603052c57a6afe332f78f7144c3f8c7_Z),
          FieldElementT::ConstexprFromBigInt(
              0x4b28fea7f8ff0d9f5a75ecd89fd22d53a9250dafbb0a6203b252be800f5df3b_Z),
          FieldElementT::ConstexprFromBigInt(
              0xc45fe4e28d564b5322797fedc424c622938cc665a4eeb7fea6163a5411442_Z),
          FieldElementT::ConstexprFromBigInt(
              0x7466c11f6d2549a8c57f1c10580b65514f5ef22d7cfd22ee13f711ae0343e86_Z),
          FieldElementT::ConstexprFromBigInt(
              0x2c17b269b44f9311d5a6e8aea309b00622bdf141c0fa65a4053b333d375ecf_Z),
          FieldElementT::ConstexprFromBigInt(
              0x287c7e81f45637d126a8ec376b6924a19275c1198721e07ebd681be329efe62_Z),
          FieldElementT::ConstexprFromBigInt(
              0x2ef688dd0e9fc4b826b1917ca7d701a09e9a321771f4e2beae3150c8d50252b_Z),
          FieldElementT::ConstexprFromBigInt(
              0x3538bec3d3fb96a8d04a62bc7b7aecfbdbbbc99609f04d13d0b29a19030c493_Z),
          FieldElementT::ConstexprFromBigInt(
              0x51e4a321b12529d119070885749dcbbfd35b053f6a947871846e37d93e4125e_Z),
          FieldElementT::ConstexprFromBigInt(
              0x68c5cd0e4f3a18955ac549ba02558f582807bd37c1384f6721edbc6a4e87b50_Z),
          FieldElementT::ConstexprFromBigInt(
              0x158f5b3b127bacacb4d70db1e141d40f672dd952203182ab13538c23cc10203_Z),
          FieldElementT::ConstexprFromBigInt(
              0x1b93897bd2814e95e7dc8ee28a41d26a6523886e7957207bcf47893ec059548_Z),
          FieldElementT::ConstexprFromBigInt(
              0x57ab04dd907c69ff4973c27bf6c5fc6ab0b5c71c435fbe6c5f787a7bbd3d11f_Z),
          FieldElementT::ConstexprFromBigInt(
              0x38ca1e0831624fc75b68c2822dfce9c903b388f48731bdf0ab0cee778de199c_Z),
          FieldElementT::ConstexprFromBigInt(
              0x14bda74f19e9f1369f9a7e48bca35ebdc07c9ad9e7cabc09d9e83d8c29d2187_Z),
          FieldElementT::ConstexprFromBigInt(
              0xf9b81eb59d4ac9bcc53ef63c78635c81cf45815fa4c83c42f22d81284e3788_Z),
          FieldElementT::ConstexprFromBigInt(
              0x6d964abf31dd63255cca67730495afbc1ba8e7004afdae6687b70d2670afe2c_Z),
          FieldElementT::ConstexprFromBigInt(
              0x2588074c224c19afef635e4717b0390e34cffa6548e1553889c5e50e2083812_Z),
          FieldElementT::ConstexprFromBigInt(
              0xcc6dd68e59720483ab43aeae5b99ce79512c4699dc449370eec9e387692d1_Z),
          FieldElementT::ConstexprFromBigInt(
              0x4bd8261f67884e37eda9f6e75f1b95e23f6e86f880649aae70ca34b9cc5267a_Z),
          FieldElementT::ConstexprFromBigInt(
              0xce6d6f6f1f3e74dfef1057abc6d332c2afe679e1713824605139b06103a6de_Z),
          FieldElementT::ConstexprFromBigInt(
              0x65647cb339495ec4239e6a850297c0f76f1c979910c492bc12a1e5b4a58fd79_Z),
          FieldElementT::ConstexprFromBigInt(
              0x5be004a78944d16611a2c389c4e752a51b240c214ce2ec7a142157b0ba96e03_Z),
          FieldElementT::ConstexprFromBigInt(
              0xdf923be3c2a398058cdce719c696276f5a60277c8ec6b9b9547a982d23747c_Z),
          FieldElementT::ConstexprFromBigInt(
              0x25df62276f298c58566b24406177d9e9dfb4006d24db6822cfd679939d69cee_Z),
          FieldElementT::ConstexprFromBigInt(
              0x5835ff8630a9b66763b207d06b15926e2e04cbb2f1b17598e86c9ab6214a83c_Z),
          FieldElementT::ConstexprFromBigInt(
              0xd9649e6b75b73d946f93d980593a5d065f93651775d89f104378cf4e10a4f0_Z),
          FieldElementT::ConstexprFromBigInt(
              0x29a0d81fd50a25c43d9c08b4f492234472c95944de89cda8c3e68a9ce1038cd_Z),
          FieldElementT::ConstexprFromBigInt(
              0x3c13bfeee938e20658258e37b3e9cc2c2aa32af771096531d108c0de0c33b4f_Z),
          FieldElementT::ConstexprFromBigInt(
              0x262b58f49fd3501a4a0b8591dd22beb1001c330f06ec41300ac1ac57e3461b6_Z),
          FieldElementT::ConstexprFromBigInt(
              0x3697301bfec2af8ca29d19f79732d7cd3d8a0d3ae6130e5cf4a7cbb00a8dbc4_Z),
          FieldElementT::ConstexprFromBigInt(
              0x47815e2ebc1a4534c61ff464d9148e86c975b1b8f7ab1be929a639dbf1ea489_Z),
          FieldElementT::ConstexprFromBigInt(
              0xa40273965c5fe856134070f37b593f5010fc9df0b0fd243bcce0b8534821a2_Z),
          FieldElementT::ConstexprFromBigInt(
              0x6243d924df7513656eb3d772ea48e97ed5ee8428b396d72d31b56634d5359cd_Z),
          FieldElementT::ConstexprFromBigInt(
              0x4f66141b2694a0c3bd9e9616e0f72bba57b2f810d962ab0be63cb05261b0a55_Z),
          FieldElementT::ConstexprFromBigInt(
              0x6dbebbbdb1739c86ca1791e1d798e2238d9f84ceca4795fd53c9c1aafe97984_Z),
          FieldElementT::ConstexprFromBigInt(
              0x5758e0f890a002cbebc50afbfe6b2f766cf142ae8f70759c7e8e829d474ca4_Z),
          FieldElementT::ConstexprFromBigInt(
              0x260bdb625e9c7b8ee5177df61bcfc78385af770d78f595dd848901f715e062b_Z),
          FieldElementT::ConstexprFromBigInt(
              0x5f554b507757085ef34844f24c228b1ee7a9ed78471ac8cdeaab28f44fdebd6_Z),
          FieldElementT::ConstexprFromBigInt(
              0x2838a1799807455c74eb26d30010c7767dc582a906ef2276447102e4aa95b50_Z),
          FieldElementT::ConstexprFromBigInt(
              0x318438cab0cd45f6c7c5b83c20e6530ab8f9e14ec36e5f25bce0245a227fc8e_Z),
          FieldElementT::ConstexprFromBigInt(
              0x605ec3323ce6f87481800e379d8ef8a9fefc5c16945cad3f2f046bbc1d58749_Z),
          FieldElementT::ConstexprFromBigInt(
              0x30e2b1d9f20f77a90a9e0dab045a4f19fa92ddcf6cb29af2023d3e421351c61_Z),
          FieldElementT::ConstexprFromBigInt(
              0xbc81cd550756672fbbd413350f5390c78707bcc8dd3f84c6b6944b7e1c6935_Z),
          FieldElementT::ConstexprFromBigInt(
              0x1213d9c19af5fe57ab5fe4c3bf587df7abccc93306d22883b7f7752711ef644_Z),
          FieldElementT::ConstexprFromBigInt(
              0x1ddee1410bb9611e2abf6df4a8e72ea7d932e24eb26b327123c137dd4219d0_Z),
          FieldElementT::ConstexprFromBigInt(
              0x355e3353dd6e7e53a93d85e4deb6d677f808cee9cd142e7d722b2ae5590bc2b_Z),
          FieldElementT::ConstexprFromBigInt(
              0x5d676b601883b5c169d81fd5f1e5824da434a1ec404d55846d4a5b57aa6f5f9_Z),
          FieldElementT::ConstexprFromBigInt(
              0x57a07f058b27374146514086ae8a9babcb474645c8ccfee467bef0e0e5e530a_Z),
          FieldElementT::ConstexprFromBigInt(
              0x2b7c48a9211e88ca8f91f115c74892ec030c9a39030f9f29c49b3cb984bcebc_Z),
          FieldElementT::ConstexprFromBigInt(
              0x6800159d8ed8a62f4fe4ddfa5f14f83dd69847678aa2b20e4ab4831e51ee32d_Z),
          FieldElementT::ConstexprFromBigInt(
              0x2ed96ffb09ebed796092bfffd203ff11087315d087e1423cebc0bec2ac048a6_Z),
          FieldElementT::ConstexprFromBigInt(
              0x2f8dd9f5234b83202ae8793e70abb5c593611ba1418849da7ab2b2b92092462_Z),
          FieldElementT::ConstexprFromBigInt(
              0x15eb2c9f8e6b01e0b4de942b68085dc039156ed1f8dd7758a9ce9be033e8865_Z),
          FieldElementT::ConstexprFromBigInt(
              0x3c87e01ac8304a4b645c20073cfab7a4124be87d1d8f6dce3ee31a0a6c0c2e8_Z),
          FieldElementT::ConstexprFromBigInt(
              0x29df860b182740755e8d90d7c4d410bfe713fdfa596a074ffc8925ce77b395a_Z),
          FieldElementT::ConstexprFromBigInt(
              0x4a804728589d061e138ebc0eebafbe845226595ecc1ca1e4a6b116364c8aeac_Z),
          FieldElementT::ConstexprFromBigInt(
              0x5e0976db075fc19a0a19c9ca8664b557c26aef2d9bc76076337de3142081e6f_Z),
          FieldElementT::ConstexprFromBigInt(
              0x24f2bb5ec21e55004abb54231a96eaa40ce0fd4de12b25945e2e7c5266b9094_Z),
          FieldElementT::ConstexprFromBigInt(
              0x7214c366c41127ddcf0c07d9bcd2e92250317009622cb435e9669440e80d7ef_Z),
          FieldElementT::ConstexprFromBigInt(
              0x3e36f1fb242409ebe5c8b1ef1be7642cb4d924c0ef0d525278a0e556cb01141_Z),
          FieldElementT::Zero(),
          FieldElementT::Zero(),
          FieldElementT::Zero(),
      };
  static constexpr std::array<FieldElementT, 32>
      kPoseidonPoseidonPartialRoundKey1PeriodicColumnData = {
          FieldElementT::ConstexprFromBigInt(
              0x541a4e5bc368c682310641e43a5a95e1c973fab4268788b133af83db456cfc0_Z),
          FieldElementT::ConstexprFromBigInt(
              0x4225daee2754d8f8bb3041c83cfab9b597452edc0468de17b94d09f66bf6753_Z),
          FieldElementT::ConstexprFromBigInt(
              0x3b2036e65c2934de444f2d07a5f9aebccfadcabe1d494b7bc9047931e5aa921_Z),
          FieldElementT::ConstexprFromBigInt(
              0x5ac8536f3ddddf891b84bedfa4c5dcdbfd59fd6531960e30a57ab0ebf6b8671_Z),
          FieldElementT::ConstexprFromBigInt(
              0x39909d669f56588ee5056734a3c8055b807de9be7166bb290dc383f97f7fd8d_Z),
          FieldElementT::ConstexprFromBigInt(
              0x1fdb5e04d99ea129ca99ab7c80493d7d05bc99ee7bd684520bf6c48f3e2b168_Z),
          FieldElementT::ConstexprFromBigInt(
              0x4c7b52b1eae22cee2c06b7ca87e4d05d95a0782aad334b10b570457aafef191_Z),
          FieldElementT::ConstexprFromBigInt(
              0x31c2d85e7ebf7bb8539fc3191bbb72b106b04dc3d66417ceb4a6198bbd2a0c7_Z),
          FieldElementT::ConstexprFromBigInt(
              0x303985fc1e0fd56f2a4121fda030a80137f10dfa723a8b42bc426728953a8ab_Z),
          FieldElementT::ConstexprFromBigInt(
              0x1495cf4bc6f3a0bfc640a274e665e2c5e6b7a900b7b674b55fea1bbf60e11d_Z),
          FieldElementT::ConstexprFromBigInt(
              0x17c26425524168308c0b5f539c822036f7d6035905a8ab0822aeaa8cf3b9e32_Z),
          FieldElementT::ConstexprFromBigInt(
              0x73ffc9200911993648f49ec4cdac1e148c6d437d00e218e9334cca7f788a34a_Z),
          FieldElementT::ConstexprFromBigInt(
              0x5d584063c0782f76a2feec19ef4da8a5855c38e399da7a2c83566823b342923_Z),
          FieldElementT::ConstexprFromBigInt(
              0x411c1e7788d2d40d1fd4a8a486700b95dc85e57551ad225c678d624dba46be5_Z),
          FieldElementT::ConstexprFromBigInt(
              0x4bfafe37da6bdd3bf02ddd9c10f1e6b53ea0a7a873808be9691a7263058a0f0_Z),
          FieldElementT::ConstexprFromBigInt(
              0x5cf86ee759938b52eac230a0b79ce883efc571d332ea50cb71c4a75a75b8169_Z),
          FieldElementT::ConstexprFromBigInt(
              0x154da3a6854e67128b7895123dca30e3a18279c6e7bb2ae0c701bcba91b5028_Z),
          FieldElementT::ConstexprFromBigInt(
              0x1bfe0c88b382a9391b8dbca0aba43f62e04a992720f5de29d323be99ead4098_Z),
          FieldElementT::ConstexprFromBigInt(
              0x363013ba7f311db7a97d71f1b89c2540dc9b4dc96adad851c970d2682f7475f_Z),
          FieldElementT::Zero(),
          FieldElementT::Zero(),
          FieldElementT::Zero(),
          FieldElementT::Zero(),
          FieldElementT::Zero(),
          FieldElementT::Zero(),
          FieldElementT::Zero(),
          FieldElementT::Zero(),
          FieldElementT::Zero(),
          FieldElementT::Zero(),
          FieldElementT::Zero(),
          FieldElementT::Zero(),
          FieldElementT::Zero(),
      };

  enum Constraints {
    kCpuDecodeOpcodeRcBitCond,                                      // Constraint 0.
    kCpuDecodeOpcodeRcZeroCond,                                     // Constraint 1.
    kCpuDecodeOpcodeRcInputCond,                                    // Constraint 2.
    kCpuDecodeFlagOp1BaseOp0BitCond,                                // Constraint 3.
    kCpuDecodeFlagResOp1BitCond,                                    // Constraint 4.
    kCpuDecodeFlagPcUpdateRegularBitCond,                           // Constraint 5.
    kCpuDecodeFpUpdateRegularBitCond,                               // Constraint 6.
    kCpuOperandsMemDstAddrCond,                                     // Constraint 7.
    kCpuOperandsMem0AddrCond,                                       // Constraint 8.
    kCpuOperandsMem1AddrCond,                                       // Constraint 9.
    kCpuOperandsOpsMulCond,                                         // Constraint 10.
    kCpuOperandsResCond,                                            // Constraint 11.
    kCpuUpdateRegistersUpdatePcTmp0Cond,                            // Constraint 12.
    kCpuUpdateRegistersUpdatePcTmp1Cond,                            // Constraint 13.
    kCpuUpdateRegistersUpdatePcPcCondNegativeCond,                  // Constraint 14.
    kCpuUpdateRegistersUpdatePcPcCondPositiveCond,                  // Constraint 15.
    kCpuUpdateRegistersUpdateApApUpdateCond,                        // Constraint 16.
    kCpuUpdateRegistersUpdateFpFpUpdateCond,                        // Constraint 17.
    kCpuOpcodesCallPushFpCond,                                      // Constraint 18.
    kCpuOpcodesCallPushPcCond,                                      // Constraint 19.
    kCpuOpcodesCallOff0Cond,                                        // Constraint 20.
    kCpuOpcodesCallOff1Cond,                                        // Constraint 21.
    kCpuOpcodesCallFlagsCond,                                       // Constraint 22.
    kCpuOpcodesRetOff0Cond,                                         // Constraint 23.
    kCpuOpcodesRetOff2Cond,                                         // Constraint 24.
    kCpuOpcodesRetFlagsCond,                                        // Constraint 25.
    kCpuOpcodesAssertEqAssertEqCond,                                // Constraint 26.
    kInitialApCond,                                                 // Constraint 27.
    kInitialFpCond,                                                 // Constraint 28.
    kInitialPcCond,                                                 // Constraint 29.
    kFinalApCond,                                                   // Constraint 30.
    kFinalFpCond,                                                   // Constraint 31.
    kFinalPcCond,                                                   // Constraint 32.
    kMemoryMultiColumnPermPermInit0Cond,                            // Constraint 33.
    kMemoryMultiColumnPermPermStep0Cond,                            // Constraint 34.
    kMemoryMultiColumnPermPermLastCond,                             // Constraint 35.
    kMemoryDiffIsBitCond,                                           // Constraint 36.
    kMemoryIsFuncCond,                                              // Constraint 37.
    kMemoryInitialAddrCond,                                         // Constraint 38.
    kPublicMemoryAddrZeroCond,                                      // Constraint 39.
    kPublicMemoryValueZeroCond,                                     // Constraint 40.
    kRc16PermInit0Cond,                                             // Constraint 41.
    kRc16PermStep0Cond,                                             // Constraint 42.
    kRc16PermLastCond,                                              // Constraint 43.
    kRc16DiffIsBitCond,                                             // Constraint 44.
    kRc16MinimumCond,                                               // Constraint 45.
    kRc16MaximumCond,                                               // Constraint 46.
    kDilutedCheckPermutationInit0Cond,                              // Constraint 47.
    kDilutedCheckPermutationStep0Cond,                              // Constraint 48.
    kDilutedCheckPermutationLastCond,                               // Constraint 49.
    kDilutedCheckInitCond,                                          // Constraint 50.
    kDilutedCheckFirstElementCond,                                  // Constraint 51.
    kDilutedCheckStepCond,                                          // Constraint 52.
    kDilutedCheckLastCond,                                          // Constraint 53.
    kPedersenHash0EcSubsetSumBitUnpackingLastOneIsZeroCond,         // Constraint 54.
    kPedersenHash0EcSubsetSumBitUnpackingZeroesBetweenOnes0Cond,    // Constraint 55.
    kPedersenHash0EcSubsetSumBitUnpackingCumulativeBit192Cond,      // Constraint 56.
    kPedersenHash0EcSubsetSumBitUnpackingZeroesBetweenOnes192Cond,  // Constraint 57.
    kPedersenHash0EcSubsetSumBitUnpackingCumulativeBit196Cond,      // Constraint 58.
    kPedersenHash0EcSubsetSumBitUnpackingZeroesBetweenOnes196Cond,  // Constraint 59.
    kPedersenHash0EcSubsetSumBooleanityTestCond,                    // Constraint 60.
    kPedersenHash0EcSubsetSumBitExtractionEndCond,                  // Constraint 61.
    kPedersenHash0EcSubsetSumZerosTailCond,                         // Constraint 62.
    kPedersenHash0EcSubsetSumAddPointsSlopeCond,                    // Constraint 63.
    kPedersenHash0EcSubsetSumAddPointsXCond,                        // Constraint 64.
    kPedersenHash0EcSubsetSumAddPointsYCond,                        // Constraint 65.
    kPedersenHash0EcSubsetSumCopyPointXCond,                        // Constraint 66.
    kPedersenHash0EcSubsetSumCopyPointYCond,                        // Constraint 67.
    kPedersenHash0CopyPointXCond,                                   // Constraint 68.
    kPedersenHash0CopyPointYCond,                                   // Constraint 69.
    kPedersenHash0InitXCond,                                        // Constraint 70.
    kPedersenHash0InitYCond,                                        // Constraint 71.
    kPedersenInput0Value0Cond,                                      // Constraint 72.
    kPedersenInput0AddrCond,                                        // Constraint 73.
    kPedersenInitAddrCond,                                          // Constraint 74.
    kPedersenInput1Value0Cond,                                      // Constraint 75.
    kPedersenInput1AddrCond,                                        // Constraint 76.
    kPedersenOutputValue0Cond,                                      // Constraint 77.
    kPedersenOutputAddrCond,                                        // Constraint 78.
    kRcBuiltinValueCond,                                            // Constraint 79.
    kRcBuiltinAddrStepCond,                                         // Constraint 80.
    kRcBuiltinInitAddrCond,                                         // Constraint 81.
    kEcdsaSignature0DoublingKeySlopeCond,                           // Constraint 82.
    kEcdsaSignature0DoublingKeyXCond,                               // Constraint 83.
    kEcdsaSignature0DoublingKeyYCond,                               // Constraint 84.
    kEcdsaSignature0ExponentiateGeneratorBooleanityTestCond,        // Constraint 85.
    kEcdsaSignature0ExponentiateGeneratorBitExtractionEndCond,      // Constraint 86.
    kEcdsaSignature0ExponentiateGeneratorZerosTailCond,             // Constraint 87.
    kEcdsaSignature0ExponentiateGeneratorAddPointsSlopeCond,        // Constraint 88.
    kEcdsaSignature0ExponentiateGeneratorAddPointsXCond,            // Constraint 89.
    kEcdsaSignature0ExponentiateGeneratorAddPointsYCond,            // Constraint 90.
    kEcdsaSignature0ExponentiateGeneratorAddPointsXDiffInvCond,     // Constraint 91.
    kEcdsaSignature0ExponentiateGeneratorCopyPointXCond,            // Constraint 92.
    kEcdsaSignature0ExponentiateGeneratorCopyPointYCond,            // Constraint 93.
    kEcdsaSignature0ExponentiateKeyBooleanityTestCond,              // Constraint 94.
    kEcdsaSignature0ExponentiateKeyBitExtractionEndCond,            // Constraint 95.
    kEcdsaSignature0ExponentiateKeyZerosTailCond,                   // Constraint 96.
    kEcdsaSignature0ExponentiateKeyAddPointsSlopeCond,              // Constraint 97.
    kEcdsaSignature0ExponentiateKeyAddPointsXCond,                  // Constraint 98.
    kEcdsaSignature0ExponentiateKeyAddPointsYCond,                  // Constraint 99.
    kEcdsaSignature0ExponentiateKeyAddPointsXDiffInvCond,           // Constraint 100.
    kEcdsaSignature0ExponentiateKeyCopyPointXCond,                  // Constraint 101.
    kEcdsaSignature0ExponentiateKeyCopyPointYCond,                  // Constraint 102.
    kEcdsaSignature0InitGenXCond,                                   // Constraint 103.
    kEcdsaSignature0InitGenYCond,                                   // Constraint 104.
    kEcdsaSignature0InitKeyXCond,                                   // Constraint 105.
    kEcdsaSignature0InitKeyYCond,                                   // Constraint 106.
    kEcdsaSignature0AddResultsSlopeCond,                            // Constraint 107.
    kEcdsaSignature0AddResultsXCond,                                // Constraint 108.
    kEcdsaSignature0AddResultsYCond,                                // Constraint 109.
    kEcdsaSignature0AddResultsXDiffInvCond,                         // Constraint 110.
    kEcdsaSignature0ExtractRSlopeCond,                              // Constraint 111.
    kEcdsaSignature0ExtractRXCond,                                  // Constraint 112.
    kEcdsaSignature0ExtractRXDiffInvCond,                           // Constraint 113.
    kEcdsaSignature0ZNonzeroCond,                                   // Constraint 114.
    kEcdsaSignature0RAndWNonzeroCond,                               // Constraint 115.
    kEcdsaSignature0QOnCurveXSquaredCond,                           // Constraint 116.
    kEcdsaSignature0QOnCurveOnCurveCond,                            // Constraint 117.
    kEcdsaInitAddrCond,                                             // Constraint 118.
    kEcdsaMessageAddrCond,                                          // Constraint 119.
    kEcdsaPubkeyAddrCond,                                           // Constraint 120.
    kEcdsaMessageValue0Cond,                                        // Constraint 121.
    kEcdsaPubkeyValue0Cond,                                         // Constraint 122.
    kBitwiseInitVarPoolAddrCond,                                    // Constraint 123.
    kBitwiseStepVarPoolAddrCond,                                    // Constraint 124.
    kBitwiseXOrYAddrCond,                                           // Constraint 125.
    kBitwiseNextVarPoolAddrCond,                                    // Constraint 126.
    kBitwisePartitionCond,                                          // Constraint 127.
    kBitwiseOrIsAndPlusXorCond,                                     // Constraint 128.
    kBitwiseAdditionIsXorWithAndCond,                               // Constraint 129.
    kBitwiseUniqueUnpacking192Cond,                                 // Constraint 130.
    kBitwiseUniqueUnpacking193Cond,                                 // Constraint 131.
    kBitwiseUniqueUnpacking194Cond,                                 // Constraint 132.
    kBitwiseUniqueUnpacking195Cond,                                 // Constraint 133.
    kEcOpInitAddrCond,                                              // Constraint 134.
    kEcOpPXAddrCond,                                                // Constraint 135.
    kEcOpPYAddrCond,                                                // Constraint 136.
    kEcOpQXAddrCond,                                                // Constraint 137.
    kEcOpQYAddrCond,                                                // Constraint 138.
    kEcOpMAddrCond,                                                 // Constraint 139.
    kEcOpRXAddrCond,                                                // Constraint 140.
    kEcOpRYAddrCond,                                                // Constraint 141.
    kEcOpDoublingQSlopeCond,                                        // Constraint 142.
    kEcOpDoublingQXCond,                                            // Constraint 143.
    kEcOpDoublingQYCond,                                            // Constraint 144.
    kEcOpGetQXCond,                                                 // Constraint 145.
    kEcOpGetQYCond,                                                 // Constraint 146.
    kEcOpEcSubsetSumBitUnpackingLastOneIsZeroCond,                  // Constraint 147.
    kEcOpEcSubsetSumBitUnpackingZeroesBetweenOnes0Cond,             // Constraint 148.
    kEcOpEcSubsetSumBitUnpackingCumulativeBit192Cond,               // Constraint 149.
    kEcOpEcSubsetSumBitUnpackingZeroesBetweenOnes192Cond,           // Constraint 150.
    kEcOpEcSubsetSumBitUnpackingCumulativeBit196Cond,               // Constraint 151.
    kEcOpEcSubsetSumBitUnpackingZeroesBetweenOnes196Cond,           // Constraint 152.
    kEcOpEcSubsetSumBooleanityTestCond,                             // Constraint 153.
    kEcOpEcSubsetSumBitExtractionEndCond,                           // Constraint 154.
    kEcOpEcSubsetSumZerosTailCond,                                  // Constraint 155.
    kEcOpEcSubsetSumAddPointsSlopeCond,                             // Constraint 156.
    kEcOpEcSubsetSumAddPointsXCond,                                 // Constraint 157.
    kEcOpEcSubsetSumAddPointsYCond,                                 // Constraint 158.
    kEcOpEcSubsetSumAddPointsXDiffInvCond,                          // Constraint 159.
    kEcOpEcSubsetSumCopyPointXCond,                                 // Constraint 160.
    kEcOpEcSubsetSumCopyPointYCond,                                 // Constraint 161.
    kEcOpGetMCond,                                                  // Constraint 162.
    kEcOpGetPXCond,                                                 // Constraint 163.
    kEcOpGetPYCond,                                                 // Constraint 164.
    kEcOpSetRXCond,                                                 // Constraint 165.
    kEcOpSetRYCond,                                                 // Constraint 166.
    kKeccakInitInputOutputAddrCond,                                 // Constraint 167.
    kKeccakAddrInputOutputStepCond,                                 // Constraint 168.
    kKeccakKeccakParseToDilutedReshapeIntermediate0W0Cond,          // Constraint 169.
    kKeccakKeccakParseToDilutedReshapeIntermediate0W1Cond,          // Constraint 170.
    kKeccakKeccakParseToDilutedReshapeIntermediate0W2Cond,          // Constraint 171.
    kKeccakKeccakParseToDilutedReshapeIntermediate0W3Cond,          // Constraint 172.
    kKeccakKeccakParseToDilutedReshapeIntermediate0W4Cond,          // Constraint 173.
    kKeccakKeccakParseToDilutedReshapeIntermediate0W5Cond,          // Constraint 174.
    kKeccakKeccakParseToDilutedReshapeIntermediate0W6Cond,          // Constraint 175.
    kKeccakKeccakParseToDilutedReshapeIntermediate0W7Cond,          // Constraint 176.
    kKeccakKeccakParseToDilutedReshapeIntermediate1W0Cond,          // Constraint 177.
    kKeccakKeccakParseToDilutedReshapeIntermediate1W1Cond,          // Constraint 178.
    kKeccakKeccakParseToDilutedReshapeIntermediate1W2Cond,          // Constraint 179.
    kKeccakKeccakParseToDilutedReshapeIntermediate1W3Cond,          // Constraint 180.
    kKeccakKeccakParseToDilutedReshapeIntermediate1W4Cond,          // Constraint 181.
    kKeccakKeccakParseToDilutedReshapeIntermediate1W5Cond,          // Constraint 182.
    kKeccakKeccakParseToDilutedReshapeIntermediate1W6Cond,          // Constraint 183.
    kKeccakKeccakParseToDilutedReshapeIntermediate1W7Cond,          // Constraint 184.
    kKeccakKeccakParseToDilutedReshapeFinal0Cond,                   // Constraint 185.
    kKeccakKeccakParseToDilutedReshapeFinal1Cond,                   // Constraint 186.
    kKeccakKeccakParseToDilutedReshapeFinal2Cond,                   // Constraint 187.
    kKeccakKeccakParseToDilutedReshapeFinal3Cond,                   // Constraint 188.
    kKeccakKeccakParseToDilutedReshapeFinal4Cond,                   // Constraint 189.
    kKeccakKeccakParseToDilutedReshapeFinal5Cond,                   // Constraint 190.
    kKeccakKeccakParseToDilutedReshapeFinal6Cond,                   // Constraint 191.
    kKeccakKeccakParseToDilutedReshapeFinal7Cond,                   // Constraint 192.
    kKeccakKeccakParseToDilutedReshapeFinal8Cond,                   // Constraint 193.
    kKeccakKeccakParseToDilutedReshapeFinal9Cond,                   // Constraint 194.
    kKeccakKeccakParseToDilutedReshapeFinal10Cond,                  // Constraint 195.
    kKeccakKeccakParseToDilutedReshapeFinal11Cond,                  // Constraint 196.
    kKeccakKeccakParseToDilutedReshapeFinal12Cond,                  // Constraint 197.
    kKeccakKeccakParseToDilutedReshapeFinal13Cond,                  // Constraint 198.
    kKeccakKeccakParseToDilutedReshapeFinal14Cond,                  // Constraint 199.
    kKeccakKeccakParseToDilutedReshapeFinal15Cond,                  // Constraint 200.
    kKeccakKeccakParseToDilutedStartAccumulationCond,               // Constraint 201.
    kKeccakKeccakParseToDilutedInitFirstInvocation0Cond,            // Constraint 202.
    kKeccakKeccakParseToDilutedInitOtherInvocations0Cond,           // Constraint 203.
    kKeccakKeccakParseToDilutedInitFirstInvocation1Cond,            // Constraint 204.
    kKeccakKeccakParseToDilutedInitOtherInvocations1Cond,           // Constraint 205.
    kKeccakKeccakParseToDilutedInitFirstInvocation2Cond,            // Constraint 206.
    kKeccakKeccakParseToDilutedInitOtherInvocations2Cond,           // Constraint 207.
    kKeccakKeccakParseToDilutedInitFirstInvocation3Cond,            // Constraint 208.
    kKeccakKeccakParseToDilutedInitOtherInvocations3Cond,           // Constraint 209.
    kKeccakKeccakParseToDilutedInitFirstInvocation4Cond,            // Constraint 210.
    kKeccakKeccakParseToDilutedInitOtherInvocations4Cond,           // Constraint 211.
    kKeccakKeccakParseToDilutedInitFirstInvocation5Cond,            // Constraint 212.
    kKeccakKeccakParseToDilutedInitOtherInvocations5Cond,           // Constraint 213.
    kKeccakKeccakParseToDilutedInitFirstInvocation6Cond,            // Constraint 214.
    kKeccakKeccakParseToDilutedInitOtherInvocations6Cond,           // Constraint 215.
    kKeccakKeccakParseToDilutedInitFirstInvocation7Cond,            // Constraint 216.
    kKeccakKeccakParseToDilutedInitOtherInvocations7Cond,           // Constraint 217.
    kKeccakKeccakParseToDilutedExtractBitFirstInvocation1Cond,      // Constraint 218.
    kKeccakKeccakParseToDilutedExtractBitOtherInvocations1Cond,     // Constraint 219.
    kKeccakKeccakParseToDilutedToDiluted0P1Cond,                    // Constraint 220.
    kKeccakKeccakParseToDilutedToDiluted1P1Cond,                    // Constraint 221.
    kKeccakKeccakParseToDilutedExtractBitFirstInvocation0Cond,      // Constraint 222.
    kKeccakKeccakParseToDilutedExtractBitOtherInvocations0Cond,     // Constraint 223.
    kKeccakKeccakParseToDilutedToDiluted0P0Cond,                    // Constraint 224.
    kKeccakKeccakParseToDilutedToDiluted1P0Cond,                    // Constraint 225.
    kKeccakKeccakParity0Cond,                                       // Constraint 226.
    kKeccakKeccakParity1Cond,                                       // Constraint 227.
    kKeccakKeccakParity2Cond,                                       // Constraint 228.
    kKeccakKeccakParity3Cond,                                       // Constraint 229.
    kKeccakKeccakParity4Cond,                                       // Constraint 230.
    kKeccakKeccakRotateParity0N0Cond,                               // Constraint 231.
    kKeccakKeccakRotateParity0N1Cond,                               // Constraint 232.
    kKeccakKeccakRotateParity1N0Cond,                               // Constraint 233.
    kKeccakKeccakRotateParity1N1Cond,                               // Constraint 234.
    kKeccakKeccakRotateParity2N0Cond,                               // Constraint 235.
    kKeccakKeccakRotateParity2N1Cond,                               // Constraint 236.
    kKeccakKeccakRotateParity3N0Cond,                               // Constraint 237.
    kKeccakKeccakRotateParity3N1Cond,                               // Constraint 238.
    kKeccakKeccakRotateParity4N0Cond,                               // Constraint 239.
    kKeccakKeccakRotateParity4N1Cond,                               // Constraint 240.
    kKeccakKeccakThetaRhoPiI0J0Cond,                                // Constraint 241.
    kKeccakKeccakThetaRhoPiI0J1N0Cond,                              // Constraint 242.
    kKeccakKeccakThetaRhoPiI0J1N1Cond,                              // Constraint 243.
    kKeccakKeccakThetaRhoPiI0J2N0Cond,                              // Constraint 244.
    kKeccakKeccakThetaRhoPiI0J2N1Cond,                              // Constraint 245.
    kKeccakKeccakThetaRhoPiI0J3N0Cond,                              // Constraint 246.
    kKeccakKeccakThetaRhoPiI0J3N1Cond,                              // Constraint 247.
    kKeccakKeccakThetaRhoPiI0J4N0Cond,                              // Constraint 248.
    kKeccakKeccakThetaRhoPiI0J4N1Cond,                              // Constraint 249.
    kKeccakKeccakThetaRhoPiI1J0N0Cond,                              // Constraint 250.
    kKeccakKeccakThetaRhoPiI1J0N1Cond,                              // Constraint 251.
    kKeccakKeccakThetaRhoPiI1J1N0Cond,                              // Constraint 252.
    kKeccakKeccakThetaRhoPiI1J1N1Cond,                              // Constraint 253.
    kKeccakKeccakThetaRhoPiI1J2N0Cond,                              // Constraint 254.
    kKeccakKeccakThetaRhoPiI1J2N1Cond,                              // Constraint 255.
    kKeccakKeccakThetaRhoPiI1J3N0Cond,                              // Constraint 256.
    kKeccakKeccakThetaRhoPiI1J3N1Cond,                              // Constraint 257.
    kKeccakKeccakThetaRhoPiI1J3N2Cond,                              // Constraint 258.
    kKeccakKeccakThetaRhoPiI1J3N3Cond,                              // Constraint 259.
    kKeccakKeccakThetaRhoPiI1J3N4Cond,                              // Constraint 260.
    kKeccakKeccakThetaRhoPiI1J3N5Cond,                              // Constraint 261.
    kKeccakKeccakThetaRhoPiI1J4N0Cond,                              // Constraint 262.
    kKeccakKeccakThetaRhoPiI1J4N1Cond,                              // Constraint 263.
    kKeccakKeccakThetaRhoPiI2J0N0Cond,                              // Constraint 264.
    kKeccakKeccakThetaRhoPiI2J0N1Cond,                              // Constraint 265.
    kKeccakKeccakThetaRhoPiI2J1N0Cond,                              // Constraint 266.
    kKeccakKeccakThetaRhoPiI2J1N1Cond,                              // Constraint 267.
    kKeccakKeccakThetaRhoPiI2J2N0Cond,                              // Constraint 268.
    kKeccakKeccakThetaRhoPiI2J2N1Cond,                              // Constraint 269.
    kKeccakKeccakThetaRhoPiI2J3N0Cond,                              // Constraint 270.
    kKeccakKeccakThetaRhoPiI2J3N1Cond,                              // Constraint 271.
    kKeccakKeccakThetaRhoPiI2J4N0Cond,                              // Constraint 272.
    kKeccakKeccakThetaRhoPiI2J4N1Cond,                              // Constraint 273.
    kKeccakKeccakThetaRhoPiI2J4N2Cond,                              // Constraint 274.
    kKeccakKeccakThetaRhoPiI2J4N3Cond,                              // Constraint 275.
    kKeccakKeccakThetaRhoPiI2J4N4Cond,                              // Constraint 276.
    kKeccakKeccakThetaRhoPiI2J4N5Cond,                              // Constraint 277.
    kKeccakKeccakThetaRhoPiI3J0N0Cond,                              // Constraint 278.
    kKeccakKeccakThetaRhoPiI3J0N1Cond,                              // Constraint 279.
    kKeccakKeccakThetaRhoPiI3J0N2Cond,                              // Constraint 280.
    kKeccakKeccakThetaRhoPiI3J0N3Cond,                              // Constraint 281.
    kKeccakKeccakThetaRhoPiI3J0N4Cond,                              // Constraint 282.
    kKeccakKeccakThetaRhoPiI3J0N5Cond,                              // Constraint 283.
    kKeccakKeccakThetaRhoPiI3J1N0Cond,                              // Constraint 284.
    kKeccakKeccakThetaRhoPiI3J1N1Cond,                              // Constraint 285.
    kKeccakKeccakThetaRhoPiI3J2N0Cond,                              // Constraint 286.
    kKeccakKeccakThetaRhoPiI3J2N1Cond,                              // Constraint 287.
    kKeccakKeccakThetaRhoPiI3J3N0Cond,                              // Constraint 288.
    kKeccakKeccakThetaRhoPiI3J3N1Cond,                              // Constraint 289.
    kKeccakKeccakThetaRhoPiI3J4N0Cond,                              // Constraint 290.
    kKeccakKeccakThetaRhoPiI3J4N1Cond,                              // Constraint 291.
    kKeccakKeccakThetaRhoPiI4J0N0Cond,                              // Constraint 292.
    kKeccakKeccakThetaRhoPiI4J0N1Cond,                              // Constraint 293.
    kKeccakKeccakThetaRhoPiI4J1N0Cond,                              // Constraint 294.
    kKeccakKeccakThetaRhoPiI4J1N1Cond,                              // Constraint 295.
    kKeccakKeccakThetaRhoPiI4J1N2Cond,                              // Constraint 296.
    kKeccakKeccakThetaRhoPiI4J1N3Cond,                              // Constraint 297.
    kKeccakKeccakThetaRhoPiI4J1N4Cond,                              // Constraint 298.
    kKeccakKeccakThetaRhoPiI4J1N5Cond,                              // Constraint 299.
    kKeccakKeccakThetaRhoPiI4J2N0Cond,                              // Constraint 300.
    kKeccakKeccakThetaRhoPiI4J2N1Cond,                              // Constraint 301.
    kKeccakKeccakThetaRhoPiI4J3N0Cond,                              // Constraint 302.
    kKeccakKeccakThetaRhoPiI4J3N1Cond,                              // Constraint 303.
    kKeccakKeccakThetaRhoPiI4J4N0Cond,                              // Constraint 304.
    kKeccakKeccakThetaRhoPiI4J4N1Cond,                              // Constraint 305.
    kKeccakKeccakChiIota0Cond,                                      // Constraint 306.
    kKeccakKeccakChiIota1Cond,                                      // Constraint 307.
    kKeccakKeccakChiIota3Cond,                                      // Constraint 308.
    kKeccakKeccakChiIota7Cond,                                      // Constraint 309.
    kKeccakKeccakChiIota15Cond,                                     // Constraint 310.
    kKeccakKeccakChiIota31Cond,                                     // Constraint 311.
    kKeccakKeccakChiIota63Cond,                                     // Constraint 312.
    kKeccakKeccakChi0Cond,                                          // Constraint 313.
    kKeccakKeccakChi1Cond,                                          // Constraint 314.
    kKeccakKeccakChi2Cond,                                          // Constraint 315.
    kPoseidonInitInputOutputAddrCond,                               // Constraint 316.
    kPoseidonAddrInputOutputStepInnerCond,                          // Constraint 317.
    kPoseidonAddrInputOutputStepOutterCond,                         // Constraint 318.
    kPoseidonPoseidonFullRoundsState0SquaringCond,                  // Constraint 319.
    kPoseidonPoseidonFullRoundsState1SquaringCond,                  // Constraint 320.
    kPoseidonPoseidonFullRoundsState2SquaringCond,                  // Constraint 321.
    kPoseidonPoseidonPartialRoundsState0SquaringCond,               // Constraint 322.
    kPoseidonPoseidonPartialRoundsState1SquaringCond,               // Constraint 323.
    kPoseidonPoseidonAddFirstRoundKey0Cond,                         // Constraint 324.
    kPoseidonPoseidonAddFirstRoundKey1Cond,                         // Constraint 325.
    kPoseidonPoseidonAddFirstRoundKey2Cond,                         // Constraint 326.
    kPoseidonPoseidonFullRound0Cond,                                // Constraint 327.
    kPoseidonPoseidonFullRound1Cond,                                // Constraint 328.
    kPoseidonPoseidonFullRound2Cond,                                // Constraint 329.
    kPoseidonPoseidonLastFullRound0Cond,                            // Constraint 330.
    kPoseidonPoseidonLastFullRound1Cond,                            // Constraint 331.
    kPoseidonPoseidonLastFullRound2Cond,                            // Constraint 332.
    kPoseidonPoseidonCopyPartialRounds0I0Cond,                      // Constraint 333.
    kPoseidonPoseidonCopyPartialRounds0I1Cond,                      // Constraint 334.
    kPoseidonPoseidonCopyPartialRounds0I2Cond,                      // Constraint 335.
    kPoseidonPoseidonMarginFullToPartial0Cond,                      // Constraint 336.
    kPoseidonPoseidonMarginFullToPartial1Cond,                      // Constraint 337.
    kPoseidonPoseidonMarginFullToPartial2Cond,                      // Constraint 338.
    kPoseidonPoseidonPartialRound0Cond,                             // Constraint 339.
    kPoseidonPoseidonPartialRound1Cond,                             // Constraint 340.
    kPoseidonPoseidonMarginPartialToFull0Cond,                      // Constraint 341.
    kPoseidonPoseidonMarginPartialToFull1Cond,                      // Constraint 342.
    kPoseidonPoseidonMarginPartialToFull2Cond,                      // Constraint 343.
    kNumConstraints,                                                // Number of constraints.
  };

 public:
  using EcPointT = EcPoint<FieldElementT>;
  using HashContextT = PedersenHashContext<FieldElementT>;
  using SigConfigT = typename EcdsaComponent<FieldElementT>::Config;
  using EcOpCurveConfigT = typename EllipticCurveConstants<FieldElementT>::CurveConfig;

  explicit CpuAirDefinition(
      uint64_t trace_length, const FieldElementT& rc_min, const FieldElementT& rc_max,
      const MemSegmentAddresses& mem_segment_addresses,
      const HashContextT& hash_context)
      : Air(trace_length),
        initial_ap_(
            FieldElementT::FromUint(GetSegment(mem_segment_addresses, "execution").begin_addr)),
        final_ap_(FieldElementT::FromUint(GetSegment(mem_segment_addresses, "execution").stop_ptr)),
        initial_pc_(
            FieldElementT::FromUint(GetSegment(mem_segment_addresses, "program").begin_addr)),
        final_pc_(FieldElementT::FromUint(GetSegment(mem_segment_addresses, "program").stop_ptr)),
        pedersen_begin_addr_(
            kHasPedersenBuiltin ? GetSegment(mem_segment_addresses, "pedersen").begin_addr : 0),
        rc_begin_addr_(
            kHasRangeCheckBuiltin ? GetSegment(mem_segment_addresses, "range_check").begin_addr
                                  : 0),
        ecdsa_begin_addr_(
            kHasEcdsaBuiltin ? GetSegment(mem_segment_addresses, "ecdsa").begin_addr : 0),
        bitwise_begin_addr_(
            kHasBitwiseBuiltin ? GetSegment(mem_segment_addresses, "bitwise").begin_addr : 0),
        ec_op_begin_addr_(
            kHasEcOpBuiltin ? GetSegment(mem_segment_addresses, "ec_op").begin_addr : 0),
        keccak_begin_addr_(
            kHasKeccakBuiltin ? GetSegment(mem_segment_addresses, "keccak").begin_addr : 0),
        poseidon_begin_addr_(
            kHasPoseidonBuiltin ? GetSegment(mem_segment_addresses, "poseidon").begin_addr : 0),
        rc_min_(rc_min),
        rc_max_(rc_max),
        pedersen__shift_point_(hash_context.shift_point),
        ecdsa__sig_config_(EcdsaComponent<FieldElementT>::GetSigConfig()),
        ec_op__curve_config_{
            kPrimeFieldEc0.k_alpha, kPrimeFieldEc0.k_beta, kPrimeFieldEc0.k_order} {}

  static constexpr uint64_t kOffsetBits = CpuComponent<FieldElementT>::kOffsetBits;

 protected:
  const FieldElementT offset_size_ = FieldElementT::FromUint(Pow2(kOffsetBits));
  const FieldElementT half_offset_size_ = FieldElementT::FromUint(Pow2(kOffsetBits - 1));
  const FieldElementT initial_ap_;
  const FieldElementT final_ap_;
  const FieldElementT initial_pc_;
  const FieldElementT final_pc_;

  const CompileTimeOptional<uint64_t, kHasPedersenBuiltin> pedersen_begin_addr_;
  const CompileTimeOptional<FieldElementT, kHasPedersenBuiltin> initial_pedersen_addr_ =
      FieldElementT::FromUint(ExtractHiddenMemberValue(pedersen_begin_addr_));

  const CompileTimeOptional<uint64_t, kHasRangeCheckBuiltin> rc_begin_addr_;
  const CompileTimeOptional<FieldElementT, kHasRangeCheckBuiltin> initial_rc_addr_ =
      FieldElementT::FromUint(ExtractHiddenMemberValue(rc_begin_addr_));

  const CompileTimeOptional<uint64_t, kHasEcdsaBuiltin> ecdsa_begin_addr_;
  const CompileTimeOptional<FieldElementT, kHasEcdsaBuiltin> initial_ecdsa_addr_ =
      FieldElementT::FromUint(ExtractHiddenMemberValue(ecdsa_begin_addr_));

  const CompileTimeOptional<uint64_t, kHasBitwiseBuiltin> bitwise_begin_addr_;
  const CompileTimeOptional<FieldElementT, kHasBitwiseBuiltin> initial_bitwise_addr_ =
      FieldElementT::FromUint(ExtractHiddenMemberValue(bitwise_begin_addr_));

  const CompileTimeOptional<uint64_t, kHasEcOpBuiltin> ec_op_begin_addr_;
  const CompileTimeOptional<FieldElementT, kHasEcOpBuiltin> initial_ec_op_addr_ =
      FieldElementT::FromUint(ExtractHiddenMemberValue(ec_op_begin_addr_));

  const CompileTimeOptional<uint64_t, kHasKeccakBuiltin> keccak_begin_addr_;
  const CompileTimeOptional<FieldElementT, kHasKeccakBuiltin> initial_keccak_addr_ =
      FieldElementT::FromUint(ExtractHiddenMemberValue(keccak_begin_addr_));

  const CompileTimeOptional<uint64_t, kHasPoseidonBuiltin> poseidon_begin_addr_;
  const CompileTimeOptional<FieldElementT, kHasPoseidonBuiltin> initial_poseidon_addr_ =
      FieldElementT::FromUint(ExtractHiddenMemberValue(poseidon_begin_addr_));

  const FieldElementT rc_min_;
  const FieldElementT rc_max_;
  const EcPointT pedersen__shift_point_;
  const SigConfigT ecdsa__sig_config_;
  const EcOpCurveConfigT ec_op__curve_config_;

  // Interaction elements.
  FieldElementT memory__multi_column_perm__perm__interaction_elm_ = FieldElementT::Uninitialized();
  FieldElementT memory__multi_column_perm__hash_interaction_elm0_ = FieldElementT::Uninitialized();
  FieldElementT rc16__perm__interaction_elm_ = FieldElementT::Uninitialized();
  CompileTimeOptional<FieldElementT, kHasDilutedPool> diluted_check__permutation__interaction_elm_ =
      FieldElementT::Uninitialized();
  CompileTimeOptional<FieldElementT, kHasDilutedPool> diluted_check__interaction_z_ =
      FieldElementT::Uninitialized();
  CompileTimeOptional<FieldElementT, kHasDilutedPool> diluted_check__interaction_alpha_ =
      FieldElementT::Uninitialized();

  FieldElementT memory__multi_column_perm__perm__public_memory_prod_ =
      FieldElementT::Uninitialized();
  const FieldElementT rc16__perm__public_memory_prod_ = FieldElementT::One();
  const CompileTimeOptional<FieldElementT, kHasDilutedPool> diluted_check__first_elm_ =
      FieldElementT::Zero();
  const CompileTimeOptional<FieldElementT, kHasDilutedPool>
      diluted_check__permutation__public_memory_prod_ = FieldElementT::One();
  CompileTimeOptional<FieldElementT, kHasDilutedPool> diluted_check__final_cum_val_ =
      FieldElementT::Uninitialized();
};

}  // namespace cpu
}  // namespace starkware

#include "starkware/air/cpu/board/cpu_air_definition9.inl"

#endif  // STARKWARE_AIR_CPU_BOARD_CPU_AIR9_H_
