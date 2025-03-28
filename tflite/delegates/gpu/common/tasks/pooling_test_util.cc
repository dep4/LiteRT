/* Copyright 2021 The TensorFlow Authors. All Rights Reserved.

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

#include "tflite/delegates/gpu/common/tasks/pooling_test_util.h"

#include <memory>
#include <vector>

#include "tflite/delegates/gpu/common/operations.h"
#include "tflite/delegates/gpu/common/status.h"
#include "tflite/delegates/gpu/common/task/testing_util.h"
#include "tflite/delegates/gpu/common/tasks/pooling.h"

namespace tflite {
namespace gpu {

absl::Status AveragePoolingTest(TestExecutionEnvironment* env) {
  TensorFloat32 src_tensor;
  src_tensor.shape = BHWC(1, 2, 2, 2);
  src_tensor.data = {0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f};

  Pooling2DAttributes attr;
  attr.padding.prepended = HW(0, 0);
  attr.padding.appended = HW(0, 0);
  attr.strides = HW(2, 2);
  attr.kernel = HW(2, 2);
  attr.type = PoolingType::AVERAGE;

  for (auto precision : env->GetSupportedPrecisions()) {
    auto data_type = DeduceDataTypeFromPrecision(precision);
    for (auto storage : env->GetSupportedStorages(data_type)) {
      const float eps = precision == CalculationsPrecision::F32 ? 1e-6f : 1e-3f;
      OperationDef op_def;
      op_def.precision = precision;
      op_def.src_tensors.push_back({data_type, storage, Layout::HWC});
      op_def.dst_tensors.push_back({data_type, storage, Layout::HWC});
      TensorFloat32 dst_tensor;
      GPUOperation operation = CreatePooling(op_def, env->GetGpuInfo(), attr);
      RETURN_IF_ERROR(env->ExecuteGPUOperation(
          src_tensor, std::make_unique<GPUOperation>(std::move(operation)),
          BHWC(1, 1, 1, 2), &dst_tensor));
      RETURN_IF_ERROR(PointWiseNear({3.0f, 4.0f}, dst_tensor.data, eps));
    }
  }
  return absl::OkStatus();
}

absl::Status AveragePoolingNonEmptyPaddingTest(TestExecutionEnvironment* env) {
  TensorFloat32 src_tensor;
  src_tensor.shape = BHWC(1, 2, 2, 1);
  src_tensor.data = {0.0f, 1.0f, 2.0f, 3.0f};

  Pooling2DAttributes attr;
  attr.padding.prepended = HW(0, 0);
  attr.padding.appended = HW(1, 1);
  attr.strides = HW(1, 1);
  attr.kernel = HW(2, 2);
  attr.type = PoolingType::AVERAGE;

  for (auto precision : env->GetSupportedPrecisions()) {
    auto data_type = DeduceDataTypeFromPrecision(precision);
    for (auto storage : env->GetSupportedStorages(data_type)) {
      const float eps = precision == CalculationsPrecision::F32 ? 1e-6f : 1e-3f;
      OperationDef op_def;
      op_def.precision = precision;
      op_def.src_tensors.push_back({data_type, storage, Layout::HWC});
      op_def.dst_tensors.push_back({data_type, storage, Layout::HWC});
      TensorFloat32 dst_tensor;
      GPUOperation operation = CreatePooling(op_def, env->GetGpuInfo(), attr);
      RETURN_IF_ERROR(env->ExecuteGPUOperation(
          src_tensor, std::make_unique<GPUOperation>(std::move(operation)),
          BHWC(1, 2, 2, 1), &dst_tensor));
      RETURN_IF_ERROR(
          PointWiseNear({1.5f, 2.0f, 2.5f, 3.0f}, dst_tensor.data, eps));
    }
  }
  return absl::OkStatus();
}

absl::Status MaxPoolingTest(TestExecutionEnvironment* env) {
  TensorFloat32 src_tensor;
  src_tensor.shape = BHWC(1, 2, 2, 2);
  src_tensor.data = {8.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f};

  Pooling2DAttributes attr;
  attr.padding.prepended = HW(0, 0);
  attr.padding.appended = HW(0, 0);
  attr.strides = HW(2, 2);
  attr.kernel = HW(2, 2);
  attr.type = PoolingType::MAX;

  for (auto precision : env->GetSupportedPrecisions()) {
    auto data_type = DeduceDataTypeFromPrecision(precision);
    for (auto storage : env->GetSupportedStorages(data_type)) {
      const float eps = precision == CalculationsPrecision::F32 ? 1e-6f : 1e-3f;
      OperationDef op_def;
      op_def.precision = precision;
      op_def.src_tensors.push_back({data_type, storage, Layout::HWC});
      op_def.dst_tensors.push_back({data_type, storage, Layout::HWC});
      TensorFloat32 dst_tensor;
      GPUOperation operation = CreatePooling(op_def, env->GetGpuInfo(), attr);
      RETURN_IF_ERROR(env->ExecuteGPUOperation(
          src_tensor, std::make_unique<GPUOperation>(std::move(operation)),
          BHWC(1, 1, 1, 2), &dst_tensor));
      RETURN_IF_ERROR(PointWiseNear({8.0f, 7.0f}, dst_tensor.data, eps));
    }
  }
  return absl::OkStatus();
}

absl::Status MaxPoolingIndicesTest(TestExecutionEnvironment* env) {
  TensorFloat32 src_tensor;
  src_tensor.shape = BHWC(1, 2, 2, 2);
  src_tensor.data = {8.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f};

  Pooling2DAttributes attr;
  attr.padding.prepended = HW(0, 0);
  attr.padding.appended = HW(0, 0);
  attr.strides = HW(2, 2);
  attr.kernel = HW(2, 2);
  attr.type = PoolingType::MAX;
  attr.output_indices = true;

  for (auto precision : env->GetSupportedPrecisions()) {
    auto data_type = DeduceDataTypeFromPrecision(precision);
    for (auto storage : env->GetSupportedStorages(data_type)) {
      const float eps = precision == CalculationsPrecision::F32 ? 1e-6f : 1e-3f;
      OperationDef op_def;
      op_def.precision = precision;
      op_def.src_tensors.push_back({data_type, storage, Layout::HWC});
      op_def.dst_tensors.push_back({data_type, storage, Layout::HWC});
      op_def.dst_tensors.push_back({data_type, storage, Layout::HWC});
      TensorFloat32 dst_tensor;
      TensorFloat32 dst_tensor_ind;
      GPUOperation operation = CreatePooling(op_def, env->GetGpuInfo(), attr);
      RETURN_IF_ERROR(env->ExecuteGPUOperation(
          {src_tensor}, std::make_unique<GPUOperation>(std::move(operation)),
          {BHWC(1, 1, 1, 2), BHWC(1, 1, 1, 2)},
          {&dst_tensor, &dst_tensor_ind}));
      RETURN_IF_ERROR(PointWiseNear({8.0f, 7.0f}, dst_tensor.data, eps));
      for (auto& v : dst_tensor_ind.data) {
        v = static_cast<int>(v);
      }
      RETURN_IF_ERROR(PointWiseNear({0.0f, 3.0f}, dst_tensor_ind.data, eps));
    }
  }

  // Testing writing of indices in int tensor
  for (auto precision : env->GetSupportedPrecisions()) {
    auto data_type = DeduceDataTypeFromPrecision(precision);
    for (auto storage : env->GetSupportedStorages(data_type)) {
      OperationDef op_def;
      op_def.precision = precision;
      op_def.src_tensors.push_back({data_type, storage, Layout::HWC});
      op_def.dst_tensors.push_back({data_type, storage, Layout::HWC});
      op_def.dst_tensors.push_back({DataType::INT32, storage, Layout::HWC});

      TensorDescriptor src_0, dst_0, dst_1;
      src_0 = op_def.src_tensors[0];
      src_0.UploadData(src_tensor);
      dst_0.SetBHWCShape(BHWC(1, 1, 1, 2));
      dst_1.SetBHWCShape(BHWC(1, 1, 1, 2));

      GPUOperation operation = CreatePooling(op_def, env->GetGpuInfo(), attr);
      RETURN_IF_ERROR(env->ExecuteGPUOperation(
          {&src_0}, {&dst_0, &dst_1},
          std::make_unique<GPUOperation>(std::move(operation))));

      TensorFloat32 dst_tensor;
      dst_0.DownloadData(&dst_tensor);
      tflite::gpu::Tensor<BHWC, DataType::INT32> dst_tensor_ind;
      dst_1.DownloadData(&dst_tensor_ind);
      const float eps = precision == CalculationsPrecision::F32 ? 1e-6f : 1e-3f;
      RETURN_IF_ERROR(PointWiseNear({8.0f, 7.0f}, dst_tensor.data, eps));
      tflite::gpu::Tensor<BHWC, DataType::INT32> ref_tensor;
      ref_tensor.shape = BHWC(1, 1, 1, 2);
      ref_tensor.data = {0, 3};
      if (dst_tensor_ind.data != ref_tensor.data) {
        return absl::InternalError("not equal");
      }
    }
  }
  return absl::OkStatus();
}

}  // namespace gpu
}  // namespace tflite
