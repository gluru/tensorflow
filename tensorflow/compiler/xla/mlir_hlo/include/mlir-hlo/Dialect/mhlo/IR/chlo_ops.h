/* Copyright 2020 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#ifndef MLIR_HLO_DIALECT_MHLO_IR_CHLO_OPS_H
#define MLIR_HLO_DIALECT_MHLO_IR_CHLO_OPS_H

#include "dialect/Base.h"
#include "llvm/ADT/StringRef.h"
#include "mlir-hlo/utils/hlo_utils.h"
#include "mlir/Dialect/Complex/IR/Complex.h"
#include "mlir/Dialect/Quant/QuantTypes.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/Dialect.h"
#include "mlir/IR/DialectImplementation.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/OpDefinition.h"
#include "mlir/IR/Operation.h"
#include "mlir/IR/TypeUtilities.h"
#include "mlir/IR/Types.h"
#include "mlir/Interfaces/ControlFlowInterfaces.h"
#include "mlir/Interfaces/InferTypeOpInterface.h"
#include "mlir/Interfaces/SideEffectInterfaces.h"

// Include order below matters.
#include "mlir-hlo/Dialect/mhlo/IR/chlo_ops_enums.h.inc"
#define GET_ATTRDEF_CLASSES
#include "mlir-hlo/Dialect/mhlo/IR/chlo_ops_attrs.h.inc"

namespace mlir {
namespace chlo {

class ChloDialect : public Dialect {
 public:
  explicit ChloDialect(MLIRContext* context);
  static StringRef getDialectNamespace() { return "chlo"; }

  Operation* materializeConstant(OpBuilder& builder, Attribute value, Type type,
                                 Location loc) override;

  Attribute parseAttribute(DialectAsmParser& parser, Type type) const override;

  void printAttribute(Attribute attr, DialectAsmPrinter& os) const override;
};

}  // namespace chlo
}  // namespace mlir

namespace mlir {
namespace chlo {
namespace OpTrait {

template <typename ConcreteType>
class Broadcasting
    : public mlir::OpTrait::TraitBase<ConcreteType, Broadcasting> {};

}  // namespace OpTrait
}  // namespace chlo
}  // namespace mlir

#define GET_OP_CLASSES
#include "mlir-hlo/Dialect/mhlo/IR/chlo_ops.h.inc"

namespace mlir {
namespace chlo {

template <typename T>
static Value getConstantLike(OpBuilder& b, Location loc, T constant,
                             Value val) {
  Type ty = getElementTypeOrSelf(val.getType());
  auto getAttr = [&]() -> Attribute {
    if (ty.isa<IntegerType>()) return b.getIntegerAttr(ty, constant);
    if (ty.isa<FloatType>()) return b.getFloatAttr(ty, constant);
    if (auto complexTy = ty.dyn_cast<ComplexType>())
      return complex::NumberAttr::get(complexTy, constant, 0);
    llvm_unreachable("unhandled element type");
  };
  return b.create<ConstantLikeOp>(loc, getAttr(), val);
}

Value getConstantLike(OpBuilder& b, Location loc, const APFloat& constant,
                      Value val);

Value getConstantLikeMaxFiniteValue(OpBuilder& b, Location loc, Value val);

Value getConstantLikeInfValue(OpBuilder& b, Location loc, Value val,
                              bool negative);

Value getConstantLikeSmallestFiniteValue(OpBuilder& b, Location loc, Value val);

}  // namespace chlo
}  // namespace mlir

#endif  // MLIR_HLO_DIALECT_MHLO_IR_CHLO_OPS_H
