# Copyright 2019 The TensorFlow Authors. All Rights Reserved.
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
"""Test configs for unique."""
import tensorflow as tf
from tflite.testing.zip_test_utils import create_tensor_data
from tflite.testing.zip_test_utils import make_zip_of_tests
from tflite.testing.zip_test_utils import register_make_test_function


@register_make_test_function()
def make_unique_tests(options):
  """Make a set of tests for Unique op."""

  test_parameters = [{
      "input_shape": [[1]],
      "index_type": [tf.int32, tf.int64, None],
      "input_values": [3]
  }, {
      "input_shape": [[5]],
      "index_type": [tf.int32, tf.int64],
      "input_values": [[3, 2, 1, 2, 3]]
  }, {
      "input_shape": [[7]],
      "index_type": [tf.int32, tf.int64],
      "input_values": [[1, 1, 1, 1, 1, 1, 1]]
  }, {
      "input_shape": [[5]],
      "index_type": [tf.int32, tf.int64],
      "input_values": [[3, 2, 1, 0, -1]]
  }]

  def build_graph(parameters):
    """Build the graph for the test case."""

    input_tensor = tf.compat.v1.placeholder(
        dtype=tf.int32, name="input", shape=parameters["input_shape"])
    if parameters["index_type"] is None:
      output = tf.unique(input_tensor)
    else:
      output = tf.unique(input_tensor, parameters["index_type"])

    return [input_tensor], output

  def build_inputs(parameters, sess, inputs, outputs):
    input_values = [create_tensor_data(tf.int32, parameters["input_shape"])]
    return input_values, sess.run(
        outputs, feed_dict=dict(zip(inputs, input_values)))

  make_zip_of_tests(options, test_parameters, build_graph, build_inputs)
