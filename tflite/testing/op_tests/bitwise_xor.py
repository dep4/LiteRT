# Copyright 2023 The TensorFlow Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ==============================================================================
"""Test configs for bitwise_xor operator."""
import tensorflow as tf
from tflite.testing.zip_test_utils import create_tensor_data
from tflite.testing.zip_test_utils import make_zip_of_tests
from tflite.testing.zip_test_utils import register_make_test_function


@register_make_test_function()
def make_bitwise_xor_tests(options):
  """Generate examples for bitwise_xor."""
  test_parameters = [
      {
          "input_dtype": [
              tf.uint8,
              tf.int8,
              tf.uint16,
              tf.int16,
              tf.uint32,
              tf.int32,
          ],
          "input_shape_pair": [
              ([], []),
              ([2, 3, 4], [2, 3, 4]),
              ([1, 1, 1, 3], [1, 1, 1, 3]),
              ([5, 5], [1]),
              ([10], [2, 4, 10]),
              ([2, 3, 3], [2, 3]),  # this test case is intended to fail
          ],
      },
  ]

  def build_graph(parameters):
    """Build the bitwise_xor testing graph."""
    input_value1 = tf.compat.v1.placeholder(
        dtype=parameters["input_dtype"],
        name="input1",
        shape=parameters["input_shape_pair"][0],
    )
    input_value2 = tf.compat.v1.placeholder(
        dtype=parameters["input_dtype"],
        name="input2",
        shape=parameters["input_shape_pair"][1],
    )
    out = tf.bitwise.bitwise_xor(input_value1, input_value2)
    return [input_value1, input_value2], [out]

  def build_inputs(parameters, sess, inputs, outputs):
    input_value1 = create_tensor_data(
        parameters["input_dtype"], parameters["input_shape_pair"][0]
    )
    input_value2 = create_tensor_data(
        parameters["input_dtype"], parameters["input_shape_pair"][1]
    )
    return [input_value1, input_value2], sess.run(
        outputs, feed_dict=dict(zip(inputs, [input_value1, input_value2]))
    )

  make_zip_of_tests(
      options,
      test_parameters,
      build_graph,
      build_inputs,
      expected_tf_failures=6,
  )
