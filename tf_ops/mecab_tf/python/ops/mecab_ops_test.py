import os
import tensorflow as tf
from tensorflow.python.platform import test
from mecab_tf.python.ops.mecab_ops import MecabTagger


class MecabTest(test.TestCase):
    def testMecabTagger_TagMethod(self):
        tagger = MecabTagger("./test-data/dic")
        surfaces, features = tagger.tag(["シリーズ中、カンフーシーンが一番多い。", "※撮影中に、ジェット・リーが失踪。"])

        self.assertEqual(surfaces.nrows(), 2)
        self.assertEqual(features.nrows(), 2)
        self.assertEqual(surfaces[0, 0], "")  # bos
        self.assertEqual(surfaces[0, -1], "")  # eos
        self.assertTrue(features[0, 0].numpy().decode('UTF-8').startswith("BOS/EOS"))  # bos
        self.assertTrue(features[0, -1].numpy().decode('UTF-8').startswith("BOS/EOS")) #  eos

        self.assertEqual(surfaces[0, 1], "シリーズ")
        self.assertAllEqual(surfaces.row_lengths(), features.row_lengths())
        self.assertAllEqual(surfaces.row_lengths(), [10, 11])

    def testMecabTagger_otherInputs(self):
        tagger = MecabTagger("./test-data/dic")
        surfaces_scalar, features_scalar = tagger.tag("シリーズ中、カンフーシーンが一番多い。")
        surfaces, features = tagger.tag(["シリーズ中、カンフーシーンが一番多い。", "※撮影中に、ジェット・リーが失踪。"])
        surfaces_2d, features_2d = tagger.tag([["シリーズ中、カンフーシーンが一番多い。", "※撮影中に、ジェット・リーが失踪。"]])

        self.assertAllEqual(surfaces_scalar, surfaces[0])
        self.assertAllEqual(features_scalar, features[0])
        self.assertAllEqual(surfaces_scalar, surfaces_2d[0,0])
        self.assertAllEqual(features_scalar, features_2d[0,0])

    def testMecabTagger_tfFunction(self):
        tagger = MecabTagger("./test-data/dic")

        @tf.function(input_signature=[tf.TensorSpec([None], tf.string)])
        def tag(sentences):
            return tagger.tag(sentences)

        surfaces, features = tag(["シリーズ中、カンフーシーンが一番多い。"])

        self.assertEqual(surfaces.nrows(), 1)
        self.assertEqual(features.nrows(), 1)
        self.assertAllEqual(surfaces.row_lengths(), [10])

        surfaces, features = tag(["シリーズ中、カンフーシーンが一番多い。", "※撮影中に、ジェット・リーが失踪。"])

        self.assertEqual(surfaces.nrows(), 2)
        self.assertEqual(features.nrows(), 2)
        self.assertAllEqual(surfaces.row_lengths(), [10, 11])

    def testMecabTagger_savedModelWithSignature(self):
        out_dir = self.create_tempdir()
        model_path = os.path.join(out_dir, "model")

        tagger = MecabTagger("./test-data/dic")
        @tf.function
        def tag(sentences):
            surfaces, features = tagger.tag(sentences)
            return surfaces.values, features.values, surfaces.row_splits

        tag = tag.get_concrete_function(tf.TensorSpec([None], tf.string))
        tagger.__call__ = tag
        tf.saved_model.save(tagger, model_path, signatures={'serving_defaults': tag})

        tagger_loaded = tf.saved_model.load(model_path)
        surfaces, features, row_splits = tagger_loaded(sentences=tf.constant(["シリーズ中、カンフーシーンが一番多い。"]))
        surfaces = tf.RaggedTensor.from_row_splits(values=surfaces, row_splits=row_splits)
        features = tf.RaggedTensor.from_row_splits(values=features, row_splits=row_splits)

        self.assertEqual(surfaces.nrows(), 1)
        self.assertEqual(features.nrows(), 1)
        self.assertAllEqual(surfaces.row_lengths(), [10])
        self.assertEqual(surfaces[0, 1], "シリーズ")


if __name__ == '__main__':
    test.main()
