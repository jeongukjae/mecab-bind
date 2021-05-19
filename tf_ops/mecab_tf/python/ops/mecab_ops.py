"""MeCab Tagger for string tensors"""

import tensorflow as tf
from tensorflow.python.framework import ops
from tensorflow.python.framework import load_library
from tensorflow.python.platform import resource_loader
from tensorflow.python.training.tracking import tracking
from tensorflow.python.ops.ragged import ragged_tensor

gen_mecab_ops = load_library.load_op_library(resource_loader.get_path_to_datafile('_mecab_ops.so'))


class _MecabModelResource(tracking.TrackableResource):
    def __init__(self, model_path, name):
        super(_MecabModelResource, self).__init__()
        self._model_path = model_path
        self._name = name
        _ = self.resource_handle  # Accessing this property creates the resource.

    def _create_resource(self):
        model_path, name = self._model_path, self._name
        with ops.name_scope(name, "MecabInitializer", [model_path]):
            return gen_mecab_ops.mecab_load(model_path=model_path)


class MecabTagger(tf.Module):
    """
    MeCab Tagger

    Note: If you use this Module in SavedModel format, it is recommended to use model_path as absolute path.
    The `model_path` is serialized, not the dictionary data.
    """

    def __init__(self, model_path, name=None, **kwargs):
        """Initialize MeCab Tagger

        Args:
            model_path: MeCab dictionary path.(`dicdir` option in MeCab)
        """
        super().__init__(name=name, **kwargs)

        self.model_resource = _MecabModelResource(model_path, name)

    def tag(self, input_sentence, name=None):
        """Tag a string tensor.

        Args:
            input_sentence: The tensor to tokenize
            name: The name for this op, optional.
        Returns:
            Tuple of RaggedTensors of surfaces and features.
        """
        with ops.name_scope("MecabTag"):
            input_tensor = ragged_tensor.convert_to_tensor_or_ragged_tensor(input_sentence)
            if input_tensor.shape.ndims is None:
                raise ValueError("Rank of input_tensor must be statically known.")

            if ragged_tensor.is_ragged(input_tensor):
                surfaces, features = self.tag(input_tensor.flat_values)
                return (
                    input_tensor.with_flat_values(surfaces),
                    input_tensor.with_flat_values(features),
                )

            if input_tensor.shape.ndims > 1:
                # Convert the input tensor to ragged and process it.
                return self.tag(tf.RaggedTensor.from_tensor(input_tensor))

            if input_tensor.shape.ndims == 0:
                surfaces, features = self.tag(tf.stack([input_tensor]))
                return surfaces.values, features.values

            (flat_surfaces, flat_features, row_splits) = gen_mecab_ops.mecab_tag(self.model_resource.resource_handle, input_tensor)
            surfaces = tf.RaggedTensor.from_row_splits(values=flat_surfaces, row_splits=row_splits)
            features = tf.RaggedTensor.from_row_splits(values=flat_features, row_splits=row_splits)
            return surfaces, features
